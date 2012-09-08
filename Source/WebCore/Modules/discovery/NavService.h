/*
 * NavService.h
 *
 *  Created on: Jan 10, 2012
 *      Author: gar
 */

#ifndef NAVIGATORNETWORKSERVICE_H_
#define NAVIGATORNETWORKSERVICE_H_

#include "Event.h"
#include <wtf/text/WTFString.h>
#include <wtf/RefCounted.h>

#include "ActiveDOMObject.h"
#include "EventListener.h"
#include "EventNames.h"
#include "EventTarget.h"


namespace WebCore {

class NavService : public RefCounted<NavService>, public EventTarget, public ActiveDOMObject
{
public:
    // Should be kept in sync with the values in the idl file.
    enum ReadyState {
    	CONNECTED = 1,
    	DISCONNECTED = 2
    };

    enum ProtocolType {
    	UPNP_TYPE = 1,
    	ZCONF_TYPE = 2
    };

    static PassRefPtr<NavService> create(ScriptExecutionContext* context)
    {
        return adoptRef(new NavService(context));
    }

    NavService(ScriptExecutionContext* context)
	: ActiveDOMObject(context, this)
    , m_code(CONNECTED) {}

    // Copy Constructor
    NavService(const NavService &that);

    NavService& operator= (const NavService &that);

    virtual ~NavService();

    virtual const AtomicString& interfaceName() const;
    virtual ScriptExecutionContext* scriptExecutionContext() const;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(upnpevent);

    using RefCounted<NavService>::ref;
    using RefCounted<NavService>::deref;


    ReadyState code() const { return m_code; }

    String name() const {return m_name;}
    String url() const {return m_url;}
    String eventUrl() const {return m_eventUrl;}
    String uuid() const {return m_uuid;}
    String config() const {return m_config;}
    String serviceType() const {return m_serviceType;}
    ProtocolType pType() const {return m_protocol;}

    void setName(String name) {m_name = name;}
    void setUrl(String url) {m_url = url;}
    void setEventUrl(String url) {m_eventUrl = url;}
    void setuuid(String uuid) {m_uuid = uuid;}
    void setConfig(String config) {m_config = config;}
    void setServiceType(String type) {m_serviceType = type;}
    void setPType(ProtocolType type) {m_protocol = type;}

    // Unique identifier for server offering the service

    int readyState() const {return m_code;}

protected:
    virtual EventTargetData* eventTargetData();
    virtual EventTargetData* ensureEventTargetData();

private:

    virtual void refEventTarget() { ref(); }
    virtual void derefEventTarget() { deref(); }

    ReadyState m_code;

    String m_name;
    String m_url;
    String m_eventUrl;
    String m_uuid;
    String m_config;
    String m_serviceType;

    ProtocolType m_protocol;

    EventTargetData m_eventTargetData;
};


}; // namespace WebCore

#endif // NAVIGATORNETWORKSERVICE_H_


