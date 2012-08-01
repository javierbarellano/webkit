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

#include "DownloadProxy.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#include "DataReference.h"
#include "DownloadProxyMessages.h"
#include "HandleMessage.h"
#include "SandboxExtension.h"
#include "WebCoreArgumentCoders.h"
#include <WebCore/AuthenticationChallenge.h>
#include <WebCore/ResourceError.h>
#include <WebCore/ResourceRequest.h>
#include <WebCore/ResourceResponse.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

void DownloadProxy::didReceiveDownloadProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::DownloadProxy::Kind>()) {
    case Messages::DownloadProxy::DidStartID:
        CoreIPC::handleMessage<Messages::DownloadProxy::DidStart>(arguments, this, &DownloadProxy::didStart);
        return;
    case Messages::DownloadProxy::DidReceiveAuthenticationChallengeID:
        CoreIPC::handleMessage<Messages::DownloadProxy::DidReceiveAuthenticationChallenge>(arguments, this, &DownloadProxy::didReceiveAuthenticationChallenge);
        return;
    case Messages::DownloadProxy::DidReceiveResponseID:
        CoreIPC::handleMessage<Messages::DownloadProxy::DidReceiveResponse>(arguments, this, &DownloadProxy::didReceiveResponse);
        return;
    case Messages::DownloadProxy::DidReceiveDataID:
        CoreIPC::handleMessage<Messages::DownloadProxy::DidReceiveData>(arguments, this, &DownloadProxy::didReceiveData);
        return;
    case Messages::DownloadProxy::DidCreateDestinationID:
        CoreIPC::handleMessage<Messages::DownloadProxy::DidCreateDestination>(arguments, this, &DownloadProxy::didCreateDestination);
        return;
    case Messages::DownloadProxy::DidFinishID:
        CoreIPC::handleMessage<Messages::DownloadProxy::DidFinish>(arguments, this, &DownloadProxy::didFinish);
        return;
    case Messages::DownloadProxy::DidFailID:
        CoreIPC::handleMessage<Messages::DownloadProxy::DidFail>(arguments, this, &DownloadProxy::didFail);
        return;
    case Messages::DownloadProxy::DidCancelID:
        CoreIPC::handleMessage<Messages::DownloadProxy::DidCancel>(arguments, this, &DownloadProxy::didCancel);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

void DownloadProxy::didReceiveSyncDownloadProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    switch (messageID.get<Messages::DownloadProxy::Kind>()) {
    case Messages::DownloadProxy::ShouldDecodeSourceDataOfMIMETypeID:
        CoreIPC::handleMessage<Messages::DownloadProxy::ShouldDecodeSourceDataOfMIMEType>(arguments, reply.get(), this, &DownloadProxy::shouldDecodeSourceDataOfMIMEType);
        return;
    case Messages::DownloadProxy::DecideDestinationWithSuggestedFilenameID:
        CoreIPC::handleMessage<Messages::DownloadProxy::DecideDestinationWithSuggestedFilename>(arguments, reply.get(), this, &DownloadProxy::decideDestinationWithSuggestedFilename);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit
