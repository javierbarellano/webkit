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

#ifndef DrawingAreaProxyMessages_h
#define DrawingAreaProxyMessages_h

#include "Arguments.h"
#include "MessageID.h"

namespace WebKit {
    class UpdateInfo;
    class LayerTreeContext;
}

namespace Messages {

namespace DrawingAreaProxy {

enum Kind {
    UpdateID,
    DidUpdateBackingStoreStateID,
#if USE(ACCELERATED_COMPOSITING)
    EnterAcceleratedCompositingModeID,
#endif
#if USE(ACCELERATED_COMPOSITING)
    ExitAcceleratedCompositingModeID,
#endif
#if USE(ACCELERATED_COMPOSITING)
    UpdateAcceleratedCompositingModeID,
#endif
#if PLATFORM(MAC)
    DidUpdateGeometryID,
#endif
};

struct Update : CoreIPC::Arguments2<uint64_t, const WebKit::UpdateInfo&> {
    static const Kind messageID = UpdateID;
    typedef CoreIPC::Arguments2<uint64_t, const WebKit::UpdateInfo&> DecodeType;
    Update(uint64_t stateID, const WebKit::UpdateInfo& updateInfo)
        : CoreIPC::Arguments2<uint64_t, const WebKit::UpdateInfo&>(stateID, updateInfo)
    {
    }
};

struct DidUpdateBackingStoreState : CoreIPC::Arguments3<uint64_t, const WebKit::UpdateInfo&, const WebKit::LayerTreeContext&> {
    static const Kind messageID = DidUpdateBackingStoreStateID;
    typedef CoreIPC::Arguments3<uint64_t, const WebKit::UpdateInfo&, const WebKit::LayerTreeContext&> DecodeType;
    DidUpdateBackingStoreState(uint64_t backingStoreStateID, const WebKit::UpdateInfo& updateInfo, const WebKit::LayerTreeContext& context)
        : CoreIPC::Arguments3<uint64_t, const WebKit::UpdateInfo&, const WebKit::LayerTreeContext&>(backingStoreStateID, updateInfo, context)
    {
    }
};

#if USE(ACCELERATED_COMPOSITING)
struct EnterAcceleratedCompositingMode : CoreIPC::Arguments2<uint64_t, const WebKit::LayerTreeContext&> {
    static const Kind messageID = EnterAcceleratedCompositingModeID;
    typedef CoreIPC::Arguments2<uint64_t, const WebKit::LayerTreeContext&> DecodeType;
    EnterAcceleratedCompositingMode(uint64_t backingStoreStateID, const WebKit::LayerTreeContext& context)
        : CoreIPC::Arguments2<uint64_t, const WebKit::LayerTreeContext&>(backingStoreStateID, context)
    {
    }
};
#endif

#if USE(ACCELERATED_COMPOSITING)
struct ExitAcceleratedCompositingMode : CoreIPC::Arguments2<uint64_t, const WebKit::UpdateInfo&> {
    static const Kind messageID = ExitAcceleratedCompositingModeID;
    typedef CoreIPC::Arguments2<uint64_t, const WebKit::UpdateInfo&> DecodeType;
    ExitAcceleratedCompositingMode(uint64_t backingStoreStateID, const WebKit::UpdateInfo& updateInfo)
        : CoreIPC::Arguments2<uint64_t, const WebKit::UpdateInfo&>(backingStoreStateID, updateInfo)
    {
    }
};
#endif

#if USE(ACCELERATED_COMPOSITING)
struct UpdateAcceleratedCompositingMode : CoreIPC::Arguments2<uint64_t, const WebKit::LayerTreeContext&> {
    static const Kind messageID = UpdateAcceleratedCompositingModeID;
    typedef CoreIPC::Arguments2<uint64_t, const WebKit::LayerTreeContext&> DecodeType;
    UpdateAcceleratedCompositingMode(uint64_t backingStoreStateID, const WebKit::LayerTreeContext& context)
        : CoreIPC::Arguments2<uint64_t, const WebKit::LayerTreeContext&>(backingStoreStateID, context)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct DidUpdateGeometry : CoreIPC::Arguments0 {
    static const Kind messageID = DidUpdateGeometryID;
    typedef CoreIPC::Arguments0 DecodeType;
};
#endif

} // namespace DrawingAreaProxy

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::DrawingAreaProxy::Kind> {
    static const MessageClass messageClass = MessageClassDrawingAreaProxy;
};

} // namespace CoreIPC

#endif // DrawingAreaProxyMessages_h
