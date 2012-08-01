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

#ifndef PluginControllerProxyMessages_h
#define PluginControllerProxyMessages_h

#if ENABLE(PLUGIN_PROCESS)

#include "Arguments.h"
#include "MessageID.h"
#include "ShareableBitmap.h"
#include <wtf/ThreadSafeRefCounted.h>

namespace CoreIPC {
    class ArgumentEncoder;
    class Connection;
    class DataReference;
}

namespace WTF {
    class String;
}

namespace WebCore {
    class AffineTransform;
    class IntRect;
    class IntSize;
}

namespace WebKit {
    class WebMouseEvent;
    class WebWheelEvent;
    class WebKeyboardEvent;
}

namespace Messages {

namespace PluginControllerProxy {

enum Kind {
    GeometryDidChangeID,
    FrameDidFinishLoadingID,
    FrameDidFailID,
    DidEvaluateJavaScriptID,
    StreamDidReceiveResponseID,
    StreamDidReceiveDataID,
    StreamDidFinishLoadingID,
    StreamDidFailID,
    ManualStreamDidReceiveResponseID,
    ManualStreamDidReceiveDataID,
    ManualStreamDidFinishLoadingID,
    ManualStreamDidFailID,
    HandleMouseEventID,
    HandleWheelEventID,
    HandleMouseEnterEventID,
    HandleMouseLeaveEventID,
    HandleKeyboardEventID,
    SetFocusID,
    DidUpdateID,
    PaintEntirePluginID,
    GetPluginScriptableNPObjectID,
#if PLATFORM(MAC)
    SendComplexTextInputID,
#endif
#if PLATFORM(MAC)
    WindowFocusChangedID,
#endif
#if PLATFORM(MAC)
    WindowAndViewFramesChangedID,
#endif
#if PLATFORM(MAC)
    WindowVisibilityChangedID,
#endif
#if PLATFORM(MAC)
    SetLayerHostingModeID,
#endif
    SnapshotID,
    PrivateBrowsingStateChangedID,
    GetFormValueID,
};

struct GeometryDidChange : CoreIPC::Arguments5<const WebCore::IntSize&, const WebCore::IntRect&, const WebCore::AffineTransform&, float, const WebKit::ShareableBitmap::Handle&> {
    static const Kind messageID = GeometryDidChangeID;
    typedef CoreIPC::Arguments5<const WebCore::IntSize&, const WebCore::IntRect&, const WebCore::AffineTransform&, float, const WebKit::ShareableBitmap::Handle&> DecodeType;
    GeometryDidChange(const WebCore::IntSize& pluginSize, const WebCore::IntRect& clipRect, const WebCore::AffineTransform& pluginToRootViewTransform, float scaleFactor, const WebKit::ShareableBitmap::Handle& backingStoreHandle)
        : CoreIPC::Arguments5<const WebCore::IntSize&, const WebCore::IntRect&, const WebCore::AffineTransform&, float, const WebKit::ShareableBitmap::Handle&>(pluginSize, clipRect, pluginToRootViewTransform, scaleFactor, backingStoreHandle)
    {
    }
};

struct FrameDidFinishLoading : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = FrameDidFinishLoadingID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit FrameDidFinishLoading(uint64_t requestID)
        : CoreIPC::Arguments1<uint64_t>(requestID)
    {
    }
};

struct FrameDidFail : CoreIPC::Arguments2<uint64_t, bool> {
    static const Kind messageID = FrameDidFailID;
    typedef CoreIPC::Arguments2<uint64_t, bool> DecodeType;
    FrameDidFail(uint64_t requestID, bool wasCancelled)
        : CoreIPC::Arguments2<uint64_t, bool>(requestID, wasCancelled)
    {
    }
};

struct DidEvaluateJavaScript : CoreIPC::Arguments2<uint64_t, const WTF::String&> {
    static const Kind messageID = DidEvaluateJavaScriptID;
    typedef CoreIPC::Arguments2<uint64_t, const WTF::String&> DecodeType;
    DidEvaluateJavaScript(uint64_t requestID, const WTF::String& result)
        : CoreIPC::Arguments2<uint64_t, const WTF::String&>(requestID, result)
    {
    }
};

struct StreamDidReceiveResponse : CoreIPC::Arguments6<uint64_t, const WTF::String&, uint32_t, uint32_t, const WTF::String&, const WTF::String&> {
    static const Kind messageID = StreamDidReceiveResponseID;
    typedef CoreIPC::Arguments6<uint64_t, const WTF::String&, uint32_t, uint32_t, const WTF::String&, const WTF::String&> DecodeType;
    StreamDidReceiveResponse(uint64_t streamID, const WTF::String& responseURLString, uint32_t streamLength, uint32_t lastModifiedTime, const WTF::String& mimeType, const WTF::String& headers)
        : CoreIPC::Arguments6<uint64_t, const WTF::String&, uint32_t, uint32_t, const WTF::String&, const WTF::String&>(streamID, responseURLString, streamLength, lastModifiedTime, mimeType, headers)
    {
    }
};

struct StreamDidReceiveData : CoreIPC::Arguments2<uint64_t, const CoreIPC::DataReference&> {
    static const Kind messageID = StreamDidReceiveDataID;
    typedef CoreIPC::Arguments2<uint64_t, const CoreIPC::DataReference&> DecodeType;
    StreamDidReceiveData(uint64_t streamID, const CoreIPC::DataReference& data)
        : CoreIPC::Arguments2<uint64_t, const CoreIPC::DataReference&>(streamID, data)
    {
    }
};

struct StreamDidFinishLoading : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = StreamDidFinishLoadingID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit StreamDidFinishLoading(uint64_t streamID)
        : CoreIPC::Arguments1<uint64_t>(streamID)
    {
    }
};

struct StreamDidFail : CoreIPC::Arguments2<uint64_t, bool> {
    static const Kind messageID = StreamDidFailID;
    typedef CoreIPC::Arguments2<uint64_t, bool> DecodeType;
    StreamDidFail(uint64_t streamID, bool wasCancelled)
        : CoreIPC::Arguments2<uint64_t, bool>(streamID, wasCancelled)
    {
    }
};

struct ManualStreamDidReceiveResponse : CoreIPC::Arguments5<const WTF::String&, uint32_t, uint32_t, const WTF::String&, const WTF::String&> {
    static const Kind messageID = ManualStreamDidReceiveResponseID;
    typedef CoreIPC::Arguments5<const WTF::String&, uint32_t, uint32_t, const WTF::String&, const WTF::String&> DecodeType;
    ManualStreamDidReceiveResponse(const WTF::String& responseURLString, uint32_t streamLength, uint32_t lastModifiedTime, const WTF::String& mimeType, const WTF::String& headers)
        : CoreIPC::Arguments5<const WTF::String&, uint32_t, uint32_t, const WTF::String&, const WTF::String&>(responseURLString, streamLength, lastModifiedTime, mimeType, headers)
    {
    }
};

struct ManualStreamDidReceiveData : CoreIPC::Arguments1<const CoreIPC::DataReference&> {
    static const Kind messageID = ManualStreamDidReceiveDataID;
    typedef CoreIPC::Arguments1<const CoreIPC::DataReference&> DecodeType;
    explicit ManualStreamDidReceiveData(const CoreIPC::DataReference& data)
        : CoreIPC::Arguments1<const CoreIPC::DataReference&>(data)
    {
    }
};

struct ManualStreamDidFinishLoading : CoreIPC::Arguments0 {
    static const Kind messageID = ManualStreamDidFinishLoadingID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct ManualStreamDidFail : CoreIPC::Arguments1<bool> {
    static const Kind messageID = ManualStreamDidFailID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit ManualStreamDidFail(bool wasCancelled)
        : CoreIPC::Arguments1<bool>(wasCancelled)
    {
    }
};

struct HandleMouseEvent : CoreIPC::Arguments1<const WebKit::WebMouseEvent&> {
    static const Kind messageID = HandleMouseEventID;
    struct DelayedReply : public ThreadSafeRefCounted<DelayedReply> {
        DelayedReply(PassRefPtr<CoreIPC::Connection>, PassOwnPtr<CoreIPC::ArgumentEncoder>);
        ~DelayedReply();

        bool send(bool handled);

    private:
        RefPtr<CoreIPC::Connection> m_connection;
        OwnPtr<CoreIPC::ArgumentEncoder> m_arguments;
    };

    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<const WebKit::WebMouseEvent&> DecodeType;
    explicit HandleMouseEvent(const WebKit::WebMouseEvent& mouseEvent)
        : CoreIPC::Arguments1<const WebKit::WebMouseEvent&>(mouseEvent)
    {
    }
};

struct HandleWheelEvent : CoreIPC::Arguments1<const WebKit::WebWheelEvent&> {
    static const Kind messageID = HandleWheelEventID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<const WebKit::WebWheelEvent&> DecodeType;
    explicit HandleWheelEvent(const WebKit::WebWheelEvent& wheelEvent)
        : CoreIPC::Arguments1<const WebKit::WebWheelEvent&>(wheelEvent)
    {
    }
};

struct HandleMouseEnterEvent : CoreIPC::Arguments1<const WebKit::WebMouseEvent&> {
    static const Kind messageID = HandleMouseEnterEventID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<const WebKit::WebMouseEvent&> DecodeType;
    explicit HandleMouseEnterEvent(const WebKit::WebMouseEvent& mouseEvent)
        : CoreIPC::Arguments1<const WebKit::WebMouseEvent&>(mouseEvent)
    {
    }
};

struct HandleMouseLeaveEvent : CoreIPC::Arguments1<const WebKit::WebMouseEvent&> {
    static const Kind messageID = HandleMouseLeaveEventID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<const WebKit::WebMouseEvent&> DecodeType;
    explicit HandleMouseLeaveEvent(const WebKit::WebMouseEvent& mouseEvent)
        : CoreIPC::Arguments1<const WebKit::WebMouseEvent&>(mouseEvent)
    {
    }
};

struct HandleKeyboardEvent : CoreIPC::Arguments1<const WebKit::WebKeyboardEvent&> {
    static const Kind messageID = HandleKeyboardEventID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<const WebKit::WebKeyboardEvent&> DecodeType;
    explicit HandleKeyboardEvent(const WebKit::WebKeyboardEvent& keyboardEvent)
        : CoreIPC::Arguments1<const WebKit::WebKeyboardEvent&>(keyboardEvent)
    {
    }
};

struct SetFocus : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetFocusID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetFocus(bool isFocused)
        : CoreIPC::Arguments1<bool>(isFocused)
    {
    }
};

struct DidUpdate : CoreIPC::Arguments0 {
    static const Kind messageID = DidUpdateID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct PaintEntirePlugin : CoreIPC::Arguments0 {
    static const Kind messageID = PaintEntirePluginID;
    typedef CoreIPC::Arguments0 Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct GetPluginScriptableNPObject : CoreIPC::Arguments0 {
    static const Kind messageID = GetPluginScriptableNPObjectID;
    typedef CoreIPC::Arguments1<uint64_t&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

#if PLATFORM(MAC)
struct SendComplexTextInput : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = SendComplexTextInputID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit SendComplexTextInput(const WTF::String& textInput)
        : CoreIPC::Arguments1<const WTF::String&>(textInput)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct WindowFocusChanged : CoreIPC::Arguments1<bool> {
    static const Kind messageID = WindowFocusChangedID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit WindowFocusChanged(bool hasFocus)
        : CoreIPC::Arguments1<bool>(hasFocus)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct WindowAndViewFramesChanged : CoreIPC::Arguments2<const WebCore::IntRect&, const WebCore::IntRect&> {
    static const Kind messageID = WindowAndViewFramesChangedID;
    typedef CoreIPC::Arguments2<const WebCore::IntRect&, const WebCore::IntRect&> DecodeType;
    WindowAndViewFramesChanged(const WebCore::IntRect& windowFrameInScreenCoordinates, const WebCore::IntRect& viewFrameInWindowCoordinates)
        : CoreIPC::Arguments2<const WebCore::IntRect&, const WebCore::IntRect&>(windowFrameInScreenCoordinates, viewFrameInWindowCoordinates)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct WindowVisibilityChanged : CoreIPC::Arguments1<bool> {
    static const Kind messageID = WindowVisibilityChangedID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit WindowVisibilityChanged(bool isVisible)
        : CoreIPC::Arguments1<bool>(isVisible)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SetLayerHostingMode : CoreIPC::Arguments1<uint32_t> {
    static const Kind messageID = SetLayerHostingModeID;
    typedef CoreIPC::Arguments1<uint32_t> DecodeType;
    explicit SetLayerHostingMode(uint32_t layerHostingMode)
        : CoreIPC::Arguments1<uint32_t>(layerHostingMode)
    {
    }
};
#endif

struct Snapshot : CoreIPC::Arguments0 {
    static const Kind messageID = SnapshotID;
    typedef CoreIPC::Arguments1<WebKit::ShareableBitmap::Handle&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct PrivateBrowsingStateChanged : CoreIPC::Arguments1<bool> {
    static const Kind messageID = PrivateBrowsingStateChangedID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit PrivateBrowsingStateChanged(bool isPrivateBrowsingEnabled)
        : CoreIPC::Arguments1<bool>(isPrivateBrowsingEnabled)
    {
    }
};

struct GetFormValue : CoreIPC::Arguments0 {
    static const Kind messageID = GetFormValueID;
    typedef CoreIPC::Arguments2<bool&, WTF::String&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

} // namespace PluginControllerProxy

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::PluginControllerProxy::Kind> {
    static const MessageClass messageClass = MessageClassPluginControllerProxy;
};

} // namespace CoreIPC

#endif // ENABLE(PLUGIN_PROCESS)

#endif // PluginControllerProxyMessages_h
