/* Copyright (C) 2013 Cable Television Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#if ENABLE(DISCOVERY)

#include "UPnPSearch.h"

#include "HNEventError.h"
#include "HNEventServerHandle.h"
#include "HNEventServerHandleClient.h"
#include "URL.h"
#include "NavDsc.h"
#include "UDPSocketHandleClient.h"
#include "soup/HNEventHandle.h"
#include "soup/TCPSocketHandle.h"
#include "soup/UDPSocketError.h"
#include "soup/UDPSocketHandle.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <vector>

#define LOGGING_NAV 1

#ifdef LOGGING_NAV
#define NAV_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define NAV_LOG(fmt, ...)
#endif
#define ERR_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)

#define MAX_CONTACT_ATTEMPTS 2
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

    while (upnp->m_droppedStillRunning) {
        struct timeval now;
        gettimeofday(&now, 0);

        long elapsedSeconds = now.tv_sec - lastSendSeconds;

        if (elapsedSeconds >= 5) {
            lastSendSeconds = now.tv_sec;
            upnp->checkForDroppedDevs();
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
        gettimeofday(&now, 0);

        long elapsedSeconds = now.tv_sec - lastSendSeconds;

        // Even though not required to, send a M-SEARCH set once per minute
        if (elapsedSeconds >= 60) {

            if (!upnp->m_udpSocket) {
                URL url(URL(), String(upnp->m_url));
                upnp->m_udpSocket = UDPSocketHandle::create(url, false, upnp);
                if (!upnp->m_udpSocket->connected()) {
                    upnp->m_udpSocket.release();
                    upnp->m_udpSocket = 0;
                }

            }

            if (upnp->m_udpSocket) {

                // Spec recommends more than one, but less than 10, in a 250ms window.
                // We'll send 3 repeats, 75ms apart.

				NAV_LOG("Issueing M-SEARCH (3x): \n%s\n", upnp->m_sendData.c_str());

                for (int i = 0; i < 3; i++) {
                    bool netIdUp = upnp->m_udpSocket->send(upnp->m_sendData.c_str(), upnp->m_sendData.length());
                    if (netIdUp != upnp->m_netIsUp)
                        upnp->m_netIsUp = netIdUp;

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
            URL url(URL(), String(upnp->m_url));
            upnp->m_mcastSocket = UDPSocketHandle::create(url, true, upnp);
            if (!upnp->m_mcastSocket->connected()) {
                upnp->m_mcastSocket.release();
                upnp->m_mcastSocket = 0;

                if (upnp->m_netIsUp)
                    upnp->m_netIsUp = false;

                usleep(20000L*1000L); // 20 secs
                continue;
            }

            if (!upnp->m_netIsUp)
                upnp->m_netIsUp = true;
        }

        upnp->m_mcastSocket->receive();
        usleep(1000L*1000L); // 1 sec
    }
}

UPnPSearch* UPnPSearch::m_instance = 0;

// static
UPnPSearch* UPnPSearch::getInstance()
{
    if (!m_instance)
        m_instance = new UPnPSearch(0);

    return m_instance;
}

// static
UPnPSearch* UPnPSearch::create()
{
    createConnect(0);

    return m_instance;
}


UPnPSearch::UPnPSearch(const char *type) :
DiscoveryBase()
{
    if (type)
        m_regTypes.insert(std::string(type));

    m_navDsc = 0;

    m_sendData = "M-SEARCH * HTTP/1.1\r\nST: upnp:rootdevice\r\nMX: 5\r\nMAN: \"ssdp:discover\"\r\nHOST: 239.255.255.250:1900\r\n\r\n";

    m_threadDone = true;
    m_devs.clear();
    m_lastSend = -1;
    m_sendPending = false;

    strcpy(m_url, "udp://239.255.255.250:1900");
}

UPnPSearch::~UPnPSearch()
{
    if (m_instance->m_serverHandle)
        closeServer();

    m_devs.clear();
    m_instance = 0;
}

void UPnPSearch::closeServer()
{
    m_instance->m_serverHandle->close();
}

void UPnPSearch::getUPnPFriendlyName(std::string uuid, std::string type, std::string& name)
{
    MutexLocker lock(m_devLock);
    if (m_devs.find(type) != m_devs.end()) {
        UPnPDevMap dm = m_devs[type];
        if (dm.devMap.find(uuid) != dm.devMap.end())
            name.append(dm.devMap[uuid].friendlyName);
        else
            ERR_LOG("getUPnPFriendlyName() UUID(%s) not found! dm.devMao.size=%d\n", uuid.c_str(), (int)dm.devMap.size());
    } else
        ERR_LOG("getUPnPFriendlyName() Type(%s) not found!\n", type.c_str());

}

// static
void UPnPSearch::createConnect(const char *type)
{

    if (!m_instance)
        m_instance = new UPnPSearch(type);

    m_instance->m_netIsUp = true;
    URL url(URL(), String(m_instance->m_url));

    if (!m_instance->m_udpSocket) {

        // Constructor connects to socket
        m_instance->m_udpSocket = UDPSocketHandle::create(url, false, m_instance);
        if (!m_instance->m_udpSocket->connected()) {
            ERR_LOG("createConnect() Not connected\n");
            m_instance->m_udpSocket.release();
            m_instance->m_udpSocket = 0;
            m_instance->m_netIsUp = false;
        }

        m_instance->m_tID = WTF::createThread(discoveryThread, m_instance, "UPnP_discovery");
        m_instance->m_tDroppedID = WTF::createThread(droppedDevsThread, m_instance, "DroppedUPnP");
    }

    if (!m_instance->m_mcastSocket) {

        // Constructor connects to socket
        m_instance->m_mcastSocket = UDPSocketHandle::create(url, true, m_instance);
        if (!m_instance->m_mcastSocket->connected()) {
            m_instance->m_mcastSocket.release();
            m_instance->m_mcastSocket = 0;
            m_instance->m_netIsUp = false;
        }

        m_instance->m_tNotifyID = WTF::createThread(notifyThread, m_instance, "UPnP_notify");
    }

}

// static
std::map<std::string, UPnPDevice> UPnPSearch::discoverDevs(const char *type, IDiscoveryAPI *navDsc)
{
    std::map<std::string, UPnPDevice> empty;
    createConnect(type);
    m_instance->m_navDsc = navDsc;

    MutexLocker lock(m_instance->m_devLock);
    m_instance->m_regTypes.insert(std::string(type));

    if (m_instance->m_devs.find(std::string(type)) != m_instance->m_devs.end()) {
        UPnPDevMap dm = m_instance->m_devs.find(std::string(type))->second;
        return (dm.devMap);
    }

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

        std::map<std::string, std::string> headers = parseUDPMessage(data, dLen);

        std::string usn = getTokenValue(headers, "USN");
        std::string location = getTokenValue(headers, "LOCATION");
        std::string st = getTokenValue(headers, "NTS");
        std::string type = getTokenValue(headers, "NT");

        std::string uuid;
        size_t pos = usn.find_first_of(':');
        if (pos != std::string::npos)
            uuid = usn.substr(pos+1);
        else
            return;

        std::string line1 = getTokenValue(headers, "LINE1");
        pos = line1.find("NOTIFY");

        if (pos != std::string::npos) {

            // This is a NOTIFY message

            // We are only interested in root device notifications.
            if (type == "upnp:rootdevice") {
                if (st == "ssdp:alive") {
                    NAV_LOG("Received NOTIFY:alive - %s: %s\n", location.c_str(), uuid.c_str());
                    parseDev(data, dLen, 0);
                } else if (st == "ssdp:byebye") {
                    NAV_LOG("Received NOTIFY:byebye: %s\n", uuid.c_str());
                    removeDevice(&m_devs, uuid);
                }
            }
        } else if (location.size() > 0) {
            // Not a NOTIFY. Must be an M_SEARCH reply
            parseDev(data, dLen, 0);
            NAV_LOG("Received M-SEARCH reply - %s: %s\n", location.c_str(), uuid.c_str());
        }
    }
}

void UPnPSearch::UDPdidClose(UDPSocketHandle* handle)
{
}

void UPnPSearch::UDPdidFail(UDPSocketHandle* udpHandle, UDPSocketError& error)
{
    ERR_LOG("UPnPSearch::didFail() UDPSocketHandle error: %d\n", error.getErr());
    if (m_navDsc)
        m_navDsc->onError(error.getErr());
}

void UPnPSearch::checkForDroppedDevs()
{
    // NAV_LOG("checkForDroppedDevs() start devs: %d\n", devs_.size());
    MutexLocker lock(m_devLock);
    for (std::map<std::string, UPnPDevMap>::iterator i = m_devs.begin(); i != m_devs.end(); i++) {
        std::vector<std::string> dropMe;
        dropMe.clear();
        UPnPDevMap dm = (*i).second;
        std::string type = (*i).first;
        for (std::map<std::string, UPnPDevice>::iterator k = dm.devMap.begin(); k != dm.devMap.end(); k++) {
            UPnPDevice dv = (*k).second;
            char host[1024];
            char path[1024];
            int port;
            getHostPort(dv.descURL.c_str(), host, &port);
            getPath(dv.descURL.c_str(), path);
            char bf[8000];
            bf[0] = 0;
            size_t len = sizeof(bf)-1;

            // Get Description of service to insure the device is still working
            HTTPget(host, port, path, bf, &len);

            std::string reply = bf;
            int pos = reply.find("HTTP/1.1 404");
            if (!len || pos != std::string::npos) {

                // Unresponsive. See note at top of file for reason for retries
                m_devs[type].devMap[dv.uuid].contactAttempts++;
                NAV_LOG("Timeout or 404 fetching device description. Attempt: %d  Url: %s, Reply: %s\n", dv.contactAttempts, dv.descURL.c_str(), bf);

                if (m_devs[type].devMap[dv.uuid].contactAttempts > MAX_CONTACT_ATTEMPTS) {
                    NAV_LOG("Device Off line... Url: %s, Reply: %s, navDsc: %p\n", dv.descURL.c_str(), bf, m_navDsc);
                    // Device was never going away - remove instead of marking offline.
                    /*
                    if (m_devs[type].devMap[dv.uuid].online) {
                        m_devs[type].devMap[dv.uuid].online = false;
                        if (m_navDsc)
                            m_navDsc->UPnPDevDropped(type, m_devs[type].devMap[dv.uuid]);
                    }
                    */
                    removeDevice(&m_devs, dv.uuid);
                }
            } else {
                m_devs[type].devMap[dv.uuid].contactAttempts = 0;
                if (!m_devs[type].devMap[dv.uuid].online) {
                    NAV_LOG("Device On line... Url: %s, navDsc: %p\n", dv.descURL.c_str(), m_navDsc);
                    m_devs[type].devMap[dv.uuid].online = true;
                    if (m_navDsc)
                        m_navDsc->UPnPDevAdded(type, m_devs[type].devMap[dv.uuid]);
                }
            }
        }

        if (dropMe.size()) {
            for (int d = 0; d < (int)dropMe.size(); d++)
                m_devs[type].devMap.find(dropMe.at(d))->second.online = true;
        }
    }
}

// Called by CrossOriginAccessControl.cpp
// Check cross domain white list
bool UPnPSearch::hostPortOk(const char* host, int port)
{
    char lhost[1000];
    int lport;

    MutexLocker lock(m_devLock);
    std::map<std::string, UPnPDevMap>::iterator it =  m_devs.begin();
    for (; it != m_devs.end(); it++) {
        UPnPDevMap dm = it->second;
        std::map<std::string, UPnPDevice>::iterator i = dm.devMap.begin();
        for (; i != dm.devMap.end(); i++) {
            UPnPDevice d = (*i).second;
            std::string url = d.descURL;

            getHostPort(url.c_str(), lhost, &lport);

            if (lport == port && !strcmp(host, lhost))
                return true;
            //NAV_LOG("hostPortOk(%s:%d) no match to: %s:%d\n", host, port, lhost, lport);
        }
    }

    return false;
}

bool UPnPSearch::parseDev(const char* resp, std::size_t respLen, const char* hostPort)
{
    // HTTP/1.1 200 OK
    // EXT:
    // SERVER: UPnP/1.0
    // ST: upnp:rootdevice
    // LOCATION: http://10.36.0.199:80/upnp/root_description.xml
    // CACHE-CONTROL: max-age=1800
    // USN: uuid:380D0BFB-2B2A-448C-99A8-0018DD018C56::upnp:rootdevice
    // Content-Length: 0

    std::map<std::string, std::string> headers = parseUDPMessage(resp, respLen);

    std::string sLoc = getTokenValue(headers, "LOCATION");
    if (sLoc.empty()) {
        // NAV_LOG("No Location header found!\n");
        return false;
    }

    std::string usn = getTokenValue(headers, "USN");
    if (usn.empty()) {
        ERR_LOG("parseDev(): No USN header found!\n");
        return false;
    }

    std::string sUuid;
    size_t pos = usn.find_first_of(':');
    if (pos != std::string::npos)
        sUuid = usn.substr(pos+1);
    else {
        ERR_LOG("parseDev(): USN didn't contain ':' USN: '%s'\n", usn.c_str());
        return false;
    }

    UPnPDevMap dm;

    // Now get the friendly name, ugh...
    std::string path;
    std::string port;
    pos = sLoc.find("://");
    if (pos == std::string::npos) {
        ERR_LOG("parseDev(): LOCATION: didn't contain '://' LOCATION: '%s'\n", sLoc.c_str());
        return false;
    }
    std::string host = sLoc.substr(pos+3);

    pos = host.find("/");
    if (pos == std::string::npos) {
        ERR_LOG("parseDev(): host: didn't contain '/' LOCATION: '%s'\n", host.c_str());
        return false;
    }
    path = host.substr(pos);

    pos = host.find(":");
    if (pos != host.npos) {
        port = host.substr(pos+1);
        pos = port.find("/");
        if (pos == std::string::npos) {
            ERR_LOG("parseDev(): port: didn't contain '/' port: '%s'\n", port.c_str());
            return false;
        }
        port = port.substr(0, pos);

        host = host.substr(0, host.find(":"));
    } else {
        port = "80";
        host = host.substr(0, host.find("/"));
    }

    char bf[8000];
    size_t len = sizeof(bf)-1;
    HTTPget(host.c_str(), atoi(port.c_str()), path.c_str(), bf, &len);

    if (!len) {
        NAV_LOG("Parse(): HTTPget(%s) len==0! Skipping...\n", sLoc.c_str());
        return false;
    }

    bf[len] = 0;
    std::vector<std::string> foundTypes;
    // NAV_LOG("Parse(%s): len=%d\n%s\n", sLoc.c_str(), (int)len, bf);

    // Look for the service type that was given to us from JavaScript
    // Form is something like:
    //    urn:schemas-upnp-org:service:ContentDirectory:1
    if (!isRegisteredType(bf, foundTypes)) {
        if (notInBadList(sUuid)) {
            m_badDevs.push_back(sUuid);
            // NAV_LOG("%s doesn't include a registered service.\n",host.c_str());
        }
        return false;
    }

    // Now look for something like: '<friendlyName>XPDESKTOP:</friendlyName>'
    std::string friendlyName = getElementValue(bf, (char*)"friendlyName");
    if (!friendlyName.length()) {
        m_badDevs.push_back(sUuid);
        ERR_LOG("%s has no friendlyName!!!!\n", host.c_str());
        return false;
    }

    // Now look for event url:
    /*
     *     <serviceList>
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
                    if (isRegisteredType(serviceType.c_str(), foundTypes))
                        eventUrl = getElementValue(service.c_str(), "eventSubURL");

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
    if (isRegisteredType(bf, foundTypes)) {
        for (int i = 0; i < foundTypes.size(); i++) {
            std::string foundType = foundTypes.at(i);

            MutexLocker lock(m_devLock);
            if (m_devs.find(foundType) == m_devs.end() || m_devs[foundType].devMap.find(sUuid) == m_devs[foundType].devMap.end()) {

                dm = m_devs[foundType];

                std::map<std::string, UPnPDevice>::iterator it;
                it = dm.devMap.find(sUuid);
                if (it != dm.devMap.end()) {
                    if (!dm.devMap[sUuid].changed(d))
                        continue;
                }

                dm.devMap[sUuid] = d;
                m_devs[foundType] = dm;

                NAV_LOG("device: %s : %s, %s.\n", host.c_str(), d.friendlyName.c_str(), sUuid.c_str());
                if (m_navDsc)
                    m_navDsc->UPnPDevAdded(foundType, m_devs[foundType].devMap[sUuid]);
            }
        }
    }

    return true;
}

// This is a temporary solution to facilitate discovery of versioned services. A client looking for serviceXYZ:1 should discover
// services with a higher version number, e.g. serviceXYZ:9 (but not vice versa).
// This should be refactored to parse the document instead of searching buffers for strings
bool serviceMatch(const char* buffer, const char* registeredType)
{
	// Sample service type: urn:schemas-upnp-org:device:RemoteUIServerDevice:1

	bool match = false;

	//NAV_LOG("serviceMatch for type: %s in buffer:\n%s\n", registeredType, buffer);

	// Separate the service type from the version (registered type)
	char *rootType = (char *) malloc(strlen(registeredType)+1);
	strcpy(rootType, registeredType);
	char* lastColon = strrchr(rootType,':');
	if (lastColon) {
		int registeredVersion = atoi(lastColon+1);
		*lastColon = 0;
		
		// !! This only searches for the first occurance of a service in the buffer, and will not find subsequent matches if the first is not qualified.
		char *deviceService = strstr((char*)buffer, rootType);
		if (deviceService) {
			// Check device service version. Must be greater or equal to our registered version.
			int deviceVersion = atoi(deviceService+strlen(rootType)+1);

			// printf(" - found root type: %s. registered version: %d, device version: %d\n", rootType, registeredVersion, deviceVersion);

			if (deviceVersion >= registeredVersion) {
				match = true;
			}
		}
	}

	free(rootType);

	return match;
}

bool UPnPSearch::isRegisteredType(const char* type, std::vector<std::string> &regType)
{
    MutexLocker lock(m_devLock);
    if (m_regTypes.size() > 0) {
        std::set<std::string>::iterator i = m_regTypes.begin();
        while (i != m_regTypes.end()) {
            //if (strstr(type, i->c_str()))
            if (serviceMatch(type, i->c_str()))
                regType.push_back(*i);

            i++;
        }
        return regType.size()>0;
    }

    return false;
}

bool UPnPSearch::notInBadList(std::string sUuid)
{
    for (int i = 0; i < (int)m_badDevs.size(); i++)
        if (!m_badDevs[i].compare(sUuid))
            return false;

    return true;
}


void UPnPSearch::eventServer(const char *type, std::string eventUrl, std::string uuid, std::string host, std::string port)
{
    if (!strlen(eventUrl.c_str()))
        return;


    if (!m_instance->m_serverHandle) {
        m_instance->m_serverHandle = new HNEventHandle(m_instance);
        m_instance->m_serverHandle->connect();
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
    //NAV_LOG("eventServer() returned:\n%s\n", bf);
}

// Called when Socket Stream is opened.
void UPnPSearch::HNdidOpenStream(HNEventServerHandle* hServer)
{
}

// Called when |amountSent| bytes are sent.
void UPnPSearch::HNdidSendData(HNEventServerHandle* hServer, int amountSent)
{
    m_instance->m_serverHandle->receive();
}

// Called when data are received.
void UPnPSearch::HNdidReceiveData(HNEventServerHandle* hServer, const char *data, int dLen, const char *hostPort, int hpLen)
{
    char bf[8024];
    memcpy(bf, data, dLen);
    bf[dLen] = 0;

    NAV_LOG("GOT Event:\n%s\n", bf);

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
    if (resp.find("NOTIFY ") == std::string::npos) {
        ERR_LOG("-ERROR- UPnPSearch::didReceiveData() 'NOTIFY ' not found in response.\n");
        return;
    }

    std::string notify = resp.substr(resp.find("NOTIFY ")+7);
    notify = notify.substr(0, notify.find(" "));

    std::string uuid = resp.substr(resp.find("/")+1);
    uuid = uuid.substr(0, uuid.find("/"));

    std::string type = resp.substr(resp.find(uuid)+uuid.length()+1);
    type = type.substr(0, type.find(" "));


    if (resp.find("<?xml") == std::string::npos) {
        ERR_LOG("-ERROR- UPnPSearch::didReceiveData() No body found in response.\n");
        return;
    }
    std::string body = resp.substr(resp.find("<?xml"));

    // NAV_LOG("Got Event(%s, %s)\n", uuid.c_str(), type.c_str());

    m_navDsc->sendEvent(uuid, type, body);
}

// Called when Socket Stream is closed.
void UPnPSearch::HNdidClose(HNEventServerHandle* hServer)
{
    m_instance->m_serverHandle = 0;
}

// Called when Socket Stream has an error.
void UPnPSearch::HNdidFail(HNEventServerHandle* hServer, HNEventError& err)
{
    // NAV_LOG("UPnPSearch: didFail(): HNEventServerHandle error: %d\n", err.getErr());
}

bool UPnPSearch::removeDevice(std::map<std::string, UPnPDevMap>* devices, std::string uuid)
{
    bool found = false;

    for (std::map<std::string, UPnPDevMap>::iterator i = devices->begin(); i != devices->end(); i++) {
        UPnPDevMap dm = (*i).second;
        std::string type = (*i).first;
        for (std::map<std::string, UPnPDevice>::iterator k = dm.devMap.begin(); k != dm.devMap.end(); k++) {

            UPnPDevice dv = (*k).second;

            if (dv.uuid == uuid) {

                found = true;
                dm.devMap.erase((*k).first);
                (*devices)[type] = dm;
                m_navDsc->serverListUpdate(type, &dm.devMap);

                break;
            }

        }
        if (found)
            break;
    }

    return found;
}




}; // namespace WebCore

#endif // ENABLE_DISCOVERY
