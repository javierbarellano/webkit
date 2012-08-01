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

#ifndef LayerTreeHostProxyMessages_h
#define LayerTreeHostProxyMessages_h

#if USE(ACCELERATED_COMPOSITING) && USE(TILED_BACKING_STORE)

#include "Arguments.h"
#include "MessageID.h"
#include "ShareableBitmap.h"

namespace WebKit {
    class WebLayerInfo;
    class UpdateInfo;
}

namespace Messages {

namespace LayerTreeHostProxy {

enum Kind {
    SyncCompositingLayerStateID,
    SetRootCompositingLayerID,
    DeleteCompositingLayerID,
    CreateTileForLayerID,
    UpdateTileForLayerID,
    RemoveTileForLayerID,
    CreateDirectlyCompositedImageID,
    DestroyDirectlyCompositedImageID,
    DidRenderFrameID,
};

struct SyncCompositingLayerState : CoreIPC::Arguments1<const WebKit::WebLayerInfo&> {
    static const Kind messageID = SyncCompositingLayerStateID;
    typedef CoreIPC::Arguments1<const WebKit::WebLayerInfo&> DecodeType;
    explicit SyncCompositingLayerState(const WebKit::WebLayerInfo& layerInfo)
        : CoreIPC::Arguments1<const WebKit::WebLayerInfo&>(layerInfo)
    {
    }
};

struct SetRootCompositingLayer : CoreIPC::Arguments1<uint32_t> {
    static const Kind messageID = SetRootCompositingLayerID;
    typedef CoreIPC::Arguments1<uint32_t> DecodeType;
    explicit SetRootCompositingLayer(uint32_t id)
        : CoreIPC::Arguments1<uint32_t>(id)
    {
    }
};

struct DeleteCompositingLayer : CoreIPC::Arguments1<uint32_t> {
    static const Kind messageID = DeleteCompositingLayerID;
    typedef CoreIPC::Arguments1<uint32_t> DecodeType;
    explicit DeleteCompositingLayer(uint32_t id)
        : CoreIPC::Arguments1<uint32_t>(id)
    {
    }
};

struct CreateTileForLayer : CoreIPC::Arguments3<uint32_t, const int&, const WebKit::UpdateInfo&> {
    static const Kind messageID = CreateTileForLayerID;
    typedef CoreIPC::Arguments3<uint32_t, const int&, const WebKit::UpdateInfo&> DecodeType;
    CreateTileForLayer(uint32_t layerID, const int& tileID, const WebKit::UpdateInfo& updateInfo)
        : CoreIPC::Arguments3<uint32_t, const int&, const WebKit::UpdateInfo&>(layerID, tileID, updateInfo)
    {
    }
};

struct UpdateTileForLayer : CoreIPC::Arguments3<uint32_t, const int&, const WebKit::UpdateInfo&> {
    static const Kind messageID = UpdateTileForLayerID;
    typedef CoreIPC::Arguments3<uint32_t, const int&, const WebKit::UpdateInfo&> DecodeType;
    UpdateTileForLayer(uint32_t layerID, const int& tileID, const WebKit::UpdateInfo& updateInfo)
        : CoreIPC::Arguments3<uint32_t, const int&, const WebKit::UpdateInfo&>(layerID, tileID, updateInfo)
    {
    }
};

struct RemoveTileForLayer : CoreIPC::Arguments2<uint32_t, const int&> {
    static const Kind messageID = RemoveTileForLayerID;
    typedef CoreIPC::Arguments2<uint32_t, const int&> DecodeType;
    RemoveTileForLayer(uint32_t layerID, const int& tileID)
        : CoreIPC::Arguments2<uint32_t, const int&>(layerID, tileID)
    {
    }
};

struct CreateDirectlyCompositedImage : CoreIPC::Arguments2<int64_t, const WebKit::ShareableBitmap::Handle&> {
    static const Kind messageID = CreateDirectlyCompositedImageID;
    typedef CoreIPC::Arguments2<int64_t, const WebKit::ShareableBitmap::Handle&> DecodeType;
    CreateDirectlyCompositedImage(int64_t key, const WebKit::ShareableBitmap::Handle& handle)
        : CoreIPC::Arguments2<int64_t, const WebKit::ShareableBitmap::Handle&>(key, handle)
    {
    }
};

struct DestroyDirectlyCompositedImage : CoreIPC::Arguments1<int64_t> {
    static const Kind messageID = DestroyDirectlyCompositedImageID;
    typedef CoreIPC::Arguments1<int64_t> DecodeType;
    explicit DestroyDirectlyCompositedImage(int64_t key)
        : CoreIPC::Arguments1<int64_t>(key)
    {
    }
};

struct DidRenderFrame : CoreIPC::Arguments0 {
    static const Kind messageID = DidRenderFrameID;
    typedef CoreIPC::Arguments0 DecodeType;
};

} // namespace LayerTreeHostProxy

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::LayerTreeHostProxy::Kind> {
    static const MessageClass messageClass = MessageClassLayerTreeHostProxy;
};

} // namespace CoreIPC

#endif // USE(ACCELERATED_COMPOSITING) && USE(TILED_BACKING_STORE)

#endif // LayerTreeHostProxyMessages_h
