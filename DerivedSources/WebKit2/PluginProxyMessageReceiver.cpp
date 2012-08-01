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

#if ENABLE(PLUGIN_PROCESS)

#include "PluginProxy.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#include "HandleMessage.h"
#include "NPVariantData.h"
#include "PluginProxyMessages.h"
#include "WebCoreArgumentCoders.h"
#include <WebCore/HTTPHeaderMap.h>
#include <WebCore/IntRect.h>
#include <WebCore/ProtectionSpace.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

void PluginProxy::didReceivePluginProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::PluginProxy::Kind>()) {
    case Messages::PluginProxy::LoadURLID:
        CoreIPC::handleMessage<Messages::PluginProxy::LoadURL>(arguments, this, &PluginProxy::loadURL);
        return;
    case Messages::PluginProxy::UpdateID:
        CoreIPC::handleMessage<Messages::PluginProxy::Update>(arguments, this, &PluginProxy::update);
        return;
    case Messages::PluginProxy::SetCookiesForURLID:
        CoreIPC::handleMessage<Messages::PluginProxy::SetCookiesForURL>(arguments, this, &PluginProxy::setCookiesForURL);
        return;
    case Messages::PluginProxy::CancelStreamLoadID:
        CoreIPC::handleMessage<Messages::PluginProxy::CancelStreamLoad>(arguments, this, &PluginProxy::cancelStreamLoad);
        return;
    case Messages::PluginProxy::CancelManualStreamLoadID:
        CoreIPC::handleMessage<Messages::PluginProxy::CancelManualStreamLoad>(arguments, this, &PluginProxy::cancelManualStreamLoad);
        return;
    case Messages::PluginProxy::SetStatusbarTextID:
        CoreIPC::handleMessage<Messages::PluginProxy::SetStatusbarText>(arguments, this, &PluginProxy::setStatusbarText);
        return;
#if PLATFORM(MAC)
    case Messages::PluginProxy::PluginFocusOrWindowFocusChangedID:
        CoreIPC::handleMessage<Messages::PluginProxy::PluginFocusOrWindowFocusChanged>(arguments, this, &PluginProxy::pluginFocusOrWindowFocusChanged);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::PluginProxy::SetComplexTextInputStateID:
        CoreIPC::handleMessage<Messages::PluginProxy::SetComplexTextInputState>(arguments, this, &PluginProxy::setComplexTextInputState);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::PluginProxy::SetLayerHostingContextIDID:
        CoreIPC::handleMessage<Messages::PluginProxy::SetLayerHostingContextID>(arguments, this, &PluginProxy::setLayerHostingContextID);
        return;
#endif
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

void PluginProxy::didReceiveSyncPluginProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    switch (messageID.get<Messages::PluginProxy::Kind>()) {
    case Messages::PluginProxy::ProxiesForURLID:
        CoreIPC::handleMessage<Messages::PluginProxy::ProxiesForURL>(arguments, reply.get(), this, &PluginProxy::proxiesForURL);
        return;
    case Messages::PluginProxy::CookiesForURLID:
        CoreIPC::handleMessage<Messages::PluginProxy::CookiesForURL>(arguments, reply.get(), this, &PluginProxy::cookiesForURL);
        return;
    case Messages::PluginProxy::GetAuthenticationInfoID:
        CoreIPC::handleMessage<Messages::PluginProxy::GetAuthenticationInfo>(arguments, reply.get(), this, &PluginProxy::getAuthenticationInfo);
        return;
    case Messages::PluginProxy::GetPluginElementNPObjectID:
        CoreIPC::handleMessage<Messages::PluginProxy::GetPluginElementNPObject>(arguments, reply.get(), this, &PluginProxy::getPluginElementNPObject);
        return;
    case Messages::PluginProxy::EvaluateID:
        CoreIPC::handleMessage<Messages::PluginProxy::Evaluate>(arguments, reply.get(), this, &PluginProxy::evaluate);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit

#endif // ENABLE(PLUGIN_PROCESS)
