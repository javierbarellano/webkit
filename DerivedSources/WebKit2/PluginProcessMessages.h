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

#ifndef PluginProcessMessages_h
#define PluginProcessMessages_h

#if ENABLE(PLUGIN_PROCESS)

#include "Arguments.h"
#include "MessageID.h"
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebKit {
    struct PluginProcessCreationParameters;
}

namespace Messages {

namespace PluginProcess {

enum Kind {
    InitializePluginProcessID,
    CreateWebProcessConnectionID,
    GetSitesWithDataID,
    ClearSiteDataID,
};

struct InitializePluginProcess : CoreIPC::Arguments1<const WebKit::PluginProcessCreationParameters&> {
    static const Kind messageID = InitializePluginProcessID;
    typedef CoreIPC::Arguments1<const WebKit::PluginProcessCreationParameters&> DecodeType;
    explicit InitializePluginProcess(const WebKit::PluginProcessCreationParameters& processCreationParameters)
        : CoreIPC::Arguments1<const WebKit::PluginProcessCreationParameters&>(processCreationParameters)
    {
    }
};

struct CreateWebProcessConnection : CoreIPC::Arguments0 {
    static const Kind messageID = CreateWebProcessConnectionID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct GetSitesWithData : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = GetSitesWithDataID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit GetSitesWithData(uint64_t callbackID)
        : CoreIPC::Arguments1<uint64_t>(callbackID)
    {
    }
};

struct ClearSiteData : CoreIPC::Arguments4<const Vector<WTF::String>&, uint64_t, uint64_t, uint64_t> {
    static const Kind messageID = ClearSiteDataID;
    typedef CoreIPC::Arguments4<const Vector<WTF::String>&, uint64_t, uint64_t, uint64_t> DecodeType;
    ClearSiteData(const Vector<WTF::String>& sites, uint64_t flags, uint64_t maxAgeInSeconds, uint64_t callbackID)
        : CoreIPC::Arguments4<const Vector<WTF::String>&, uint64_t, uint64_t, uint64_t>(sites, flags, maxAgeInSeconds, callbackID)
    {
    }
};

} // namespace PluginProcess

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::PluginProcess::Kind> {
    static const MessageClass messageClass = MessageClassPluginProcess;
};

} // namespace CoreIPC

#endif // ENABLE(PLUGIN_PROCESS)

#endif // PluginProcessMessages_h
