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

#ifndef WebProcessProxyMessages_h
#define WebProcessProxyMessages_h

#include "Arguments.h"
#include "MessageID.h"
#include <wtf/ThreadSafeRefCounted.h>

namespace CoreIPC {
    class ArgumentEncoder;
    class Connection;
    class DataReference;
    class Attachment;
}

namespace WTF {
    class String;
}

namespace WebKit {
    class SecItemRequestData;
    class SecKeychainItemRequestData;
}

namespace Messages {

namespace WebProcessProxy {

enum Kind {
    AddBackForwardItemID,
    DidDestroyFrameID,
    ShouldTerminateID,
#if ENABLE(PLUGIN_PROCESS)
    GetPluginProcessConnectionID,
#endif
#if ENABLE(PLUGIN_PROCESS)
    PluginSyncMessageSendTimedOutID,
#endif
#if PLATFORM(MAC)
    SecItemRequestID,
#endif
#if PLATFORM(MAC)
    SecKeychainItemRequestID,
#endif
};

struct AddBackForwardItem : CoreIPC::Arguments5<uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, const CoreIPC::DataReference&> {
    static const Kind messageID = AddBackForwardItemID;
    typedef CoreIPC::Arguments5<uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, const CoreIPC::DataReference&> DecodeType;
    AddBackForwardItem(uint64_t itemID, const WTF::String& originalURL, const WTF::String& url, const WTF::String& title, const CoreIPC::DataReference& backForwardData)
        : CoreIPC::Arguments5<uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, const CoreIPC::DataReference&>(itemID, originalURL, url, title, backForwardData)
    {
    }
};

struct DidDestroyFrame : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = DidDestroyFrameID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit DidDestroyFrame(uint64_t frameID)
        : CoreIPC::Arguments1<uint64_t>(frameID)
    {
    }
};

struct ShouldTerminate : CoreIPC::Arguments0 {
    static const Kind messageID = ShouldTerminateID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

#if ENABLE(PLUGIN_PROCESS)
struct GetPluginProcessConnection : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = GetPluginProcessConnectionID;
    struct DelayedReply : public ThreadSafeRefCounted<DelayedReply> {
        DelayedReply(PassRefPtr<CoreIPC::Connection>, PassOwnPtr<CoreIPC::ArgumentEncoder>);
        ~DelayedReply();

        bool send(const CoreIPC::Attachment& connectionHandle);

    private:
        RefPtr<CoreIPC::Connection> m_connection;
        OwnPtr<CoreIPC::ArgumentEncoder> m_arguments;
    };

    typedef CoreIPC::Arguments1<CoreIPC::Attachment&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit GetPluginProcessConnection(const WTF::String& pluginPath)
        : CoreIPC::Arguments1<const WTF::String&>(pluginPath)
    {
    }
};
#endif

#if ENABLE(PLUGIN_PROCESS)
struct PluginSyncMessageSendTimedOut : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = PluginSyncMessageSendTimedOutID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit PluginSyncMessageSendTimedOut(const WTF::String& pluginPath)
        : CoreIPC::Arguments1<const WTF::String&>(pluginPath)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SecItemRequest : CoreIPC::Arguments2<uint64_t, const WebKit::SecItemRequestData&> {
    static const Kind messageID = SecItemRequestID;
    typedef CoreIPC::Arguments2<uint64_t, const WebKit::SecItemRequestData&> DecodeType;
    SecItemRequest(uint64_t requestID, const WebKit::SecItemRequestData& request)
        : CoreIPC::Arguments2<uint64_t, const WebKit::SecItemRequestData&>(requestID, request)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SecKeychainItemRequest : CoreIPC::Arguments2<uint64_t, const WebKit::SecKeychainItemRequestData&> {
    static const Kind messageID = SecKeychainItemRequestID;
    typedef CoreIPC::Arguments2<uint64_t, const WebKit::SecKeychainItemRequestData&> DecodeType;
    SecKeychainItemRequest(uint64_t requestID, const WebKit::SecKeychainItemRequestData& request)
        : CoreIPC::Arguments2<uint64_t, const WebKit::SecKeychainItemRequestData&>(requestID, request)
    {
    }
};
#endif

} // namespace WebProcessProxy

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::WebProcessProxy::Kind> {
    static const MessageClass messageClass = MessageClassWebProcessProxy;
};

} // namespace CoreIPC

#endif // WebProcessProxyMessages_h
