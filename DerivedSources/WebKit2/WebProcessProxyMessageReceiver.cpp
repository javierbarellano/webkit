/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "WebProcessProxy.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#if ENABLE(PLUGIN_PROCESS)
#include "Attachment.h"
#endif
#include "DataReference.h"
#include "HandleMessage.h"
#if PLATFORM(MAC)
#include "SecItemRequestData.h"
#endif
#if PLATFORM(MAC)
#include "SecKeychainItemRequestData.h"
#endif
#include "WebProcessProxyMessages.h"
#include <wtf/text/WTFString.h>

namespace Messages {

namespace WebProcessProxy {

#if ENABLE(PLUGIN_PROCESS)

GetPluginProcessConnection::DelayedReply::DelayedReply(PassRefPtr<CoreIPC::Connection> connection, PassOwnPtr<CoreIPC::ArgumentEncoder> arguments)
    : m_connection(connection)
    , m_arguments(arguments)
{
}

GetPluginProcessConnection::DelayedReply::~DelayedReply()
{
    ASSERT(!m_connection);
}

bool GetPluginProcessConnection::DelayedReply::send(const CoreIPC::Attachment& connectionHandle)
{
    ASSERT(m_arguments);
    m_arguments->encode(connectionHandle);
    bool result = m_connection->sendSyncReply(m_arguments.release());
    m_connection = nullptr;
    return result;
}

#endif

} // namespace WebProcessProxy

} // namespace Messages

namespace WebKit {

void WebProcessProxy::didReceiveWebProcessProxyMessageOnConnectionWorkQueue(CoreIPC::Connection* connection, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, bool& didHandleMessage)
{
#if COMPILER(MSVC)
#pragma warning(push)
#pragma warning(disable: 4065)
#endif
    switch (messageID.get<Messages::WebProcessProxy::Kind>()) {
#if PLATFORM(MAC)
    case Messages::WebProcessProxy::SecItemRequestID:
        CoreIPC::handleMessageOnConnectionQueue<Messages::WebProcessProxy::SecItemRequest>(connection, arguments, this, &WebProcessProxy::secItemRequest);
        didHandleMessage = true;
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebProcessProxy::SecKeychainItemRequestID:
        CoreIPC::handleMessageOnConnectionQueue<Messages::WebProcessProxy::SecKeychainItemRequest>(connection, arguments, this, &WebProcessProxy::secKeychainItemRequest);
        didHandleMessage = true;
        return;
#endif
    default:
        return;
    }
#if COMPILER(MSVC)
#pragma warning(pop)
#endif
}

void WebProcessProxy::didReceiveWebProcessProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::WebProcessProxy::Kind>()) {
    case Messages::WebProcessProxy::AddBackForwardItemID:
        CoreIPC::handleMessage<Messages::WebProcessProxy::AddBackForwardItem>(arguments, this, &WebProcessProxy::addBackForwardItem);
        return;
    case Messages::WebProcessProxy::DidDestroyFrameID:
        CoreIPC::handleMessage<Messages::WebProcessProxy::DidDestroyFrame>(arguments, this, &WebProcessProxy::didDestroyFrame);
        return;
#if ENABLE(PLUGIN_PROCESS)
    case Messages::WebProcessProxy::PluginSyncMessageSendTimedOutID:
        CoreIPC::handleMessage<Messages::WebProcessProxy::PluginSyncMessageSendTimedOut>(arguments, this, &WebProcessProxy::pluginSyncMessageSendTimedOut);
        return;
#endif
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

void WebProcessProxy::didReceiveSyncWebProcessProxyMessage(CoreIPC::Connection* connection, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    switch (messageID.get<Messages::WebProcessProxy::Kind>()) {
    case Messages::WebProcessProxy::ShouldTerminateID:
        CoreIPC::handleMessage<Messages::WebProcessProxy::ShouldTerminate>(arguments, reply.get(), this, &WebProcessProxy::shouldTerminate);
        return;
#if ENABLE(PLUGIN_PROCESS)
    case Messages::WebProcessProxy::GetPluginProcessConnectionID:
        CoreIPC::handleMessageDelayed<Messages::WebProcessProxy::GetPluginProcessConnection>(connection, arguments, reply, this, &WebProcessProxy::getPluginProcessConnection);
        return;
#endif
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit
