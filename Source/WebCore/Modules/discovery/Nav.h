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

#ifndef Nav_h
#define Nav_h

#include "DOMWindowProperty.h"
#include "IDiscoveryAPI.h"
#include "NavDsc.h"
#include "NavEvent.h"
#include "NavServices.h"
#include "Supplementable.h"
#include "UPnPDevice.h"
#include "ZCDevice.h"
#include <map>
#include <queue>
#include <string>
#include <vector>
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
    enum ProtocolType {
        BadProtocol = 0,
        UPnPProtocol = 1,
        ZCProtocol = 2
    };
    static void getNetworkServices(
        Navigator*,
        const String& type,
        PassRefPtr<NavServiceOkCB>,
        PassRefPtr<NavServiceErrorCB>);

    // Implement IDiscoveryAPI interface
    virtual void onError(int) OVERRIDE { }
    virtual void onZCError(int) OVERRIDE { }
    virtual void UPnPDevAdded(std::string, UPnPDevice&) OVERRIDE { }
    virtual void ZCDevAdded(std::string, ZCDevice&) OVERRIDE { }
    virtual void UPnPDevDropped(std::string, UPnPDevice&) OVERRIDE { }
    virtual void ZCDevDropped(std::string, ZCDevice&) OVERRIDE { }
    virtual void sendEvent(std::string, std::string, std::string) OVERRIDE { }
    virtual void serverListUpdate(std::string, std::map<std::string, UPnPDevice>*) OVERRIDE { }
    virtual void receiveID(long idFromHN) { m_hnId = idFromHN; }

    // Events
    std::queue<std::string> m_currentType;
    int m_eventType;
    std::queue<RefPtr<NavEvent> > m_events;

    enum EventType {
        AddedUPnPEvent,
        DroppedUPnPEvent,
        SendEventUPnPEvent,
        AddedZCEvent,
        DroppedZCEvent,
    };

private:
    Nav(Frame*);
    ProtocolType readRemoveTypePrefix(WTF::CString &cType, char *sType, bool *reset);
    static const char* supplementName();

    Mutex m_mutex;
    static long m_reqID;
    long m_hnId;
    // Home Networking Data
};

}

#endif /* Nav_h */
