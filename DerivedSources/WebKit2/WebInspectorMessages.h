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

#ifndef WebInspectorMessages_h
#define WebInspectorMessages_h

#if ENABLE(INSPECTOR)

#include "Arguments.h"
#include "MessageID.h"


namespace Messages {

namespace WebInspector {

enum Kind {
    ShowID,
    CloseID,
    ShowConsoleID,
    ShowResourcesID,
    ShowMainResourceForFrameID,
    StartJavaScriptDebuggingID,
    StopJavaScriptDebuggingID,
    StartJavaScriptProfilingID,
    StopJavaScriptProfilingID,
    StartPageProfilingID,
    StopPageProfilingID,
};

struct Show : CoreIPC::Arguments0 {
    static const Kind messageID = ShowID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct Close : CoreIPC::Arguments0 {
    static const Kind messageID = CloseID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct ShowConsole : CoreIPC::Arguments0 {
    static const Kind messageID = ShowConsoleID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct ShowResources : CoreIPC::Arguments0 {
    static const Kind messageID = ShowResourcesID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct ShowMainResourceForFrame : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = ShowMainResourceForFrameID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit ShowMainResourceForFrame(uint64_t frameID)
        : CoreIPC::Arguments1<uint64_t>(frameID)
    {
    }
};

struct StartJavaScriptDebugging : CoreIPC::Arguments0 {
    static const Kind messageID = StartJavaScriptDebuggingID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct StopJavaScriptDebugging : CoreIPC::Arguments0 {
    static const Kind messageID = StopJavaScriptDebuggingID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct StartJavaScriptProfiling : CoreIPC::Arguments0 {
    static const Kind messageID = StartJavaScriptProfilingID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct StopJavaScriptProfiling : CoreIPC::Arguments0 {
    static const Kind messageID = StopJavaScriptProfilingID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct StartPageProfiling : CoreIPC::Arguments0 {
    static const Kind messageID = StartPageProfilingID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct StopPageProfiling : CoreIPC::Arguments0 {
    static const Kind messageID = StopPageProfilingID;
    typedef CoreIPC::Arguments0 DecodeType;
};

} // namespace WebInspector

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::WebInspector::Kind> {
    static const MessageClass messageClass = MessageClassWebInspector;
};

} // namespace CoreIPC

#endif // ENABLE(INSPECTOR)

#endif // WebInspectorMessages_h
