

#ifndef NavDsc_h
#define NavDsc_h

#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "Modules/discovery/IDiscoveryAPI.h"
#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/ZCDevice.h"

#include "Modules/discovery/UPnPSearch.h"
#include "Modules/discovery/ZeroConf.h"

namespace WebCore {

class Nav;
class IDiscoveryAPI;
class Frame;

class NavDsc
{
public:
	static FILE *HN_FD_; // Used for logging

	static NavDsc *create(Frame * frame);

	static NavDsc *getInstance() { return instance;}

	virtual ~NavDsc();

	std::map<std::string, UPnPDevice> startUPnPInternalDiscovery(const char *type, IDiscoveryAPI *api);


	std::map<std::string, UPnPDevice> startUPnPDiscovery(const char *type);
	std::map<std::string, ZCDevice> startZeroConfDiscovery(const char *type);

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

	void upnpReset() {UPnPSearch::getInstance()->reset();}
	void zcReset() {ZeroConf::getInstance()->reset();}

	void onUPnPError(int error);
	void onZCError(int error);

	void foundUPnPDev(std::string type);
	void foundZCDev(std::string type);

	void lostUPnPDev(std::string type);
	void lostZCDev(std::string type);

	void sendEvent(std::string uuid, std::string stype, std::string body);

private:
	pthread_t threadHandle;
	static NavDsc *instance;

	NavDsc(Frame * frame);

	Frame* m_frame;

	// map key:service type
	std::map<std::string, IDiscoveryAPI *> m_UPnPnav;
	std::map<std::string, IDiscoveryAPI *> m_ZCnav;

	// UUID, false == Don't use, true == ok to use
	std::map<std::string, bool> m_whiteBlackList;

};
} // namespace WebCore

#endif
