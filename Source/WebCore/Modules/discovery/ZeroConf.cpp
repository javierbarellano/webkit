/*
 * ZeroConf.cpp
 *
 *  Created on: Dec 27, 2011
 *      Author: gar
 */

#include "config.h"
#include "config.h"

#if ENABLE(DISCOVERY)

#define WEBKIT_IMPLEMENTATION 1

#include "../../../../Source/WebCore/platform/network/soup/TCPSocketHandle.h"

#include "../../../../Source/WebCore/platform/network/UDPSocketHandleClient.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketHandle.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketError.h"

//#include "base/bind.h"
#include "HTMLInputElement.h"

#include "NavDsc.h"
#include "ZeroConf.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/time.h>

#include <sstream>
#include <string>
#include <map>

#include <stdlib.h>
#include <stdio.h>

#define SKIP true
#define GET_NAME false

//#define LOGGING_NAV 1

#ifdef LOGGING_NAV
#define NAV_LOG(fmt,...) printf(fmt, ##__VA_ARGS__)
#else
#define NAV_LOG(fmt,...)
#endif
#define ERR_LOG(fmt,...) printf(fmt, ##__VA_ARGS__)

namespace WebCore
{

ZeroConf* ZeroConf::m_instance = 0;

char ZeroConf::m_prefix[] = {
    0x00, 0x00, // Transaction ID
    0x00, 0x00, // Flags (Standard query)
    0x00, 0x01, // Questions
    0x00, 0x00, // Answers RRs
    0x00, 0x00, // Authority RRs
    0x00, 0x00 // Additional RRs
};
int ZeroConf::m_prfixLen = 12;

char ZeroConf::m_postfix[] = {
    0x04, 0x5f, 0x74, 0x63, 0x70, // tcp
    0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, // local
    0x00,

    0x00, 0x0c, // Type: PTR (Domain name pointer)
    0x00, 0x01 // Class: IN, "QU" question: false (QM)
};
int ZeroConf::m_postfixLen = 16;


void zcDroppedDevsThread(void *context)
{
    long lastSend = -1L;
    ZeroConf* zc = (ZeroConf*)context;
    zc->m_droppedStillRunning = true;

    while (zc->m_droppedStillRunning) {
        struct timeval now;
        gettimeofday(&now, 0);
        long nowMs = (now.tv_sec * 1000L + now.tv_usec / 1000.0) + 0.5;

        if (lastSend < 0L || (nowMs - lastSend) > 25000) {
            lastSend = nowMs;
            zc->checkForDroppedDevs();
        }

        usleep(1000L*1000L); // 1000 ms
    }
}

void zcDiscoveryThread(void *context)
{
    long lastSend = -1L;
    ZeroConf* zc = (ZeroConf*)context;

    zc->m_stillRunning = true;

    while (zc->m_stillRunning) {
        struct timeval now;
        gettimeofday(&now, 0);
        long nowMs = (now.tv_sec * 1000L + now.tv_usec / 1000.0) + 0.5;

        if (lastSend < 0L || (nowMs - lastSend) > 20000) {
            if (!zc->m_udpSocket) {
                KURL url(ParsedURLString, String(zc->m_url));
                zc->m_udpSocket = UDPSocketHandle::create(url, true, zc);
                if (!zc->m_udpSocket->connected()) {
                    zc->m_udpSocket.release();
                    zc->m_udpSocket = 0;
                }
            }

            if (zc->m_udpSocket) {
                lastSend = nowMs;
                bool netIdUp = zc->m_udpSocket->send(zc->m_query, zc->m_queryLen);
                if (netIdUp != zc->m_netIsUp) {
                    zc->m_netIsUp = netIdUp;
                }
            }
        }

        zc->m_udpSocket->receive();
        usleep(100L*1000L); // 100 ms
    }
}

// static
std::map<std::string, ZCDevice> ZeroConf::discoverDevs(const char *type, NavDsc *navDsc)
{
    NAV_LOG("ZeroConf::discoverDevs(%s)\n", type);

    if (!m_instance)
        m_instance = new ZeroConf(type);

    if (!m_instance->m_udpSocket) {
        KURL url(ParsedURLString, String(m_instance->m_url));

        m_instance->m_udpSocket = UDPSocketHandle::create(url, true, m_instance);
        if (!m_instance->m_udpSocket->connected()) {
            m_instance->m_udpSocket.release();
            m_instance->m_udpSocket = 0;
            m_instance->m_netIsUp = false;
        }
        m_instance->m_tID = WTF::createThread(zcDiscoveryThread, m_instance, "ZC_discovery");
        m_instance->m_tDroppedID = WTF::createThread(zcDroppedDevsThread, m_instance, "DroppedZC");

        NAV_LOG("ZeroConf::discoverDevs(%s) UDP set up. URL: %s, Connected: %s\n",
            type, m_instance->m_url, m_instance->m_udpSocket->connected() ? "true":"false");
    }
    m_instance->m_navDsc = navDsc;


    m_instance->m_devLock->lock();
    if (m_instance->m_devs.find(std::string(type)) != m_instance->m_devs.end()) {
        ZCDevMap dm = m_instance->m_devs.find(std::string(type))->second;
        NAV_LOG("ZeroConf::discoverDevs(%s) Found one or more devs, size: %d\n", type, dm.devMap.size());
        m_instance->m_devLock->unlock();
        return (dm.devMap);
    }
    m_instance->m_devLock->unlock();

    return std::map<std::string, ZCDevice>();
}

// static
ZeroConf* ZeroConf::getInstance()
{
    if (!m_instance)
        m_instance = new ZeroConf(0);

    return m_instance;
}

ZeroConf::ZeroConf(const char *type) :
DiscoveryBase()
{
    m_devLock = new Mutex();
    m_devs.clear();

    strcpy(m_url, "udp://224.0.0.251:5353");

    std::string t(type ? type:"_daap");
    m_daapType = t;

    initQuery(t.c_str());
}

ZeroConf::~ZeroConf()
{
    m_devs.clear();
    delete[] m_query;
    delete m_devLock;
}


// Called when Socket Stream is opened.
void ZeroConf::UDPdidOpenStream(UDPSocketHandle* handle)
{
}


// Called when |amountSent| bytes are sent.
void ZeroConf::UDPdidSendData(UDPSocketHandle* handle, int amountSent)
{
}

// Called when data are received.
void ZeroConf::UDPdidReceiveData(UDPSocketHandle*, const char* data, int dLen, const char *hostPort, int hpLen)
{
    if (dLen > 0) {
        // hexDump(data, dLen);
        parseDev(data, dLen, hostPort);
    }
}

void ZeroConf::UDPdidClose(UDPSocketHandle* udpHandle)
{
}

void ZeroConf::UDPdidFail(UDPSocketHandle* handle, UDPSocketError& error)
{
    // NAV_LOG("ZeroConf::didFail() UDPSocketHandle error: %d\n", error.getErr());
    m_navDsc->onZCError(error.getErr());
}

ZCDevMap* ZeroConf::getDevs(const char *type)
{
    if (!m_instance)
        return 0;

    if (m_instance->m_devs.find(type) != m_instance->m_devs.end())
        return &(m_instance->m_devs.find(type)->second);

    return 0;
}

void ZeroConf::checkForDroppedDevs()
{
    m_devLock->lock();
    // NAV_LOG("checkForDroppedDevs() start\n");
    for (std::map<std::string, ZCDevMap>::iterator i = m_devs.begin(); i != m_devs.end(); i++) {
        ZCDevMap dm = (*i).second;
        std::string type = (*i).first;
        for (std::map<std::string, ZCDevice>::iterator k = dm.devMap.begin(); k != dm.devMap.end(); k++) {
            ZCDevice dv = (*k).second;
            char bf[8000];
            size_t len = sizeof(bf)-1;
            std::string host = dv.url.substr(7);
            host = host.substr(0, host.find(":"));
            unsigned port = atoi(dv.url.substr(dv.url.find(":", 7) + 1).c_str());
            HTTPget(host.c_str(), port, (char*)"/databases/1/containers", bf, &len);

            std::string path = dv.url+"/databases/1/containers";
            NAV_LOG("Try: %s\n", path.c_str());

            // Get Description of servcie to insure the device is still working
            if (len > 0 || strstr(bf, "HTTP/1.1 200 OK")) {
                if (!dv.online) {
                    dv.online = true;
                    NAV_LOG("Device ON line... Url: %s, navDsc: %p\n", dv.url.c_str(), m_navDsc);
                    if (m_navDsc)
                        m_navDsc->zcServiceOnline(type, dv);
                }
                continue;
            }

            // Found one that dropped
            if (dv.online) {
                dv.online = false;
                NAV_LOG("Device Off line... Url: %s, Reply: %s, navDsc: %p\n", dv.url.c_str(), bf, m_navDsc);
                if (m_navDsc)
                    m_navDsc->zcServiceOffline(type, dv);
            }
        }

    }
    m_devLock->unlock();

}

bool ZeroConf::parseDev(const char* resp, size_t respLen, const char* hostPort)
{
    ZCDevice zcd;

    NAV_LOG("parseDev(%s) start\n", hostPort);

    zcd.url = "http://";
    std::string hp(hostPort);
    if (hp.find(":") != std::string::npos)
        hp = hp.substr(0, hp.find(":"));
    zcd.url += hp;

    // the following is not used
    // int transID = ((int)(resp[0]&0xff)<<8) + (int)(resp[1]&0xff);

    int flags = ((int)(resp[2] & 0xff) << 8) + (int)(resp[3] & 0xff);

    int questions = ((int)(resp[4] & 0xff) << 8) + (int)(resp[5] & 0xff);
    int answers = ((int)(resp[6] & 0xff) << 8) + (int)(resp[7] & 0xff);
    int authorities = ((int)(resp[8] & 0xff) << 8) + (int)(resp[9] & 0xff);
    int additionals = ((int)(resp[10] & 0xff) << 8) + (int)(resp[11] & 0xff);

    if (!flags || !answers)
        return false;

    // NAV_LOG("parseDev() questions: %d, len: %d\n", questions, (int)respLen);
    int pos = 12;
    std::string dummy;
    std::string port;
    pos = parseRec(resp, respLen, pos, questions, dummy, port);

    NAV_LOG("parseDev() answers: %d, pos: %d, len: %d\n", answers, pos, (int)respLen);
    // Get friendly name
    pos = parseRec(resp, respLen, pos, answers, zcd.friendlyName, port);

    // NAV_LOG("parseDev() authorities: %d, pos: %d, len: %d\n", authorities, pos, (int)respLen);
    // Skip authorities
    pos = parseRec(resp, respLen, pos, authorities, dummy, port);

    NAV_LOG("parseDev() additionals: %d, pos: %d, len: %d\n", additionals, pos, (int)respLen);

    // Get port
    if (!port.length())
        pos = parseRec(resp, respLen, pos, additionals, dummy, port);

    if (port.length())
        zcd.url += ":"+port;

    NAV_LOG("ZC parseRec(): url: %s\n", zcd.url.c_str());

    ZCDevMap dm;
    m_devLock->lock();
    if (m_devs.find(m_daapType) != m_devs.end())
        dm = m_devs.find(m_daapType)->second;

    if (dm.devMap.find(zcd.friendlyName) == dm.devMap.end()) {
        zcd.online = true;
        dm.devMap[zcd.friendlyName] = zcd;
        m_devs[m_daapType] = dm;

        NAV_LOG("---Adding ZeroConf device: %s : %s, devs: %d\n", zcd.url.c_str(), zcd.friendlyName.c_str(), (int)dm.devMap.size());
        if (m_navDsc) {
            m_navDsc->zcServiceOnline(m_daapType, zcd);
        }
    }
    m_devLock->unlock();

    return true;
}

int ZeroConf::parseRec(const char* resp, size_t respLen, int pos, int numRecs, std::string& name, std::string& portnm)
{
    // NAV_LOG("parseRec() start - ");

    if (!respLen || !numRecs) {
        // NAV_LOG("end len==0\n");
        return pos;
    }

    NAV_LOG("parseRec(): numRecs: %d, len: 0x%x (%d)\n", numRecs, (int)respLen, (int)respLen);
    for (int p = 0; p < numRecs; p++) {
        NAV_LOG("Record[%d](%d) 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
            p, pos, resp[pos]&0xff, resp[pos+1]&0xff, resp[pos+2]&0xff, resp[pos+3]&0xff, resp[pos+4]&0xff, resp[pos+5]&0xff);

        // Rec name
        std::string recName;
        pos = setName(resp, pos, recName);

        int type = ((resp[pos]&0xff) << 8) + (resp[pos+1]&0xff);
        NAV_LOG(" Type: %d, pos: %d\n", type, pos);

        pos += 2; // Type
        pos += 2; // Class

        if (!resp[pos] && !resp[pos+1]) {

            int dataLen = 0;

            // time to live
            pos += 4;

            // Data
            dataLen = ((int)(resp[pos] & 0xff) << 8);
            pos++;
            dataLen += (int)(resp[pos] & 0xff);
            pos++;

            if (type == 12) { // PTR (Domain Name)
                setName(resp, pos, name);
                NAV_LOG("PTR (Domain Name): pos: %d\n", pos);
            } else if (type == 33) { // SRV (get port)
                // int priority = ((resp[pos]&0xff) << 8) + (resp[pos+1]&0xff);
                // int weight = ((resp[pos+2]&0xff) << 8) + (resp[pos+3]&0xff);
                int port = ((resp[pos+4] & 0xff) << 8) + (resp[pos+5] & 0xff);
                std::stringstream ss;
                ss << port;
                portnm = ss.str();
                NAV_LOG("SRV port: %s len: %d\n", portnm.c_str(), dataLen);
            }

            pos += dataLen;
        }

    }

    // NAV_LOG("end\n");
    return pos;
}

int ZeroConf::setName(const char* resp, int pos, std::string& name)
{
    bool c0Char = false;
    do {
        if (name.length()>0)
            name += ".";

        int curPos = pos;

        pos = cxSupport(resp, pos, name, curPos);
        c0Char = (curPos != pos);

        if (!c0Char) {
            int nameLen = (int)resp[pos++] & 0xff;
            std::string subName;
            for (int i = 0; i < nameLen; i++) {
                curPos = pos;
                pos = cxSupport(resp, pos, subName, curPos);

                if (curPos == pos)
                    subName += resp[pos++];
                else
                    i = i + pos - curPos - 1;
            }
            name += subName;
        }
    } while (resp[pos] && !c0Char);

    if (!c0Char)
        pos++;

    return pos;
}

int ZeroConf::cxSupport(const char* resp, int pos, std::string& subName, int curPos)
{
    if ((resp[pos] & (char)0xc0) == (char)0xc0) {
        if (curPos != pos)
            subName += ".";

        std::string indirectName;
        int inPos = (((int)resp[pos] & 0xf) << 8) + ((int)resp[pos+1] & 0xff);
        setName(resp, inPos, indirectName);
        subName += indirectName;
        pos += 2;
    }
    return pos;

}

void ZeroConf::initQuery(const char *type)
{
    std::string sType(type);
    m_queryLen = m_prfixLen + m_postfixLen + sType.length()+1;
    m_query = new char[m_queryLen];

    memcpy(m_query, m_prefix, m_prfixLen);
    m_query[m_prfixLen]=(char)sType.length();
    memcpy(&m_query[m_prfixLen+1],                    sType.c_str(), sType.length());
    memcpy(&m_query[m_prfixLen+1+sType.length()], m_postfix, m_postfixLen);
}

// Called by CrossOriginAccessControl.cpp
// Check cross domain white list
bool ZeroConf::hostPortOk(const char* host, int port)
{
    char lhost[1000];
    int lport;

    m_devLock->lock();
    std::map<std::string, ZCDevMap>::iterator it =  m_devs.begin();
    for (; it != m_devs.end(); it++) {
        ZCDevMap dm = it->second;
        std::map<std::string, ZCDevice>::iterator i = dm.devMap.begin();
        for (; i!= dm.devMap.end(); i++) {
            ZCDevice d = (*i).second;
            std::string url = d.url;

            getHostPort(url.c_str(), lhost, &lport);

            NAV_LOG("ZeroConf::hostPortOk(%s, %d) lhost: '%s', lport: %d\n",
                host, port, lhost, lport);

            if (lport == port && !strcmp(host, lhost)) {
                m_devLock->unlock();
                return true;
            }
        }
    }
    m_devLock->unlock();

    return false;
}

}  // Namespace

#endif // ENABLE_DISCOVERY

