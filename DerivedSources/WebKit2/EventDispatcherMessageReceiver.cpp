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

#include "EventDispatcher.h"

#include "ArgumentDecoder.h"
#include "EventDispatcherMessages.h"
#include "HandleMessage.h"
#include "WebEvent.h"

namespace WebKit {

void EventDispatcher::didReceiveEventDispatcherMessageOnConnectionWorkQueue(CoreIPC::Connection* connection, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, bool& didHandleMessage)
{
#if COMPILER(MSVC)
#pragma warning(push)
#pragma warning(disable: 4065)
#endif
    switch (messageID.get<Messages::EventDispatcher::Kind>()) {
    case Messages::EventDispatcher::WheelEventID:
        CoreIPC::handleMessageOnConnectionQueue<Messages::EventDispatcher::WheelEvent>(connection, arguments, this, &EventDispatcher::wheelEvent);
        didHandleMessage = true;
        return;
#if ENABLE(GESTURE_EVENTS)
    case Messages::EventDispatcher::GestureEventID:
        CoreIPC::handleMessageOnConnectionQueue<Messages::EventDispatcher::GestureEvent>(connection, arguments, this, &EventDispatcher::gestureEvent);
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


} // namespace WebKit
