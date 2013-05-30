/*
 * UPnPSearch.h
 *
 *  Created on: Nov 25, 2011
 *      Author: gar
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

class NavDsc;

struct UPnPDevMap
{
    // key == UUID
    std::map<std::string, UPnPDevice> devMap;
};

class UPnPSearch : public DiscoveryBase
, public HNEventServerHandleClient
{

public:
    static std::map<std::string, UPnPDevice> discoverInternalDevs(const char *type, IDiscoveryAPI *);
    static std::map<std::string, UPnPDevice> discoverDevs(const char *type, NavDsc *);

    static UPnPSearch* getInstance();

    static UPnPSearch* create();

    void getUPnPFriendlyName(std::string uuid, std::string type, std::string& name);

    void checkForDroppedInternalDevs();

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

    void reset() {m_devLock->lock(); m_devs.clear(); m_internalDevs.clear(); m_devLock->unlock();}

    std::string m_sendData;

    static UPnPDevMap *getDevs(const char *type)
    {
        if (!m_instance)
            return 0;

        if (m_instance->m_devs.find(type) != m_instance->m_devs.end())
            return &(m_instance->m_devs.find(type)->second);

        return 0;
    }

protected:

    virtual bool parseDev(const char* resp, std::size_t respLen, const char* hostPort);

private:
    UPnPSearch(const char *type);

    static void createConnect(const char *type);

    std::string getLocalIp();

    bool notInBadList(std::string sUUid);

    bool isRegisteredType(const char* type, std::vector<std::string> &regType);
    bool isInternalType(const char* type);
    bool removeDevice(std::map<std::string, UPnPDevMap>* devices, std::string uuid);
    static UPnPSearch* m_instance;

    std::set<std::string> m_regTypes;

    // key == service type
    // dev key == UUID
    std::map<std::string, UPnPDevMap> m_devs;
    Mutex *m_devLock;


    std::vector<std::string> m_badDevs;

    long m_lastSend;

    bool m_sendPending;

    std::map<long, KURL> *m_tcpSocket;

    // Private API support
    IDiscoveryAPI *m_api;
    std::map<std::string, UPnPDevMap> m_internalDevs;
    std::string m_internalType;
};

} // namespace WebCore
#endif

#endif /* UPNPSEARCH_H_ */
