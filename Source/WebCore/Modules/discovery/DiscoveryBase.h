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

#ifndef DiscoveryBase_h
#define DiscoveryBase_h

#include "HNEventServerHandle.h"
#include "UDPSocketHandleClient.h"
#include "soup/UDPSocketError.h"
#include "soup/UDPSocketHandle.h"
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace WebCore {

class IDiscoveryAPI;

class DiscoveryBase : public UDPSocketHandleClient {
public:
    DiscoveryBase();
    virtual ~DiscoveryBase();

    static void socketSend(const char* url, int port, const char* toSend, size_t sLen, char* bf, size_t *len);
    static void HTTPget(const char* host, int port, const char* path, char* bf, size_t *len);

    // White list imp
    virtual bool hostPortOk(const char* host, int port) = 0;

    virtual void getHostPort(const char *url, char* host, int *port);
    virtual void getPath(const char *url, char* path);

    bool canReceiveAnotherDev() { return m_canReceiveAnotherDev; }
    void resetCanReceiveAnotherDev() { m_canReceiveAnotherDev = false; }

    static void hexDump(const char* data, int len, int pos = 0);

    virtual bool networkIsUp() { return m_netIsUp; }

    std::string getElementValue(const char* buffer, const char* tag, char** ppos = 0);
    std::vector<std::string> getElementArray(const char* buffer, const char* tag, char** ppos = 0);

    UDPSocketHandle* m_socketHandle;
    HNEventServerHandle* m_serverHandle;

    bool m_stillRunning;
    bool m_droppedStillRunning;

    mutable RefPtr<UDPSocketHandle> m_udpSocket;
    mutable RefPtr<UDPSocketHandle> m_mcastSocket;

    bool m_stopDiscovery;
    bool m_threadDone;

    char m_url[1024];

    IDiscoveryAPI* m_navDsc;
    bool m_netIsUp;

protected:
    virtual bool parseDev(const char* resp, size_t respLen, const char* hostPort) = 0;

    std::map<std::string, std::string> parseUDPMessage(const char* data, int dLen);
    std::string getTokenValue(std::map<std::string, std::string>, std::string token);

    bool m_canReceiveAnotherDev;

    ThreadIdentifier m_tID;
    ThreadIdentifier m_tNotifyID;
    ThreadIdentifier m_tDroppedID;

};

} // namespace WebCore

#endif /* DiscoveryBase_h */
