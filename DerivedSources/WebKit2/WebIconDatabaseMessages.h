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

#ifndef WebIconDatabaseMessages_h
#define WebIconDatabaseMessages_h

#include "Arguments.h"
#include "MessageID.h"

namespace CoreIPC {
    class DataReference;
}

namespace WTF {
    class String;
}

namespace Messages {

namespace WebIconDatabase {

enum Kind {
    RetainIconForPageURLID,
    ReleaseIconForPageURLID,
    SetIconURLForPageURLID,
    SetIconDataForIconURLID,
    SynchronousIconDataForPageURLID,
    SynchronousIconURLForPageURLID,
    SynchronousIconDataKnownForIconURLID,
    SynchronousLoadDecisionForIconURLID,
    GetLoadDecisionForIconURLID,
};

struct RetainIconForPageURL : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = RetainIconForPageURLID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit RetainIconForPageURL(const WTF::String& pageURL)
        : CoreIPC::Arguments1<const WTF::String&>(pageURL)
    {
    }
};

struct ReleaseIconForPageURL : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = ReleaseIconForPageURLID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit ReleaseIconForPageURL(const WTF::String& pageURL)
        : CoreIPC::Arguments1<const WTF::String&>(pageURL)
    {
    }
};

struct SetIconURLForPageURL : CoreIPC::Arguments2<const WTF::String&, const WTF::String&> {
    static const Kind messageID = SetIconURLForPageURLID;
    typedef CoreIPC::Arguments2<const WTF::String&, const WTF::String&> DecodeType;
    SetIconURLForPageURL(const WTF::String& iconURL, const WTF::String& pageURL)
        : CoreIPC::Arguments2<const WTF::String&, const WTF::String&>(iconURL, pageURL)
    {
    }
};

struct SetIconDataForIconURL : CoreIPC::Arguments2<const CoreIPC::DataReference&, const WTF::String&> {
    static const Kind messageID = SetIconDataForIconURLID;
    typedef CoreIPC::Arguments2<const CoreIPC::DataReference&, const WTF::String&> DecodeType;
    SetIconDataForIconURL(const CoreIPC::DataReference& iconData, const WTF::String& iconURL)
        : CoreIPC::Arguments2<const CoreIPC::DataReference&, const WTF::String&>(iconData, iconURL)
    {
    }
};

struct SynchronousIconDataForPageURL : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = SynchronousIconDataForPageURLID;
    typedef CoreIPC::Arguments1<CoreIPC::DataReference&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit SynchronousIconDataForPageURL(const WTF::String& pageURL)
        : CoreIPC::Arguments1<const WTF::String&>(pageURL)
    {
    }
};

struct SynchronousIconURLForPageURL : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = SynchronousIconURLForPageURLID;
    typedef CoreIPC::Arguments1<WTF::String&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit SynchronousIconURLForPageURL(const WTF::String& pageURL)
        : CoreIPC::Arguments1<const WTF::String&>(pageURL)
    {
    }
};

struct SynchronousIconDataKnownForIconURL : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = SynchronousIconDataKnownForIconURLID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit SynchronousIconDataKnownForIconURL(const WTF::String& iconURL)
        : CoreIPC::Arguments1<const WTF::String&>(iconURL)
    {
    }
};

struct SynchronousLoadDecisionForIconURL : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = SynchronousLoadDecisionForIconURLID;
    typedef CoreIPC::Arguments1<int&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit SynchronousLoadDecisionForIconURL(const WTF::String& iconURL)
        : CoreIPC::Arguments1<const WTF::String&>(iconURL)
    {
    }
};

struct GetLoadDecisionForIconURL : CoreIPC::Arguments2<const WTF::String&, uint64_t> {
    static const Kind messageID = GetLoadDecisionForIconURLID;
    typedef CoreIPC::Arguments2<const WTF::String&, uint64_t> DecodeType;
    GetLoadDecisionForIconURL(const WTF::String& iconURL, uint64_t callbackID)
        : CoreIPC::Arguments2<const WTF::String&, uint64_t>(iconURL, callbackID)
    {
    }
};

} // namespace WebIconDatabase

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::WebIconDatabase::Kind> {
    static const MessageClass messageClass = MessageClassWebIconDatabase;
};

} // namespace CoreIPC

#endif // WebIconDatabaseMessages_h
