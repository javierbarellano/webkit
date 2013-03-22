/*
 * NavServices.h
 *
 *  Created on: Jan 10, 2012
 *      Author: gar
 */

#ifndef NAVIGATORNETWORKSERVICES_H_
#define NAVIGATORNETWORKSERVICES_H_

#include <string>

#include <runtime/JSExportMacros.h>

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

    virtual ~NavServices(){ clearSrvs(); }

    int servicesAvailable() const {return m_services.size();}

    int length() const {return m_services.size();}
    bool online() const {return m_online;}
    void setOnline(bool online) { m_online = online;}

    NavService* item(unsigned short index) {return m_services.at(index).get();}

    void setServices(Vector<RefPtr<NavService> >* vDevs)
    {
    	m_services.clear();
    	for (int i=0; i < (int)vDevs->size(); i++)
    		m_services.append(vDevs->at(i));
    }

    NavService* find(std::string uuid)
    {
    	for (int i=0; i<(int)m_services.size(); i++)
//    		if (std::string(m_services[i]->uuid().ascii().data()) == uuid)
    			return m_services[i].get();

    	return NULL;
    }

    void clearSrvs()
    {
    	for (int i=0; i<(int)m_services.size(); i++)
    		m_services.at(i).release();

    	m_services.clear();
    }

    virtual const AtomicString& interfaceName() const;
    virtual ScriptExecutionContext* scriptExecutionContext() const;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(devadded);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(devdropped);

    DEFINE_ATTRIBUTE_EVENT_LISTENER(servicesonline);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(servicesoffline);

    using RefCounted<NavServices>::ref;
    using RefCounted<NavServices>::deref;

    Vector<RefPtr<NavService> > m_services;

    std::string m_serviceType;

    ScriptExecutionContext* getContext() { return m_context; }

protected:
    virtual EventTargetData* eventTargetData();
    virtual EventTargetData* ensureEventTargetData();

private:
    NavServices(ScriptExecutionContext* context, ReadyState code)
    	: ActiveDOMObject(context, this)
    	, m_code(code)
    	, m_context(context)
		, m_online(false)
    {
    }

    virtual void refEventTarget() { ref(); }
    virtual void derefEventTarget() { deref(); }

    ReadyState m_code;

    EventTargetData m_eventTargetData;

    ScriptExecutionContext* m_context;

    bool m_online;

};

} // namespace WebCore

#endif /* NAVIGATORNETWORKSERVICES_H_ */
