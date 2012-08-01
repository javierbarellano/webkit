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

#include "WebCookieManager.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#include "HandleMessage.h"
#include "WebCookieManagerMessages.h"
#include <wtf/text/WTFString.h>

namespace WebKit {

void WebCookieManager::didReceiveWebCookieManagerMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::WebCookieManager::Kind>()) {
    case Messages::WebCookieManager::GetHostnamesWithCookiesID:
        CoreIPC::handleMessage<Messages::WebCookieManager::GetHostnamesWithCookies>(arguments, this, &WebCookieManager::getHostnamesWithCookies);
        return;
    case Messages::WebCookieManager::DeleteCookiesForHostnameID:
        CoreIPC::handleMessage<Messages::WebCookieManager::DeleteCookiesForHostname>(arguments, this, &WebCookieManager::deleteCookiesForHostname);
        return;
    case Messages::WebCookieManager::DeleteAllCookiesID:
        CoreIPC::handleMessage<Messages::WebCookieManager::DeleteAllCookies>(arguments, this, &WebCookieManager::deleteAllCookies);
        return;
    case Messages::WebCookieManager::SetHTTPCookieAcceptPolicyID:
        CoreIPC::handleMessage<Messages::WebCookieManager::SetHTTPCookieAcceptPolicy>(arguments, this, &WebCookieManager::setHTTPCookieAcceptPolicy);
        return;
    case Messages::WebCookieManager::GetHTTPCookieAcceptPolicyID:
        CoreIPC::handleMessage<Messages::WebCookieManager::GetHTTPCookieAcceptPolicy>(arguments, this, &WebCookieManager::getHTTPCookieAcceptPolicy);
        return;
    case Messages::WebCookieManager::StartObservingCookieChangesID:
        CoreIPC::handleMessage<Messages::WebCookieManager::StartObservingCookieChanges>(arguments, this, &WebCookieManager::startObservingCookieChanges);
        return;
    case Messages::WebCookieManager::StopObservingCookieChangesID:
        CoreIPC::handleMessage<Messages::WebCookieManager::StopObservingCookieChanges>(arguments, this, &WebCookieManager::stopObservingCookieChanges);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit
