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

#ifndef WebFullScreenManagerProxyMessages_h
#define WebFullScreenManagerProxyMessages_h

#if ENABLE(FULLSCREEN_API)

#include "Arguments.h"
#include "MessageID.h"

namespace WebCore {
    class IntRect;
}

namespace Messages {

namespace WebFullScreenManagerProxy {

enum Kind {
    SupportsFullScreenID,
    EnterFullScreenID,
    ExitFullScreenID,
    BeganEnterFullScreenID,
    BeganExitFullScreenID,
};

struct SupportsFullScreen : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SupportsFullScreenID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SupportsFullScreen(bool withKeyboard)
        : CoreIPC::Arguments1<bool>(withKeyboard)
    {
    }
};

struct EnterFullScreen : CoreIPC::Arguments0 {
    static const Kind messageID = EnterFullScreenID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct ExitFullScreen : CoreIPC::Arguments0 {
    static const Kind messageID = ExitFullScreenID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct BeganEnterFullScreen : CoreIPC::Arguments2<const WebCore::IntRect&, const WebCore::IntRect&> {
    static const Kind messageID = BeganEnterFullScreenID;
    typedef CoreIPC::Arguments2<const WebCore::IntRect&, const WebCore::IntRect&> DecodeType;
    BeganEnterFullScreen(const WebCore::IntRect& initialRect, const WebCore::IntRect& finalRect)
        : CoreIPC::Arguments2<const WebCore::IntRect&, const WebCore::IntRect&>(initialRect, finalRect)
    {
    }
};

struct BeganExitFullScreen : CoreIPC::Arguments2<const WebCore::IntRect&, const WebCore::IntRect&> {
    static const Kind messageID = BeganExitFullScreenID;
    typedef CoreIPC::Arguments2<const WebCore::IntRect&, const WebCore::IntRect&> DecodeType;
    BeganExitFullScreen(const WebCore::IntRect& initialRect, const WebCore::IntRect& finalRect)
        : CoreIPC::Arguments2<const WebCore::IntRect&, const WebCore::IntRect&>(initialRect, finalRect)
    {
    }
};

} // namespace WebFullScreenManagerProxy

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::WebFullScreenManagerProxy::Kind> {
    static const MessageClass messageClass = MessageClassWebFullScreenManagerProxy;
};

} // namespace CoreIPC

#endif // ENABLE(FULLSCREEN_API)

#endif // WebFullScreenManagerProxyMessages_h
