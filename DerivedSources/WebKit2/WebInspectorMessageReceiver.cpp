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

#if ENABLE(INSPECTOR)

#include "WebInspector.h"

#include "ArgumentDecoder.h"
#include "HandleMessage.h"
#include "WebInspectorMessages.h"

namespace WebKit {

void WebInspector::didReceiveWebInspectorMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::WebInspector::Kind>()) {
    case Messages::WebInspector::ShowID:
        CoreIPC::handleMessage<Messages::WebInspector::Show>(arguments, this, &WebInspector::show);
        return;
    case Messages::WebInspector::CloseID:
        CoreIPC::handleMessage<Messages::WebInspector::Close>(arguments, this, &WebInspector::close);
        return;
    case Messages::WebInspector::ShowConsoleID:
        CoreIPC::handleMessage<Messages::WebInspector::ShowConsole>(arguments, this, &WebInspector::showConsole);
        return;
    case Messages::WebInspector::ShowResourcesID:
        CoreIPC::handleMessage<Messages::WebInspector::ShowResources>(arguments, this, &WebInspector::showResources);
        return;
    case Messages::WebInspector::ShowMainResourceForFrameID:
        CoreIPC::handleMessage<Messages::WebInspector::ShowMainResourceForFrame>(arguments, this, &WebInspector::showMainResourceForFrame);
        return;
    case Messages::WebInspector::StartJavaScriptDebuggingID:
        CoreIPC::handleMessage<Messages::WebInspector::StartJavaScriptDebugging>(arguments, this, &WebInspector::startJavaScriptDebugging);
        return;
    case Messages::WebInspector::StopJavaScriptDebuggingID:
        CoreIPC::handleMessage<Messages::WebInspector::StopJavaScriptDebugging>(arguments, this, &WebInspector::stopJavaScriptDebugging);
        return;
    case Messages::WebInspector::StartJavaScriptProfilingID:
        CoreIPC::handleMessage<Messages::WebInspector::StartJavaScriptProfiling>(arguments, this, &WebInspector::startJavaScriptProfiling);
        return;
    case Messages::WebInspector::StopJavaScriptProfilingID:
        CoreIPC::handleMessage<Messages::WebInspector::StopJavaScriptProfiling>(arguments, this, &WebInspector::stopJavaScriptProfiling);
        return;
    case Messages::WebInspector::StartPageProfilingID:
        CoreIPC::handleMessage<Messages::WebInspector::StartPageProfiling>(arguments, this, &WebInspector::startPageProfiling);
        return;
    case Messages::WebInspector::StopPageProfilingID:
        CoreIPC::handleMessage<Messages::WebInspector::StopPageProfiling>(arguments, this, &WebInspector::stopPageProfiling);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit

#endif // ENABLE(INSPECTOR)
