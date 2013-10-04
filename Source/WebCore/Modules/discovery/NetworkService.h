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

#ifndef NetworkService_h
#define NetworkService_h

#include "ActiveDOMObject.h"
#include "Event.h"
#include "EventListener.h"
#include "EventNames.h"
#include "EventTarget.h"
#include <wtf/RefCounted.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class NetworkService : public RefCounted<NetworkService>, public EventTarget, public ActiveDOMObject {
public:
    // Should be kept in sync with the values in the idl file.
    enum ReadyState {
        Connected = 1,
        Disconnected = 2
    };

    enum ProtocolType {
        UPnPType = 1,
        ZConfType = 2
    };

    static PassRefPtr<NetworkService> create(ScriptExecutionContext* context)
    {
        return adoptRef(new NetworkService(context));
    }

    NetworkService(ScriptExecutionContext* context)
        : RefCounted()
        , ActiveDOMObject(context)
        , m_id("")
        , m_code(Connected)
        , m_name("")
        , m_url("")
        , m_config("")
        , m_type("")
        , m_hasPermission(false)
        , m_online(true)
        , m_protocol(UPnPType)
    {
    }

    NetworkService(const NetworkService &that);
    NetworkService& operator= (const NetworkService &that);
    virtual ~NetworkService();

    virtual EventTargetInterface eventTargetInterface() const OVERRIDE;
    virtual ScriptExecutionContext* scriptExecutionContext() const OVERRIDE;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(serviceonline);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(serviceoffline);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(notify);

    using RefCounted<NetworkService>::ref;
    using RefCounted<NetworkService>::deref;

    ReadyState code() const { return m_code; }

    String name() const { return m_name; }
    String url() const { return m_url; }
    String id() const { return m_id; }
    String config() const { return m_config; }
    String type() const { return m_type; }
    bool online() const { return m_online; }
    bool hasPermission() const;

    ProtocolType protocolType() const { return m_protocol; }

    void setName(String name) { m_name = name; }
    void setUrl(String url) { m_url = url; }
    void setid(String uuid) { m_id = uuid; }
    void setConfig(String config) { m_config = config; }
    void setType(String type) { m_type = type; }
    void setOnline(bool online) { m_online = online; }
    void setPermission(bool hasPermission) { m_hasPermission = hasPermission; }

    void setProtocolType(ProtocolType type) { m_protocol = type; }

    // Unique identifier for server offering the service

    int readyState() const { return m_code; }

    String m_id;

protected:
    virtual EventTargetData* eventTargetData() OVERRIDE;
    virtual EventTargetData& ensureEventTargetData() OVERRIDE;

private:
    virtual void refEventTarget() { ref(); }
    virtual void derefEventTarget() { deref(); }

    ReadyState m_code;
    String m_name;
    String m_url;
    String m_config;
    String m_type;
    bool m_hasPermission;
    bool m_online;
    ProtocolType m_protocol;
    EventTargetData m_eventTargetData;
};


}; // namespace WebCore

#endif // NetworkService_h


