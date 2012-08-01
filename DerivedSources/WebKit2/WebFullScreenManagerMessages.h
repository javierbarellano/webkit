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

#ifndef WebFullScreenManagerMessages_h
#define WebFullScreenManagerMessages_h

#if ENABLE(FULLSCREEN_API)

#include "Arguments.h"
#include "MessageID.h"


namespace Messages {

namespace WebFullScreenManager {

enum Kind {
    WillEnterFullScreenID,
    DidEnterFullScreenID,
    WillExitFullScreenID,
    DidExitFullScreenID,
    SetAnimatingFullScreenID,
};

struct WillEnterFullScreen : CoreIPC::Arguments0 {
    static const Kind messageID = WillEnterFullScreenID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct DidEnterFullScreen : CoreIPC::Arguments0 {
    static const Kind messageID = DidEnterFullScreenID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct WillExitFullScreen : CoreIPC::Arguments0 {
    static const Kind messageID = WillExitFullScreenID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct DidExitFullScreen : CoreIPC::Arguments0 {
    static const Kind messageID = DidExitFullScreenID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct SetAnimatingFullScreen : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetAnimatingFullScreenID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetAnimatingFullScreen(bool animating)
        : CoreIPC::Arguments1<bool>(animating)
    {
    }
};

} // namespace WebFullScreenManager

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::WebFullScreenManager::Kind> {
    static const MessageClass messageClass = MessageClassWebFullScreenManager;
};

} // namespace CoreIPC

#endif // ENABLE(FULLSCREEN_API)

#endif // WebFullScreenManagerMessages_h
