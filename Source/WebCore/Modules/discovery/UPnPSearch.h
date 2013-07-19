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
#ifndef UPNPSEARCH_H_
#define UPNPSEARCH_H_

#if ENABLE(DISCOVERY)
#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "../../platform/KURL.h"

#ifdef LOG
#undef LOG
#endif

#include "../../../../Source/WebCore/platform/network/soup/UDPSocketHandle.h"
#include "../../../../Source/WebCore/platform/network/HNEventServerHandle.h"
#include "../../../../Source/WebCore/platform/network/HNEventServerHandleClient.h"

#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/DiscoveryBase.h"
#include "Modules/discovery/IDiscoveryAPI.h"

namespace WebCore
{

struct UPnPDevMap
{
    // key == UUID
    std::map<std::string, UPnPDevice> devMap;
};

class UPnPSearch : public DiscoveryBase
, public HNEventServerHandleClient
{

public:
    static std::map<std::string, UPnPDevice> discoverDevs(const char *type, IDiscoveryAPI *);

    static UPnPSearch* getInstance();

    static UPnPSearch* create();

    void getUPnPFriendlyName(std::string uuid, std::string type, std::string& name);

    //
    // UDPSocketHandleClient support
    //

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

    //
    // HNEventServer interface
    //
    // Called when Socket Stream is opened.
    virtual void HNdidOpenStream(HNEventServerHandle* hServer) ;

    // Called when |amountSent| bytes are sent.
    virtual void HNdidSendData(HNEventServerHandle* hServer, int /* amountSent */);

    // Called when data are received.
    virtual void HNdidReceiveData(HNEventServerHandle* hServer, const char *data, int dLen, const char *hostPort, int hpLen);

    // Called when Socket Stream is closed.
    virtual void HNdidClose(HNEventServerHandle* hServer);

    // Called when Socket Stream has an error.
    virtual void HNdidFail(HNEventServerHandle* hServer, HNEventError& err);



    virtual ~UPnPSearch();

    void eventServer(const char *type, std::string eventUrl, std::string uuid, std::string host, std::string port);

    void closeServer();

    virtual bool hostPortOk(const char* host, int port);

    void checkForDroppedDevs();

    void reset() {MutexLocker lock(m_devLock); m_devs.clear();}

    std::string m_sendData;

    static UPnPDevMap getDevs(const std::string &type)
    {
        UPnPDevMap pdm;

        if (!m_instance)
            return pdm;

        std::map<std::string, UPnPDevMap>::iterator i = m_instance->m_devs.find(type);
        if(i != m_instance->m_devs.end())
            pdm.devMap = i->second.devMap;
        return pdm;
    }

protected:

    virtual bool parseDev(const char* resp, std::size_t respLen, const char* hostPort);

private:
    UPnPSearch(const char *type);

    static void createConnect(const char *type);

    std::string getLocalIp();

    bool notInBadList(std::string sUUid);

    bool isRegisteredType(const char* type, std::vector<std::string> &regType);
    bool removeDevice(std::map<std::string, UPnPDevMap>* devices, std::string uuid);
    static UPnPSearch* m_instance;

    std::set<std::string> m_regTypes;

    // key == service type
    // dev key == UUID
    std::map<std::string, UPnPDevMap> m_devs;
    Mutex m_devLock;

    std::vector<std::string> m_badDevs;

    long m_lastSend;

    bool m_sendPending;
};

} // namespace WebCore
#endif

#endif /* UPNPSEARCH_H_ */
