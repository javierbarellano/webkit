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

#ifndef WebInspectorProxyMessages_h
#define WebInspectorProxyMessages_h

#if ENABLE(INSPECTOR)

#include "Arguments.h"
#include "MessageID.h"

namespace WTF {
    class String;
}

namespace WebKit {
    struct WebPageCreationParameters;
}

namespace Messages {

namespace WebInspectorProxy {

enum Kind {
    CreateInspectorPageID,
    DidLoadInspectorPageID,
    DidCloseID,
    BringToFrontID,
    InspectedURLChangedID,
    AttachID,
    DetachID,
    SetAttachedWindowHeightID,
};

struct CreateInspectorPage : CoreIPC::Arguments0 {
    static const Kind messageID = CreateInspectorPageID;
    typedef CoreIPC::Arguments2<uint64_t&, WebKit::WebPageCreationParameters&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct DidLoadInspectorPage : CoreIPC::Arguments0 {
    static const Kind messageID = DidLoadInspectorPageID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct DidClose : CoreIPC::Arguments0 {
    static const Kind messageID = DidCloseID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct BringToFront : CoreIPC::Arguments0 {
    static const Kind messageID = BringToFrontID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct InspectedURLChanged : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = InspectedURLChangedID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit InspectedURLChanged(const WTF::String& urlString)
        : CoreIPC::Arguments1<const WTF::String&>(urlString)
    {
    }
};

struct Attach : CoreIPC::Arguments0 {
    static const Kind messageID = AttachID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct Detach : CoreIPC::Arguments0 {
    static const Kind messageID = DetachID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct SetAttachedWindowHeight : CoreIPC::Arguments1<const unsigned&> {
    static const Kind messageID = SetAttachedWindowHeightID;
    typedef CoreIPC::Arguments1<const unsigned&> DecodeType;
    explicit SetAttachedWindowHeight(const unsigned& height)
        : CoreIPC::Arguments1<const unsigned&>(height)
    {
    }
};

} // namespace WebInspectorProxy

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::WebInspectorProxy::Kind> {
    static const MessageClass messageClass = MessageClassWebInspectorProxy;
};

} // namespace CoreIPC

#endif // ENABLE(INSPECTOR)

#endif // WebInspectorProxyMessages_h
