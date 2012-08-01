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

#ifndef WebProcessMessages_h
#define WebProcessMessages_h

#include "Arguments.h"
#include "MessageID.h"
#include "SandboxExtension.h"
#include "SharedMemory.h"
#include <WebCore/LinkHash.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WTF {
    class String;
}

namespace WebCore {
    class ResourceRequest;
}

namespace WebKit {
    class WebContextUserMessageEncoder;
    struct WebProcessCreationParameters;
    struct TextCheckerState;
    class SecItemResponseData;
    class SecKeychainItemResponseData;
    struct WebPageCreationParameters;
}

namespace Messages {

namespace WebProcess {

enum Kind {
    InitializeWebProcessID,
    CreateWebPageID,
    SetVisitedLinkTableID,
    VisitedLinkStateChangedID,
    AllVisitedLinkStateChangedID,
    SetShouldTrackVisitedLinksID,
    SetCacheModelID,
    RegisterURLSchemeAsEmptyDocumentID,
    RegisterURLSchemeAsSecureID,
    SetDomainRelaxationForbiddenForURLSchemeID,
    SetDefaultRequestTimeoutIntervalID,
    SetAlwaysUsesComplexTextCodePathID,
    SetShouldUseFontSmoothingID,
    UserPreferredLanguagesChangedID,
    FullKeyboardAccessModeChangedID,
#if PLATFORM(WIN)
    SetShouldPaintNativeControlsID,
#endif
#if !ENABLE(PLUGIN_PROCESS)
    GetSitesWithPluginDataID,
#endif
#if !ENABLE(PLUGIN_PROCESS)
    ClearPluginSiteDataID,
#endif
#if ENABLE(PLUGIN_PROCESS)
    PluginProcessCrashedID,
#endif
    StartMemorySamplerID,
    StopMemorySamplerID,
    DownloadRequestID,
    CancelDownloadID,
#if PLATFORM(QT)
    StartTransferID,
#endif
    SetTextCheckerStateID,
    SetEnhancedAccessibilityID,
    GetWebCoreStatisticsID,
    GarbageCollectJavaScriptObjectsID,
#if PLATFORM(MAC)
    SecItemResponseID,
#endif
#if PLATFORM(MAC)
    SecKeychainItemResponseID,
#endif
};

struct InitializeWebProcess : CoreIPC::Arguments2<const WebKit::WebProcessCreationParameters&, const WebKit::WebContextUserMessageEncoder&> {
    static const Kind messageID = InitializeWebProcessID;
    typedef CoreIPC::Arguments1<WebKit::WebProcessCreationParameters&> DecodeType;
    InitializeWebProcess(const WebKit::WebProcessCreationParameters& processCreationParameters, const WebKit::WebContextUserMessageEncoder& initializationUserData)
        : CoreIPC::Arguments2<const WebKit::WebProcessCreationParameters&, const WebKit::WebContextUserMessageEncoder&>(processCreationParameters, initializationUserData)
    {
    }
};

struct CreateWebPage : CoreIPC::Arguments2<uint64_t, const WebKit::WebPageCreationParameters&> {
    static const Kind messageID = CreateWebPageID;
    typedef CoreIPC::Arguments2<uint64_t, const WebKit::WebPageCreationParameters&> DecodeType;
    CreateWebPage(uint64_t newPageID, const WebKit::WebPageCreationParameters& pageCreationParameters)
        : CoreIPC::Arguments2<uint64_t, const WebKit::WebPageCreationParameters&>(newPageID, pageCreationParameters)
    {
    }
};

struct SetVisitedLinkTable : CoreIPC::Arguments1<const WebKit::SharedMemory::Handle&> {
    static const Kind messageID = SetVisitedLinkTableID;
    typedef CoreIPC::Arguments1<const WebKit::SharedMemory::Handle&> DecodeType;
    explicit SetVisitedLinkTable(const WebKit::SharedMemory::Handle& handle)
        : CoreIPC::Arguments1<const WebKit::SharedMemory::Handle&>(handle)
    {
    }
};

struct VisitedLinkStateChanged : CoreIPC::Arguments1<const Vector<WebCore::LinkHash>&> {
    static const Kind messageID = VisitedLinkStateChangedID;
    typedef CoreIPC::Arguments1<const Vector<WebCore::LinkHash>&> DecodeType;
    explicit VisitedLinkStateChanged(const Vector<WebCore::LinkHash>& linkHashes)
        : CoreIPC::Arguments1<const Vector<WebCore::LinkHash>&>(linkHashes)
    {
    }
};

struct AllVisitedLinkStateChanged : CoreIPC::Arguments0 {
    static const Kind messageID = AllVisitedLinkStateChangedID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct SetShouldTrackVisitedLinks : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetShouldTrackVisitedLinksID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetShouldTrackVisitedLinks(bool shouldTrackVisitedLinks)
        : CoreIPC::Arguments1<bool>(shouldTrackVisitedLinks)
    {
    }
};

struct SetCacheModel : CoreIPC::Arguments1<uint32_t> {
    static const Kind messageID = SetCacheModelID;
    typedef CoreIPC::Arguments1<uint32_t> DecodeType;
    explicit SetCacheModel(uint32_t cacheModel)
        : CoreIPC::Arguments1<uint32_t>(cacheModel)
    {
    }
};

struct RegisterURLSchemeAsEmptyDocument : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = RegisterURLSchemeAsEmptyDocumentID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit RegisterURLSchemeAsEmptyDocument(const WTF::String& scheme)
        : CoreIPC::Arguments1<const WTF::String&>(scheme)
    {
    }
};

struct RegisterURLSchemeAsSecure : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = RegisterURLSchemeAsSecureID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit RegisterURLSchemeAsSecure(const WTF::String& scheme)
        : CoreIPC::Arguments1<const WTF::String&>(scheme)
    {
    }
};

struct SetDomainRelaxationForbiddenForURLScheme : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = SetDomainRelaxationForbiddenForURLSchemeID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit SetDomainRelaxationForbiddenForURLScheme(const WTF::String& scheme)
        : CoreIPC::Arguments1<const WTF::String&>(scheme)
    {
    }
};

struct SetDefaultRequestTimeoutInterval : CoreIPC::Arguments1<double> {
    static const Kind messageID = SetDefaultRequestTimeoutIntervalID;
    typedef CoreIPC::Arguments1<double> DecodeType;
    explicit SetDefaultRequestTimeoutInterval(double timeoutInterval)
        : CoreIPC::Arguments1<double>(timeoutInterval)
    {
    }
};

struct SetAlwaysUsesComplexTextCodePath : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetAlwaysUsesComplexTextCodePathID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetAlwaysUsesComplexTextCodePath(bool alwaysUseComplexText)
        : CoreIPC::Arguments1<bool>(alwaysUseComplexText)
    {
    }
};

struct SetShouldUseFontSmoothing : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetShouldUseFontSmoothingID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetShouldUseFontSmoothing(bool useFontSmoothing)
        : CoreIPC::Arguments1<bool>(useFontSmoothing)
    {
    }
};

struct UserPreferredLanguagesChanged : CoreIPC::Arguments1<const Vector<WTF::String>&> {
    static const Kind messageID = UserPreferredLanguagesChangedID;
    typedef CoreIPC::Arguments1<const Vector<WTF::String>&> DecodeType;
    explicit UserPreferredLanguagesChanged(const Vector<WTF::String>& languages)
        : CoreIPC::Arguments1<const Vector<WTF::String>&>(languages)
    {
    }
};

struct FullKeyboardAccessModeChanged : CoreIPC::Arguments1<bool> {
    static const Kind messageID = FullKeyboardAccessModeChangedID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit FullKeyboardAccessModeChanged(bool fullKeyboardAccessEnabled)
        : CoreIPC::Arguments1<bool>(fullKeyboardAccessEnabled)
    {
    }
};

#if PLATFORM(WIN)
struct SetShouldPaintNativeControls : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetShouldPaintNativeControlsID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetShouldPaintNativeControls(bool shouldPaintNativeControls)
        : CoreIPC::Arguments1<bool>(shouldPaintNativeControls)
    {
    }
};
#endif

#if !ENABLE(PLUGIN_PROCESS)
struct GetSitesWithPluginData : CoreIPC::Arguments2<const Vector<WTF::String>&, uint64_t> {
    static const Kind messageID = GetSitesWithPluginDataID;
    typedef CoreIPC::Arguments2<const Vector<WTF::String>&, uint64_t> DecodeType;
    GetSitesWithPluginData(const Vector<WTF::String>& pluginPaths, uint64_t callbackID)
        : CoreIPC::Arguments2<const Vector<WTF::String>&, uint64_t>(pluginPaths, callbackID)
    {
    }
};
#endif

#if !ENABLE(PLUGIN_PROCESS)
struct ClearPluginSiteData : CoreIPC::Arguments5<const Vector<WTF::String>&, const Vector<WTF::String>&, uint64_t, uint64_t, uint64_t> {
    static const Kind messageID = ClearPluginSiteDataID;
    typedef CoreIPC::Arguments5<const Vector<WTF::String>&, const Vector<WTF::String>&, uint64_t, uint64_t, uint64_t> DecodeType;
    ClearPluginSiteData(const Vector<WTF::String>& pluginPaths, const Vector<WTF::String>& sites, uint64_t flags, uint64_t maxAgeInSeconds, uint64_t callbackID)
        : CoreIPC::Arguments5<const Vector<WTF::String>&, const Vector<WTF::String>&, uint64_t, uint64_t, uint64_t>(pluginPaths, sites, flags, maxAgeInSeconds, callbackID)
    {
    }
};
#endif

#if ENABLE(PLUGIN_PROCESS)
struct PluginProcessCrashed : CoreIPC::Arguments1<const String&> {
    static const Kind messageID = PluginProcessCrashedID;
    typedef CoreIPC::Arguments1<const String&> DecodeType;
    explicit PluginProcessCrashed(const String& pluginProcess)
        : CoreIPC::Arguments1<const String&>(pluginProcess)
    {
    }
};
#endif

struct StartMemorySampler : CoreIPC::Arguments3<const WebKit::SandboxExtension::Handle&, const WTF::String&, double> {
    static const Kind messageID = StartMemorySamplerID;
    typedef CoreIPC::Arguments3<const WebKit::SandboxExtension::Handle&, const WTF::String&, double> DecodeType;
    StartMemorySampler(const WebKit::SandboxExtension::Handle& sampleLogFileHandle, const WTF::String& sampleLogFilePath, double interval)
        : CoreIPC::Arguments3<const WebKit::SandboxExtension::Handle&, const WTF::String&, double>(sampleLogFileHandle, sampleLogFilePath, interval)
    {
    }
};

struct StopMemorySampler : CoreIPC::Arguments0 {
    static const Kind messageID = StopMemorySamplerID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct DownloadRequest : CoreIPC::Arguments3<uint64_t, uint64_t, const WebCore::ResourceRequest&> {
    static const Kind messageID = DownloadRequestID;
    typedef CoreIPC::Arguments3<uint64_t, uint64_t, const WebCore::ResourceRequest&> DecodeType;
    DownloadRequest(uint64_t downloadID, uint64_t initiatingPageID, const WebCore::ResourceRequest& request)
        : CoreIPC::Arguments3<uint64_t, uint64_t, const WebCore::ResourceRequest&>(downloadID, initiatingPageID, request)
    {
    }
};

struct CancelDownload : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = CancelDownloadID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit CancelDownload(uint64_t downloadID)
        : CoreIPC::Arguments1<uint64_t>(downloadID)
    {
    }
};

#if PLATFORM(QT)
struct StartTransfer : CoreIPC::Arguments2<uint64_t, const WTF::String&> {
    static const Kind messageID = StartTransferID;
    typedef CoreIPC::Arguments2<uint64_t, const WTF::String&> DecodeType;
    StartTransfer(uint64_t downloadID, const WTF::String& destination)
        : CoreIPC::Arguments2<uint64_t, const WTF::String&>(downloadID, destination)
    {
    }
};
#endif

struct SetTextCheckerState : CoreIPC::Arguments1<const WebKit::TextCheckerState&> {
    static const Kind messageID = SetTextCheckerStateID;
    typedef CoreIPC::Arguments1<const WebKit::TextCheckerState&> DecodeType;
    explicit SetTextCheckerState(const WebKit::TextCheckerState& textCheckerState)
        : CoreIPC::Arguments1<const WebKit::TextCheckerState&>(textCheckerState)
    {
    }
};

struct SetEnhancedAccessibility : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetEnhancedAccessibilityID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetEnhancedAccessibility(bool flag)
        : CoreIPC::Arguments1<bool>(flag)
    {
    }
};

struct GetWebCoreStatistics : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = GetWebCoreStatisticsID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit GetWebCoreStatistics(uint64_t callbackID)
        : CoreIPC::Arguments1<uint64_t>(callbackID)
    {
    }
};

struct GarbageCollectJavaScriptObjects : CoreIPC::Arguments0 {
    static const Kind messageID = GarbageCollectJavaScriptObjectsID;
    typedef CoreIPC::Arguments0 DecodeType;
};

#if PLATFORM(MAC)
struct SecItemResponse : CoreIPC::Arguments2<uint64_t, const WebKit::SecItemResponseData&> {
    static const Kind messageID = SecItemResponseID;
    typedef CoreIPC::Arguments2<uint64_t, const WebKit::SecItemResponseData&> DecodeType;
    SecItemResponse(uint64_t requestID, const WebKit::SecItemResponseData& response)
        : CoreIPC::Arguments2<uint64_t, const WebKit::SecItemResponseData&>(requestID, response)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SecKeychainItemResponse : CoreIPC::Arguments2<uint64_t, const WebKit::SecKeychainItemResponseData&> {
    static const Kind messageID = SecKeychainItemResponseID;
    typedef CoreIPC::Arguments2<uint64_t, const WebKit::SecKeychainItemResponseData&> DecodeType;
    SecKeychainItemResponse(uint64_t requestID, const WebKit::SecKeychainItemResponseData& response)
        : CoreIPC::Arguments2<uint64_t, const WebKit::SecKeychainItemResponseData&>(requestID, response)
    {
    }
};
#endif

} // namespace WebProcess

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::WebProcess::Kind> {
    static const MessageClass messageClass = MessageClassWebProcess;
};

} // namespace CoreIPC

#endif // WebProcessMessages_h
