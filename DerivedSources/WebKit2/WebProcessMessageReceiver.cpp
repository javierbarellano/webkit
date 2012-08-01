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

#include "config.h"

#include "WebProcess.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#include "HandleMessage.h"
#include "SandboxExtension.h"
#if PLATFORM(MAC)
#include "SecItemResponseData.h"
#endif
#if PLATFORM(MAC)
#include "SecKeychainItemResponseData.h"
#endif
#include "SharedMemory.h"
#include "TextCheckerState.h"
#include "WebContextUserMessageCoders.h"
#include "WebCoreArgumentCoders.h"
#include "WebPageCreationParameters.h"
#include "WebProcessCreationParameters.h"
#include "WebProcessMessages.h"
#include <WebCore/LinkHash.h>
#include <WebCore/ResourceRequest.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

void WebProcess::didReceiveWebProcessMessageOnConnectionWorkQueue(CoreIPC::Connection* connection, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, bool& didHandleMessage)
{
#if COMPILER(MSVC)
#pragma warning(push)
#pragma warning(disable: 4065)
#endif
    switch (messageID.get<Messages::WebProcess::Kind>()) {
#if ENABLE(PLUGIN_PROCESS)
    case Messages::WebProcess::PluginProcessCrashedID:
        CoreIPC::handleMessageOnConnectionQueue<Messages::WebProcess::PluginProcessCrashed>(connection, arguments, this, &WebProcess::pluginProcessCrashed);
        didHandleMessage = true;
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebProcess::SecItemResponseID:
        CoreIPC::handleMessageOnConnectionQueue<Messages::WebProcess::SecItemResponse>(connection, arguments, this, &WebProcess::secItemResponse);
        didHandleMessage = true;
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebProcess::SecKeychainItemResponseID:
        CoreIPC::handleMessageOnConnectionQueue<Messages::WebProcess::SecKeychainItemResponse>(connection, arguments, this, &WebProcess::secKeychainItemResponse);
        didHandleMessage = true;
        return;
#endif
    default:
        return;
    }
#if COMPILER(MSVC)
#pragma warning(pop)
#endif
}

void WebProcess::didReceiveWebProcessMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::WebProcess::Kind>()) {
    case Messages::WebProcess::InitializeWebProcessID:
        CoreIPC::handleMessageVariadic<Messages::WebProcess::InitializeWebProcess>(arguments, this, &WebProcess::initializeWebProcess);
        return;
    case Messages::WebProcess::CreateWebPageID:
        CoreIPC::handleMessage<Messages::WebProcess::CreateWebPage>(arguments, this, &WebProcess::createWebPage);
        return;
    case Messages::WebProcess::SetVisitedLinkTableID:
        CoreIPC::handleMessage<Messages::WebProcess::SetVisitedLinkTable>(arguments, this, &WebProcess::setVisitedLinkTable);
        return;
    case Messages::WebProcess::VisitedLinkStateChangedID:
        CoreIPC::handleMessage<Messages::WebProcess::VisitedLinkStateChanged>(arguments, this, &WebProcess::visitedLinkStateChanged);
        return;
    case Messages::WebProcess::AllVisitedLinkStateChangedID:
        CoreIPC::handleMessage<Messages::WebProcess::AllVisitedLinkStateChanged>(arguments, this, &WebProcess::allVisitedLinkStateChanged);
        return;
    case Messages::WebProcess::SetShouldTrackVisitedLinksID:
        CoreIPC::handleMessage<Messages::WebProcess::SetShouldTrackVisitedLinks>(arguments, this, &WebProcess::setShouldTrackVisitedLinks);
        return;
    case Messages::WebProcess::SetCacheModelID:
        CoreIPC::handleMessage<Messages::WebProcess::SetCacheModel>(arguments, this, &WebProcess::setCacheModel);
        return;
    case Messages::WebProcess::RegisterURLSchemeAsEmptyDocumentID:
        CoreIPC::handleMessage<Messages::WebProcess::RegisterURLSchemeAsEmptyDocument>(arguments, this, &WebProcess::registerURLSchemeAsEmptyDocument);
        return;
    case Messages::WebProcess::RegisterURLSchemeAsSecureID:
        CoreIPC::handleMessage<Messages::WebProcess::RegisterURLSchemeAsSecure>(arguments, this, &WebProcess::registerURLSchemeAsSecure);
        return;
    case Messages::WebProcess::SetDomainRelaxationForbiddenForURLSchemeID:
        CoreIPC::handleMessage<Messages::WebProcess::SetDomainRelaxationForbiddenForURLScheme>(arguments, this, &WebProcess::setDomainRelaxationForbiddenForURLScheme);
        return;
    case Messages::WebProcess::SetDefaultRequestTimeoutIntervalID:
        CoreIPC::handleMessage<Messages::WebProcess::SetDefaultRequestTimeoutInterval>(arguments, this, &WebProcess::setDefaultRequestTimeoutInterval);
        return;
    case Messages::WebProcess::SetAlwaysUsesComplexTextCodePathID:
        CoreIPC::handleMessage<Messages::WebProcess::SetAlwaysUsesComplexTextCodePath>(arguments, this, &WebProcess::setAlwaysUsesComplexTextCodePath);
        return;
    case Messages::WebProcess::SetShouldUseFontSmoothingID:
        CoreIPC::handleMessage<Messages::WebProcess::SetShouldUseFontSmoothing>(arguments, this, &WebProcess::setShouldUseFontSmoothing);
        return;
    case Messages::WebProcess::UserPreferredLanguagesChangedID:
        CoreIPC::handleMessage<Messages::WebProcess::UserPreferredLanguagesChanged>(arguments, this, &WebProcess::userPreferredLanguagesChanged);
        return;
    case Messages::WebProcess::FullKeyboardAccessModeChangedID:
        CoreIPC::handleMessage<Messages::WebProcess::FullKeyboardAccessModeChanged>(arguments, this, &WebProcess::fullKeyboardAccessModeChanged);
        return;
#if PLATFORM(WIN)
    case Messages::WebProcess::SetShouldPaintNativeControlsID:
        CoreIPC::handleMessage<Messages::WebProcess::SetShouldPaintNativeControls>(arguments, this, &WebProcess::setShouldPaintNativeControls);
        return;
#endif
#if !ENABLE(PLUGIN_PROCESS)
    case Messages::WebProcess::GetSitesWithPluginDataID:
        CoreIPC::handleMessage<Messages::WebProcess::GetSitesWithPluginData>(arguments, this, &WebProcess::getSitesWithPluginData);
        return;
#endif
#if !ENABLE(PLUGIN_PROCESS)
    case Messages::WebProcess::ClearPluginSiteDataID:
        CoreIPC::handleMessage<Messages::WebProcess::ClearPluginSiteData>(arguments, this, &WebProcess::clearPluginSiteData);
        return;
#endif
    case Messages::WebProcess::StartMemorySamplerID:
        CoreIPC::handleMessage<Messages::WebProcess::StartMemorySampler>(arguments, this, &WebProcess::startMemorySampler);
        return;
    case Messages::WebProcess::StopMemorySamplerID:
        CoreIPC::handleMessage<Messages::WebProcess::StopMemorySampler>(arguments, this, &WebProcess::stopMemorySampler);
        return;
    case Messages::WebProcess::DownloadRequestID:
        CoreIPC::handleMessage<Messages::WebProcess::DownloadRequest>(arguments, this, &WebProcess::downloadRequest);
        return;
    case Messages::WebProcess::CancelDownloadID:
        CoreIPC::handleMessage<Messages::WebProcess::CancelDownload>(arguments, this, &WebProcess::cancelDownload);
        return;
#if PLATFORM(QT)
    case Messages::WebProcess::StartTransferID:
        CoreIPC::handleMessage<Messages::WebProcess::StartTransfer>(arguments, this, &WebProcess::startTransfer);
        return;
#endif
    case Messages::WebProcess::SetTextCheckerStateID:
        CoreIPC::handleMessage<Messages::WebProcess::SetTextCheckerState>(arguments, this, &WebProcess::setTextCheckerState);
        return;
    case Messages::WebProcess::SetEnhancedAccessibilityID:
        CoreIPC::handleMessage<Messages::WebProcess::SetEnhancedAccessibility>(arguments, this, &WebProcess::setEnhancedAccessibility);
        return;
    case Messages::WebProcess::GetWebCoreStatisticsID:
        CoreIPC::handleMessage<Messages::WebProcess::GetWebCoreStatistics>(arguments, this, &WebProcess::getWebCoreStatistics);
        return;
    case Messages::WebProcess::GarbageCollectJavaScriptObjectsID:
        CoreIPC::handleMessage<Messages::WebProcess::GarbageCollectJavaScriptObjects>(arguments, this, &WebProcess::garbageCollectJavaScriptObjects);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit
