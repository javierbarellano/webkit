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

#ifndef NPObjectMessageReceiverMessages_h
#define NPObjectMessageReceiverMessages_h

#if ENABLE(PLUGIN_PROCESS)

#include "Arguments.h"
#include "MessageID.h"
#include "NPIdentifierData.h"
#include "NPVariantData.h"
#include <wtf/Vector.h>

namespace WebKit {
    class NPVariantData;
    class NPIdentifierData;
}

namespace Messages {

namespace NPObjectMessageReceiver {

enum Kind {
    DeallocateID,
    HasMethodID,
    InvokeID,
    InvokeDefaultID,
    HasPropertyID,
    GetPropertyID,
    SetPropertyID,
    RemovePropertyID,
    EnumerateID,
    ConstructID,
};

struct Deallocate : CoreIPC::Arguments0 {
    static const Kind messageID = DeallocateID;
    typedef CoreIPC::Arguments0 Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct HasMethod : CoreIPC::Arguments1<const WebKit::NPIdentifierData&> {
    static const Kind messageID = HasMethodID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<const WebKit::NPIdentifierData&> DecodeType;
    explicit HasMethod(const WebKit::NPIdentifierData& methodName)
        : CoreIPC::Arguments1<const WebKit::NPIdentifierData&>(methodName)
    {
    }
};

struct Invoke : CoreIPC::Arguments2<const WebKit::NPIdentifierData&, const Vector<WebKit::NPVariantData>&> {
    static const Kind messageID = InvokeID;
    typedef CoreIPC::Arguments2<bool&, WebKit::NPVariantData&> Reply;
    typedef CoreIPC::Arguments2<const WebKit::NPIdentifierData&, const Vector<WebKit::NPVariantData>&> DecodeType;
    Invoke(const WebKit::NPIdentifierData& methodName, const Vector<WebKit::NPVariantData>& argumentsData)
        : CoreIPC::Arguments2<const WebKit::NPIdentifierData&, const Vector<WebKit::NPVariantData>&>(methodName, argumentsData)
    {
    }
};

struct InvokeDefault : CoreIPC::Arguments1<const Vector<WebKit::NPVariantData>&> {
    static const Kind messageID = InvokeDefaultID;
    typedef CoreIPC::Arguments2<bool&, WebKit::NPVariantData&> Reply;
    typedef CoreIPC::Arguments1<const Vector<WebKit::NPVariantData>&> DecodeType;
    explicit InvokeDefault(const Vector<WebKit::NPVariantData>& argumentsData)
        : CoreIPC::Arguments1<const Vector<WebKit::NPVariantData>&>(argumentsData)
    {
    }
};

struct HasProperty : CoreIPC::Arguments1<const WebKit::NPIdentifierData&> {
    static const Kind messageID = HasPropertyID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<const WebKit::NPIdentifierData&> DecodeType;
    explicit HasProperty(const WebKit::NPIdentifierData& propertyName)
        : CoreIPC::Arguments1<const WebKit::NPIdentifierData&>(propertyName)
    {
    }
};

struct GetProperty : CoreIPC::Arguments1<const WebKit::NPIdentifierData&> {
    static const Kind messageID = GetPropertyID;
    typedef CoreIPC::Arguments2<bool&, WebKit::NPVariantData&> Reply;
    typedef CoreIPC::Arguments1<const WebKit::NPIdentifierData&> DecodeType;
    explicit GetProperty(const WebKit::NPIdentifierData& propertyName)
        : CoreIPC::Arguments1<const WebKit::NPIdentifierData&>(propertyName)
    {
    }
};

struct SetProperty : CoreIPC::Arguments2<const WebKit::NPIdentifierData&, const WebKit::NPVariantData&> {
    static const Kind messageID = SetPropertyID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments2<const WebKit::NPIdentifierData&, const WebKit::NPVariantData&> DecodeType;
    SetProperty(const WebKit::NPIdentifierData& propertyName, const WebKit::NPVariantData& propertyValueData)
        : CoreIPC::Arguments2<const WebKit::NPIdentifierData&, const WebKit::NPVariantData&>(propertyName, propertyValueData)
    {
    }
};

struct RemoveProperty : CoreIPC::Arguments1<const WebKit::NPIdentifierData&> {
    static const Kind messageID = RemovePropertyID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<const WebKit::NPIdentifierData&> DecodeType;
    explicit RemoveProperty(const WebKit::NPIdentifierData& propertyName)
        : CoreIPC::Arguments1<const WebKit::NPIdentifierData&>(propertyName)
    {
    }
};

struct Enumerate : CoreIPC::Arguments0 {
    static const Kind messageID = EnumerateID;
    typedef CoreIPC::Arguments2<bool&, Vector<WebKit::NPIdentifierData>&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct Construct : CoreIPC::Arguments1<const Vector<WebKit::NPVariantData>&> {
    static const Kind messageID = ConstructID;
    typedef CoreIPC::Arguments2<bool&, WebKit::NPVariantData&> Reply;
    typedef CoreIPC::Arguments1<const Vector<WebKit::NPVariantData>&> DecodeType;
    explicit Construct(const Vector<WebKit::NPVariantData>& argumentsData)
        : CoreIPC::Arguments1<const Vector<WebKit::NPVariantData>&>(argumentsData)
    {
    }
};

} // namespace NPObjectMessageReceiver

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::NPObjectMessageReceiver::Kind> {
    static const MessageClass messageClass = MessageClassNPObjectMessageReceiver;
};

} // namespace CoreIPC

#endif // ENABLE(PLUGIN_PROCESS)

#endif // NPObjectMessageReceiverMessages_h
