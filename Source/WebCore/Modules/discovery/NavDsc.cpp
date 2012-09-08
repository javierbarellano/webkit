
#include "config.h"

#if ENABLE(DISCOVERY)

#include "Page.h"
#include "Chrome.h"
#include "Frame.h"
#include "Document.h"
#include "FrameLoadRequest.h"
#include "FrameLoaderClient.h"
#include "ResourceHandle.h"

#include "Modules/discovery/IDiscoveryAPI.h"
#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/ZCDevice.h"
#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/ZCDevice.h"

#include "Modules/discovery/UPnPSearch.h"
#include "Modules/discovery/ZeroConf.h"

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <pthread.h>
#include <vector>
#include <map>

#include "Modules/discovery/Nav.h"
#include "NavDsc.h"

namespace WebCore {

NavDsc *NavDsc::instance = NULL;

// Static
NavDsc *NavDsc::create(Frame * frame)
{
	if (!instance)
		instance = new NavDsc(frame);

	return instance;
}

NavDsc::NavDsc(Frame * frame) :
		  m_frame(frame)
		, m_UPnPnav(NULL)
		, m_ZCnav(NULL)
{
}

NavDsc::~NavDsc()
{
	if (UPnPSearch::getInstance())
		delete UPnPSearch::getInstance();

	instance = NULL;
}

void NavDsc::foundUPnPDev(std::string type)
{
	if (m_UPnPnav)
		m_UPnPnav->UPnPDevAdded(type);
}
void NavDsc::foundZCDev(std::string type)
{
	if (m_ZCnav)
		m_ZCnav->ZCDevAdded(type);
}

void NavDsc::lostUPnPDev(std::string type)
{
	if (m_UPnPnav)
		m_UPnPnav->UPnPDevDropped(type);
}
void NavDsc::lostZCDev(std::string type)
{
	if (m_ZCnav)
		m_ZCnav->ZCDevDropped(type);
}

std::map<std::string, UPnPDevice> NavDsc::startUPnPInternalDiscovery(const char *type, IDiscoveryAPI *api)
{
	std::map<std::string, UPnPDevice> empty;
	std::map<std::string, UPnPDevice> devs = UPnPSearch::discoverInternalDevs(type, api);

	if (devs.size()==0)
		return devs;

	// We have devices to look at

	if (!m_frame)
		return empty;

	Page* page = m_frame->page();
	if (!page)
		return empty;

	for (std::map<std::string, UPnPDevice>::iterator it = devs.begin(); it != devs.end(); it++)
	{
		UPnPDevice d = (*it).second;
		d.isOkToUse = true;
		UPnPSearch::getInstance()->eventServer(type, d.eventURL, d.uuid, d.host, d.port);
		devs[(*it).first] = d;
	}


	return devs;

}



std::map<std::string, UPnPDevice> NavDsc::startUPnPDiscovery(const char *type)
{
	std::map<std::string, UPnPDevice> empty;
	std::map<std::string, UPnPDevice> devs = UPnPSearch::discoverDevs(type, this);

	if (devs.size()==0)
		return devs;

	// We have devices to look at

	if (!m_frame)
		return empty;

	Page* page = m_frame->page();
	if (!page)
		return empty;
	
	for (std::map<std::string, UPnPDevice>::iterator it = devs.begin(); it != devs.end(); it++)
	{
		UPnPDevice d = (*it).second;
		d.isOkToUse = true;
		UPnPSearch::getInstance()->eventServer(type, d.eventURL, d.uuid, d.host, d.port);
		devs[(*it).first] = d;
	}


// 	WebKit::ChromeClientImpl *cc = new WebKit::ChromeClientImpl(page->chrome()->client());
//
//	WTF::String msg("This web page wants to access the following servers:\n");
//	std::vector<WTF::String> cbtext;
//	int pos = 0;
//	int cbMask = 0;
//	for (std::map<std::string, UPnPDevice>::iterator it = devs.begin(); it != devs.end(); it++)
//	{
//		UPnPDevice d = (*it).second;
//		cbtext.push_back(WTF::String(d.friendlyName.c_str()));
//		cbMask |= ((d.isOkToUse ? 1:0)<<pos++);
//	}
//
//	bool ok = cc->runJavaScriptDiscovery(m_frame, msg, cbtext, &cbMask);
//
//	int i=0;
//	int count = 0;
//	for (std::map<std::string, UPnPDevice>::iterator it = devs.begin(); it != devs.end(); it++)
//	{
//		UPnPDevice d = (*it).second;
//		d.isOkToUse = ok && (cbMask & (1<<i))!=0;
//		i++;
//		if (d.isOkToUse)
//		{
//			UPnPSearch::getInstance()->eventServer(d.eventURL, d.uuid, d.host, d.port);
//			count++;
//		}
//		devs[(*it).first] = d;
//	}
//
	return devs;

}

std::map<std::string, ZCDevice> NavDsc::startZeroConfDiscovery(const char *type)
{
	std::map<std::string, ZCDevice> enpty;
	std::map<std::string, ZCDevice> devs = ZeroConf::discoverDevs(type, this);

	if (devs.size()==0)
		return devs;

	// We have devices to look at
	if (!m_frame)
		return enpty;

	Page* page = m_frame->page();
	if (!page)
		return enpty;

	for (std::map<std::string, ZCDevice>::iterator it = devs.begin(); it != devs.end(); it++)
	{
		ZCDevice d = (*it).second;
		d.isOkToUse = true;
		devs[(*it).first] = d;
	}
		
//	WebKit::ChromeClientImpl *cc = new WebKit::ChromeClientImpl(page->chrome()->client());
//
//	std::string smsg("This web page wants to access the following servers:\n");
//	std::map<std::string, ZCDevice>::iterator it = devs.begin();
//	for (; it != devs.end(); it++)
//	{
//		ZCDevice d = (*it).second;
//		smsg += d.friendlyName+"\n";
//	}
//	WTF::String msg(smsg.c_str());
//
//
//	std::vector<WTF::String> cbtext;
//	int pos = 0;
//	int cbMask = 0;
//	for (std::map<std::string, ZCDevice>::iterator it = devs.begin(); it != devs.end(); it++)
//	{
//		ZCDevice d = (*it).second;
//		cbtext.push_back(WTF::String(d.friendlyName.c_str()));
//		cbMask |= ((d.isOkToUse ? 1:0)<<pos++);
//	}
//
//	bool ok = cc->runJavaScriptDiscovery(m_frame, msg, cbtext, &cbMask);
//
//	int i=0;
//	int count = 0;
//	for (std::map<std::string, ZCDevice>::iterator it = devs.begin(); it != devs.end(); it++)
//	{
//		ZCDevice d = (*it).second;
//		d.isOkToUse = ok && (cbMask & (1<<i))!=0;
//		i++;
//		if (d.isOkToUse)
//			count++;
//		devs[(*it).first] = d;
//	}
//
//	printf("GB:startDiscovery(): ZeroConf devs: %u\n", count);
//
	return devs;

}

void NavDsc::sendEvent(std::string uuid, std::string stype, std::string body)
{
	if (m_UPnPnav)
	{
		//printf("NavDsc::sendEvent() uuid: %s\n", uuid.c_str());
		m_UPnPnav->sendEvent(uuid, stype, body);
	}
}

void NavDsc::onUPnPError(int error)
{
	if (m_UPnPnav)
	{
		printf("NavDsc::onUPnPError() error: %d\n", error);
		m_UPnPnav->onError(error);
	}
}

void NavDsc::onZCError(int error)
{
	if (m_ZCnav)
	{
		printf("NavDsc::onZCError() error: %d\n", error);
		m_ZCnav->onError(error);
	}
}


} // namespace WebCore

#endif // ENABLE_DISCOVERY
