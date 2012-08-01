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

#if USE(ACCELERATED_COMPOSITING) && USE(TILED_BACKING_STORE)

#include "LayerTreeHostProxy.h"

#include "ArgumentDecoder.h"
#include "HandleMessage.h"
#include "LayerTreeHostProxyMessages.h"
#include "ShareableBitmap.h"
#include "UpdateInfo.h"
#include "WebLayerTreeInfo.h"

namespace WebKit {

void LayerTreeHostProxy::didReceiveLayerTreeHostProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::LayerTreeHostProxy::Kind>()) {
    case Messages::LayerTreeHostProxy::SyncCompositingLayerStateID:
        CoreIPC::handleMessage<Messages::LayerTreeHostProxy::SyncCompositingLayerState>(arguments, this, &LayerTreeHostProxy::syncCompositingLayerState);
        return;
    case Messages::LayerTreeHostProxy::SetRootCompositingLayerID:
        CoreIPC::handleMessage<Messages::LayerTreeHostProxy::SetRootCompositingLayer>(arguments, this, &LayerTreeHostProxy::setRootCompositingLayer);
        return;
    case Messages::LayerTreeHostProxy::DeleteCompositingLayerID:
        CoreIPC::handleMessage<Messages::LayerTreeHostProxy::DeleteCompositingLayer>(arguments, this, &LayerTreeHostProxy::deleteCompositingLayer);
        return;
    case Messages::LayerTreeHostProxy::CreateTileForLayerID:
        CoreIPC::handleMessage<Messages::LayerTreeHostProxy::CreateTileForLayer>(arguments, this, &LayerTreeHostProxy::createTileForLayer);
        return;
    case Messages::LayerTreeHostProxy::UpdateTileForLayerID:
        CoreIPC::handleMessage<Messages::LayerTreeHostProxy::UpdateTileForLayer>(arguments, this, &LayerTreeHostProxy::updateTileForLayer);
        return;
    case Messages::LayerTreeHostProxy::RemoveTileForLayerID:
        CoreIPC::handleMessage<Messages::LayerTreeHostProxy::RemoveTileForLayer>(arguments, this, &LayerTreeHostProxy::removeTileForLayer);
        return;
    case Messages::LayerTreeHostProxy::CreateDirectlyCompositedImageID:
        CoreIPC::handleMessage<Messages::LayerTreeHostProxy::CreateDirectlyCompositedImage>(arguments, this, &LayerTreeHostProxy::createDirectlyCompositedImage);
        return;
    case Messages::LayerTreeHostProxy::DestroyDirectlyCompositedImageID:
        CoreIPC::handleMessage<Messages::LayerTreeHostProxy::DestroyDirectlyCompositedImage>(arguments, this, &LayerTreeHostProxy::destroyDirectlyCompositedImage);
        return;
    case Messages::LayerTreeHostProxy::DidRenderFrameID:
        CoreIPC::handleMessage<Messages::LayerTreeHostProxy::DidRenderFrame>(arguments, this, &LayerTreeHostProxy::didRenderFrame);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit

#endif // USE(ACCELERATED_COMPOSITING) && USE(TILED_BACKING_STORE)
