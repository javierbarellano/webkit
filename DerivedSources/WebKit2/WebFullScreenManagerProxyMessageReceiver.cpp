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

#if ENABLE(FULLSCREEN_API)

#include "WebFullScreenManagerProxy.h"

#include "ArgumentDecoder.h"
#include "HandleMessage.h"
#include "WebCoreArgumentCoders.h"
#include "WebFullScreenManagerProxyMessages.h"
#include <WebCore/IntRect.h>

namespace WebKit {

void WebFullScreenManagerProxy::didReceiveWebFullScreenManagerProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::WebFullScreenManagerProxy::Kind>()) {
    case Messages::WebFullScreenManagerProxy::EnterFullScreenID:
        CoreIPC::handleMessage<Messages::WebFullScreenManagerProxy::EnterFullScreen>(arguments, this, &WebFullScreenManagerProxy::enterFullScreen);
        return;
    case Messages::WebFullScreenManagerProxy::ExitFullScreenID:
        CoreIPC::handleMessage<Messages::WebFullScreenManagerProxy::ExitFullScreen>(arguments, this, &WebFullScreenManagerProxy::exitFullScreen);
        return;
    case Messages::WebFullScreenManagerProxy::BeganEnterFullScreenID:
        CoreIPC::handleMessage<Messages::WebFullScreenManagerProxy::BeganEnterFullScreen>(arguments, this, &WebFullScreenManagerProxy::beganEnterFullScreen);
        return;
    case Messages::WebFullScreenManagerProxy::BeganExitFullScreenID:
        CoreIPC::handleMessage<Messages::WebFullScreenManagerProxy::BeganExitFullScreen>(arguments, this, &WebFullScreenManagerProxy::beganExitFullScreen);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

void WebFullScreenManagerProxy::didReceiveSyncWebFullScreenManagerProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    switch (messageID.get<Messages::WebFullScreenManagerProxy::Kind>()) {
    case Messages::WebFullScreenManagerProxy::SupportsFullScreenID:
        CoreIPC::handleMessage<Messages::WebFullScreenManagerProxy::SupportsFullScreen>(arguments, reply.get(), this, &WebFullScreenManagerProxy::supportsFullScreen);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit

#endif // ENABLE(FULLSCREEN_API)
