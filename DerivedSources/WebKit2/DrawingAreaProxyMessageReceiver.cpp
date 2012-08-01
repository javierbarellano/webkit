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

#include "DrawingAreaProxy.h"

#include "ArgumentDecoder.h"
#include "DrawingAreaProxyMessages.h"
#include "HandleMessage.h"
#include "LayerTreeContext.h"
#include "UpdateInfo.h"

namespace WebKit {

void DrawingAreaProxy::didReceiveDrawingAreaProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::DrawingAreaProxy::Kind>()) {
    case Messages::DrawingAreaProxy::UpdateID:
        CoreIPC::handleMessage<Messages::DrawingAreaProxy::Update>(arguments, this, &DrawingAreaProxy::update);
        return;
    case Messages::DrawingAreaProxy::DidUpdateBackingStoreStateID:
        CoreIPC::handleMessage<Messages::DrawingAreaProxy::DidUpdateBackingStoreState>(arguments, this, &DrawingAreaProxy::didUpdateBackingStoreState);
        return;
#if USE(ACCELERATED_COMPOSITING)
    case Messages::DrawingAreaProxy::EnterAcceleratedCompositingModeID:
        CoreIPC::handleMessage<Messages::DrawingAreaProxy::EnterAcceleratedCompositingMode>(arguments, this, &DrawingAreaProxy::enterAcceleratedCompositingMode);
        return;
#endif
#if USE(ACCELERATED_COMPOSITING)
    case Messages::DrawingAreaProxy::ExitAcceleratedCompositingModeID:
        CoreIPC::handleMessage<Messages::DrawingAreaProxy::ExitAcceleratedCompositingMode>(arguments, this, &DrawingAreaProxy::exitAcceleratedCompositingMode);
        return;
#endif
#if USE(ACCELERATED_COMPOSITING)
    case Messages::DrawingAreaProxy::UpdateAcceleratedCompositingModeID:
        CoreIPC::handleMessage<Messages::DrawingAreaProxy::UpdateAcceleratedCompositingMode>(arguments, this, &DrawingAreaProxy::updateAcceleratedCompositingMode);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::DrawingAreaProxy::DidUpdateGeometryID:
        CoreIPC::handleMessage<Messages::DrawingAreaProxy::DidUpdateGeometry>(arguments, this, &DrawingAreaProxy::didUpdateGeometry);
        return;
#endif
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit
