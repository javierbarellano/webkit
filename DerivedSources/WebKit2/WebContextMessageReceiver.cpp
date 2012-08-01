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

#include "WebContext.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#include "HandleMessage.h"
#if PLATFORM(MAC)
#include "SharedMemory.h"
#endif
#include "StatisticsData.h"
#include "WebContextMessages.h"
#include "WebCoreArgumentCoders.h"
#include "WebNavigationDataStore.h"
#if PLATFORM(MAC)
#include <WebCore/Color.h>
#endif
#include <WebCore/PluginData.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

void WebContext::didReceiveWebContextMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::WebContext::Kind>()) {
    case Messages::WebContext::DidNavigateWithNavigationDataID:
        CoreIPC::handleMessage<Messages::WebContext::DidNavigateWithNavigationData>(arguments, this, &WebContext::didNavigateWithNavigationData);
        return;
    case Messages::WebContext::DidPerformClientRedirectID:
        CoreIPC::handleMessage<Messages::WebContext::DidPerformClientRedirect>(arguments, this, &WebContext::didPerformClientRedirect);
        return;
    case Messages::WebContext::DidPerformServerRedirectID:
        CoreIPC::handleMessage<Messages::WebContext::DidPerformServerRedirect>(arguments, this, &WebContext::didPerformServerRedirect);
        return;
    case Messages::WebContext::DidUpdateHistoryTitleID:
        CoreIPC::handleMessage<Messages::WebContext::DidUpdateHistoryTitle>(arguments, this, &WebContext::didUpdateHistoryTitle);
        return;
    case Messages::WebContext::AddVisitedLinkHashID:
        CoreIPC::handleMessage<Messages::WebContext::AddVisitedLinkHash>(arguments, this, &WebContext::addVisitedLinkHash);
        return;
#if !ENABLE(PLUGIN_PROCESS)
    case Messages::WebContext::DidGetSitesWithPluginDataID:
        CoreIPC::handleMessage<Messages::WebContext::DidGetSitesWithPluginData>(arguments, this, &WebContext::didGetSitesWithPluginData);
        return;
#endif
#if !ENABLE(PLUGIN_PROCESS)
    case Messages::WebContext::DidClearPluginSiteDataID:
        CoreIPC::handleMessage<Messages::WebContext::DidClearPluginSiteData>(arguments, this, &WebContext::didClearPluginSiteData);
        return;
#endif
    case Messages::WebContext::DidGetWebCoreStatisticsID:
        CoreIPC::handleMessage<Messages::WebContext::DidGetWebCoreStatistics>(arguments, this, &WebContext::didGetWebCoreStatistics);
        return;
#if PLATFORM(MAC)
    case Messages::WebContext::PasteboardCopyID:
        CoreIPC::handleMessage<Messages::WebContext::PasteboardCopy>(arguments, this, &WebContext::pasteboardCopy);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::AddPasteboardTypesID:
        CoreIPC::handleMessage<Messages::WebContext::AddPasteboardTypes>(arguments, this, &WebContext::addPasteboardTypes);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::SetPasteboardTypesID:
        CoreIPC::handleMessage<Messages::WebContext::SetPasteboardTypes>(arguments, this, &WebContext::setPasteboardTypes);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::SetPasteboardPathnamesForTypeID:
        CoreIPC::handleMessage<Messages::WebContext::SetPasteboardPathnamesForType>(arguments, this, &WebContext::setPasteboardPathnamesForType);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::SetPasteboardStringForTypeID:
        CoreIPC::handleMessage<Messages::WebContext::SetPasteboardStringForType>(arguments, this, &WebContext::setPasteboardStringForType);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::SetPasteboardBufferForTypeID:
        CoreIPC::handleMessage<Messages::WebContext::SetPasteboardBufferForType>(arguments, this, &WebContext::setPasteboardBufferForType);
        return;
#endif
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

void WebContext::didReceiveSyncWebContextMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    switch (messageID.get<Messages::WebContext::Kind>()) {
    case Messages::WebContext::GetPluginsID:
        CoreIPC::handleMessage<Messages::WebContext::GetPlugins>(arguments, reply.get(), this, &WebContext::getPlugins);
        return;
    case Messages::WebContext::GetPluginPathID:
        CoreIPC::handleMessage<Messages::WebContext::GetPluginPath>(arguments, reply.get(), this, &WebContext::getPluginPath);
        return;
#if PLATFORM(MAC)
    case Messages::WebContext::GetPasteboardTypesID:
        CoreIPC::handleMessage<Messages::WebContext::GetPasteboardTypes>(arguments, reply.get(), this, &WebContext::getPasteboardTypes);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::GetPasteboardPathnamesForTypeID:
        CoreIPC::handleMessage<Messages::WebContext::GetPasteboardPathnamesForType>(arguments, reply.get(), this, &WebContext::getPasteboardPathnamesForType);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::GetPasteboardStringForTypeID:
        CoreIPC::handleMessage<Messages::WebContext::GetPasteboardStringForType>(arguments, reply.get(), this, &WebContext::getPasteboardStringForType);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::GetPasteboardBufferForTypeID:
        CoreIPC::handleMessage<Messages::WebContext::GetPasteboardBufferForType>(arguments, reply.get(), this, &WebContext::getPasteboardBufferForType);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::GetPasteboardChangeCountID:
        CoreIPC::handleMessage<Messages::WebContext::GetPasteboardChangeCount>(arguments, reply.get(), this, &WebContext::getPasteboardChangeCount);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::GetPasteboardUniqueNameID:
        CoreIPC::handleMessage<Messages::WebContext::GetPasteboardUniqueName>(arguments, reply.get(), this, &WebContext::getPasteboardUniqueName);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::GetPasteboardColorID:
        CoreIPC::handleMessage<Messages::WebContext::GetPasteboardColor>(arguments, reply.get(), this, &WebContext::getPasteboardColor);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebContext::GetPasteboardURLID:
        CoreIPC::handleMessage<Messages::WebContext::GetPasteboardURL>(arguments, reply.get(), this, &WebContext::getPasteboardURL);
        return;
#endif
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit
