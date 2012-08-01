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

#include "WebIconDatabase.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#include "DataReference.h"
#include "HandleMessage.h"
#include "WebIconDatabaseMessages.h"
#include <wtf/text/WTFString.h>

namespace WebKit {

void WebIconDatabase::didReceiveWebIconDatabaseMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::WebIconDatabase::Kind>()) {
    case Messages::WebIconDatabase::RetainIconForPageURLID:
        CoreIPC::handleMessage<Messages::WebIconDatabase::RetainIconForPageURL>(arguments, this, &WebIconDatabase::retainIconForPageURL);
        return;
    case Messages::WebIconDatabase::ReleaseIconForPageURLID:
        CoreIPC::handleMessage<Messages::WebIconDatabase::ReleaseIconForPageURL>(arguments, this, &WebIconDatabase::releaseIconForPageURL);
        return;
    case Messages::WebIconDatabase::SetIconURLForPageURLID:
        CoreIPC::handleMessage<Messages::WebIconDatabase::SetIconURLForPageURL>(arguments, this, &WebIconDatabase::setIconURLForPageURL);
        return;
    case Messages::WebIconDatabase::SetIconDataForIconURLID:
        CoreIPC::handleMessage<Messages::WebIconDatabase::SetIconDataForIconURL>(arguments, this, &WebIconDatabase::setIconDataForIconURL);
        return;
    case Messages::WebIconDatabase::GetLoadDecisionForIconURLID:
        CoreIPC::handleMessage<Messages::WebIconDatabase::GetLoadDecisionForIconURL>(arguments, this, &WebIconDatabase::getLoadDecisionForIconURL);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

void WebIconDatabase::didReceiveSyncWebIconDatabaseMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    switch (messageID.get<Messages::WebIconDatabase::Kind>()) {
    case Messages::WebIconDatabase::SynchronousIconDataForPageURLID:
        CoreIPC::handleMessage<Messages::WebIconDatabase::SynchronousIconDataForPageURL>(arguments, reply.get(), this, &WebIconDatabase::synchronousIconDataForPageURL);
        return;
    case Messages::WebIconDatabase::SynchronousIconURLForPageURLID:
        CoreIPC::handleMessage<Messages::WebIconDatabase::SynchronousIconURLForPageURL>(arguments, reply.get(), this, &WebIconDatabase::synchronousIconURLForPageURL);
        return;
    case Messages::WebIconDatabase::SynchronousIconDataKnownForIconURLID:
        CoreIPC::handleMessage<Messages::WebIconDatabase::SynchronousIconDataKnownForIconURL>(arguments, reply.get(), this, &WebIconDatabase::synchronousIconDataKnownForIconURL);
        return;
    case Messages::WebIconDatabase::SynchronousLoadDecisionForIconURLID:
        CoreIPC::handleMessage<Messages::WebIconDatabase::SynchronousLoadDecisionForIconURL>(arguments, reply.get(), this, &WebIconDatabase::synchronousLoadDecisionForIconURL);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit
