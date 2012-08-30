
#include "config.h"

#if ENABLE(DISCOVERY)

#define WEBKIT_IMPLEMENTATION 1

#include "../../../../Source/WebCore/platform/network/soup/TCPSocketHandle.h"

#include "../../../../Source/WebCore/platform/network/soup/UDPSocketError.h"
#include "../../../../Source/WebCore/platform/network/UDPSocketHandleClient.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketHandle.h"

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

#include <string>
#include <map>
#include <vector>

#include <stdlib.h>
#include <stdio.h>

namespace WebCore {

void droppedDevsThread(void *context)
{
	long lastSend = -1L;
	UPnPSearch* upnp = (UPnPSearch*)context;
	upnp->m_droppedStillRunning = true;
	
	while (upnp->m_droppedStillRunning)
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		long now_ms = (now.tv_sec*1000L + now.tv_usec/1000.0) + 0.5;

		if (lastSend < 0L || (now_ms - lastSend) > 25000)
		{
			lastSend = now_ms;
			upnp->checkForDroppedDevs();
		}

		usleep(1000L*1000L); // 1000 ms
	}
}

void discoveryThread(void *context)
{
	long lastSend = -1L;
	UPnPSearch* upnp = (UPnPSearch*)context;
	
	upnp->m_stillRunning = true;
	
	while (upnp->m_stillRunning) {
		struct timeval now;
		gettimeofday(&now, NULL);
		long now_ms = (now.tv_sec*1000L + now.tv_usec/1000.0) + 0.5;

		if (lastSend < 0L || (now_ms - lastSend) > 25000)
		{
			lastSend = now_ms;
			upnp->m_udpSocket->send(upnp->sendData_.c_str(), upnp->sendData_.length());
		}
		
		upnp->m_udpSocket->receive();
		usleep(100L*1000L); // 100 ms
	}
}

UPnPSearch* UPnPSearch::instance_ = NULL;

// static
UPnPSearch* UPnPSearch::getInstance()
{
	return instance_;
}

// static
UPnPSearch* UPnPSearch::create()
{
	createConnect(NULL);

	return instance_;
}


UPnPSearch::UPnPSearch(const char *type) :
		DiscoveryBase(type)
{
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
}

void UPnPSearch::closeServer()
{
	instance_->serverHandle_->close();
}

void UPnPSearch::getUPnPFriendlyName(std::string uuid, std::string type, std::string& name)
{
	if (devs_.find(type) != devs_.end())
	{
		UPnPDevMap dm = devs_[type];
		if (dm.devMap.find(uuid) != dm.devMap.end())
		{
			name.append(dm.devMap[uuid].friendlyName);
		}
	}

}

//static
void UPnPSearch::createConnect(const char *type)
{
	if (!instance_)
	{
		instance_ = new UPnPSearch(type);
		KURL url(ParsedURLString, String(instance_->url_));
		
		// Constructor connects to socket
		instance_->m_udpSocket = UDPSocketHandle::create(url, false, instance_) ;
		instance_->m_tID = WTF::createThread(discoveryThread, instance_, "UPnP_discovery");
		instance_->m_tDroppedID = WTF::createThread(droppedDevsThread, instance_, "DroppedUPnP");
	}

}

// static
std::map<std::string, UPnPDevice> UPnPSearch::discoverDevs(const char *type, NavDsc *navDsc)
{
	createConnect(type);
	instance_->cur_type_ = std::string(type);
	instance_->navDsc_ = navDsc;

	if (instance_->devs_.find(std::string(type)) != instance_->devs_.end())
	{
		UPnPDevMap dm = instance_->devs_.find(std::string(type))->second;
		return (dm.devMap);
	}

	return std::map<std::string, UPnPDevice>();
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
	char bf[8000];
	memcpy(bf, data, dLen);
	bf[dLen] = 0;

	if (dLen > 0)
		parseDev(bf, dLen, NULL);
}

void UPnPSearch::UDPdidClose(UDPSocketHandle* handle)
{
}

void UPnPSearch::UDPdidFail(UDPSocketHandle* udpHandle, UDPSocketError& error)
{
	printf("UPnPSearch::didFail() UDPSocketHandle error: %d\n", error.getErr());
	navDsc_->onUPnPError(error.getErr());
}

void UPnPSearch::checkForDroppedDevs()
{
	printf("checkForDroppedDevs() start\n");
	for (std::map<std::string, UPnPDevMap>::iterator i = devs_.begin(); i != devs_.end(); i++)
	{
		std::vector<std::string> dropMe;
		dropMe.clear();
		UPnPDevMap dm = (*i).second;
		std::string type = (*i).first;
		for (std::map<std::string, UPnPDevice>::iterator k = dm.devMap.begin(); k != dm.devMap.end(); k++)
		{
			UPnPDevice dv = (*k).second;
			if (dv.isOkToUse)
			{
				KURL url(ParsedURLString, String(dv.descURL.c_str()));
				char bf[8000];
				size_t len = sizeof(bf)-1;
				HTTPget(url, bf, &len);

				// Get Description of servcie to insure the device is still working
 				if (len > 0)
					continue;

				// Found one that dropped
 				dropMe.push_back((*k).first);
			}
		}

		if (dropMe.size())
		{
			for (int d=0; d<(int)dropMe.size(); d++)
				dm.devMap.erase(dm.devMap.find(dropMe.at(d)));

			devs_[type] = dm;
			navDsc_->lostUPnPDev(type);
		}
	}
}



// Called by CrossOriginAccessControl.cpp
// Check cross domain white list
bool UPnPSearch::hostPortOk(const char* host, int port)
{
	char lhost[1000];
	int lport;

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
			printf("hostPortOk(): host: %s, port: %d\n", lhost, lport);

			if (lport== port && !strcmp(host,lhost))
				return true;
		}
	}

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

	std::string sResp(resp);

	size_t loc = sResp.find("OCATION:");
	if (loc == std::string::npos)
		loc = sResp.find("ocation:");
	if (loc == std::string::npos)
	{
		return false;
	}

	size_t uuid = sResp.find("UUID:");
	if (uuid == std::string::npos)
		uuid = sResp.find("uuid:");
	if (uuid == std::string::npos)
	{
		return false;
	}

	loc += 8;
	if (sResp.at(loc)== ' ') loc++;

	std::string sLoc = sResp.substr(loc);
	size_t locEnd = sLoc.find_first_of('\r');
	sLoc = sLoc.substr(0,locEnd);

	std::string host = sLoc.substr(7);
	host = host.substr(0,host.find_first_of(':'));

	std::string port = sLoc.substr(7);
	port = port.substr(port.find_first_of(':')+1);
	port = port.substr(0,port.find_first_of('/'));

	uuid += 5;
	std::string sUuid = sResp.substr(uuid);
	size_t uuidEnd = sUuid.find_first_of(':');
	sUuid = sUuid.substr(0,uuidEnd);

	if (isbadDev(sUuid))
		return false;

	UPnPDevMap dm;
	if (devs_.find(cur_type_) != devs_.end())
		dm = devs_.find(cur_type_)->second;

	if (dm.devMap.find(sUuid) != dm.devMap.end())
	{
		return false;
	}

	printf("parseDev() - New Dev\n");

	// Now get the friendly name, ugh...
	KURL url(ParsedURLString, String(sLoc.c_str(), sLoc.length()));
	printf("parseDev() -After url\n");

	char bf[8000];
	size_t len = sizeof(bf)-1;
	HTTPget(url, bf, &len);
	
	bf[len] = 0;

	// Look for the service type that was given to us from JavaScript
	// Form is something like:
	//    urn:schemas-upnp-org:service:ContentDirectory:1
	if (!strstr(bf,cur_type_.c_str()))
	{
		badDevs_.push_back(sUuid);
		printf("%s has no %s service!!!!\n",url.host().ascii().data(), cur_type_.c_str());
		return false;
	}

	// Now look for something like: '<friendlyName>XPDESKTOP:</friendlyName>'
	if (!strstr(bf,"<friendlyName>"))
	{
		badDevs_.push_back(sUuid);
		printf("%s has no friendlyName!!!!\n",url.host().ascii().data());
		return false;
	}

	char friendlyName[512];
	char *start = strstr(bf,"<friendlyName>")+14;
	char *end = strstr(bf,"</friendlyName>");
	strncpy(friendlyName, start,  end - start);
	friendlyName[end - start] = 0;

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
	char eventUrl[2048]; eventUrl[0]=0;
	start = strstr(bf,"<serviceList>")+13;
	if (start)
	{
		char *end   = strstr(bf,"</serviceList>");
		char srvLst[4000];
		strncpy(srvLst, start, end - start);
		srvLst[end - start] = 0;

		char * srvEnd = srvLst;
		char * srv = strstr(srvEnd, "<service>")+9;
		srvEnd = strstr(srvEnd+10, "</service>");
		while (srv && srvEnd)
		{
			char srvBody[2048];
			strncpy(srvBody, srv, srvEnd - srv);
			srvBody[srvEnd - srv] = 0;

			if (strstr(srvBody, cur_type_.c_str()))
			{
				char *eUrl = strstr(srvBody, "<eventSubURL>")+13;
				char *eUrlEnd = strstr(srvBody, "</eventSubURL>");
				strncpy(eventUrl, eUrl, eUrlEnd - eUrl);
				eventUrl[eUrlEnd - eUrl] = 0;
			}

			srv = strstr(srvEnd, "<service>")+9;
			srvEnd = strstr(srvEnd+10, "</service>");
		}
	}


	UPnPDevice d;
	d.descURL = sLoc;
	d.friendlyName = std::string(friendlyName);
	d.eventURL = std::string(eventUrl);
	d.host = host;
	d.port = port;
	d.isOkToUse = true;
	d.uuid = sUuid;

	dm.devMap[sUuid] = d;
	devs_[cur_type_] = dm;

	printf("Adding device %s\n", d.friendlyName.c_str());

	navDsc_->foundUPnPDev(cur_type_);

	return true;
}


void UPnPSearch::eventServer(std::string eventUrl, std::string uuid, std::string host, std::string port)
{
// 	if (!strlen(eventUrl.c_str()))
// 		return;
// 
// 
// 	if (!instance_->serverHandle_)
// 	{
// 		WebKitPlatformSupport* wsp =  new WPS();
// 		instance_->serverHandle_ = wsp->createHNEventServerHandle();
// 		instance_->serverHandle_->set_client(instance_);
// 
// 		WebURL *url;
// 		setURL(eventUrl.c_str(), &url);
// 		instance_->serverHandle_->connect(*url, false);
// 	}
// 
// 	std::string sData;
// 
// 	sData.append("host: ");
// 	sData.append(host);
// 	sData.append("\n");
// 
// 	sData.append("port: ");
// 	sData.append(port);
// 	sData.append("\n");
// 
// 	sData.append("url: ");
// 	sData.append(eventUrl.c_str());
// 	sData.append("\n");
// 
// 	sData.append("uuid: ");
// 	sData.append(uuid);
// 	sData.append("\n");
// 
// 	sData.append("type: ");
// 	sData.append(cur_type_);
// 	sData.append("\n");
// 
// 	WebData wData(sData.c_str(), sData.length());
// 	instance_->serverHandle_->send(wData);
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
// 	std::string resp(data.data());
// 	if (resp.find("NOTIFY ") == std::string::npos)
// 	{
// 		//printf("-ERROR- UPnPSearch::didReceiveData() 'NOTIFY ' not found in response.\n");
// 		return;
// 	}
// 
// 	std::string notify = resp.substr(resp.find("NOTIFY ")+7);
// 	notify = notify.substr(0,notify.find(" "));
// 	std::string uuid = notify.substr(1);
// 	uuid = uuid.substr(0, uuid.find("/"));
// 	std::string stype = notify.substr(1);
// 	stype = stype.substr(stype.find("/")+1);
// 
// 	if (resp.find("<?xml")== std::string::npos)
// 	{
// 		//printf("-ERROR- UPnPSearch::didReceiveData() No body found in response.\n");
// 		return;
// 	}
// 	std::string body = resp.substr(resp.find("<?xml"));
// 
// 	navDsc_->sendEvent(uuid, stype, body);
// 	instance_->serverHandle_->receive();
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



}; // namespace WebCore

#endif // ENABLE_DISCOVERY
