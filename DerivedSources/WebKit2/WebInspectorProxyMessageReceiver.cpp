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

#if ENABLE(INSPECTOR)

#include "WebInspectorProxy.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#include "HandleMessage.h"
#include "WebInspectorProxyMessages.h"
#include "WebPageCreationParameters.h"
#include <wtf/text/WTFString.h>

namespace WebKit {

void WebInspectorProxy::didReceiveWebInspectorProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::WebInspectorProxy::Kind>()) {
    case Messages::WebInspectorProxy::DidLoadInspectorPageID:
        CoreIPC::handleMessage<Messages::WebInspectorProxy::DidLoadInspectorPage>(arguments, this, &WebInspectorProxy::didLoadInspectorPage);
        return;
    case Messages::WebInspectorProxy::DidCloseID:
        CoreIPC::handleMessage<Messages::WebInspectorProxy::DidClose>(arguments, this, &WebInspectorProxy::didClose);
        return;
    case Messages::WebInspectorProxy::BringToFrontID:
        CoreIPC::handleMessage<Messages::WebInspectorProxy::BringToFront>(arguments, this, &WebInspectorProxy::bringToFront);
        return;
    case Messages::WebInspectorProxy::InspectedURLChangedID:
        CoreIPC::handleMessage<Messages::WebInspectorProxy::InspectedURLChanged>(arguments, this, &WebInspectorProxy::inspectedURLChanged);
        return;
    case Messages::WebInspectorProxy::AttachID:
        CoreIPC::handleMessage<Messages::WebInspectorProxy::Attach>(arguments, this, &WebInspectorProxy::attach);
        return;
    case Messages::WebInspectorProxy::DetachID:
        CoreIPC::handleMessage<Messages::WebInspectorProxy::Detach>(arguments, this, &WebInspectorProxy::detach);
        return;
    case Messages::WebInspectorProxy::SetAttachedWindowHeightID:
        CoreIPC::handleMessage<Messages::WebInspectorProxy::SetAttachedWindowHeight>(arguments, this, &WebInspectorProxy::setAttachedWindowHeight);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

void WebInspectorProxy::didReceiveSyncWebInspectorProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    switch (messageID.get<Messages::WebInspectorProxy::Kind>()) {
    case Messages::WebInspectorProxy::CreateInspectorPageID:
        CoreIPC::handleMessage<Messages::WebInspectorProxy::CreateInspectorPage>(arguments, reply.get(), this, &WebInspectorProxy::createInspectorPage);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit

#endif // ENABLE(INSPECTOR)
