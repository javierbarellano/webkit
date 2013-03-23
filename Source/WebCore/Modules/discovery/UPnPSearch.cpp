
#include "config.h"

#if ENABLE(DISCOVERY)

#define WEBKIT_IMPLEMENTATION 1

#include "../../../../Source/WebCore/platform/network/soup/TCPSocketHandle.h"

#include "../../../../Source/WebCore/platform/network/soup/UDPSocketError.h"
#include "../../../../Source/WebCore/platform/network/UDPSocketHandleClient.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketHandle.h"

#include "../../../../Source/WebCore/platform/network/soup/HNEventHandle.h"
#include "../../../../Source/WebCore/platform/network/HNEventError.h"
#include "../../../../Source/WebCore/platform/network/HNEventServerHandleClient.h"
#include "../../../../Source/WebCore/platform/network/HNEventServerHandle.h"

//#include "base/bind.h"

#include "NavDsc.h"

#include "UPnPSearch.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/time.h>

#include <sstream>
#include <string>
#include <map>
#include <vector>

#include <stdlib.h>
#include <stdio.h>

#define MAX_CONTACT_ATTEMPTS 3
/* Note: The reason we try to contact the device multiple times is that there appears to be a bug in WebKit
 * that causes UPnPSearch to timeout on a legitimate HTTPGet, if WebKit is elsewhere trying to load a non-existant page at the same time.
 * Steps to duplicate:
 * - Change MAX_CONTACT_ATTEMPTS to 1
 * - Uncomment debug printouts for checkForDroppedInternalDevs
 * - Rebuild WebKit and Discovery enabled browser (QtRUIBrowser)
 * - Bring up a Discovery enabled browser
 * - Bring up a RUI Server configured with a bad RUI uri. (I just changed http to https)
 * - Observe debug messages for checkForDroppedInternalDevs
 * - Select the RUI with a bad uri.
 * - Observe that our HTTPGet to the running server times out.
 */

namespace WebCore {

void droppedDevsThread(void *context)
{
    long lastSendSeconds = 0;
    UPnPSearch* upnp = (UPnPSearch*)context;
	upnp->m_droppedStillRunning = true;
	
	while (upnp->m_droppedStillRunning)
	{
		struct timeval now;
		gettimeofday(&now, NULL);

        long elapsedSeconds = now.tv_sec - lastSendSeconds;

        if (elapsedSeconds >= 10)
		{
            lastSendSeconds = now.tv_sec;
			upnp->checkForDroppedDevs();
			upnp->checkForDroppedInternalDevs();
		}

		usleep(1000L*1000L); // 1000 ms
	}
}

/* The DLNA/UPnP specs are somewhat vague on frequency and timing of M-SEARCH messages. I think it says:
 * - Because of unreliability of UDP, an M-SEARCH message should be repeated up to 10 times
 * - Repeated M-SEARCH messages should occur within a 200 ms window.
 * - There is no requirement for periodic M-SEARCH requests.
 */

void discoveryThread(void *context)
{
    long lastSendSeconds = 0;
    UPnPSearch* upnp = (UPnPSearch*)context;

    upnp->m_stillRunning = true;

    while (upnp->m_stillRunning) {

        struct timeval now;
        gettimeofday(&now, NULL);

        long elapsedSeconds = now.tv_sec - lastSendSeconds;

        // Even though not required to, send a M-SEARCH set once per minute
        if (elapsedSeconds >= 20) {

        	if (!upnp->m_udpSocket) {
                KURL url(ParsedURLString, String(upnp->url_));
        		upnp->m_udpSocket = UDPSocketHandle::create(url, false, upnp);
                if (!upnp->m_udpSocket->connected()) {
                	upnp->m_udpSocket.release();
                	upnp->m_udpSocket = NULL;
                }

        	}

        	if (upnp->m_udpSocket) {

				// Spec recommends more than one, but less than 10, in a 250ms window.
				// We'll send 3 repeats, 75ms apart.

				for ( int i=0; i < 3; i++ ) {
					bool netIdUp = upnp->m_udpSocket->send(upnp->sendData_.c_str(), upnp->sendData_.length());
					if (netIdUp != upnp->m_netIsUp) {
						upnp->m_netIsUp = netIdUp;
						if (upnp->navDsc_)
							upnp->navDsc_->onNetworkChanged(netIdUp);
					}
					usleep(75L*1000L); // 75ms
				}
        	}

            lastSendSeconds = now.tv_sec;
        }

        upnp->m_udpSocket->receive();
        usleep(100L*1000L); // 100 ms
    }
}

void notifyThread(void *context)
{
    UPnPSearch* upnp = (UPnPSearch*)context;

    upnp->m_stillRunning = true;

    while (upnp->m_stillRunning) {

    	if (!upnp->m_mcastSocket) {
            KURL url(ParsedURLString, String(upnp->url_));
    		upnp->m_mcastSocket = UDPSocketHandle::create(url, true, upnp);
            if (!upnp->m_mcastSocket->connected()) {
            	upnp->m_mcastSocket.release();
            	upnp->m_mcastSocket = NULL;

				if (upnp->m_netIsUp) {
					upnp->m_netIsUp = false;
					if (upnp->navDsc_)
						upnp->navDsc_->onNetworkChanged(upnp->m_netIsUp);
				}
    			usleep(20000L*1000L); // 20 secs
    			continue;
            }

			if (!upnp->m_netIsUp) {
				upnp->m_netIsUp = true;
				if (upnp->navDsc_)
					upnp->navDsc_->onNetworkChanged(upnp->m_netIsUp);
			}
    	}

		upnp->m_mcastSocket->receive();
		usleep(1000L*1000L); // 1 sec
    }
}

UPnPSearch* UPnPSearch::instance_ = NULL;

// static
UPnPSearch* UPnPSearch::getInstance()
{
	if (!instance_)
		instance_ = new UPnPSearch(NULL);

	return instance_;
}

// static
UPnPSearch* UPnPSearch::create()
{
	createConnect(NULL);

	return instance_;
}


UPnPSearch::UPnPSearch(const char *type) :
		DiscoveryBase()
{
	devLock_ = new Mutex();
	internal_type_ = "";
	if (type)
		regTypes_.insert(std::string(type));

	api_ = NULL;
	navDsc_ = NULL;
	tcpSocket_ = new std::map<long, KURL>();
	
	sendData_ = "M-SEARCH * HTTP/1.1\r\nST: upnp:rootdevice\r\nMX: 5\r\nMAN: \"ssdp:discover\"\r\nHOST: 239.255.255.250:1900\r\n\r\n";

	threadDone_ = true;
	devs_.clear();
	lastSend_ = -1;
	sendPending_ = false;

    strcpy(url_, "udp://239.255.255.250:1900");
}

UPnPSearch::~UPnPSearch()
{
	if (instance_->serverHandle_)
		closeServer();

	devs_.clear();
	delete tcpSocket_;
	instance_ = NULL;
	delete devLock_;
}

void UPnPSearch::closeServer()
{
	instance_->serverHandle_->close();
}

void UPnPSearch::getUPnPFriendlyName(std::string uuid, std::string type, std::string& name)
{
	devLock_->lock();
	if (devs_.find(type) != devs_.end())
	{
		UPnPDevMap dm = devs_[type];
		if (dm.devMap.find(uuid) != dm.devMap.end())
		{
			name.append(dm.devMap[uuid].friendlyName);
		}
		else
			printf("getUPnPFriendlyName() UUID(%s) not found! dm.devMao.size=%d\n", uuid.c_str(), (int)dm.devMap.size());
	}
	else
		printf("getUPnPFriendlyName() Type(%s) not found!\n", type.c_str());
	devLock_->unlock();

}

//static
void UPnPSearch::createConnect(const char *type)
{

	if (!instance_)
		instance_ = new UPnPSearch(type);

	instance_->m_netIsUp = true;

    if (!instance_->m_udpSocket)
    {
        KURL url(ParsedURLString, String(instance_->url_));

        // Constructor connects to socket
        instance_->m_udpSocket = UDPSocketHandle::create(url, false, instance_);
        if (!instance_->m_udpSocket->connected()) {
        	printf("createConnect() Not connected\n");
        	instance_->m_udpSocket.release();
        	instance_->m_udpSocket = NULL;
        	instance_->m_netIsUp = false;
        }

		instance_->m_tID = WTF::createThread(discoveryThread, instance_, "UPnP_discovery");
		instance_->m_tDroppedID = WTF::createThread(droppedDevsThread, instance_, "DroppedUPnP");
    }

    if (!instance_->m_mcastSocket)
    {
        KURL url(ParsedURLString, String(instance_->url_));

        // Constructor connects to socket
        instance_->m_mcastSocket = UDPSocketHandle::create(url, true, instance_) ;
        if (!instance_->m_mcastSocket->connected()) {
        	instance_->m_mcastSocket.release();
        	instance_->m_mcastSocket = NULL;
        	instance_->m_netIsUp = false;
        }

        instance_->m_tNotifyID = WTF::createThread(notifyThread, instance_, "UPnP_notify");
    }

}

// static
std::map<std::string, UPnPDevice> UPnPSearch::discoverInternalDevs(const char *type, IDiscoveryAPI *api)
{

	createConnect(type);
	instance_->api_ = api;
    instance_->internal_type_ = type;

	if (instance_->internalDevs_.find(std::string(type)) != instance_->internalDevs_.end())
	{
		UPnPDevMap dm = instance_->internalDevs_.find(std::string(type))->second;
		return (dm.devMap);
	}

	return std::map<std::string, UPnPDevice>();
}

// static
std::map<std::string, UPnPDevice> UPnPSearch::discoverDevs(const char *type, NavDsc *navDsc)
{
	std::map<std::string, UPnPDevice> empty;
	createConnect(type);
	instance_->navDsc_ = navDsc;

	instance_->devLock_->lock();
	instance_->regTypes_.insert(std::string(type));

	if (instance_->devs_.find(std::string(type)) != instance_->devs_.end())
	{
		UPnPDevMap dm = instance_->devs_.find(std::string(type))->second;
		instance_->devLock_->unlock();
		return (dm.devMap);
	}
	instance_->devLock_->unlock();

	return empty;
}

// Called when Socket Stream is opened.
void UPnPSearch::UDPdidOpenStream(UDPSocketHandle* handle)
{
}

// Called when |amountSent| bytes are sent.
void UPnPSearch::UDPdidSendData(UDPSocketHandle* handle, int amountSent)
{
}

// Called when data are received.
void UPnPSearch::UDPdidReceiveData(UDPSocketHandle* handle, const char *data, int dLen, const char *hostPort, int hpLen)
{
    if (dLen > 0) {

        std::map<std::string,std::string> headers = parseUDPMessage(data, dLen);

        std::string usn = getTokenValue(headers, "USN");
        std::string location = getTokenValue(headers, "LOCATION");
        std::string st = getTokenValue(headers, "NTS");
        std::string type = getTokenValue(headers, "NT");

        std::string uuid;
        size_t pos = usn.find_first_of(':');
        if (pos != std::string::npos) {
            uuid = usn.substr(pos+1);
        }

        std::string line1 = getTokenValue(headers,"LINE1");
        pos = line1.find("NOTIFY");

        if (pos != std::string::npos) {

            // This is a NOTIFY message

            // We are only interested in root device notifications.
            if ( type == "upnp:rootdevice")
            {
                if (st == "ssdp:alive") {
                    //fprintf(stderr,"Received NOTIFY:alive - %s: %s\n", location.c_str(), uuid.c_str());
                    parseDev(data, dLen, NULL);
                } else if (st == "ssdp:byebye"){
                    //fprintf(stderr,"Received NOTIFY:byebye: %s\n", uuid.c_str());
                    // We don't know which list(s) the device is on, so check them both.
                	devLock_->lock();
                    removeDevice(&devs_, uuid);
                    removeDevice(&internalDevs_, uuid);
                    devLock_->unlock();
                }
            }
        } else if (location.size() > 0){
            // Not a NOTIFY. Must be an M_SEARCH reply
            parseDev(data, dLen, NULL);
            //fprintf(stderr,"Received M-SEARCH reply - %s: %s\n", location.c_str(), uuid.c_str());
        }
    }
}

void UPnPSearch::UDPdidClose(UDPSocketHandle* handle)
{
}

void UPnPSearch::UDPdidFail(UDPSocketHandle* udpHandle, UDPSocketError& error)
{
	//printf("UPnPSearch::didFail() UDPSocketHandle error: %d\n", error.getErr());
	if (navDsc_)
		navDsc_->onUPnPError(error.getErr());
	if (api_)
		api_->onError(error.getErr());
}

void UPnPSearch::checkForDroppedDevs()
{
	//printf("checkForDroppedDevs() start\n");
	devLock_->lock();
	for (std::map<std::string, UPnPDevMap>::iterator i = devs_.begin(); i != devs_.end(); i++)
	{
		std::vector<std::string> dropMe;
		dropMe.clear();
		UPnPDevMap dm = (*i).second;
		std::string type = (*i).first;
		for (std::map<std::string, UPnPDevice>::iterator k = dm.devMap.begin(); k != dm.devMap.end(); k++)
		{
			UPnPDevice dv = (*k).second;
			KURL url(ParsedURLString, String(dv.descURL.c_str()));
			char bf[8000];
			bf[0] = 0;
			size_t len = sizeof(bf)-1;

			// Get Description of service to insure the device is still working
			HTTPget(url, bf, &len);

			std::string reply = bf;
			int pos = reply.find("HTTP/1.1 404");
			if (len == 0 || pos != std::string::npos) {

				// Unresponsive. See note at top of file for reason for retries
				devs_[type].devMap[dv.uuid].contactAttempts++;
				//fprintf(stderr,"Timeout or 404 fetching device description. Attempt: %d  Url: %s, Reply: %s\n", dv.contactAttempts, dv.descURL.c_str(), bf );

				if ( devs_[type].devMap[dv.uuid].contactAttempts > MAX_CONTACT_ATTEMPTS) {
					//fprintf(stderr,"Dropping Device. Url: %s, Reply: %s\n", dv.descURL.c_str(), bf );
					if (devs_[type].devMap[dv.uuid].online) {
						devs_[type].devMap[dv.uuid].online = false;
						if (navDsc_)
							navDsc_->serviceOffline(type, devs_[type].devMap[dv.uuid]);
					}
				}
			} else {
				devs_[type].devMap[dv.uuid].contactAttempts = 0;
				if (!devs_[type].devMap[dv.uuid].online) {
					devs_[type].devMap[dv.uuid].online = true;
					if (navDsc_)
						navDsc_->serviceOnline(type, devs_[type].devMap[dv.uuid]);
				}
			}
		}

		if (dropMe.size())
		{
            for (int d=0; d<(int)dropMe.size(); d++) {
            	devs_[type].devMap.find(dropMe.at(d))->second.online = true;
            }

			if (navDsc_)
				navDsc_->lostUPnPDev(type);
		}
	}
	devLock_->unlock();
}

void UPnPSearch::checkForDroppedInternalDevs()
{
    //fprintf(stderr,"checkForDroppedInternalDevs() start\n");
	for (std::map<std::string, UPnPDevMap>::iterator i = internalDevs_.begin(); i != internalDevs_.end(); i++)
	{
		std::vector<std::string> dropMe;
		dropMe.clear();
		UPnPDevMap dm = (*i).second;
		std::string type = (*i).first;
		for (std::map<std::string, UPnPDevice>::iterator k = dm.devMap.begin(); k != dm.devMap.end(); k++)
		{
			UPnPDevice dv = (*k).second;
			char host[1024];
			char path[1024];
			int port;
			getHostPort(dv.descURL.c_str(),host, &port);
			getPath(dv.descURL.c_str(),path);
			//KURL url(ParsedURLString, String(dv.descURL.c_str()));
			char bf[8000];
			bf[0] = 0;
			size_t len = sizeof(bf)-1;

			// Get Description of service to insure the device is still working
			HTTPget(host, port, bf, path, &len);

			std::string reply = bf;
			int pos = reply.find("HTTP/1.1 404");
			if (len == 0 || pos != std::string::npos) {

				// Unresponsive. See note at top of file for reason for retries
				dv.contactAttempts++;
				//fprintf(stderr,"Timeout or 404 fetching device description. Attempt: %d  Url: %s, Reply: %s\n", dv.contactAttempts, dv.descURL.c_str(), bf );

				if ( dv.contactAttempts > MAX_CONTACT_ATTEMPTS) {
					//fprintf(stderr,"Dropping Device. Url: %s, Reply: %s\n", dv.descURL.c_str(), bf );
					dropMe.push_back((*k).first);
				}
			} else {
				dv.contactAttempts = 0;
			}

			// Update device record.
			dm.devMap[dv.uuid] = dv;
			internalDevs_[type] = dm;
 		}

		if (dropMe.size())
		{
            for (int d=0; d<(int)dropMe.size(); d++) {
                dm.devMap.erase(dm.devMap.find(dropMe.at(d)));
                internalDevs_[type] = dm;

            }

			internalDevs_[type] = dm;
            if (api_) {
				api_->serverListUpdate(type, &dm.devMap);
            }
		}
	}
}



// Called by CrossOriginAccessControl.cpp
// Check cross domain white list
bool UPnPSearch::hostPortOk(const char* host, int port)
{
	char lhost[1000];
	int lport;

	devLock_->lock();
	std::map<std::string, UPnPDevMap>::iterator it =  devs_.begin();
	for (; it!=devs_.end(); it++)
	{
		UPnPDevMap dm = it->second;
		std::map<std::string, UPnPDevice>::iterator i = dm.devMap.begin();
		for (; i!= dm.devMap.end(); i++)
		{
			UPnPDevice d = (*i).second;
			std::string url = d.descURL;

			getHostPort(url.c_str(),lhost, &lport);

			if (lport == port && !strcmp(host,lhost)) {
				devLock_->unlock();
				return true;
			}
		}
	}
	devLock_->unlock();

	return false;
}

bool UPnPSearch::parseDev(const char* resp, std::size_t respLen, const char* hostPort)
{
//	HTTP/1.1 200 OK
//	EXT:
//	SERVER: UPnP/1.0
//	ST: upnp:rootdevice
//	LOCATION: http://10.36.0.199:80/upnp/root_description.xml
//	CACHE-CONTROL: max-age=1800
//	USN: uuid:380D0BFB-2B2A-448C-99A8-0018DD018C56::upnp:rootdevice
//	Content-Length: 0

    std::map<std::string,std::string> headers = parseUDPMessage(resp, respLen);

    std::string sLoc = getTokenValue(headers, "LOCATION");
    if ( sLoc.empty()) {
        //printf("No Location header found!\n");
        return false;
    }

    std::string usn = getTokenValue(headers, "USN");
    if ( usn.empty()) {
        printf("No USN header found!\n");
        return false;
    }

    std::string sUuid;
    size_t pos = usn.find_first_of(':');
    if (pos != std::string::npos) {
        sUuid = usn.substr(pos+1);
    }

    UPnPDevMap dm;

	// Now get the friendly name, ugh...
	std::string path;
	std::string port;
	std::string host = sLoc.substr(sLoc.find("://")+3);

	path = host.substr(host.find("/"));

	if (host.find(":") != host.npos)
	{
		port = host.substr(host.find(":")+1);
		port = port.substr(0,port.find("/"));

		host = host.substr(0, host.find(":"));
	}
	else
	{
		port = "80";
		host = host.substr(0, host.find("/"));
	}

	char bf[8000];
	size_t len = sizeof(bf)-1;
	HTTPget(host.c_str(), atoi(port.c_str()), path.c_str(), bf, &len);
	
	if (!len) {
		//printf("Parse(): HTTPget(%s) len==0! Skipping...\n", sLoc.c_str());
		return false;
	}

	bf[len] = 0;
	std::vector<std::string> foundTypes;
	//printf("Parse(%s): len=%d\n%s\n", sLoc.c_str(), (int)len, bf);

	// Look for the service type that was given to us from JavaScript
	// Form is something like:
	//    urn:schemas-upnp-org:service:ContentDirectory:1
	if (!isCurrentType(bf, foundTypes) && !isInternalType(bf))
	{
		if (notInBadList(sUuid)) {
			badDevs_.push_back(sUuid);
			//printf("%s doesn't include a registered service.\n",host.c_str());
		}
		return false;
	}

    // Now look for something like: '<friendlyName>XPDESKTOP:</friendlyName>'
    std::string friendlyName = getElementValue(bf, (char*)"friendlyName");
    if (friendlyName.length() == 0)
	{
		badDevs_.push_back(sUuid);
		printf("%s has no friendlyName!!!!\n",host.c_str());
		return false;
	}

	// Now look for event url:
	/*
	 * 	<serviceList>
			<service>
				<serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>
				<serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId>
				<controlURL>/upnphost/udhisapi.dll?control=uuid:924738b0-a1c1-4713-9d8b-c9e70e73a1b4+urn:upnp-org:serviceId:ConnectionManager</controlURL>
				<eventSubURL>/upnphost/udhisapi.dll?event=uuid:924738b0-a1c1-4713-9d8b-c9e70e73a1b4+urn:upnp-org:serviceId:ConnectionManager</eventSubURL>
				<SCPDURL>/upnphost/udhisapi.dll?content=uuid:ee998394-35df-45fd-9df5-d4a28a142e63</SCPDURL>
			</service>
			<service>
				<serviceType>urn:schemas-upnp-org:service:ContentDirectory:1</serviceType>
				<serviceId>urn:upnp-org:serviceId:ContentDirectory</serviceId>
				<controlURL>/upnphost/udhisapi.dll?control=uuid:924738b0-a1c1-4713-9d8b-c9e70e73a1b4+urn:upnp-org:serviceId:ContentDirectory</controlURL>
				<eventSubURL>/upnphost/udhisapi.dll?event=uuid:924738b0-a1c1-4713-9d8b-c9e70e73a1b4+urn:upnp-org:serviceId:ContentDirectory</eventSubURL>
				<SCPDURL>/upnphost/udhisapi.dll?content=uuid:d8e35e6b-5fe7-409a-b6fa-f48b9a92dae4</SCPDURL>
			</service>
			<service>
				<serviceType>urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1</serviceType>
				<serviceId>urn:microsoft.com:serviceId:X_MS_MediaReceiverRegistrar</serviceId>
				<controlURL>/upnphost/udhisapi.dll?control=uuid:924738b0-a1c1-4713-9d8b-c9e70e73a1b4+urn:microsoft.com:serviceId:X_MS_MediaReceiverRegistrar</controlURL>
                <eventSubURL>/upnphost/udhisapi.dll?event=uuid:924738b0-a1c1-4713-9d8b-c9e70e73a1b4+urn:microsoft.com:serviceId:X_MS_MediaReceiverRegistrar</eventSubURL>
				<SCPDURL>/upnphost/udhisapi.dll?content=uuid:03bf5b96-fec6-41ab-8990-a5f7f2a17070</SCPDURL>
			</service>
		</serviceList>
	 *
	 */

    // Changed this to process each service list instead of just the first. No guarantees which device will have the service we want.
    // TODO: This implementation assumes/limits one matching service per device description, which is not guaranteed.
    std::vector<std::string> serviceLists = getElementArray(bf, (char*)"serviceList");

    std::string eventUrl;

    // For each service list
    if (serviceLists.size() > 0) {
        std::vector<std::string>::iterator i = serviceLists.begin();
        while (i != serviceLists.end()) {

            std::string serviceList = *i;

            std::vector<std::string> services = getElementArray(bf, (char*)"service");

            // For each service
            if (services.size() > 0) {
                std::vector<std::string>::iterator j = services.begin();
                while (j != services.end()) {

                    std::string service = *j;

                    // Check service type
                    std::string serviceType = getElementValue(service.c_str(), (char*)"serviceType");
                    if (isCurrentType(serviceType.c_str(), foundTypes) || isInternalType(serviceType.c_str())) {
                        eventUrl = getElementValue(service.c_str(), "eventSubURL");
                    }
                    j++;
                }
            }
            i++;
        }
    }

	UPnPDevice d;
	d.descURL = sLoc;
    d.friendlyName = friendlyName;
    d.eventURL = eventUrl;
    d.host = host;
	d.port = port;
	d.uuid = sUuid;
    d.contactAttempts = 0;


    foundTypes.clear();
	if (isCurrentType(bf, foundTypes))
	{
		for (int i=0; i<foundTypes.size(); i++) {
            std::string foundType = foundTypes.at(i);

            devLock_->lock();
            if (devs_.find(foundType)==devs_.end() || devs_[foundType].devMap.find(sUuid)==devs_[foundType].devMap.end()) {

                dm = devs_[foundType];
                //char* action = (char*)"Adding";

                std::map<std::string, UPnPDevice>::iterator it;
                it = dm.devMap.find(sUuid);
                if (it != dm.devMap.end()) {
                    //action = (char*)"Updating";
                    if (!dm.devMap[sUuid].changed(d)) {
                        continue;
                    }
                }

				dm.devMap[sUuid] = d;
                devs_[foundType] = dm;
                //fprintf(stderr,"%s device: %s : %s, %s.\n", action, host.c_str(), d.friendlyName.c_str(), sUuid.c_str());
                if (navDsc_) {
                    navDsc_->foundUPnPDev(foundType);
                }
			}
            devLock_->unlock();
		}
	}

	if (isInternalType(bf))
	{
        dm = internalDevs_[internal_type_];
        char* action = (char*)"Adding";

        std::map<std::string, UPnPDevice>::iterator it;
        it = dm.devMap.find(sUuid);
        if (it != dm.devMap.end()) {
            action = (char*)"Updating";
            if (!dm.devMap[sUuid].changed(d)) {
                action = 0;
            }
        }

        if (action != 0) {

            dm.devMap[sUuid] = d;
            internalDevs_[internal_type_] = dm;  // By value, so copy in new contents.

            //fprintf(stderr,"%s internal device: %s : %s, %s\n", action, host.c_str(), d.friendlyName.c_str(), sUuid.c_str());
            fflush(stderr);
            if (api_) {
                api_->serverListUpdate(internal_type_, &dm.devMap);
            }
        }
	}

	return true;
}

bool UPnPSearch::isCurrentType(const char* type, std::vector<std::string> &regType)
{
	devLock_->lock();
	if (regTypes_.size() > 0) {
		std::set<std::string>::iterator i = regTypes_.begin();
		while (i != regTypes_.end()) {
			if (strstr(type,i->c_str())) {
				regType.push_back(*i);
			}
			i++;
		}
		devLock_->unlock();
		return regType.size()>0;
	}
	devLock_->unlock();

	return false;
}

bool UPnPSearch::isInternalType(const char* type)
{
	if (internal_type_.length() > 0 && strstr(type,internal_type_.c_str()))
		return true;

	return false;
}

bool UPnPSearch::notInBadList(std::string sUuid)
{
	for (int i=0; i < (int)badDevs_.size(); i++) {
		if (badDevs_[i].compare(sUuid)==0) return false;
	}

	return true;
}


void UPnPSearch::eventServer(const char *type, std::string eventUrl, std::string uuid, std::string host, std::string port)
{
 	if (!strlen(eventUrl.c_str()))
 		return;


 	if (!instance_->serverHandle_)
 	{
 		instance_->serverHandle_ = new HNEventHandle(instance_);
 		instance_->serverHandle_->connect();
 	}

 	/*
	SUBSCRIBE /upnphost/udhisapi.dll?event=uuid:6a66eb21-7c9c-4699-a49d-f47752c5afd5+urn:upnp-org:serviceId:ConnectionManager HTTP/1.1
	NT: upnp:event
	CALLBACK: <http://10.36.0.189:50041/6a66eb21-7c9c-4699-a49d-f47752c5afd5/urn:upnp-org:serviceId:ConnectionManager>
	TIMEOUT: Second-1800
	User-Agent: Platinum/0.5.3.0, DLNADOC/1.50
	Host: 10.36.0.237:2869
 	*/

 	std::stringstream ss;

 	ss << "SUBSCRIBE " << eventUrl.c_str() << " HTTP/1.1\r\n";
 	ss << "HOST: " << host << ":" << port << "\r\n";
 	ss << "USER-AGENT: OS/version UPnP/1.1 product/version\r\n";
 	ss << "CALLBACK: " << "<http://10.36.0.166" << ":" << "2777/" << uuid << "/" << type << ">\r\n";
 	ss << "NT: upnp:event\r\n";
 	ss << "TIMEOUT: 500\r\n\r\n";

 	std::stringstream url;
 	url << host << ":" << port;

 	char bf[8024];
 	size_t len = sizeof(bf);
 	DiscoveryBase::socketSend(host.c_str(), atoi(port.c_str()), ss.str().c_str(), ss.str().length(), bf, &len);
 	bf[len] = 0;
 	//printf("eventServer() returned:\n%s\n", bf);
}

// Called when Socket Stream is opened.
void UPnPSearch::HNdidOpenStream(HNEventServerHandle* hServer)
{
}

// Called when |amountSent| bytes are sent.
void UPnPSearch::HNdidSendData(HNEventServerHandle* hServer, int amountSent)
{
	instance_->serverHandle_->receive();
}

// Called when data are received.
void UPnPSearch::HNdidReceiveData(HNEventServerHandle* hServer, const char *data, int dLen, const char *hostPort, int hpLen)
{
	char bf[8024];
	memcpy(bf, data, dLen);
	bf[dLen] = 0;

    //printf("GOT Event:\n%s\n", bf);

	/*
		NOTIFY /fd079abb-e638-46d4-bc74-5f62e453a95f/urn:schemas-upnp-org:service:ContentDirectory:1 HTTP/1.1
		Cache-Control: no-cache
		Connection: Close
		Pragma: no-cache
		Content-Type: text/xml; charset="utf-8"
		User-Agent: Microsoft-Windows/6.1 UPnP/1.0
		NT: upnp:event
		NTS: upnp:propchange
		SID: uuid:c0674b98-aea5-456c-8285-54c4ba0e1da1
		SEQ: 0
		Content-Length: 481
		Host: 10.36.0.170:8483

		<?xml version="1.0"?>
		<e:propertyset xmlns:e="urn:schemas-upnp-org:event-1-0">
			<e:property>
				<SystemUpdateID xmlns:dt="urn:schemas-microsoft-com:datatypes" dt:dt="ui4">0</SystemUpdateID>
			</e:property>
			<e:property>
				<ContainerUpdateIDs xmlns:dt="urn:schemas-microsoft-com:datatypes" dt:dt="string"></ContainerUpdateIDs>
			</e:property>
			<e:property><X_RemoteSharingEnabled xmlns:dt="urn:schemas-microsoft-com:datatypes" dt:dt="boolean">0</X_RemoteSharingEnabled></e:property>
		</e:propertyset>
	 */

 	std::string resp(data);
 	if (resp.find("NOTIFY ") == std::string::npos)
 	{
 		printf("-ERROR- UPnPSearch::didReceiveData() 'NOTIFY ' not found in response.\n");
 		return;
 	}

 	std::string notify = resp.substr(resp.find("NOTIFY ")+7);
 	notify = notify.substr(0,notify.find(" "));

 	std::string uuid = resp.substr(resp.find("/")+1);
 	uuid = uuid.substr(0, uuid.find("/"));

 	std::string type = resp.substr(resp.find(uuid)+uuid.length()+1);
 	type = type.substr(0, type.find(" "));


 	if (resp.find("<?xml")== std::string::npos)
 	{
 		printf("-ERROR- UPnPSearch::didReceiveData() No body found in response.\n");
 		return;
 	}
 	std::string body = resp.substr(resp.find("<?xml"));

 	//printf("Got Event(%s, %s)\n", uuid.c_str(), type.c_str());

 	navDsc_->sendEvent(uuid, type, body);
}

// Called when Socket Stream is closed.
void UPnPSearch::HNdidClose(HNEventServerHandle* hServer)
{
	instance_->serverHandle_ = NULL;
}

// Called when Socket Stream has an error.
void UPnPSearch::HNdidFail(HNEventServerHandle* hServer, HNEventError& err)
{
	//printf("UPnPSearch: didFail(): HNEventServerHandle error: %d\n", err.getErr());
	//navDsc_->onUPnPError(err.getErr());
}

bool UPnPSearch::removeDevice(std::map<std::string, UPnPDevMap>* devices, std::string uuid)
{
    bool found = false;

    for (std::map<std::string, UPnPDevMap>::iterator i = devices->begin(); i != devices->end(); i++)
    {
        UPnPDevMap dm = (*i).second;
        std::string type = (*i).first;
        for (std::map<std::string, UPnPDevice>::iterator k = dm.devMap.begin(); k != dm.devMap.end(); k++)
        {

            UPnPDevice dv = (*k).second;

            if (dv.uuid == uuid) {

                found = true;
                dm.devMap.erase((*k).first);
                (*devices)[type] = dm;
                if ((void*)devices == (void*)&internalDevs_ && api_) {
                    api_->serverListUpdate(type, &dm.devMap);
                }

                break;
            }

        }
        if (found) {
            break;
        }
    }

    return found;
}




}; // namespace WebCore

#endif // ENABLE_DISCOVERY
