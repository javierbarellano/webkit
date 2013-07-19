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

#ifndef ZeroConf_h
#define ZeroConf_h

#if ENABLE(DISCOVERY)

#include "DiscoveryBase.h"
#include "ZCDevice.h"
#include "soup/UDPSocketHandle.h"
#include <map>
#include <vector>

namespace WebCore {

class NavDsc;

typedef struct sZCDevMap {
    std::map<std::string, ZCDevice> devMap;
} ZCDevMap;

class ZeroConf : public DiscoveryBase {
public:
    static std::map<std::string, ZCDevice> discoverDevs(const char *type, NavDsc *);

    static ZeroConf* getInstance();

    static ZCDevMap* getDevs(const char *type);

    // Called when Socket Stream is opened.
    virtual void UDPdidOpenStream(UDPSocketHandle*);

    // Called when |amountSent| bytes are sent.
    virtual void UDPdidSendData(UDPSocketHandle*, int /* amountSent */);

    // Called when data are received.
    virtual void UDPdidReceiveData(UDPSocketHandle*, const char* data, int dLen, const char *hostPort, int hpLen);

    // Called when Socket Stream is closed.
    virtual void UDPdidClose(UDPSocketHandle*);

    // Called when Socket Stream has an error.
    virtual void UDPdidFail(UDPSocketHandle*, UDPSocketError&);

    void initQuery(const char *type);

    // White list imp
    virtual bool hostPortOk(const char* host, int port);

    void checkForDroppedDevs();

    void reset() { MutexLocker lock(m_devLock); m_devs.clear(); }

    char* m_query;
    int m_queryLen;

protected:
    virtual bool parseDev(const char* resp, size_t respLen, const char* hostPort);

private:
    static ZeroConf* m_instance;
    static char m_prefix[];
    static int m_prfixLen;

    static char m_postfix[];
    static int m_postfixLen;

    ZeroConf(const char *type);
    virtual ~ZeroConf();

    std::string m_daapType;

    // Return pos
    int parseRec(const char* resp, size_t respLen, int pos, int numRecs, std::string& name, std::string& portnm);

    // Return pos
    int cxSupport(const char* resp, int pos, std::string& subName, int curPos);

    // Return pos
    int setName(const char* resp, int pos, std::string& name);

    // key == service type
    // dev key == UUID
    std::map<std::string, ZCDevMap> m_devs;
    Mutex m_devLock;
    long m_lastSend;
    bool m_sendPending;

};

}
#endif
#endif /* ZeroConf_h */
