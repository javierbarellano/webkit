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

#ifndef NAVIGATORNETWORKSERVICES_H_
#define NAVIGATORNETWORKSERVICES_H_

#include <string>

#include <runtime/JSExportMacros.h>

#include "Event.h"
#include <wtf/text/CString.h>
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

    int servicesAvailable() const {return m_devs.size();}

    int length() const {return m_devs.size();}
    bool online() const {return m_online;}
    void setOnline(bool online) { m_online = online;}

    NavService* item(unsigned short index) {return m_devs.at(index).get();}

    void setServices(Vector<RefPtr<NavService> >* vDevs)
    {
    	m_devs.clear();
    	for (int i=0; i < (int)vDevs->size(); i++)
    		m_devs.append(vDevs->at(i));
    }

    void append(RefPtr<NavService> dev)
    {
    	bool found = false;
    	for (int i=0; i<m_devs.size(); i++) {
    		if (!strcmp(m_devs[i].get()->m_id.ascii().data(), dev->m_id.ascii().data())) {
    			found = true;
    			break;
    		}
    	}

    	if (!found)
    		m_devs.append(dev);
    }

    NavService* find(std::string uuid)
    {
    	for (int i=0; i<(int)m_devs.size(); i++)
//    		if (std::string(m_services[i]->uuid().ascii().data()) == uuid)
    			return m_devs[i].get();

    	return NULL;
    }

    void clearSrvs()
    {
    	for (int i=0; i<(int)m_devs.size(); i++)
    		m_devs.at(i).release();

    	m_devs.clear();
    }

    virtual const AtomicString& interfaceName() const;
    virtual ScriptExecutionContext* scriptExecutionContext() const;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(serviceavailable);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(serviceunavailable);

    using RefCounted<NavServices>::ref;
    using RefCounted<NavServices>::deref;

    Vector<RefPtr<NavService> > m_devs;

    std::string m_serviceType;

    ScriptExecutionContext* getContext() { return m_context; }

protected:
    virtual EventTargetData* eventTargetData();
    virtual EventTargetData* ensureEventTargetData();

private:
    NavServices(ScriptExecutionContext* context, ReadyState code)
    	: ActiveDOMObject(context)
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
