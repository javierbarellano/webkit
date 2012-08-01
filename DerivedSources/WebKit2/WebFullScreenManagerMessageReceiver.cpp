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

#include "WebFullScreenManager.h"

#include "ArgumentDecoder.h"
#include "HandleMessage.h"
#include "WebFullScreenManagerMessages.h"

namespace WebKit {

void WebFullScreenManager::didReceiveWebFullScreenManagerMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::WebFullScreenManager::Kind>()) {
    case Messages::WebFullScreenManager::WillEnterFullScreenID:
        CoreIPC::handleMessage<Messages::WebFullScreenManager::WillEnterFullScreen>(arguments, this, &WebFullScreenManager::willEnterFullScreen);
        return;
    case Messages::WebFullScreenManager::DidEnterFullScreenID:
        CoreIPC::handleMessage<Messages::WebFullScreenManager::DidEnterFullScreen>(arguments, this, &WebFullScreenManager::didEnterFullScreen);
        return;
    case Messages::WebFullScreenManager::WillExitFullScreenID:
        CoreIPC::handleMessage<Messages::WebFullScreenManager::WillExitFullScreen>(arguments, this, &WebFullScreenManager::willExitFullScreen);
        return;
    case Messages::WebFullScreenManager::DidExitFullScreenID:
        CoreIPC::handleMessage<Messages::WebFullScreenManager::DidExitFullScreen>(arguments, this, &WebFullScreenManager::didExitFullScreen);
        return;
    case Messages::WebFullScreenManager::SetAnimatingFullScreenID:
        CoreIPC::handleMessage<Messages::WebFullScreenManager::SetAnimatingFullScreen>(arguments, this, &WebFullScreenManager::setAnimatingFullScreen);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit

#endif // ENABLE(FULLSCREEN_API)
