/*
 * NavServices.h
 *
 *  Created on: Jan 10, 2012
 *      Author: gar
 */

#ifndef NAVIGATORNETWORKSERVICES_H_
#define NAVIGATORNETWORKSERVICES_H_

#include <string>

#include "Event.h"
#include <wtf/text/WTFString.h>
#include <wtf/RefCounted.h>
#include "Modules/discovery/NavService.h"

#include "ActiveDOMObject.h"
#include "EventListener.h"
#include "EventNames.h"
#include "EventTarget.h"



namespace WebCore {

class NavServices : public RefCounted<NavServices>, public EventTarget, public ActiveDOMObject {
public:
    // Should be kept in sync with the values in the idl file.
    enum ReadyState {
    	CONNECTED = 1,
    	DISCONNECTED = 2
    };


    static PassRefPtr<NavServices> create(ScriptExecutionContext* context, ReadyState code)
    {
        return adoptRef(new NavServices(context, code));
    }

    // Copy Constructor
    NavServices(const NavServices &that);

    NavServices& operator= (const NavServices &that);

    virtual ~NavServices();

    int servicesAvailable() const {return m_services.size();}

    int length() const {return m_services.size();}

    NavService* item(unsigned short index) {return m_services.at(index);}

    void setServices(Vector<NavService*>* vDevs)
    {
    	m_services.clear();
    	for (int i=0; i < (int)vDevs->size(); i++)
    		m_services.append(vDevs->at(i));
    }

    NavService* find(std::string uuid)
    {
    	for (int i=0; i<(int)m_services.size(); i++)
    		if (std::string(m_services[i]->uuid().ascii().data()) == uuid)
    			return m_services[i];

    	return NULL;
    }

    void clearSrvs()
    {
    	for (int i=0; i<(int)m_services.size(); i++)
    		delete m_services.at(i);

    	m_services.clear();
    }

    virtual const AtomicString& interfaceName() const;
    virtual ScriptExecutionContext* scriptExecutionContext() const;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(devadded);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(devdropped);

    using RefCounted<NavServices>::ref;
    using RefCounted<NavServices>::deref;

    Vector<NavService*> m_services;

protected:
    virtual EventTargetData* eventTargetData();
    virtual EventTargetData* ensureEventTargetData();

private:
    NavServices(ScriptExecutionContext* context, ReadyState code)
    	: ActiveDOMObject(context, this)
    	, m_code(code)
    {
    }

    virtual void refEventTarget() { ref(); }
    virtual void derefEventTarget() { deref(); }

    ReadyState m_code;

    EventTargetData m_eventTargetData;

};

} // namespace WebCore

#endif /* NAVIGATORNETWORKSERVICES_H_ */
