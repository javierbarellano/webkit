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

#include "config.h"
#if ENABLE(DISCOVERY)

#include "NavigatorNetworkService.h"

#include "Frame.h"
#include "IDiscoveryAPI.h"
#include "NavDsc.h"
#include "NavDscCB.h"
#include "NavEvent.h"
#include "NavEventCB.h"
#include "NavigatorNetworkServiceErrorCallback.h"
#include "NavigatorNetworkServiceSuccessCallback.h"
#include "Navigator.h"
#include "UPnPDevice.h"
#include "UPnPSearch.h"
#include <wtf/MainThread.h>
#include <wtf/Threading.h>

namespace WebCore {

long NavigatorNetworkService::m_reqID = 0L;

NavigatorNetworkService::NavigatorNetworkService(Frame* frame)
    : DOMWindowProperty(frame)
{
}

NavigatorNetworkService::~NavigatorNetworkService()
{
}

// Static
const char* NavigatorNetworkService::supplementName()
{
    return "Nav";
}

// Static
NavigatorNetworkService* NavigatorNetworkService::from(Navigator* navigator)
{
    NavigatorNetworkService* supplement = static_cast<NavigatorNetworkService*>(Supplement<Navigator>::from(navigator, supplementName()));
    if (!supplement) {
        supplement = new NavigatorNetworkService(navigator->frame());
        provideTo(navigator, supplementName(), adoptPtr(supplement));
    }
    return supplement;
}

void NavigatorNetworkService::getNetworkServices(Navigator* navigator, const String& type,
    PassRefPtr<NavigatorNetworkServiceSuccessCallback> successcb, PassRefPtr<NavigatorNetworkServiceErrorCallback> errorcb)
{
    NavigatorNetworkService* nav = from(navigator);

    if (!nav->m_frame)
        return;

    WTF::CString cType(type.ascii());
    NavDsc* nd = NavDsc::create(nav->m_frame);

    // Get service type
    char sType[1000];
    bool reset;
    ProtocolType protoType = nav->readRemoveTypePrefix(cType, sType, &reset);
    std::string strType(sType);

    if (reset)
        NavDsc::disablePermissions();

    if (protoType != UPnPProtocol && protoType != ZCProtocol) {
        if (errorcb) {
            RefPtr<NavigatorNetworkServiceError> err = NavigatorNetworkServiceError::create(NavigatorNetworkServiceError::UNKNOWN_TYPE_PREFIX_ERR);
            errorcb->handleEvent(err.get());
            return;
        }
    }

    if (protoType == UPnPProtocol)
        nd->onUPnPDiscovery(sType, 0);

    else if (protoType == ZCProtocol)
        nd->onZCDiscovery(sType, 0);

    std::map<std::string, UPnPDevice> devs;
    std::map<std::string, ZCDevice> zcdevs;

    if (protoType == UPnPProtocol) {
        if (reset)
            nd->upnpReset();

        devs = nd->startUPnPDiscovery(sType, successcb, errorcb);

    } else if (protoType == ZCProtocol) {
        if (reset)
            nd->zcReset();

        zcdevs = nd->startZeroConfDiscovery(sType, successcb, errorcb);
    } else if (errorcb) {
        RefPtr<NavigatorNetworkServiceError> err = NavigatorNetworkServiceError::create(NavigatorNetworkServiceError::UNKNOWN_TYPE_PREFIX_ERR);
        errorcb->handleEvent(err.get());
    }
    // Error found and no error callback
}

NavigatorNetworkService::ProtocolType NavigatorNetworkService::readRemoveTypePrefix(WTF::CString &cType, char *sType, bool *reset)
{
    *reset = false;
    ProtocolType protoType = BadProtocol;
    if (!strncmp(cType.data(), "upnp:", 5))    {
        int start = 5;
        if (!strncmp(&cType.data()[5], "reset:", 6)) {
            start = 11;
            *reset = true;
        }
        strcpy(sType, &cType.data()[start]);
        protoType = UPnPProtocol;
    } else if (!strncmp(cType.data(), "zeroconf:", 9)) {
        int start = 9;
        if (!strncmp(&cType.data()[9], "reset:", 6)) {
            start = 15;
            *reset = true;
        }
        strcpy(sType, &cType.data()[start]);
        protoType = ZCProtocol;
    }

    return protoType;
}

};

#endif // DISCOVERY

