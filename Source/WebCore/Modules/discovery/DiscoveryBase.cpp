/* Copyright (C) 2011, 2012, 2013 Cable Television Laboratories, Inc.
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

#define WEBKIT_IMPLEMENTATION 1

#include "DiscoveryBase.h"

#include "IDiscoveryAPI.h"
#include "NavDsc.h"
#include "UDPSocketHandleClient.h"
#include "soup/TCPSocketHandle.h"
#include "soup/UDPSocketError.h"
#include "soup/UDPSocketHandle.h"
#include <errno.h>
#include <fcntl.h>
#include <iomanip>
#include <map>
#include <sstream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <wtf/ASCIICType.h>

#define MAX_RCVBUF 8192
#define MAX_TAG 256

// #define LOGGING_NAV 1

#ifdef LOGGING_NAV
#define NAV_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define NAV_LOG(fmt, ...)
#endif
#define ERR_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)

// Utility methods. Should we find a better location for these?
// trim from start
static std::string& ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<std::string::value_type, bool>(isASCIISpace))));
    return s;
}

// trim from end
static std::string& rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<std::string::value_type, bool>(isASCIISpace))).base(), s.end());
    return s;
}

// trim from both ends
static std::string& trim(std::string &s)
{
    return ltrim(rtrim(s));
}

namespace WebCore {

DiscoveryBase::DiscoveryBase()
    : m_socketHandle(0)
    , m_serverHandle(0)
    , m_stillRunning(false)
    , m_droppedStillRunning(false)
    , m_udpSocket(0)
    , m_mcastSocket(0)
    , m_stopDiscovery(0)
    , m_threadDone(false)
    , m_navDsc(0)
    , m_netIsUp(false)
    , m_canReceiveAnotherDev(false)
    , m_tID(0)
    , m_tNotifyID(0)
    , m_tDroppedID(0)
{
}

DiscoveryBase::~DiscoveryBase()
{
    m_stopDiscovery = true;
    while (!m_threadDone)
        usleep(50000); // 50 ms
}

// extremely primitive xml element parser. Returns value for FIRST occurance of tag or empty string if not found.
std::string DiscoveryBase::getElementValue(const char* buffer, const char* tag, char** ppos)
{
    std::string result;

    if (strlen(tag) < (MAX_TAG-4)) {

        char beginTag[MAX_TAG];
        char endTag[MAX_TAG];

        sprintf(beginTag, "<%s>", tag);
        sprintf(endTag, "</%s>", tag);

        char parsedValue[8192];
        char* start = const_cast<char*>(strstr(buffer, beginTag));
        char* end = const_cast<char*>(strstr(buffer, endTag));

        if (start && end) {
            start += strlen(beginTag);
            if (ppos)
                *ppos = end + strlen(endTag);
            strncpy(parsedValue, start,  end - start);
            parsedValue[end - start] = 0;
            result = std::string(parsedValue);
            trim(result);
        }
    }

    return result;
}

// extremely primitive xml element parser. Returns array of matching elements
std::vector<std::string> DiscoveryBase::getElementArray(const char* buffer, const char* tag, char** ppos)
{
    std::vector<std::string> elements;

    char* bufptr = const_cast<char*>(buffer);

    while (true) {
        std::string elem = getElementValue(bufptr, tag, &bufptr);
        if (elem.length()) {
            elements.push_back(elem);
            if (ppos)
                *ppos = bufptr;
        } else
            break;
    }

    return elements;
}

void DiscoveryBase::getHostPort(const char *url, char* host, int *port)
{
    if (!url) {
        ERR_LOG("getHostPort() url is 0.\n");
        return;
    }

    NAV_LOG("getHostPort(%s)\n", url);
    int start = 0;
    if (strstr(url, "://"))
        start = strstr(url, "://") - url + 3;

    strcpy(host, &url[start]);
    int end = strstr(host, ":")-host;
    host[end] = 0;

    char sPort[100];
    strncpy(sPort, &host[end+1], 20);
    sPort[strlen(&host[end+1])] = 0;

    if (strchr(sPort, '/')) {
        end = strchr(sPort, '/') - sPort;
        sPort[end] = 0;
    }

    *port = atoi(sPort);
}

void DiscoveryBase::getPath(const char *url, char* path)
{
    char bURL[4096];

    strcpy(bURL, url);

    int start = 0;
    if (strstr(bURL, "://"))
        start = strstr(bURL, "://") - bURL + 3;
    char* host = &bURL[start];

    strcpy(path, &host[strchr(host, '/') - host]);
}

void DiscoveryBase::socketSend(const char *host, int port, const char *toSend, size_t sLen, char *bf, size_t *len)
{
    RefPtr<TCPSocketHandle> socket = TCPSocketHandle::create(host, port);

    if (socket->getError()) {
        *len = 0;
        return;
    }

    socket->send(toSend, sLen);
    socket->receive(bf, len);
}

void DiscoveryBase::HTTPget(const char *host, int port, const char *path, char *bf, size_t *len)
{
    /*
    GET /upnphost/udhisapi.dll?content=uuid:6a66eb21-7c9c-4699-a49d-f47752c5afd5 HTTP/1.1
    User-Agent: Platinum/0.5.3.0, DLNADOC/1.50
    Host: 10.36.0.237:2869
    Connection: keep-alive
    */

    std::stringstream toSend;
    toSend << "GET "<< path << " HTTP/1.1\r\n";
    toSend << "User-Agent: Platinum/0.5.3.0, DLNADOC/1.50\r\n";
    toSend << "Host: " << host << ":" << port << "\r\n";
    toSend << "Connection: keep-alive\r\n\r\n";

    socketSend(host, port, toSend.str().c_str(), toSend.str().length(), bf, len);
}

// static
void DiscoveryBase::hexDump(const char *data, int len, int pos)
{
    std::stringstream ss;
    int start = 0; // pos;
    int ln = len;
    const char* bf = data;
    for (int row = pos / 16; row < ln / 16; row++) {
        int off = row * 16;

        for (int x = 0; x < start; x++)
            ss << "   ";

        for (int x = start; x < start + 16 && (x + off) < ln; x++) {
            if (x == 8)
                ss << "  ";
            ss << std::setfill('0') << std::setw(sizeof(char) * 2) << std::hex <<
                ((int)bf[x + off] & 0xff) << " ";
        }

        ss << "    ";

        for (int x = start; x < start + 16 && (x + off) < ln; x++) {
            if (x == 8)
                ss << "  ";
            ss << (((bf[x + off] >= ' ' && bf[x + off] <= '~') ? bf[x + off] : '.'));
        }

        ss << "\n";
        start = 0;
    }

    NAV_LOG("%s", ss.str().c_str());
}

// Here to lookup a token in a map returned from parseUDPMessage
std::string DiscoveryBase::getTokenValue(std::map<std::string, std::string> map, std::string token)
{
    std::string value;
    std::map<std::string, std::string>::iterator iter = map.find(token);

    if (iter != map.end())
        value = (*iter).second;
    return value;
}

// Here to parse a received message into a dictionary of token/value pairs
std::map<std::string, std::string> DiscoveryBase::parseUDPMessage(const char *data, int dLen)
{
    char buffer[MAX_RCVBUF];
    // FIXME: Buffer overflow exploit possible?
    memcpy(buffer, data, dLen);
    buffer[dLen] = 0;

    std::map<std::string, std::string> map;

    int lineCount = 0;
    char* p = strtok(buffer, "\r\n");
    while (p) {
        std::string tokenValue = p;
        if (++lineCount == 1)
            map["LINE1"] = p;

        size_t pos = tokenValue.find_first_of(':');
        if (pos != std::string::npos) {

            std::string token = tokenValue.substr(0, pos);
            trim(token);
            std::transform(token.begin(), token.end(), token.begin(), toASCIIUpper<char>);

            std::string value = tokenValue.substr(pos + 1);
            trim(value);

            map[token] = value;
        }

        p = strtok(0, "\r\n");
    }

    return map;
}





}; // WebCore Namespace

#endif // ENABLE_DISCOVERY

