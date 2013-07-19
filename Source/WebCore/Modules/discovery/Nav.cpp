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

#include "Frame.h"
#include "Navigator.h"
#include "Modules/discovery/IDiscoveryAPI.h"
#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/UPnPSearch.h"
#include "Modules/discovery/NavDsc.h"
#include "Modules/discovery/UPnPEvent.h"

#include "NavServiceError.h"
#include "NavServiceErrorCB.h"
#include "NavServiceOkCB.h"
#include "NavDscCB.h"
#include "NavEvent.h"
#include "NavEventCB.h"
#include <wtf/Threading.h>
#include <wtf/MainThread.h>

#include "Nav.h"

namespace WebCore {

long Nav::m_reqID = 0L;

Nav::Nav(Frame* frame)
: DOMWindowProperty(frame) {

	m_main = new Mutex();
}

Nav::~Nav() {
	delete m_main;
	m_main = NULL;
}

// Static
const char* Nav::supplementName()
{
    return "Nav";
}



// Static
Nav* Nav::from(Navigator* navigator)
{
    Nav* supplement = static_cast<Nav*>(Supplement<Navigator>::from(navigator, supplementName()));
    if (!supplement) {
        supplement = new Nav(navigator->frame());
        provideTo(navigator, supplementName(), adoptPtr(supplement));
    }
    return supplement;
}

void Nav::getNetworkServices(
		Navigator* n,
		const String& type,
		PassRefPtr<NavServiceOkCB> successcb,
		PassRefPtr<NavServiceErrorCB> errorcb)
{
	//printf("Nav::getNetworkServices() Start\n");

	Nav *nv = from(n);

	if (!nv->m_frame)
		return;

	WTF::CString cType(type.ascii());
	NavDsc *nd = NavDsc::create(nv->m_frame);

	// Get service type
	char sType[1000];
	bool reset;
	ProtocolType protoType = nv->readRemoveTypePrefix(cType, sType, &reset);
	std::string strType(sType);

	if (reset)
	    NavDsc::disablePermissions();

	if (protoType != UPNP_PROTO && protoType != ZC_PROTO) {
		if (errorcb)
		{
			RefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::UNKNOWN_TYPE);
			errorcb->handleEvent(err.get());
			return;
		}
	}

	if (protoType == UPNP_PROTO)
		nd->onUPnPDiscovery(sType, NULL);

	else if (protoType == ZC_PROTO)
		nd->onZCDiscovery(sType, NULL);

	std::map<std::string, UPnPDevice> devs;
	std::map<std::string, ZCDevice> zcdevs;

	if (protoType == UPNP_PROTO) {
		if (reset)
			nd->upnpReset();

		devs = nd->startUPnPDiscovery(sType, successcb, errorcb);

	} else if (protoType == ZC_PROTO) {
		if (reset)
			nd->zcReset();

		zcdevs = nd->startZeroConfDiscovery(sType, successcb, errorcb);
	} else if (errorcb) {
		RefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::UNKNOWN_TYPE);
		errorcb->handleEvent(err.get());
		return;
	}
	else
		return; // Error found and no error callback
}


Nav::ProtocolType Nav::readRemoveTypePrefix(WTF::CString &cType, char *sType, bool *reset)
{
	*reset = false;
	ProtocolType protoType = BAD_PROTO;
	if (!strncmp(cType.data(), "upnp:", 5))	{
		int start = 5;
		if (!strncmp(&cType.data()[5], "reset:", 6)) {
			start = 11;
			*reset = true;
		}
		strcpy(sType, &cType.data()[start]);
		protoType = UPNP_PROTO;
	}
	else if (!strncmp(cType.data(), "zeroconf:", 9)) {
		int start = 9;
		if (!strncmp(&cType.data()[9], "reset:", 6)) {
			start = 15;
			*reset = true;
		}
		strcpy(sType, &cType.data()[start]);
		protoType = ZC_PROTO;
	}

	return protoType;
}

};

#endif // DISCOVERY

