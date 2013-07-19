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

#include "DiscoveryWrapper.h"
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

enum ProtocolType
{
	BAD_PROTO = 0,
	UPNP_PROTO = 1,
	ZC_PROTO =2,
	EVENT_PROTO = 3,
};

typedef struct _EventData
{
	ProtocolType proto;
	bool online;
	std::string type;
	UPnPDevice dev;
	ZCDevice zcdev;
	RefPtr<NavEvent> evnt;
} EventData;

class NavDsc
    : public IDiscoveryAPI
{
public:
	static FILE *HN_FD_; // Used for logging

	static NavDsc *create(Frame * frame);

	static NavDsc *getInstance() {

		if (!instance)
			instance = new NavDsc(NULL);

		return instance;
	}

	virtual ~NavDsc();

	std::map<std::string, UPnPDevice> startUPnPDiscovery(
			const char *type,
			PassRefPtr<NavServiceOkCB> successcb,
			PassRefPtr<NavServiceErrorCB> errorcb);

	std::map<std::string, ZCDevice> startZeroConfDiscovery(
			const char *type,
			PassRefPtr<NavServiceOkCB> successcb,
			PassRefPtr<NavServiceErrorCB> errorcb);

	void onUPnPDiscovery(const char *type, IDiscoveryAPI *nav)
	{
		//printf("onUPnPDiscovery(): Setting m_UPnPnav...\n");
	    if (m_UPnPnav.find(std::string(type)) == m_UPnPnav.end())
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
        if (m_UPnPnav.find(std::string(type)) == m_UPnPnav.end())
            m_UPnPnav[std::string(type)] = nav;
	}

	void upnpReset() {m_resetSet = true; }
	void zcReset() {m_resetSet = true;  }

	void onError(int error);
	void onZCError(int error);

	void UPnPDevDropped(std::string type, UPnPDevice &dev);
    static void serviceOfflineInternal(void *ptr);
	void UPnPDevAdded(std::string type, UPnPDevice &dev);
    static void serviceOnlineInternal(void *ptr);

    static void dispatchServiceOnline(void *ptr);

	void ZCDevDropped(std::string type, ZCDevice &dev);
	void ZCDevAdded(std::string type, ZCDevice &dev);

	void sendEvent(std::string uuid, std::string stype, std::string body);
    static void sendEventInternal(void *ptr);

    virtual void serverListUpdate(std::string type, std::map<std::string, UPnPDevice> *devs);
    virtual void receiveID(long idFromHN){}


    std::vector<NavServices*> getNavServices(std::string type, bool isUp=true);

    static void disablePermissions() {m_permissionsEnabled = false;}

    Frame* getFrame() {return m_frame;}

    static bool permissionsEnabled() {return m_permissionsEnabled;}

    /* FIXME: This is a giant hack to keep WebKit from stripping DiscoveryWrapper
     * from the final library. The real fix probably involves putting it in the
     * public API.
     * See BitBucket issue #31:
     * https://bitbucket.org/ruihri/webkit/issue/31/discoverywrapper-is-in-the-private-api
     */
    void hack() { DiscoveryWrapper::startUPnPInternalDiscovery(NULL, NULL); }

private:
    bool has(std::vector<RefPtr<NavServices> > srvs, std::string uuid);

    void updateServices(std::string type, std::map<std::string, UPnPDevice> devs);
    void updateZCServices(std::string type, std::map<std::string, ZCDevice> devs);

    void setServices(
    		std::string strType,
    		const char* type,
    		std::map<std::string, UPnPDevice> devs,
    		std::map<std::string, ZCDevice> zcdevs,
    		ProtocolType protoType
    		);

    void createPermissionsDialog(EventData ed);

	pthread_t threadHandle;
	static NavDsc *instance;

	NavDsc(Frame * frame);

	// map key:service type
	std::map<std::string, IDiscoveryAPI *> m_UPnPnav;
	std::map<std::string, IDiscoveryAPI *> m_ZCnav;

	// UUID, false == Don't use, true == ok to use
	std::map<std::string, bool> m_whiteBlackList;

    // Events
	std::queue<EventData> m_eventData;
    Mutex m_main;

    bool m_resetSet;

    Mutex m_lockServices;
    // Map by type
    std::map<std::string, RefPtr<NavServices> > m_services;

    Frame* m_frame;
    static bool m_permissionsEnabled;

};
} // namespace WebCore

#endif
