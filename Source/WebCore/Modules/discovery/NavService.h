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

class NavService : public RefCounted<NavService>, public EventTarget, public ActiveDOMObject {
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
    , m_id("")
    , m_code(CONNECTED)
    , m_name("")
    , m_url("")
    , m_config("")
    , m_type("")
    , m_online(true)
    , m_protocol(UPNP_TYPE)
    {}

    // Copy Constructor
    NavService(const NavService &that);

    NavService& operator= (const NavService &that);

    virtual ~NavService();

    virtual const AtomicString& interfaceName() const;
    virtual ScriptExecutionContext* scriptExecutionContext() const;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(serviceonline);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(serviceoffline);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(notify);

    using RefCounted<NavService>::ref;
    using RefCounted<NavService>::deref;


    ReadyState code() const { return m_code; }

    String name() const {return m_name;}
    String url() const {return m_url;}
    String id() const {return m_id;}
    String config() const {return m_config;}
    String type() const {return m_type;}
    bool   online() const {return m_online;}

    ProtocolType pType() const {return m_protocol;}

    void setName(String name) {m_name = name;}
    void setUrl(String url) {m_url = url;}
    void setid(String uuid) {m_id = uuid;}
    void setConfig(String config) {m_config = config;}
    void setType(String type) {m_type = type;}
    void setOnline(bool online) {m_online = online;}

    void setPType(ProtocolType type) {m_protocol = type;}

    // Unique identifier for server offering the service

    int readyState() const {return m_code;}

    String m_id;

protected:
    virtual EventTargetData* eventTargetData();
    virtual EventTargetData* ensureEventTargetData();

private:

    virtual void refEventTarget() { ref(); }
    virtual void derefEventTarget() { deref(); }

    ReadyState m_code;

    String m_name;
    String m_url;
     String m_config;
    String m_type;

    bool m_online;

    ProtocolType m_protocol;

    EventTargetData m_eventTargetData;
};


}; // namespace WebCore

#endif // NAVIGATORNETWORKSERVICE_H_


