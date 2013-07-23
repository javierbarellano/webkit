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

#include "DiscoveryWrapper.h"
#include "IDiscoveryAPI.h"
#include "NavDscCB.h"
#include "NavEvent.h"
#include "NavEventCB.h"
#include "NavServiceError.h"
#include "NavServiceErrorCB.h"
#include "NavServiceOkCB.h"
#include "NavServices.h"
#include "UPnPDevice.h"
#include "UPnPSearch.h"
#include "ZCDevice.h"
#include "ZeroConf.h"
#include <map>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <wtf/ExportMacros.h>
#include <wtf/MainThread.h>
#include <wtf/Platform.h>
#include <wtf/Threading.h>

namespace WebCore {

class Nav;
class IDiscoveryAPI;
class Frame;

enum ProtocolType {
    BadProtocol = 0,
    UPnPProtocol = 1,
    ZeroConfProtocol =2,
    EventProtocol = 3,
};

typedef struct {
    ProtocolType proto;
    bool online;
    std::string type;
    UPnPDevice dev;
    ZCDevice zcdev;
    RefPtr<NavEvent> evnt;
} EventData;

class NavDsc : public IDiscoveryAPI {
public:
    static NavDsc *create(Frame*);

    static NavDsc *getInstance()
    {
        if (!instance)
            instance = new NavDsc(0);

        return instance;
    }

    virtual ~NavDsc();

    std::map<std::string, UPnPDevice> startUPnPDiscovery(const char *type,
        PassRefPtr<NavServiceOkCB> successcb, PassRefPtr<NavServiceErrorCB> errorcb);

    std::map<std::string, ZCDevice> startZeroConfDiscovery(const char *type,
        PassRefPtr<NavServiceOkCB> successcb, PassRefPtr<NavServiceErrorCB> errorcb);

    void onUPnPDiscovery(const char *type, IDiscoveryAPI *nav)
    {
        if (m_UPnPNav.find(std::string(type)) == m_UPnPNav.end())
            m_UPnPNav[std::string(type)] = nav;
        // m_UPnPnav = nav;
        // Tell UPnPSearch to callback on dev change
    }

    void onZCDiscovery(const char *type, IDiscoveryAPI *nav)
    {
        m_ZCNav[std::string(type)] = nav;
        // m_ZCnav = nav;
        // Tell UPnPSearch to callback on dev change
    }

    void onUPnPEvent(const char *type, IDiscoveryAPI *nav)
    {
        if (m_UPnPNav.find(std::string(type)) == m_UPnPNav.end())
            m_UPnPNav[std::string(type)] = nav;
    }

    void upnpReset() { m_resetSet = true; }
    void zcReset() { m_resetSet = true;  }

    void onError(int error);
    void onZCError(int error);

    void UPnPDevDropped(std::string type, UPnPDevice&);
    static void serviceOfflineInternal(void *ptr);
    void UPnPDevAdded(std::string type, UPnPDevice&);
    static void serviceOnlineInternal(void *ptr);

    static void dispatchServiceOnline(void *ptr);

    void ZCDevDropped(std::string type, ZCDevice&);
    void ZCDevAdded(std::string type, ZCDevice&);

    void sendEvent(std::string uuid, std::string stype, std::string body);
    static void sendEventInternal(void *ptr);

    virtual void serverListUpdate(std::string type, std::map<std::string, UPnPDevice>*);
    virtual void receiveID(long idFromHN) { }

    std::vector<NavServices*> getNavServices(std::string type, bool isUp = true);

    static void disablePermissions() { m_permissionsEnabled = false; }

    Frame* getFrame() { return m_frame; }

    static bool permissionsEnabled() {return m_permissionsEnabled;}

    /* FIXME: This is a giant hack to keep WebKit from stripping DiscoveryWrapper
     * from the final library. The real fix probably involves putting it in the
     * public API.
     * See BitBucket issue #31:
     * https://bitbucket.org/ruihri/webkit/issue/31/discoverywrapper-is-in-the-private-api
     */
    void hack() { DiscoveryWrapper::startUPnPInternalDiscovery(0, 0); }

private:
    bool has(std::vector<RefPtr<NavServices> > srvs, std::string uuid);

    void updateServices(std::string type, std::map<std::string, UPnPDevice> devs);
    void updateZCServices(std::string type, std::map<std::string, ZCDevice> devs);

    void setServices(std::string typeStr, const char* type, std::map<std::string, UPnPDevice> devs,
        std::map<std::string, ZCDevice> zcdevs, ProtocolType);

    void createPermissionsDialog(EventData);

    pthread_t threadHandle;
    static NavDsc *instance;

    NavDsc(Frame*);

    // map key:service type
    std::map<std::string, IDiscoveryAPI *> m_UPnPNav;
    std::map<std::string, IDiscoveryAPI *> m_ZCNav;

    // UUID, false == Don't use, true == ok to use
    std::map<std::string, bool> m_whiteBlackList;

    // Events
    std::queue<EventData> m_eventData;
    Mutex m_mainMutex;
    bool m_resetSet;
    Mutex m_lockServices;
    // Map by type
    std::map<std::string, RefPtr<NavServices> > m_services;

    Frame* m_frame;
    static bool m_permissionsEnabled;
};
} // namespace WebCore

#endif
