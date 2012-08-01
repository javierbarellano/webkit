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

#if ENABLE(PLUGIN_PROCESS)

#include "PluginProcess.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#include "HandleMessage.h"
#include "PluginProcessCreationParameters.h"
#include "PluginProcessMessages.h"
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

void PluginProcess::didReceivePluginProcessMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::PluginProcess::Kind>()) {
    case Messages::PluginProcess::InitializePluginProcessID:
        CoreIPC::handleMessage<Messages::PluginProcess::InitializePluginProcess>(arguments, this, &PluginProcess::initializePluginProcess);
        return;
    case Messages::PluginProcess::CreateWebProcessConnectionID:
        CoreIPC::handleMessage<Messages::PluginProcess::CreateWebProcessConnection>(arguments, this, &PluginProcess::createWebProcessConnection);
        return;
    case Messages::PluginProcess::GetSitesWithDataID:
        CoreIPC::handleMessage<Messages::PluginProcess::GetSitesWithData>(arguments, this, &PluginProcess::getSitesWithData);
        return;
    case Messages::PluginProcess::ClearSiteDataID:
        CoreIPC::handleMessage<Messages::PluginProcess::ClearSiteData>(arguments, this, &PluginProcess::clearSiteData);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit

#endif // ENABLE(PLUGIN_PROCESS)
