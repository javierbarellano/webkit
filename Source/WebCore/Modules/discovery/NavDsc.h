

#ifndef NavDsc_h
#define NavDsc_h

#include <map>
#include <vector>
#include <queue>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "wtf/ExportMacros.h"
#include "wtf/Platform.h"

#include "Modules/discovery/IDiscoveryAPI.h"
#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/ZCDevice.h"

#include "Modules/discovery/UPnPSearch.h"
#include "Modules/discovery/ZeroConf.h"

#include "Modules/discovery/NavServices.h"

#include "NavServiceError.h"
#include "NavServiceErrorCB.h"
#include "NavServiceOkCB.h"
#include "NavDscCB.h"
#include "NavEvent.h"
#include "NavEventCB.h"
#include <wtf/Threading.h>
#include <wtf/MainThread.h>

namespace WebCore {

class Nav;
class IDiscoveryAPI;
class Frame;

class NavDsc
{
public:
	enum ProtocolType
	{
		BAD_PROTO = 0,
		UPNP_PROTO = 1,
		ZC_PROTO =2
	};

	static FILE *HN_FD_; // Used for logging

	static NavDsc *create(Frame * frame);

	static NavDsc *getInstance() {

		if (!instance)
			instance = new NavDsc(NULL);

		return instance;
	}

	virtual ~NavDsc();

    void startUPnPInternalDiscovery(const char *type, IDiscoveryAPI *api);


	std::map<std::string, UPnPDevice> startUPnPDiscovery(const char *type, PassRefPtr<NavServiceOkCB> successcb);
	std::map<std::string, ZCDevice> startZeroConfDiscovery(const char *type, PassRefPtr<NavServiceOkCB> successcb);

	void onUPnPDiscovery(const char *type, IDiscoveryAPI *nav)
	{
		//printf("onUPnPDiscovery(): Setting m_UPnPnav...\n");
		m_UPnPnav[std::string(type)] = nav;
		//m_UPnPnav = nav;
		// Tell UPnPSearch to callback on dev change
	}
	void onZCDiscovery(const char *type, IDiscoveryAPI *nav)
	{
		//printf("onZCDiscovery(): Setting m_ZCnav...\n");
		m_ZCnav[std::string(type)] = nav;
		//m_ZCnav = nav;
		// Tell UPnPSearch to callback on dev change
	}

	void onUPnPEvent(const char *type, IDiscoveryAPI *nav)
	{
		//printf("onUPnPEvent(): Setting m_UPnPnav...\n");
		m_UPnPnav[std::string(type)] = nav;
	}

	void upnpReset() {m_resetSet = true;}
	void zcReset() {m_resetSet = true;}

	void onUPnPError(int error);
	void onZCError(int error);

	void foundUPnPDev(std::string type);
    static void UPnPDevAddedInternal(void *ptr);
	void foundZCDev(std::string type);
    static void ZCDevAddedInternal(void *ptr);

	void lostUPnPDev(std::string type);
    static void UPnPDevDroppedInternal(void *ptr);
	void lostZCDev(std::string type);
    static void ZCDevDroppedInternal(void *ptr);

	void sendEvent(std::string uuid, std::string stype, std::string body);
    static void sendEventInternal(void *ptr);

private:
    std::vector<NavServices*> getNavServices(std::string type);

    bool has(std::vector<RefPtr<NavServices> > srvs, std::string uuid);

    void setServices(
    		std::string strType,
    		const char* type,
    		std::map<std::string, UPnPDevice> devs,
    		std::map<std::string, ZCDevice> zcdevs,
    		ProtocolType protoType
    		);

	pthread_t threadHandle;
	static NavDsc *instance;

	NavDsc(Frame * frame);

	Frame* m_frame;

	// map key:service type
	std::map<std::string, IDiscoveryAPI *> m_UPnPnav;
	std::map<std::string, IDiscoveryAPI *> m_ZCnav;

	// UUID, false == Don't use, true == ok to use
	std::map<std::string, bool> m_whiteBlackList;

    // Events
	std::queue<std::string> m_curType;
	int m_eventType;
	std::queue<RefPtr<NavEvent> > m_event;

	enum EventType {
		ADDED_UPNP_EVENT,
		DROPPED_UPNP_EVENT,
		SENDEVENT_UPNP_EVENT,
		ADDED_ZC_EVENT,
		DROPPED_ZC_EVENT,
	};

    Mutex *m_main;

    bool m_resetSet;

    // Map by type
    std::map<std::string, std::vector<RefPtr<NavServices> > > m_services;

};
} // namespace WebCore

#endif
