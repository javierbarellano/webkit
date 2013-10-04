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

#ifndef NetworkServices_h
#define NetworkServices_h

#include "ActiveDOMObject.h"
#include "Event.h"
#include "EventListener.h"
#include "EventNames.h"
#include "EventTarget.h"
#include "NetworkService.h"
#include <runtime/JSExportMacros.h>
#include <string>
#include <wtf/RefCounted.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class NetworkServices : public RefCounted<NetworkServices>, public EventTarget, public ActiveDOMObject {
public:
    // Should be kept in sync with the values in the idl file.
    enum ReadyState {
        Connected = 1,
        Disconnected = 2
    };

    static PassRefPtr<NetworkServices> create(ScriptExecutionContext* context, ReadyState code)
    {
        return adoptRef(new NetworkServices(context, code));
    }

    NetworkServices(const NetworkServices &that);
    NetworkServices& operator= (const NetworkServices &that);
    virtual ~NetworkServices() { clearSrvs(); }

    int servicesAvailable() const { return m_devs.size(); }

    int length() const {return m_devs.size();}
    bool online() const {return m_online;}
    void setOnline(bool online) { m_online = online;}

    NetworkService* item(unsigned short index) {return m_devs.at(index).get();}

    void setServices(Vector<RefPtr<NetworkService> >* vDevs)
    {
        m_devs.clear();
        for (int i = 0; i < (int)vDevs->size(); i++)
            m_devs.append(vDevs->at(i));
    }

    void append(PassRefPtr<NetworkService> dev)
    {
        bool found = false;
        for (int i = 0; i < m_devs.size(); i++) {
            if (!strcmp(m_devs[i].get()->m_id.ascii().data(), dev->m_id.ascii().data())) {
                found = true;
                break;
            }
        }

        if (!found)
            m_devs.append(dev);
    }

    NetworkService* find(const std::string &uuid)
    {
        for (int i = 0; i < (int)m_devs.size(); i++)
//            if (std::string(m_services[i]->uuid().ascii().data()) == uuid)
            return m_devs[i].get();

        return 0;
    }

    void clearSrvs()
    {
        m_devs.clear();
    }

    virtual EventTargetInterface eventTargetInterface() const OVERRIDE;
    virtual ScriptExecutionContext* scriptExecutionContext() const OVERRIDE;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(serviceavailable);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(serviceunavailable);

    using RefCounted<NetworkServices>::ref;
    using RefCounted<NetworkServices>::deref;

    Vector<RefPtr<NetworkService> > m_devs;

    std::string m_serviceType;

    ScriptExecutionContext* getContext() { return m_context; }

protected:
    virtual EventTargetData* eventTargetData() OVERRIDE;
    virtual EventTargetData& ensureEventTargetData() OVERRIDE;

private:
    NetworkServices(ScriptExecutionContext* context, ReadyState code)
        : ActiveDOMObject(context)
        , m_state(code)
        , m_context(context)
        , m_online(false)
    {
    }

    virtual void refEventTarget() { ref(); }
    virtual void derefEventTarget() { deref(); }

    ReadyState m_state;
    EventTargetData m_eventTargetData;
    ScriptExecutionContext* m_context;
    bool m_online;
};

} // namespace WebCore

#endif /* NetworkServices_h */
