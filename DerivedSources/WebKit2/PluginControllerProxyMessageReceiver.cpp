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

#include "PluginControllerProxy.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#include "DataReference.h"
#include "HandleMessage.h"
#include "PluginControllerProxyMessages.h"
#include "ShareableBitmap.h"
#include "WebCoreArgumentCoders.h"
#include "WebEvent.h"
#include <WebCore/AffineTransform.h>
#include <WebCore/IntRect.h>
#include <WebCore/IntSize.h>
#include <wtf/text/WTFString.h>

namespace Messages {

namespace PluginControllerProxy {

HandleMouseEvent::DelayedReply::DelayedReply(PassRefPtr<CoreIPC::Connection> connection, PassOwnPtr<CoreIPC::ArgumentEncoder> arguments)
    : m_connection(connection)
    , m_arguments(arguments)
{
}

HandleMouseEvent::DelayedReply::~DelayedReply()
{
    ASSERT(!m_connection);
}

bool HandleMouseEvent::DelayedReply::send(bool handled)
{
    ASSERT(m_arguments);
    m_arguments->encode(handled);
    bool result = m_connection->sendSyncReply(m_arguments.release());
    m_connection = nullptr;
    return result;
}

} // namespace PluginControllerProxy

} // namespace Messages

namespace WebKit {

void PluginControllerProxy::didReceivePluginControllerProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::PluginControllerProxy::Kind>()) {
    case Messages::PluginControllerProxy::GeometryDidChangeID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::GeometryDidChange>(arguments, this, &PluginControllerProxy::geometryDidChange);
        return;
    case Messages::PluginControllerProxy::FrameDidFinishLoadingID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::FrameDidFinishLoading>(arguments, this, &PluginControllerProxy::frameDidFinishLoading);
        return;
    case Messages::PluginControllerProxy::FrameDidFailID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::FrameDidFail>(arguments, this, &PluginControllerProxy::frameDidFail);
        return;
    case Messages::PluginControllerProxy::DidEvaluateJavaScriptID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::DidEvaluateJavaScript>(arguments, this, &PluginControllerProxy::didEvaluateJavaScript);
        return;
    case Messages::PluginControllerProxy::StreamDidReceiveResponseID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::StreamDidReceiveResponse>(arguments, this, &PluginControllerProxy::streamDidReceiveResponse);
        return;
    case Messages::PluginControllerProxy::StreamDidReceiveDataID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::StreamDidReceiveData>(arguments, this, &PluginControllerProxy::streamDidReceiveData);
        return;
    case Messages::PluginControllerProxy::StreamDidFinishLoadingID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::StreamDidFinishLoading>(arguments, this, &PluginControllerProxy::streamDidFinishLoading);
        return;
    case Messages::PluginControllerProxy::StreamDidFailID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::StreamDidFail>(arguments, this, &PluginControllerProxy::streamDidFail);
        return;
    case Messages::PluginControllerProxy::ManualStreamDidReceiveResponseID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::ManualStreamDidReceiveResponse>(arguments, this, &PluginControllerProxy::manualStreamDidReceiveResponse);
        return;
    case Messages::PluginControllerProxy::ManualStreamDidReceiveDataID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::ManualStreamDidReceiveData>(arguments, this, &PluginControllerProxy::manualStreamDidReceiveData);
        return;
    case Messages::PluginControllerProxy::ManualStreamDidFinishLoadingID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::ManualStreamDidFinishLoading>(arguments, this, &PluginControllerProxy::manualStreamDidFinishLoading);
        return;
    case Messages::PluginControllerProxy::ManualStreamDidFailID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::ManualStreamDidFail>(arguments, this, &PluginControllerProxy::manualStreamDidFail);
        return;
    case Messages::PluginControllerProxy::SetFocusID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::SetFocus>(arguments, this, &PluginControllerProxy::setFocus);
        return;
    case Messages::PluginControllerProxy::DidUpdateID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::DidUpdate>(arguments, this, &PluginControllerProxy::didUpdate);
        return;
#if PLATFORM(MAC)
    case Messages::PluginControllerProxy::SendComplexTextInputID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::SendComplexTextInput>(arguments, this, &PluginControllerProxy::sendComplexTextInput);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::PluginControllerProxy::WindowFocusChangedID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::WindowFocusChanged>(arguments, this, &PluginControllerProxy::windowFocusChanged);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::PluginControllerProxy::WindowAndViewFramesChangedID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::WindowAndViewFramesChanged>(arguments, this, &PluginControllerProxy::windowAndViewFramesChanged);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::PluginControllerProxy::WindowVisibilityChangedID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::WindowVisibilityChanged>(arguments, this, &PluginControllerProxy::windowVisibilityChanged);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::PluginControllerProxy::SetLayerHostingModeID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::SetLayerHostingMode>(arguments, this, &PluginControllerProxy::setLayerHostingMode);
        return;
#endif
    case Messages::PluginControllerProxy::PrivateBrowsingStateChangedID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::PrivateBrowsingStateChanged>(arguments, this, &PluginControllerProxy::privateBrowsingStateChanged);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

void PluginControllerProxy::didReceiveSyncPluginControllerProxyMessage(CoreIPC::Connection* connection, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    switch (messageID.get<Messages::PluginControllerProxy::Kind>()) {
    case Messages::PluginControllerProxy::HandleMouseEventID:
        CoreIPC::handleMessageDelayed<Messages::PluginControllerProxy::HandleMouseEvent>(connection, arguments, reply, this, &PluginControllerProxy::handleMouseEvent);
        return;
    case Messages::PluginControllerProxy::HandleWheelEventID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::HandleWheelEvent>(arguments, reply.get(), this, &PluginControllerProxy::handleWheelEvent);
        return;
    case Messages::PluginControllerProxy::HandleMouseEnterEventID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::HandleMouseEnterEvent>(arguments, reply.get(), this, &PluginControllerProxy::handleMouseEnterEvent);
        return;
    case Messages::PluginControllerProxy::HandleMouseLeaveEventID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::HandleMouseLeaveEvent>(arguments, reply.get(), this, &PluginControllerProxy::handleMouseLeaveEvent);
        return;
    case Messages::PluginControllerProxy::HandleKeyboardEventID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::HandleKeyboardEvent>(arguments, reply.get(), this, &PluginControllerProxy::handleKeyboardEvent);
        return;
    case Messages::PluginControllerProxy::PaintEntirePluginID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::PaintEntirePlugin>(arguments, reply.get(), this, &PluginControllerProxy::paintEntirePlugin);
        return;
    case Messages::PluginControllerProxy::GetPluginScriptableNPObjectID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::GetPluginScriptableNPObject>(arguments, reply.get(), this, &PluginControllerProxy::getPluginScriptableNPObject);
        return;
    case Messages::PluginControllerProxy::SnapshotID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::Snapshot>(arguments, reply.get(), this, &PluginControllerProxy::snapshot);
        return;
    case Messages::PluginControllerProxy::GetFormValueID:
        CoreIPC::handleMessage<Messages::PluginControllerProxy::GetFormValue>(arguments, reply.get(), this, &PluginControllerProxy::getFormValue);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit

#endif // ENABLE(PLUGIN_PROCESS)
