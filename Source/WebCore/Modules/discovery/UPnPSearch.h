/*
 * UPnPSearch.h
 *
 *  Created on: Nov 25, 2011
 *      Author: gar
 */

#ifndef UPNPSEARCH_H_
#define UPNPSEARCH_H_

#include <map>
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

typedef struct sUPnPDevMap
{
	// key == UUID
	std::map<std::string, UPnPDevice> devMap;
} UPnPDevMap;

class UPnPSearch : public DiscoveryBase
					,public HNEventServerHandleClient
{

public:
	static std::map<std::string, UPnPDevice> discoverInternalDevs(const char *type, IDiscoveryAPI *api);
	static std::map<std::string, UPnPDevice> discoverDevs(const char *type, NavDsc *navDsc);

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

	std::string sendData_;

protected:

    virtual bool parseDev(const char* resp, std::size_t respLen, const char* hostPort);

private:
	UPnPSearch(const char *type);

	static void createConnect(const char *type);

	std::string getLocalIp();
	
	bool notInBadList(std::string sUUid);

	bool isCurrentType(const char* type);
	bool isInternalType(const char* type);

	static UPnPSearch* instance_;

	// key == service type
	// dev key == UUID
	std::map<std::string, UPnPDevMap> devs_;

	std::vector<std::string> badDevs_;

	long lastSend_;

	bool sendPending_;
	
	std::map<long, KURL> *tcpSocket_;

	// Private API support
	IDiscoveryAPI *api_;
	std::map<std::string, UPnPDevMap> internalDevs_;
	std::string internal_type_;



};

} // namespace WebCore

#endif /* UPNPSEARCH_H_ */
