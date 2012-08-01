

#ifndef NavDsc_h
#define NavDsc_h

#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/ZCDevice.h"

namespace WebCore {

class Navigator;
class Frame;

class NavDsc
{
public:
	static NavDsc *create(Frame * frame);

	static NavDsc *getInstance() { return instance;}

	virtual ~NavDsc();

	std::map<std::string, UPnPDevice> startUPnPDiscovery(const char *type);
	std::map<std::string, ZCDevice> startZeroConfDiscovery(const char *type);

	void onUPnPDiscovery(const char *type, Navigator *nav)
	{
		printf("Setting m_UPnPnav...\n");
		m_UPnPnav = nav;
		// Tell UPnPSearch to callback on dev change
	}
	void onZCDiscovery(const char *type, Navigator *nav)
	{
		m_ZCnav = nav;
		// Tell UPnPSearch to callback on dev change
	}

	void onUPnPEvent(const char *type, Navigator *nav)
	{
		m_UPnPnav = nav;
	}

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
	Navigator *m_UPnPnav;
	Navigator *m_ZCnav;

	// UUID, false == Don't use, true == ok to use
	std::map<std::string, bool> m_whiteBlackList;

};
} // namespace WebCore

#endif
