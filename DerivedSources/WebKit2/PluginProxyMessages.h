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

#ifndef PluginProxyMessages_h
#define PluginProxyMessages_h

#if ENABLE(PLUGIN_PROCESS)

#include "Arguments.h"
#include "MessageID.h"
#include <wtf/Vector.h>

namespace WTF {
    class String;
}

namespace WebCore {
    class ProtectionSpace;
    class IntRect;
    class HTTPHeaderMap;
}

namespace WebKit {
    class NPVariantData;
}

namespace Messages {

namespace PluginProxy {

enum Kind {
    LoadURLID,
    UpdateID,
    ProxiesForURLID,
    CookiesForURLID,
    SetCookiesForURLID,
    GetAuthenticationInfoID,
    GetPluginElementNPObjectID,
    EvaluateID,
    CancelStreamLoadID,
    CancelManualStreamLoadID,
    SetStatusbarTextID,
#if PLATFORM(MAC)
    PluginFocusOrWindowFocusChangedID,
#endif
#if PLATFORM(MAC)
    SetComplexTextInputStateID,
#endif
#if PLATFORM(MAC)
    SetLayerHostingContextIDID,
#endif
};

struct LoadURL : CoreIPC::Arguments7<uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, const WebCore::HTTPHeaderMap&, const Vector<uint8_t>&, bool> {
    static const Kind messageID = LoadURLID;
    typedef CoreIPC::Arguments7<uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, const WebCore::HTTPHeaderMap&, const Vector<uint8_t>&, bool> DecodeType;
    LoadURL(uint64_t requestID, const WTF::String& method, const WTF::String& urlString, const WTF::String& target, const WebCore::HTTPHeaderMap& headerFields, const Vector<uint8_t>& httpBody, bool allowPopups)
        : CoreIPC::Arguments7<uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, const WebCore::HTTPHeaderMap&, const Vector<uint8_t>&, bool>(requestID, method, urlString, target, headerFields, httpBody, allowPopups)
    {
    }
};

struct Update : CoreIPC::Arguments1<const WebCore::IntRect&> {
    static const Kind messageID = UpdateID;
    typedef CoreIPC::Arguments1<const WebCore::IntRect&> DecodeType;
    explicit Update(const WebCore::IntRect& paintedRect)
        : CoreIPC::Arguments1<const WebCore::IntRect&>(paintedRect)
    {
    }
};

struct ProxiesForURL : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = ProxiesForURLID;
    typedef CoreIPC::Arguments1<WTF::String&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit ProxiesForURL(const WTF::String& urlString)
        : CoreIPC::Arguments1<const WTF::String&>(urlString)
    {
    }
};

struct CookiesForURL : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = CookiesForURLID;
    typedef CoreIPC::Arguments1<WTF::String&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit CookiesForURL(const WTF::String& urlString)
        : CoreIPC::Arguments1<const WTF::String&>(urlString)
    {
    }
};

struct SetCookiesForURL : CoreIPC::Arguments2<const WTF::String&, const WTF::String&> {
    static const Kind messageID = SetCookiesForURLID;
    typedef CoreIPC::Arguments2<const WTF::String&, const WTF::String&> DecodeType;
    SetCookiesForURL(const WTF::String& urlString, const WTF::String& cookieString)
        : CoreIPC::Arguments2<const WTF::String&, const WTF::String&>(urlString, cookieString)
    {
    }
};

struct GetAuthenticationInfo : CoreIPC::Arguments1<const WebCore::ProtectionSpace&> {
    static const Kind messageID = GetAuthenticationInfoID;
    typedef CoreIPC::Arguments3<bool&, WTF::String&, WTF::String&> Reply;
    typedef CoreIPC::Arguments1<const WebCore::ProtectionSpace&> DecodeType;
    explicit GetAuthenticationInfo(const WebCore::ProtectionSpace& protectionSpace)
        : CoreIPC::Arguments1<const WebCore::ProtectionSpace&>(protectionSpace)
    {
    }
};

struct GetPluginElementNPObject : CoreIPC::Arguments0 {
    static const Kind messageID = GetPluginElementNPObjectID;
    typedef CoreIPC::Arguments1<uint64_t&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct Evaluate : CoreIPC::Arguments3<const WebKit::NPVariantData&, const WTF::String&, bool> {
    static const Kind messageID = EvaluateID;
    typedef CoreIPC::Arguments2<bool&, WebKit::NPVariantData&> Reply;
    typedef CoreIPC::Arguments3<const WebKit::NPVariantData&, const WTF::String&, bool> DecodeType;
    Evaluate(const WebKit::NPVariantData& npObjectAsVariantData, const WTF::String& scriptString, bool allowPopups)
        : CoreIPC::Arguments3<const WebKit::NPVariantData&, const WTF::String&, bool>(npObjectAsVariantData, scriptString, allowPopups)
    {
    }
};

struct CancelStreamLoad : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = CancelStreamLoadID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit CancelStreamLoad(uint64_t streamID)
        : CoreIPC::Arguments1<uint64_t>(streamID)
    {
    }
};

struct CancelManualStreamLoad : CoreIPC::Arguments0 {
    static const Kind messageID = CancelManualStreamLoadID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct SetStatusbarText : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = SetStatusbarTextID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit SetStatusbarText(const WTF::String& statusbarText)
        : CoreIPC::Arguments1<const WTF::String&>(statusbarText)
    {
    }
};

#if PLATFORM(MAC)
struct PluginFocusOrWindowFocusChanged : CoreIPC::Arguments1<bool> {
    static const Kind messageID = PluginFocusOrWindowFocusChangedID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit PluginFocusOrWindowFocusChanged(bool pluginHasFocusAndWindowHasFocus)
        : CoreIPC::Arguments1<bool>(pluginHasFocusAndWindowHasFocus)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SetComplexTextInputState : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = SetComplexTextInputStateID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit SetComplexTextInputState(uint64_t complexTextInputState)
        : CoreIPC::Arguments1<uint64_t>(complexTextInputState)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SetLayerHostingContextID : CoreIPC::Arguments1<uint32_t> {
    static const Kind messageID = SetLayerHostingContextIDID;
    typedef CoreIPC::Arguments1<uint32_t> DecodeType;
    explicit SetLayerHostingContextID(uint32_t layerHostingContextID)
        : CoreIPC::Arguments1<uint32_t>(layerHostingContextID)
    {
    }
};
#endif

} // namespace PluginProxy

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::PluginProxy::Kind> {
    static const MessageClass messageClass = MessageClassPluginProxy;
};

} // namespace CoreIPC

#endif // ENABLE(PLUGIN_PROCESS)

#endif // PluginProxyMessages_h
