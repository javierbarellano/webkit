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

#include "config.h"
#if ENABLE(DISCOVERY)

#include "NetworkService.h"

#include "NavDsc.h"

namespace WebCore {

NetworkService::NetworkService(const NetworkService &that)
    : RefCounted()
    , ActiveDOMObject(that.scriptExecutionContext())
    , m_id(that.m_id)
    , m_code(that.m_code)
    , m_name(that.m_name)
    , m_url(that.m_url)
    , m_config(that.m_config)
    , m_type(that.m_type)
    , m_hasPermission(that.m_hasPermission)
    , m_online(that.online())
    , m_protocol(that.m_protocol)
{
}

NetworkService& NetworkService::operator= (const NetworkService &that)
{
    m_id = that.m_id;
    m_code = that.m_code;
    m_name = that.m_name;
    m_url = that.m_url;
    m_config = that.m_config;
    m_type = that.m_type;
    m_hasPermission = that.m_hasPermission;
    m_online = that.m_online;
    m_protocol = that.m_protocol;
    return *this;
}

NetworkService::~NetworkService() { }

EventTargetInterface NetworkService::eventTargetInterface() const
{
    return NetworkServiceEventTargetInterfaceType;
}

ScriptExecutionContext* NetworkService::scriptExecutionContext() const
{
    return ActiveDOMObject::scriptExecutionContext();
}

EventTargetData* NetworkService::eventTargetData()
{
    return &m_eventTargetData;
}

EventTargetData& NetworkService::ensureEventTargetData()
{
    return m_eventTargetData;
}

bool NetworkService::hasPermission() const
{
    return m_hasPermission || !NavDsc::permissionsEnabled();
}

} // namespace WebCore

#endif // DISCOVERY
