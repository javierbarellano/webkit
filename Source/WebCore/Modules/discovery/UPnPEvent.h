/* Copyright (C) 2012, 2013 Cable Television Laboratories, Inc.
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

#ifndef UPnPEvent_h
#define UPnPEvent_h

#include "Event.h"

namespace WebCore {

struct UPnPEventInit : public EventInit {

    UPnPEventInit()
        : propertyset()
        , uuid()
        , friendlyName()
        , serviceType()
    {
    }

    String propertyset;
    String uuid;
    String friendlyName;
    String serviceType;
};

class UPnPEvent: public Event {
public:
    virtual ~UPnPEvent() { }

    static PassRefPtr<UPnPEvent> create()
    {
        return adoptRef(new UPnPEvent());
    }

    static PassRefPtr<UPnPEvent> create(const String& propertyset, const String& uuid,
        const String& friendlyName, const String& serviceType)
    {
        return adoptRef(new UPnPEvent(propertyset, uuid, friendlyName, serviceType));
    }

    static PassRefPtr<UPnPEvent> create(const AtomicString &type, UPnPEventInit &init)
    {
        return adoptRef(new UPnPEvent(type, init));
    }

    UPnPEvent(const UPnPEvent &that)
        : Event(that.m_typeUE, that.m_canBubbleUE, that.m_cancelableUE)
        , m_propertyset(that.m_propertyset)
        , m_uuid(that.m_uuid)
        , m_friendlyName(that.m_friendlyName)
        , m_serviceType(that.m_serviceType)
        , m_typeUE(that.m_typeUE)
        , m_canBubbleUE(that.m_canBubbleUE)
        , m_cancelableUE(that.m_cancelableUE)
    {
    }

    UPnPEvent& operator= (const UPnPEvent &that)
    {
        m_propertyset = that.m_propertyset;
        m_uuid = that.m_uuid;
        m_friendlyName = that.m_friendlyName;
        m_serviceType = that.m_serviceType;

        this->m_typeUE = that.m_typeUE;
        this->m_canBubbleUE = that.m_canBubbleUE;
        this->m_cancelableUE = that.m_cancelableUE;

        return *this;
    }

    virtual const AtomicString& interfaceName() const { return eventNames().interfaceForUPnPEvent; }

    const String& propertyset() const { return m_propertyset; }
    const String& uuid() const { return m_uuid; }
    const String& friendlyName() const { return m_friendlyName; }
    const String& serviceType() const { return m_serviceType; }

private:
    UPnPEvent() { }

    UPnPEvent(const AtomicString &type, UPnPEventInit &init)
        : Event(type, true, true)
        , m_propertyset(init.propertyset)
        , m_uuid(init.uuid)
        , m_friendlyName(init.friendlyName)
        , m_serviceType(init.serviceType)
        , m_typeUE(type)
        , m_canBubbleUE(true)
        , m_cancelableUE(true)
    {
    }

    UPnPEvent(const String& propertyset, const String& uuid, const String& friendlyName, const String& serviceType)
        : Event(eventNames().upnpEvent, true, true)
        , m_propertyset(propertyset)
        , m_uuid(uuid)
        , m_friendlyName(friendlyName)
        , m_serviceType(serviceType)
        , m_canBubbleUE(true)
        , m_cancelableUE(true)
    {
    }

    String m_propertyset;
    String m_uuid;
    String m_friendlyName;
    String m_serviceType;
    AtomicString m_typeUE;
    bool m_canBubbleUE;
    bool m_cancelableUE;
};

}; // NameSpace WebCore

#endif /* UPnPEvent_h */
