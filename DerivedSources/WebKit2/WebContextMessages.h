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

#ifndef WebContextMessages_h
#define WebContextMessages_h

#include "Arguments.h"
#include "MessageID.h"
#include "SharedMemory.h"
#include <WebCore/PluginData.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WTF {
    class String;
}

namespace WebCore {
    class Color;
}

namespace WebKit {
    struct WebNavigationDataStore;
    struct StatisticsData;
}

namespace Messages {

namespace WebContext {

enum Kind {
    DidNavigateWithNavigationDataID,
    DidPerformClientRedirectID,
    DidPerformServerRedirectID,
    DidUpdateHistoryTitleID,
    AddVisitedLinkHashID,
    GetPluginsID,
    GetPluginPathID,
#if !ENABLE(PLUGIN_PROCESS)
    DidGetSitesWithPluginDataID,
#endif
#if !ENABLE(PLUGIN_PROCESS)
    DidClearPluginSiteDataID,
#endif
    DidGetWebCoreStatisticsID,
#if PLATFORM(MAC)
    GetPasteboardTypesID,
#endif
#if PLATFORM(MAC)
    GetPasteboardPathnamesForTypeID,
#endif
#if PLATFORM(MAC)
    GetPasteboardStringForTypeID,
#endif
#if PLATFORM(MAC)
    GetPasteboardBufferForTypeID,
#endif
#if PLATFORM(MAC)
    PasteboardCopyID,
#endif
#if PLATFORM(MAC)
    GetPasteboardChangeCountID,
#endif
#if PLATFORM(MAC)
    GetPasteboardUniqueNameID,
#endif
#if PLATFORM(MAC)
    GetPasteboardColorID,
#endif
#if PLATFORM(MAC)
    GetPasteboardURLID,
#endif
#if PLATFORM(MAC)
    AddPasteboardTypesID,
#endif
#if PLATFORM(MAC)
    SetPasteboardTypesID,
#endif
#if PLATFORM(MAC)
    SetPasteboardPathnamesForTypeID,
#endif
#if PLATFORM(MAC)
    SetPasteboardStringForTypeID,
#endif
#if PLATFORM(MAC)
    SetPasteboardBufferForTypeID,
#endif
};

struct DidNavigateWithNavigationData : CoreIPC::Arguments3<uint64_t, const WebKit::WebNavigationDataStore&, uint64_t> {
    static const Kind messageID = DidNavigateWithNavigationDataID;
    typedef CoreIPC::Arguments3<uint64_t, const WebKit::WebNavigationDataStore&, uint64_t> DecodeType;
    DidNavigateWithNavigationData(uint64_t pageID, const WebKit::WebNavigationDataStore& store, uint64_t frameID)
        : CoreIPC::Arguments3<uint64_t, const WebKit::WebNavigationDataStore&, uint64_t>(pageID, store, frameID)
    {
    }
};

struct DidPerformClientRedirect : CoreIPC::Arguments4<uint64_t, const WTF::String&, const WTF::String&, uint64_t> {
    static const Kind messageID = DidPerformClientRedirectID;
    typedef CoreIPC::Arguments4<uint64_t, const WTF::String&, const WTF::String&, uint64_t> DecodeType;
    DidPerformClientRedirect(uint64_t pageID, const WTF::String& sourceURLString, const WTF::String& destinationURLString, uint64_t frameID)
        : CoreIPC::Arguments4<uint64_t, const WTF::String&, const WTF::String&, uint64_t>(pageID, sourceURLString, destinationURLString, frameID)
    {
    }
};

struct DidPerformServerRedirect : CoreIPC::Arguments4<uint64_t, const WTF::String&, const WTF::String&, uint64_t> {
    static const Kind messageID = DidPerformServerRedirectID;
    typedef CoreIPC::Arguments4<uint64_t, const WTF::String&, const WTF::String&, uint64_t> DecodeType;
    DidPerformServerRedirect(uint64_t pageID, const WTF::String& sourceURLString, const WTF::String& destinationURLString, uint64_t frameID)
        : CoreIPC::Arguments4<uint64_t, const WTF::String&, const WTF::String&, uint64_t>(pageID, sourceURLString, destinationURLString, frameID)
    {
    }
};

struct DidUpdateHistoryTitle : CoreIPC::Arguments4<uint64_t, const WTF::String&, const WTF::String&, uint64_t> {
    static const Kind messageID = DidUpdateHistoryTitleID;
    typedef CoreIPC::Arguments4<uint64_t, const WTF::String&, const WTF::String&, uint64_t> DecodeType;
    DidUpdateHistoryTitle(uint64_t pageID, const WTF::String& title, const WTF::String& url, uint64_t frameID)
        : CoreIPC::Arguments4<uint64_t, const WTF::String&, const WTF::String&, uint64_t>(pageID, title, url, frameID)
    {
    }
};

struct AddVisitedLinkHash : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = AddVisitedLinkHashID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit AddVisitedLinkHash(uint64_t linkHash)
        : CoreIPC::Arguments1<uint64_t>(linkHash)
    {
    }
};

struct GetPlugins : CoreIPC::Arguments1<bool> {
    static const Kind messageID = GetPluginsID;
    typedef CoreIPC::Arguments1<Vector<WebCore::PluginInfo>&> Reply;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit GetPlugins(bool refresh)
        : CoreIPC::Arguments1<bool>(refresh)
    {
    }
};

struct GetPluginPath : CoreIPC::Arguments2<const WTF::String&, const WTF::String&> {
    static const Kind messageID = GetPluginPathID;
    typedef CoreIPC::Arguments1<WTF::String&> Reply;
    typedef CoreIPC::Arguments2<const WTF::String&, const WTF::String&> DecodeType;
    GetPluginPath(const WTF::String& mimeType, const WTF::String& urlString)
        : CoreIPC::Arguments2<const WTF::String&, const WTF::String&>(mimeType, urlString)
    {
    }
};

#if !ENABLE(PLUGIN_PROCESS)
struct DidGetSitesWithPluginData : CoreIPC::Arguments2<const Vector<WTF::String>&, uint64_t> {
    static const Kind messageID = DidGetSitesWithPluginDataID;
    typedef CoreIPC::Arguments2<const Vector<WTF::String>&, uint64_t> DecodeType;
    DidGetSitesWithPluginData(const Vector<WTF::String>& sites, uint64_t callbackID)
        : CoreIPC::Arguments2<const Vector<WTF::String>&, uint64_t>(sites, callbackID)
    {
    }
};
#endif

#if !ENABLE(PLUGIN_PROCESS)
struct DidClearPluginSiteData : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = DidClearPluginSiteDataID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit DidClearPluginSiteData(uint64_t callbackID)
        : CoreIPC::Arguments1<uint64_t>(callbackID)
    {
    }
};
#endif

struct DidGetWebCoreStatistics : CoreIPC::Arguments2<const WebKit::StatisticsData&, uint64_t> {
    static const Kind messageID = DidGetWebCoreStatisticsID;
    typedef CoreIPC::Arguments2<const WebKit::StatisticsData&, uint64_t> DecodeType;
    DidGetWebCoreStatistics(const WebKit::StatisticsData& statisticsData, uint64_t callbackID)
        : CoreIPC::Arguments2<const WebKit::StatisticsData&, uint64_t>(statisticsData, callbackID)
    {
    }
};

#if PLATFORM(MAC)
struct GetPasteboardTypes : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = GetPasteboardTypesID;
    typedef CoreIPC::Arguments1<Vector<WTF::String>&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit GetPasteboardTypes(const WTF::String& pasteboardName)
        : CoreIPC::Arguments1<const WTF::String&>(pasteboardName)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct GetPasteboardPathnamesForType : CoreIPC::Arguments2<const WTF::String&, const WTF::String&> {
    static const Kind messageID = GetPasteboardPathnamesForTypeID;
    typedef CoreIPC::Arguments1<Vector<WTF::String>&> Reply;
    typedef CoreIPC::Arguments2<const WTF::String&, const WTF::String&> DecodeType;
    GetPasteboardPathnamesForType(const WTF::String& pasteboardName, const WTF::String& pasteboardType)
        : CoreIPC::Arguments2<const WTF::String&, const WTF::String&>(pasteboardName, pasteboardType)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct GetPasteboardStringForType : CoreIPC::Arguments2<const WTF::String&, const WTF::String&> {
    static const Kind messageID = GetPasteboardStringForTypeID;
    typedef CoreIPC::Arguments1<WTF::String&> Reply;
    typedef CoreIPC::Arguments2<const WTF::String&, const WTF::String&> DecodeType;
    GetPasteboardStringForType(const WTF::String& pasteboardName, const WTF::String& pasteboardType)
        : CoreIPC::Arguments2<const WTF::String&, const WTF::String&>(pasteboardName, pasteboardType)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct GetPasteboardBufferForType : CoreIPC::Arguments2<const WTF::String&, const WTF::String&> {
    static const Kind messageID = GetPasteboardBufferForTypeID;
    typedef CoreIPC::Arguments2<WebKit::SharedMemory::Handle&, uint64_t&> Reply;
    typedef CoreIPC::Arguments2<const WTF::String&, const WTF::String&> DecodeType;
    GetPasteboardBufferForType(const WTF::String& pasteboardName, const WTF::String& pasteboardType)
        : CoreIPC::Arguments2<const WTF::String&, const WTF::String&>(pasteboardName, pasteboardType)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct PasteboardCopy : CoreIPC::Arguments2<const WTF::String&, const WTF::String&> {
    static const Kind messageID = PasteboardCopyID;
    typedef CoreIPC::Arguments2<const WTF::String&, const WTF::String&> DecodeType;
    PasteboardCopy(const WTF::String& fromPasteboard, const WTF::String& toPasteboard)
        : CoreIPC::Arguments2<const WTF::String&, const WTF::String&>(fromPasteboard, toPasteboard)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct GetPasteboardChangeCount : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = GetPasteboardChangeCountID;
    typedef CoreIPC::Arguments1<uint64_t&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit GetPasteboardChangeCount(const WTF::String& pasteboardName)
        : CoreIPC::Arguments1<const WTF::String&>(pasteboardName)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct GetPasteboardUniqueName : CoreIPC::Arguments0 {
    static const Kind messageID = GetPasteboardUniqueNameID;
    typedef CoreIPC::Arguments1<WTF::String&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};
#endif

#if PLATFORM(MAC)
struct GetPasteboardColor : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = GetPasteboardColorID;
    typedef CoreIPC::Arguments1<WebCore::Color&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit GetPasteboardColor(const WTF::String& pasteboardName)
        : CoreIPC::Arguments1<const WTF::String&>(pasteboardName)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct GetPasteboardURL : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = GetPasteboardURLID;
    typedef CoreIPC::Arguments1<WTF::String&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit GetPasteboardURL(const WTF::String& pasteboardName)
        : CoreIPC::Arguments1<const WTF::String&>(pasteboardName)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct AddPasteboardTypes : CoreIPC::Arguments2<const WTF::String&, const Vector<WTF::String>&> {
    static const Kind messageID = AddPasteboardTypesID;
    typedef CoreIPC::Arguments2<const WTF::String&, const Vector<WTF::String>&> DecodeType;
    AddPasteboardTypes(const WTF::String& pasteboardName, const Vector<WTF::String>& pasteboardTypes)
        : CoreIPC::Arguments2<const WTF::String&, const Vector<WTF::String>&>(pasteboardName, pasteboardTypes)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SetPasteboardTypes : CoreIPC::Arguments2<const WTF::String&, const Vector<WTF::String>&> {
    static const Kind messageID = SetPasteboardTypesID;
    typedef CoreIPC::Arguments2<const WTF::String&, const Vector<WTF::String>&> DecodeType;
    SetPasteboardTypes(const WTF::String& pasteboardName, const Vector<WTF::String>& pasteboardTypes)
        : CoreIPC::Arguments2<const WTF::String&, const Vector<WTF::String>&>(pasteboardName, pasteboardTypes)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SetPasteboardPathnamesForType : CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const Vector<WTF::String>&> {
    static const Kind messageID = SetPasteboardPathnamesForTypeID;
    typedef CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const Vector<WTF::String>&> DecodeType;
    SetPasteboardPathnamesForType(const WTF::String& pasteboardName, const WTF::String& pasteboardType, const Vector<WTF::String>& pathnames)
        : CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const Vector<WTF::String>&>(pasteboardName, pasteboardType, pathnames)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SetPasteboardStringForType : CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const WTF::String&> {
    static const Kind messageID = SetPasteboardStringForTypeID;
    typedef CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const WTF::String&> DecodeType;
    SetPasteboardStringForType(const WTF::String& pasteboardName, const WTF::String& pasteboardType, const WTF::String& string)
        : CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const WTF::String&>(pasteboardName, pasteboardType, string)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SetPasteboardBufferForType : CoreIPC::Arguments4<const WTF::String&, const WTF::String&, const WebKit::SharedMemory::Handle&, uint64_t> {
    static const Kind messageID = SetPasteboardBufferForTypeID;
    typedef CoreIPC::Arguments4<const WTF::String&, const WTF::String&, const WebKit::SharedMemory::Handle&, uint64_t> DecodeType;
    SetPasteboardBufferForType(const WTF::String& pasteboardName, const WTF::String& pasteboardType, const WebKit::SharedMemory::Handle& handle, uint64_t size)
        : CoreIPC::Arguments4<const WTF::String&, const WTF::String&, const WebKit::SharedMemory::Handle&, uint64_t>(pasteboardName, pasteboardType, handle, size)
    {
    }
};
#endif

} // namespace WebContext

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::WebContext::Kind> {
    static const MessageClass messageClass = MessageClassWebContext;
};

} // namespace CoreIPC

#endif // WebContextMessages_h
