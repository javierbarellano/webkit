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

#ifndef NAV_H_
#define NAV_H_

#include <string>
#include <map>
#include <vector>
#include <queue>

#include "Modules/discovery/IDiscoveryAPI.h"
#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/ZCDevice.h"
#include "Modules/discovery/NavDsc.h"
#include "Modules/discovery/NavServices.h"
#include "Modules/discovery/NavEvent.h"

#include "DOMWindowProperty.h"
#include "Supplementable.h"

#include <wtf/Forward.h>
#include <wtf/HashMap.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>


namespace WebCore {

class DOMMimeTypeArray;
class DOMPluginArray;
class Frame;
class PointerLock;
class PluginData;

// Home Networking
class NavDscCB;
class NavServiceErrorCB;
class NavServiceOkCB;
class NavEventCB;
class Frame;
class Navigator;

typedef int ExceptionCode;

class Nav : public Supplement<Navigator>, public DOMWindowProperty, public IDiscoveryAPI {
public:
    virtual ~Nav();

    static Nav* from(Navigator*);

public:
	enum ProtocolType
	{
		BAD_PROTO = 0,
		UPNP_PROTO = 1,
		ZC_PROTO =2
	};
    static void getNetworkServices(
			Navigator* n,
			const String& type,
			PassRefPtr<NavServiceOkCB> serviceCB,
			PassRefPtr<NavServiceErrorCB> errorCB );


    // Implement IDiscoveryAPI interface
    virtual void onError(int error){}
    virtual void onZCError(int error){}
    virtual void UPnPDevAdded(std::string type, UPnPDevice &dev){}
    virtual void ZCDevAdded(std::string type, ZCDevice &dev){}
    virtual void UPnPDevDropped(std::string type, UPnPDevice &dev){}
    virtual void ZCDevDropped(std::string type, ZCDevice &dev){}
    virtual void sendEvent(std::string uuid, std::string stype, std::string body){}
    virtual void serverListUpdate(std::string type, std::map<std::string, UPnPDevice> *devs){}
    virtual void receiveID(long idFromHN) { m_hn_id = idFromHN; }

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

private:
    Nav(Frame*);


    ProtocolType readRemoveTypePrefix(WTF::CString &cType, char *sType, bool *reset);

    static const char* supplementName();

    Mutex *m_main;

    static long m_reqID;

    long m_hn_id;

    // Home Networking Data

};

}

#endif /* NAV_H_ */
