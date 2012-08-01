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

#include "NPObjectMessageReceiver.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#include "HandleMessage.h"
#include "NPIdentifierData.h"
#include "NPObjectMessageReceiverMessages.h"
#include "NPVariantData.h"
#include <wtf/Vector.h>

namespace WebKit {


void NPObjectMessageReceiver::didReceiveSyncNPObjectMessageReceiverMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    switch (messageID.get<Messages::NPObjectMessageReceiver::Kind>()) {
    case Messages::NPObjectMessageReceiver::DeallocateID:
        CoreIPC::handleMessage<Messages::NPObjectMessageReceiver::Deallocate>(arguments, reply.get(), this, &NPObjectMessageReceiver::deallocate);
        return;
    case Messages::NPObjectMessageReceiver::HasMethodID:
        CoreIPC::handleMessage<Messages::NPObjectMessageReceiver::HasMethod>(arguments, reply.get(), this, &NPObjectMessageReceiver::hasMethod);
        return;
    case Messages::NPObjectMessageReceiver::InvokeID:
        CoreIPC::handleMessage<Messages::NPObjectMessageReceiver::Invoke>(arguments, reply.get(), this, &NPObjectMessageReceiver::invoke);
        return;
    case Messages::NPObjectMessageReceiver::InvokeDefaultID:
        CoreIPC::handleMessage<Messages::NPObjectMessageReceiver::InvokeDefault>(arguments, reply.get(), this, &NPObjectMessageReceiver::invokeDefault);
        return;
    case Messages::NPObjectMessageReceiver::HasPropertyID:
        CoreIPC::handleMessage<Messages::NPObjectMessageReceiver::HasProperty>(arguments, reply.get(), this, &NPObjectMessageReceiver::hasProperty);
        return;
    case Messages::NPObjectMessageReceiver::GetPropertyID:
        CoreIPC::handleMessage<Messages::NPObjectMessageReceiver::GetProperty>(arguments, reply.get(), this, &NPObjectMessageReceiver::getProperty);
        return;
    case Messages::NPObjectMessageReceiver::SetPropertyID:
        CoreIPC::handleMessage<Messages::NPObjectMessageReceiver::SetProperty>(arguments, reply.get(), this, &NPObjectMessageReceiver::setProperty);
        return;
    case Messages::NPObjectMessageReceiver::RemovePropertyID:
        CoreIPC::handleMessage<Messages::NPObjectMessageReceiver::RemoveProperty>(arguments, reply.get(), this, &NPObjectMessageReceiver::removeProperty);
        return;
    case Messages::NPObjectMessageReceiver::EnumerateID:
        CoreIPC::handleMessage<Messages::NPObjectMessageReceiver::Enumerate>(arguments, reply.get(), this, &NPObjectMessageReceiver::enumerate);
        return;
    case Messages::NPObjectMessageReceiver::ConstructID:
        CoreIPC::handleMessage<Messages::NPObjectMessageReceiver::Construct>(arguments, reply.get(), this, &NPObjectMessageReceiver::construct);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit

#endif // ENABLE(PLUGIN_PROCESS)
