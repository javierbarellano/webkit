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

#ifndef PluginProcessProxyMessages_h
#define PluginProcessProxyMessages_h

#if ENABLE(PLUGIN_PROCESS)

#include "Arguments.h"
#include "MessageID.h"
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace CoreIPC {
    class Attachment;
}

namespace Messages {

namespace PluginProcessProxy {

enum Kind {
    DidCreateWebProcessConnectionID,
    DidGetSitesWithDataID,
    DidClearSiteDataID,
#if PLATFORM(MAC)
    SetModalWindowIsShowingID,
#endif
#if PLATFORM(MAC)
    SetFullscreenWindowIsShowingID,
#endif
};

struct DidCreateWebProcessConnection : CoreIPC::Arguments1<const CoreIPC::Attachment&> {
    static const Kind messageID = DidCreateWebProcessConnectionID;
    typedef CoreIPC::Arguments1<const CoreIPC::Attachment&> DecodeType;
    explicit DidCreateWebProcessConnection(const CoreIPC::Attachment& connectionIdentifier)
        : CoreIPC::Arguments1<const CoreIPC::Attachment&>(connectionIdentifier)
    {
    }
};

struct DidGetSitesWithData : CoreIPC::Arguments2<const Vector<WTF::String>&, uint64_t> {
    static const Kind messageID = DidGetSitesWithDataID;
    typedef CoreIPC::Arguments2<const Vector<WTF::String>&, uint64_t> DecodeType;
    DidGetSitesWithData(const Vector<WTF::String>& sites, uint64_t callbackID)
        : CoreIPC::Arguments2<const Vector<WTF::String>&, uint64_t>(sites, callbackID)
    {
    }
};

struct DidClearSiteData : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = DidClearSiteDataID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit DidClearSiteData(uint64_t callbackID)
        : CoreIPC::Arguments1<uint64_t>(callbackID)
    {
    }
};

#if PLATFORM(MAC)
struct SetModalWindowIsShowing : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetModalWindowIsShowingID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetModalWindowIsShowing(bool modalWindowIsShowing)
        : CoreIPC::Arguments1<bool>(modalWindowIsShowing)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SetFullscreenWindowIsShowing : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetFullscreenWindowIsShowingID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetFullscreenWindowIsShowing(bool fullscreenWindowIsShowing)
        : CoreIPC::Arguments1<bool>(fullscreenWindowIsShowing)
    {
    }
};
#endif

} // namespace PluginProcessProxy

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::PluginProcessProxy::Kind> {
    static const MessageClass messageClass = MessageClassPluginProcessProxy;
};

} // namespace CoreIPC

#endif // ENABLE(PLUGIN_PROCESS)

#endif // PluginProcessProxyMessages_h
