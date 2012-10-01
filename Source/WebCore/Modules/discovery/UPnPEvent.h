/*
 * UPnPEvent.h
 *
 *  Created on: Sep 28, 2012
 *      Author: gar
 */

#ifndef UPNPEVENT_H_
#define UPNPEVENT_H_

#include "Event.h"

namespace WebCore {

struct UPnPEventInit : public EventInit {

	UPnPEventInit() :
		propertyset()
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
	virtual ~UPnPEvent(){}

    static PassRefPtr<UPnPEvent> create()
    {
        return adoptRef(new UPnPEvent());
    }

    static PassRefPtr<UPnPEvent> create(
    		const String& propertyset, const String& uuid, const String& friendlyName, const String& serviceType)
    {
    	return adoptRef(new UPnPEvent(propertyset, uuid, friendlyName, serviceType));
    }

    static PassRefPtr<UPnPEvent> create(const AtomicString &type, UPnPEventInit &init)
    {
    	return adoptRef(new UPnPEvent(type, init));
    }
    // Copy Constructor
    UPnPEvent(const UPnPEvent &that) :
    	Event(that.m_typeUE, that.m_canBubbleUE, that.m_cancelableUE)
    {

		m_propertyset = that.m_propertyset;
		m_uuid = that.m_uuid;
		m_friendlyName = that.m_friendlyName;
		m_serviceType = that.m_serviceType;

		m_typeUE = that.m_typeUE;
		m_canBubbleUE = that.m_canBubbleUE;
		m_cancelableUE = that.m_cancelableUE;
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
    UPnPEvent() {}

	UPnPEvent(const AtomicString &type, UPnPEventInit &init) :
		Event(type, true, true)
	{
		m_propertyset = init.propertyset;
		m_uuid = init.uuid;
		m_friendlyName = init.friendlyName;
		m_serviceType = init.serviceType;

		m_typeUE = type;
		m_canBubbleUE = true;
		m_cancelableUE = true;
	}

	UPnPEvent(const String& propertyset, const String& uuid, const String& friendlyName, const String& serviceType) :
		Event(eventNames().upnpEvent, true, true)
	{
		m_propertyset = propertyset;
		m_uuid = uuid;
		m_friendlyName = friendlyName;
		m_serviceType = serviceType;

		m_typeUE = eventNames().upnpEvent;
		m_canBubbleUE = true;
		m_cancelableUE = true;
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

#endif /* UPNPEVENT_H_ */
