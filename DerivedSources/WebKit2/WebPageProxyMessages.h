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

#ifndef WebPageProxyMessages_h
#define WebPageProxyMessages_h

#include "Arguments.h"
#include "MessageID.h"
#include "SandboxExtension.h"
#include "ShareableBitmap.h"
#include "SharedMemory.h"
#include "WebContextMenuItemData.h"
#include "WebHitTestResult.h"
#include "WebPopupItem.h"
#include <WebCore/FloatRect.h>
#include <WebCore/IntRect.h>
#include <WebCore/KeyboardEvent.h>
#include <WebCore/TextCheckerClient.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace CoreIPC {
    class DataReference;
}

namespace WTF {
    class String;
}

namespace WebCore {
    class Cursor;
    class ProtectionSpace;
    struct GrammarDetail;
    struct WindowFeatures;
    class IntSize;
    class ResourceResponse;
    class DragData;
    class AuthenticationChallenge;
    class ResourceRequest;
    class FloatRect;
    class IntRect;
    class IntPoint;
    struct DragSession;
    class ResourceError;
    struct FileChooserSettings;
    struct ViewportArguments;
}

namespace WebKit {
    struct PlatformPopupMenuData;
    class PlatformCertificateInfo;
    class QtNetworkRequestData;
    struct EditorState;
    class StringPairVector;
    struct WebPageCreationParameters;
    struct DictionaryPopupInfo;
    class InjectedBundleUserMessageEncoder;
    struct WindowGeometry;
}

namespace Messages {

namespace WebPageProxy {

enum Kind {
    CreateNewPageID,
    ShowPageID,
    ClosePageID,
    RunJavaScriptAlertID,
    RunJavaScriptConfirmID,
    RunJavaScriptPromptID,
    ShouldInterruptJavaScriptID,
    MouseDidMoveOverElementID,
    MissingPluginButtonClickedID,
    DidChangeViewportPropertiesID,
    DidReceiveEventID,
    StopResponsivenessTimerID,
    SetCursorID,
    SetCursorHiddenUntilMouseMovesID,
    SetStatusTextID,
    SetToolTipID,
    SetFocusID,
    TakeFocusID,
    FocusedFrameChangedID,
    FrameSetLargestFrameChangedID,
    SetRenderTreeSizeID,
    SetToolbarsAreVisibleID,
    GetToolbarsAreVisibleID,
    SetMenuBarIsVisibleID,
    GetMenuBarIsVisibleID,
    SetStatusBarIsVisibleID,
    GetStatusBarIsVisibleID,
    SetIsResizableID,
    GetIsResizableID,
    SetWindowFrameID,
    GetWindowFrameID,
    ScreenToWindowID,
    WindowToScreenID,
    RunBeforeUnloadConfirmPanelID,
    PageDidScrollID,
    RunOpenPanelID,
    PrintFrameID,
    RunModalID,
    NotifyScrollerThumbIsVisibleInRectID,
    RecommendedScrollbarStyleDidChangeID,
    DidChangeScrollbarsForMainFrameID,
    DidChangeScrollOffsetPinningForMainFrameID,
    DidChangePageCountID,
    DidFailToInitializePluginID,
    SetCanShortCircuitHorizontalWheelEventsID,
#if USE(TILED_BACKING_STORE)
    PageDidRequestScrollID,
#endif
#if PLATFORM(QT)
    DidChangeContentsSizeID,
#endif
#if PLATFORM(QT)
    DidFindZoomableAreaID,
#endif
#if PLATFORM(QT)
    AuthenticationRequiredRequestID,
#endif
#if PLATFORM(QT)
    CertificateVerificationRequestID,
#endif
#if PLATFORM(QT)
    ProxyAuthenticationRequiredRequestID,
#endif
#if ENABLE(TOUCH_EVENTS)
    NeedTouchEventsID,
#endif
    DecidePolicyForResponseID,
    DecidePolicyForNavigationActionID,
    DecidePolicyForNewWindowActionID,
    UnableToImplementPolicyID,
    DidChangeProgressID,
    DidFinishProgressID,
    DidStartProgressID,
    DidCreateMainFrameID,
    DidCreateSubframeID,
    DidSaveFrameToPageCacheID,
    DidRestoreFrameFromPageCacheID,
    DidCommitLoadForFrameID,
    DidFailLoadForFrameID,
    DidFailProvisionalLoadForFrameID,
    DidFinishDocumentLoadForFrameID,
    DidFinishLoadForFrameID,
    DidFirstLayoutForFrameID,
    DidFirstVisuallyNonEmptyLayoutForFrameID,
    DidNewFirstVisuallyNonEmptyLayoutID,
    DidReceiveServerRedirectForProvisionalLoadForFrameID,
    DidRemoveFrameFromHierarchyID,
    DidStartProvisionalLoadForFrameID,
    DidReceiveTitleForFrameID,
    DidDisplayInsecureContentForFrameID,
    DidRunInsecureContentForFrameID,
    DidDetectXSSForFrameID,
    DidSameDocumentNavigationForFrameID,
    FrameDidBecomeFrameSetID,
    DidInitiateLoadForResourceID,
    DidSendRequestForResourceID,
    DidReceiveResponseForResourceID,
    DidReceiveContentLengthForResourceID,
    DidFinishLoadForResourceID,
    DidFailLoadForResourceID,
#if PLATFORM(QT)
    ResolveApplicationSchemeRequestID,
#endif
    DidFinishLoadingDataForCustomRepresentationID,
    WillSubmitFormID,
    VoidCallbackID,
    DataCallbackID,
    StringCallbackID,
    ScriptValueCallbackID,
    ComputedPagesCallbackID,
    ValidateCommandCallbackID,
#if PLATFORM(GTK)
    PrintFinishedCallbackID,
#endif
    PageScaleFactorDidChangeID,
#if PLATFORM(GTK)
    GetEditorCommandsForKeyEventID,
#endif
#if PLATFORM(GTK)
    BindAccessibilityTreeID,
#endif
    BackForwardAddItemID,
    BackForwardGoToItemID,
    BackForwardItemAtIndexID,
    BackForwardBackListCountID,
    BackForwardForwardListCountID,
    BackForwardClearID,
    ShouldGoToBackForwardListItemID,
    WillGoToBackForwardListItemID,
    RegisterEditCommandForUndoID,
    ClearAllEditCommandsID,
    CanUndoRedoID,
    ExecuteUndoRedoID,
    EditorStateChangedID,
#if PLATFORM(WIN)
    DidChangeCompositionSelectionID,
#endif
    DidCountStringMatchesID,
    SetFindIndicatorID,
    DidFindStringID,
    DidFailToFindStringID,
#if PLATFORM(WIN)
    DidInstallOrUninstallPageOverlayID,
#endif
    ShowPopupMenuID,
    HidePopupMenuID,
#if PLATFORM(WIN)
    SetPopupMenuSelectedIndexID,
#endif
    ShowContextMenuID,
    CanAuthenticateAgainstProtectionSpaceInFrameID,
    DidReceiveAuthenticationChallengeID,
    ExceededDatabaseQuotaID,
    RequestGeolocationPermissionForFrameID,
    RequestNotificationPermissionID,
    ShowNotificationID,
#if USE(UNIFIED_TEXT_CHECKING)
    CheckTextOfParagraphID,
#endif
    CheckSpellingOfStringID,
    CheckGrammarOfStringID,
    SpellingUIIsShowingID,
    UpdateSpellingUIWithMisspelledWordID,
    UpdateSpellingUIWithGrammarStringID,
    GetGuessesForWordID,
    LearnWordID,
    IgnoreWordID,
    DidPerformDragControllerActionID,
#if PLATFORM(MAC)
    SetDragImageID,
#endif
#if PLATFORM(MAC)
    SetPromisedDataID,
#endif
#if PLATFORM(WIN)
    StartDragDropID,
#endif
#if PLATFORM(QT) || PLATFORM(GTK)
    StartDragID,
#endif
#if PLATFORM(MAC)
    DidPerformDictionaryLookupID,
#endif
#if PLATFORM(MAC)
    InterpretQueuedKeyEventID,
#endif
#if PLATFORM(MAC)
    ExecuteSavedCommandBySelectorID,
#endif
#if PLATFORM(MAC)
    RegisterWebProcessAccessibilityTokenID,
#endif
#if PLATFORM(MAC)
    PluginFocusOrWindowFocusChangedID,
#endif
#if PLATFORM(MAC)
    SetPluginComplexTextInputStateID,
#endif
#if PLATFORM(MAC)
    GetIsSpeakingID,
#endif
#if PLATFORM(MAC)
    SpeakID,
#endif
#if PLATFORM(MAC)
    StopSpeakingID,
#endif
#if PLATFORM(MAC)
    MakeFirstResponderID,
#endif
#if PLATFORM(MAC)
    SearchWithSpotlightID,
#endif
#if PLATFORM(MAC)
    SubstitutionsPanelIsShowingID,
#endif
#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
    ShowCorrectionPanelID,
#endif
#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
    DismissCorrectionPanelID,
#endif
#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
    DismissCorrectionPanelSoonID,
#endif
#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
    RecordAutocorrectionResponseID,
#endif
#if PLATFORM(WIN)
    SetGestureReachedScrollingLimitID,
#endif
#if PLATFORM(WIN)
    ScheduleChildWindowGeometryUpdateID,
#endif
    SaveRecentSearchesID,
    LoadRecentSearchesID,
};

struct CreateNewPage : CoreIPC::Arguments4<const WebCore::ResourceRequest&, const WebCore::WindowFeatures&, uint32_t, int32_t> {
    static const Kind messageID = CreateNewPageID;
    typedef CoreIPC::Arguments2<uint64_t&, WebKit::WebPageCreationParameters&> Reply;
    typedef CoreIPC::Arguments4<const WebCore::ResourceRequest&, const WebCore::WindowFeatures&, uint32_t, int32_t> DecodeType;
    CreateNewPage(const WebCore::ResourceRequest& request, const WebCore::WindowFeatures& windowFeatures, uint32_t modifiers, int32_t mouseButton)
        : CoreIPC::Arguments4<const WebCore::ResourceRequest&, const WebCore::WindowFeatures&, uint32_t, int32_t>(request, windowFeatures, modifiers, mouseButton)
    {
    }
};

struct ShowPage : CoreIPC::Arguments0 {
    static const Kind messageID = ShowPageID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct ClosePage : CoreIPC::Arguments1<bool> {
    static const Kind messageID = ClosePageID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit ClosePage(bool stopResponsivenessTimer)
        : CoreIPC::Arguments1<bool>(stopResponsivenessTimer)
    {
    }
};

struct RunJavaScriptAlert : CoreIPC::Arguments2<uint64_t, const WTF::String&> {
    static const Kind messageID = RunJavaScriptAlertID;
    typedef CoreIPC::Arguments0 Reply;
    typedef CoreIPC::Arguments2<uint64_t, const WTF::String&> DecodeType;
    RunJavaScriptAlert(uint64_t frameID, const WTF::String& message)
        : CoreIPC::Arguments2<uint64_t, const WTF::String&>(frameID, message)
    {
    }
};

struct RunJavaScriptConfirm : CoreIPC::Arguments2<uint64_t, const WTF::String&> {
    static const Kind messageID = RunJavaScriptConfirmID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments2<uint64_t, const WTF::String&> DecodeType;
    RunJavaScriptConfirm(uint64_t frameID, const WTF::String& message)
        : CoreIPC::Arguments2<uint64_t, const WTF::String&>(frameID, message)
    {
    }
};

struct RunJavaScriptPrompt : CoreIPC::Arguments3<uint64_t, const WTF::String&, const WTF::String&> {
    static const Kind messageID = RunJavaScriptPromptID;
    typedef CoreIPC::Arguments1<WTF::String&> Reply;
    typedef CoreIPC::Arguments3<uint64_t, const WTF::String&, const WTF::String&> DecodeType;
    RunJavaScriptPrompt(uint64_t frameID, const WTF::String& message, const WTF::String& defaultValue)
        : CoreIPC::Arguments3<uint64_t, const WTF::String&, const WTF::String&>(frameID, message, defaultValue)
    {
    }
};

struct ShouldInterruptJavaScript : CoreIPC::Arguments0 {
    static const Kind messageID = ShouldInterruptJavaScriptID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct MouseDidMoveOverElement : CoreIPC::Arguments3<const WebKit::WebHitTestResult::Data&, uint32_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = MouseDidMoveOverElementID;
    typedef CoreIPC::Arguments2<WebKit::WebHitTestResult::Data&, uint32_t&> DecodeType;
    MouseDidMoveOverElement(const WebKit::WebHitTestResult::Data& hitTestResultData, uint32_t modifiers, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments3<const WebKit::WebHitTestResult::Data&, uint32_t, const WebKit::InjectedBundleUserMessageEncoder&>(hitTestResultData, modifiers, userData)
    {
    }
};

struct MissingPluginButtonClicked : CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const WTF::String&> {
    static const Kind messageID = MissingPluginButtonClickedID;
    typedef CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const WTF::String&> DecodeType;
    MissingPluginButtonClicked(const WTF::String& mimeType, const WTF::String& url, const WTF::String& pluginsPageURL)
        : CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const WTF::String&>(mimeType, url, pluginsPageURL)
    {
    }
};

struct DidChangeViewportProperties : CoreIPC::Arguments1<const WebCore::ViewportArguments&> {
    static const Kind messageID = DidChangeViewportPropertiesID;
    typedef CoreIPC::Arguments1<const WebCore::ViewportArguments&> DecodeType;
    explicit DidChangeViewportProperties(const WebCore::ViewportArguments& arguments)
        : CoreIPC::Arguments1<const WebCore::ViewportArguments&>(arguments)
    {
    }
};

struct DidReceiveEvent : CoreIPC::Arguments2<uint32_t, bool> {
    static const Kind messageID = DidReceiveEventID;
    typedef CoreIPC::Arguments2<uint32_t, bool> DecodeType;
    DidReceiveEvent(uint32_t type, bool handled)
        : CoreIPC::Arguments2<uint32_t, bool>(type, handled)
    {
    }
};

struct StopResponsivenessTimer : CoreIPC::Arguments0 {
    static const Kind messageID = StopResponsivenessTimerID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct SetCursor : CoreIPC::Arguments1<const WebCore::Cursor&> {
    static const Kind messageID = SetCursorID;
    typedef CoreIPC::Arguments1<const WebCore::Cursor&> DecodeType;
    explicit SetCursor(const WebCore::Cursor& cursor)
        : CoreIPC::Arguments1<const WebCore::Cursor&>(cursor)
    {
    }
};

struct SetCursorHiddenUntilMouseMoves : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetCursorHiddenUntilMouseMovesID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetCursorHiddenUntilMouseMoves(bool hiddenUntilMouseMoves)
        : CoreIPC::Arguments1<bool>(hiddenUntilMouseMoves)
    {
    }
};

struct SetStatusText : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = SetStatusTextID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit SetStatusText(const WTF::String& statusText)
        : CoreIPC::Arguments1<const WTF::String&>(statusText)
    {
    }
};

struct SetToolTip : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = SetToolTipID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit SetToolTip(const WTF::String& toolTip)
        : CoreIPC::Arguments1<const WTF::String&>(toolTip)
    {
    }
};

struct SetFocus : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetFocusID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetFocus(bool focused)
        : CoreIPC::Arguments1<bool>(focused)
    {
    }
};

struct TakeFocus : CoreIPC::Arguments1<uint32_t> {
    static const Kind messageID = TakeFocusID;
    typedef CoreIPC::Arguments1<uint32_t> DecodeType;
    explicit TakeFocus(uint32_t direction)
        : CoreIPC::Arguments1<uint32_t>(direction)
    {
    }
};

struct FocusedFrameChanged : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = FocusedFrameChangedID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit FocusedFrameChanged(uint64_t frameID)
        : CoreIPC::Arguments1<uint64_t>(frameID)
    {
    }
};

struct FrameSetLargestFrameChanged : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = FrameSetLargestFrameChangedID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit FrameSetLargestFrameChanged(uint64_t frameID)
        : CoreIPC::Arguments1<uint64_t>(frameID)
    {
    }
};

struct SetRenderTreeSize : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = SetRenderTreeSizeID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit SetRenderTreeSize(uint64_t treeSize)
        : CoreIPC::Arguments1<uint64_t>(treeSize)
    {
    }
};

struct SetToolbarsAreVisible : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetToolbarsAreVisibleID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetToolbarsAreVisible(bool toolbarsAreVisible)
        : CoreIPC::Arguments1<bool>(toolbarsAreVisible)
    {
    }
};

struct GetToolbarsAreVisible : CoreIPC::Arguments0 {
    static const Kind messageID = GetToolbarsAreVisibleID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct SetMenuBarIsVisible : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetMenuBarIsVisibleID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetMenuBarIsVisible(bool menuBarIsVisible)
        : CoreIPC::Arguments1<bool>(menuBarIsVisible)
    {
    }
};

struct GetMenuBarIsVisible : CoreIPC::Arguments0 {
    static const Kind messageID = GetMenuBarIsVisibleID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct SetStatusBarIsVisible : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetStatusBarIsVisibleID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetStatusBarIsVisible(bool statusBarIsVisible)
        : CoreIPC::Arguments1<bool>(statusBarIsVisible)
    {
    }
};

struct GetStatusBarIsVisible : CoreIPC::Arguments0 {
    static const Kind messageID = GetStatusBarIsVisibleID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct SetIsResizable : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetIsResizableID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetIsResizable(bool isResizable)
        : CoreIPC::Arguments1<bool>(isResizable)
    {
    }
};

struct GetIsResizable : CoreIPC::Arguments0 {
    static const Kind messageID = GetIsResizableID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct SetWindowFrame : CoreIPC::Arguments1<const WebCore::FloatRect&> {
    static const Kind messageID = SetWindowFrameID;
    typedef CoreIPC::Arguments1<const WebCore::FloatRect&> DecodeType;
    explicit SetWindowFrame(const WebCore::FloatRect& windowFrame)
        : CoreIPC::Arguments1<const WebCore::FloatRect&>(windowFrame)
    {
    }
};

struct GetWindowFrame : CoreIPC::Arguments0 {
    static const Kind messageID = GetWindowFrameID;
    typedef CoreIPC::Arguments1<WebCore::FloatRect&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct ScreenToWindow : CoreIPC::Arguments1<const WebCore::IntPoint&> {
    static const Kind messageID = ScreenToWindowID;
    typedef CoreIPC::Arguments1<WebCore::IntPoint&> Reply;
    typedef CoreIPC::Arguments1<const WebCore::IntPoint&> DecodeType;
    explicit ScreenToWindow(const WebCore::IntPoint& screenPoint)
        : CoreIPC::Arguments1<const WebCore::IntPoint&>(screenPoint)
    {
    }
};

struct WindowToScreen : CoreIPC::Arguments1<const WebCore::IntRect&> {
    static const Kind messageID = WindowToScreenID;
    typedef CoreIPC::Arguments1<WebCore::IntRect&> Reply;
    typedef CoreIPC::Arguments1<const WebCore::IntRect&> DecodeType;
    explicit WindowToScreen(const WebCore::IntRect& rect)
        : CoreIPC::Arguments1<const WebCore::IntRect&>(rect)
    {
    }
};

struct RunBeforeUnloadConfirmPanel : CoreIPC::Arguments2<const WTF::String&, uint64_t> {
    static const Kind messageID = RunBeforeUnloadConfirmPanelID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments2<const WTF::String&, uint64_t> DecodeType;
    RunBeforeUnloadConfirmPanel(const WTF::String& message, uint64_t frameID)
        : CoreIPC::Arguments2<const WTF::String&, uint64_t>(message, frameID)
    {
    }
};

struct PageDidScroll : CoreIPC::Arguments0 {
    static const Kind messageID = PageDidScrollID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct RunOpenPanel : CoreIPC::Arguments2<uint64_t, const WebCore::FileChooserSettings&> {
    static const Kind messageID = RunOpenPanelID;
    typedef CoreIPC::Arguments2<uint64_t, const WebCore::FileChooserSettings&> DecodeType;
    RunOpenPanel(uint64_t frameID, const WebCore::FileChooserSettings& parameters)
        : CoreIPC::Arguments2<uint64_t, const WebCore::FileChooserSettings&>(frameID, parameters)
    {
    }
};

struct PrintFrame : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = PrintFrameID;
    typedef CoreIPC::Arguments0 Reply;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit PrintFrame(uint64_t frameID)
        : CoreIPC::Arguments1<uint64_t>(frameID)
    {
    }
};

struct RunModal : CoreIPC::Arguments0 {
    static const Kind messageID = RunModalID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct NotifyScrollerThumbIsVisibleInRect : CoreIPC::Arguments1<const WebCore::IntRect&> {
    static const Kind messageID = NotifyScrollerThumbIsVisibleInRectID;
    typedef CoreIPC::Arguments1<const WebCore::IntRect&> DecodeType;
    explicit NotifyScrollerThumbIsVisibleInRect(const WebCore::IntRect& scrollerThumb)
        : CoreIPC::Arguments1<const WebCore::IntRect&>(scrollerThumb)
    {
    }
};

struct RecommendedScrollbarStyleDidChange : CoreIPC::Arguments1<int32_t> {
    static const Kind messageID = RecommendedScrollbarStyleDidChangeID;
    typedef CoreIPC::Arguments1<int32_t> DecodeType;
    explicit RecommendedScrollbarStyleDidChange(int32_t newStyle)
        : CoreIPC::Arguments1<int32_t>(newStyle)
    {
    }
};

struct DidChangeScrollbarsForMainFrame : CoreIPC::Arguments2<bool, bool> {
    static const Kind messageID = DidChangeScrollbarsForMainFrameID;
    typedef CoreIPC::Arguments2<bool, bool> DecodeType;
    DidChangeScrollbarsForMainFrame(bool hasHorizontalScrollbar, bool hasVerticalScrollbar)
        : CoreIPC::Arguments2<bool, bool>(hasHorizontalScrollbar, hasVerticalScrollbar)
    {
    }
};

struct DidChangeScrollOffsetPinningForMainFrame : CoreIPC::Arguments2<bool, bool> {
    static const Kind messageID = DidChangeScrollOffsetPinningForMainFrameID;
    typedef CoreIPC::Arguments2<bool, bool> DecodeType;
    DidChangeScrollOffsetPinningForMainFrame(bool hasHorizontalScrollbar, bool hasVerticalScrollbar)
        : CoreIPC::Arguments2<bool, bool>(hasHorizontalScrollbar, hasVerticalScrollbar)
    {
    }
};

struct DidChangePageCount : CoreIPC::Arguments1<const unsigned&> {
    static const Kind messageID = DidChangePageCountID;
    typedef CoreIPC::Arguments1<const unsigned&> DecodeType;
    explicit DidChangePageCount(const unsigned& pageCount)
        : CoreIPC::Arguments1<const unsigned&>(pageCount)
    {
    }
};

struct DidFailToInitializePlugin : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = DidFailToInitializePluginID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit DidFailToInitializePlugin(const WTF::String& mimeType)
        : CoreIPC::Arguments1<const WTF::String&>(mimeType)
    {
    }
};

struct SetCanShortCircuitHorizontalWheelEvents : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetCanShortCircuitHorizontalWheelEventsID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetCanShortCircuitHorizontalWheelEvents(bool canShortCircuitHorizontalWheelEvents)
        : CoreIPC::Arguments1<bool>(canShortCircuitHorizontalWheelEvents)
    {
    }
};

#if USE(TILED_BACKING_STORE)
struct PageDidRequestScroll : CoreIPC::Arguments1<const WebCore::IntPoint&> {
    static const Kind messageID = PageDidRequestScrollID;
    typedef CoreIPC::Arguments1<const WebCore::IntPoint&> DecodeType;
    explicit PageDidRequestScroll(const WebCore::IntPoint& point)
        : CoreIPC::Arguments1<const WebCore::IntPoint&>(point)
    {
    }
};
#endif

#if PLATFORM(QT)
struct DidChangeContentsSize : CoreIPC::Arguments1<const WebCore::IntSize&> {
    static const Kind messageID = DidChangeContentsSizeID;
    typedef CoreIPC::Arguments1<const WebCore::IntSize&> DecodeType;
    explicit DidChangeContentsSize(const WebCore::IntSize& newSize)
        : CoreIPC::Arguments1<const WebCore::IntSize&>(newSize)
    {
    }
};
#endif

#if PLATFORM(QT)
struct DidFindZoomableArea : CoreIPC::Arguments2<const WebCore::IntPoint&, const WebCore::IntRect&> {
    static const Kind messageID = DidFindZoomableAreaID;
    typedef CoreIPC::Arguments2<const WebCore::IntPoint&, const WebCore::IntRect&> DecodeType;
    DidFindZoomableArea(const WebCore::IntPoint& target, const WebCore::IntRect& area)
        : CoreIPC::Arguments2<const WebCore::IntPoint&, const WebCore::IntRect&>(target, area)
    {
    }
};
#endif

#if PLATFORM(QT)
struct AuthenticationRequiredRequest : CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const WTF::String&> {
    static const Kind messageID = AuthenticationRequiredRequestID;
    typedef CoreIPC::Arguments2<WTF::String&, WTF::String&> Reply;
    typedef CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const WTF::String&> DecodeType;
    AuthenticationRequiredRequest(const WTF::String& hostname, const WTF::String& realm, const WTF::String& prefilledUsername)
        : CoreIPC::Arguments3<const WTF::String&, const WTF::String&, const WTF::String&>(hostname, realm, prefilledUsername)
    {
    }
};
#endif

#if PLATFORM(QT)
struct CertificateVerificationRequest : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = CertificateVerificationRequestID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit CertificateVerificationRequest(const WTF::String& hostname)
        : CoreIPC::Arguments1<const WTF::String&>(hostname)
    {
    }
};
#endif

#if PLATFORM(QT)
struct ProxyAuthenticationRequiredRequest : CoreIPC::Arguments3<const WTF::String&, uint16_t, const WTF::String&> {
    static const Kind messageID = ProxyAuthenticationRequiredRequestID;
    typedef CoreIPC::Arguments2<WTF::String&, WTF::String&> Reply;
    typedef CoreIPC::Arguments3<const WTF::String&, uint16_t, const WTF::String&> DecodeType;
    ProxyAuthenticationRequiredRequest(const WTF::String& hostname, uint16_t port, const WTF::String& prefilledUsername)
        : CoreIPC::Arguments3<const WTF::String&, uint16_t, const WTF::String&>(hostname, port, prefilledUsername)
    {
    }
};
#endif

#if ENABLE(TOUCH_EVENTS)
struct NeedTouchEvents : CoreIPC::Arguments1<bool> {
    static const Kind messageID = NeedTouchEventsID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit NeedTouchEvents(bool needTouchEvents)
        : CoreIPC::Arguments1<bool>(needTouchEvents)
    {
    }
};
#endif

struct DecidePolicyForResponse : CoreIPC::Arguments5<uint64_t, const WebCore::ResourceResponse&, const WebCore::ResourceRequest&, uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DecidePolicyForResponseID;
    typedef CoreIPC::Arguments3<bool&, uint64_t&, uint64_t&> Reply;
    typedef CoreIPC::Arguments4<uint64_t&, WebCore::ResourceResponse&, WebCore::ResourceRequest&, uint64_t&> DecodeType;
    DecidePolicyForResponse(uint64_t frameID, const WebCore::ResourceResponse& response, const WebCore::ResourceRequest& request, uint64_t listenerID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments5<uint64_t, const WebCore::ResourceResponse&, const WebCore::ResourceRequest&, uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, response, request, listenerID, userData)
    {
    }
};

struct DecidePolicyForNavigationAction : CoreIPC::Arguments7<uint64_t, uint32_t, uint32_t, int32_t, const WebCore::ResourceRequest&, uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DecidePolicyForNavigationActionID;
    typedef CoreIPC::Arguments3<bool&, uint64_t&, uint64_t&> Reply;
    typedef CoreIPC::Arguments6<uint64_t&, uint32_t&, uint32_t&, int32_t&, WebCore::ResourceRequest&, uint64_t&> DecodeType;
    DecidePolicyForNavigationAction(uint64_t frameID, uint32_t navigationType, uint32_t modifiers, int32_t mouseButton, const WebCore::ResourceRequest& request, uint64_t listenerID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments7<uint64_t, uint32_t, uint32_t, int32_t, const WebCore::ResourceRequest&, uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, navigationType, modifiers, mouseButton, request, listenerID, userData)
    {
    }
};

struct DecidePolicyForNewWindowAction : CoreIPC::Arguments8<uint64_t, uint32_t, uint32_t, int32_t, const WebCore::ResourceRequest&, const WTF::String&, uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DecidePolicyForNewWindowActionID;
    typedef CoreIPC::Arguments7<uint64_t&, uint32_t&, uint32_t&, int32_t&, WebCore::ResourceRequest&, WTF::String&, uint64_t&> DecodeType;
    DecidePolicyForNewWindowAction(uint64_t frameID, uint32_t navigationType, uint32_t modifiers, int32_t mouseButton, const WebCore::ResourceRequest& request, const WTF::String& frameName, uint64_t listenerID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments8<uint64_t, uint32_t, uint32_t, int32_t, const WebCore::ResourceRequest&, const WTF::String&, uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, navigationType, modifiers, mouseButton, request, frameName, listenerID, userData)
    {
    }
};

struct UnableToImplementPolicy : CoreIPC::Arguments3<uint64_t, const WebCore::ResourceError&, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = UnableToImplementPolicyID;
    typedef CoreIPC::Arguments2<uint64_t&, WebCore::ResourceError&> DecodeType;
    UnableToImplementPolicy(uint64_t frameID, const WebCore::ResourceError& error, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments3<uint64_t, const WebCore::ResourceError&, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, error, userData)
    {
    }
};

struct DidChangeProgress : CoreIPC::Arguments1<double> {
    static const Kind messageID = DidChangeProgressID;
    typedef CoreIPC::Arguments1<double> DecodeType;
    explicit DidChangeProgress(double value)
        : CoreIPC::Arguments1<double>(value)
    {
    }
};

struct DidFinishProgress : CoreIPC::Arguments0 {
    static const Kind messageID = DidFinishProgressID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct DidStartProgress : CoreIPC::Arguments0 {
    static const Kind messageID = DidStartProgressID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct DidCreateMainFrame : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = DidCreateMainFrameID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit DidCreateMainFrame(uint64_t frameID)
        : CoreIPC::Arguments1<uint64_t>(frameID)
    {
    }
};

struct DidCreateSubframe : CoreIPC::Arguments2<uint64_t, uint64_t> {
    static const Kind messageID = DidCreateSubframeID;
    typedef CoreIPC::Arguments2<uint64_t, uint64_t> DecodeType;
    DidCreateSubframe(uint64_t frameID, uint64_t parentFrameID)
        : CoreIPC::Arguments2<uint64_t, uint64_t>(frameID, parentFrameID)
    {
    }
};

struct DidSaveFrameToPageCache : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = DidSaveFrameToPageCacheID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit DidSaveFrameToPageCache(uint64_t frameID)
        : CoreIPC::Arguments1<uint64_t>(frameID)
    {
    }
};

struct DidRestoreFrameFromPageCache : CoreIPC::Arguments2<uint64_t, uint64_t> {
    static const Kind messageID = DidRestoreFrameFromPageCacheID;
    typedef CoreIPC::Arguments2<uint64_t, uint64_t> DecodeType;
    DidRestoreFrameFromPageCache(uint64_t frameID, uint64_t parentFrameID)
        : CoreIPC::Arguments2<uint64_t, uint64_t>(frameID, parentFrameID)
    {
    }
};

struct DidCommitLoadForFrame : CoreIPC::Arguments5<uint64_t, const WTF::String&, bool, const WebKit::PlatformCertificateInfo&, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidCommitLoadForFrameID;
    typedef CoreIPC::Arguments4<uint64_t&, WTF::String&, bool&, WebKit::PlatformCertificateInfo&> DecodeType;
    DidCommitLoadForFrame(uint64_t frameID, const WTF::String& mimeType, bool hasCustomRepresentation, const WebKit::PlatformCertificateInfo& certificateInfo, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments5<uint64_t, const WTF::String&, bool, const WebKit::PlatformCertificateInfo&, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, mimeType, hasCustomRepresentation, certificateInfo, userData)
    {
    }
};

struct DidFailLoadForFrame : CoreIPC::Arguments3<uint64_t, const WebCore::ResourceError&, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidFailLoadForFrameID;
    typedef CoreIPC::Arguments2<uint64_t&, WebCore::ResourceError&> DecodeType;
    DidFailLoadForFrame(uint64_t frameID, const WebCore::ResourceError& error, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments3<uint64_t, const WebCore::ResourceError&, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, error, userData)
    {
    }
};

struct DidFailProvisionalLoadForFrame : CoreIPC::Arguments3<uint64_t, const WebCore::ResourceError&, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidFailProvisionalLoadForFrameID;
    typedef CoreIPC::Arguments2<uint64_t&, WebCore::ResourceError&> DecodeType;
    DidFailProvisionalLoadForFrame(uint64_t frameID, const WebCore::ResourceError& error, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments3<uint64_t, const WebCore::ResourceError&, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, error, userData)
    {
    }
};

struct DidFinishDocumentLoadForFrame : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidFinishDocumentLoadForFrameID;
    typedef CoreIPC::Arguments1<uint64_t&> DecodeType;
    DidFinishDocumentLoadForFrame(uint64_t frameID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, userData)
    {
    }
};

struct DidFinishLoadForFrame : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidFinishLoadForFrameID;
    typedef CoreIPC::Arguments1<uint64_t&> DecodeType;
    DidFinishLoadForFrame(uint64_t frameID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, userData)
    {
    }
};

struct DidFirstLayoutForFrame : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidFirstLayoutForFrameID;
    typedef CoreIPC::Arguments1<uint64_t&> DecodeType;
    DidFirstLayoutForFrame(uint64_t frameID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, userData)
    {
    }
};

struct DidFirstVisuallyNonEmptyLayoutForFrame : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidFirstVisuallyNonEmptyLayoutForFrameID;
    typedef CoreIPC::Arguments1<uint64_t&> DecodeType;
    DidFirstVisuallyNonEmptyLayoutForFrame(uint64_t frameID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, userData)
    {
    }
};

struct DidNewFirstVisuallyNonEmptyLayout : CoreIPC::Arguments1<const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidNewFirstVisuallyNonEmptyLayoutID;
    typedef CoreIPC::Arguments0 DecodeType;
    explicit DidNewFirstVisuallyNonEmptyLayout(const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments1<const WebKit::InjectedBundleUserMessageEncoder&>(userData)
    {
    }
};

struct DidReceiveServerRedirectForProvisionalLoadForFrame : CoreIPC::Arguments3<uint64_t, const WTF::String&, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidReceiveServerRedirectForProvisionalLoadForFrameID;
    typedef CoreIPC::Arguments2<uint64_t&, WTF::String&> DecodeType;
    DidReceiveServerRedirectForProvisionalLoadForFrame(uint64_t frameID, const WTF::String& url, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments3<uint64_t, const WTF::String&, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, url, userData)
    {
    }
};

struct DidRemoveFrameFromHierarchy : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidRemoveFrameFromHierarchyID;
    typedef CoreIPC::Arguments1<uint64_t&> DecodeType;
    DidRemoveFrameFromHierarchy(uint64_t frameID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, userData)
    {
    }
};

struct DidStartProvisionalLoadForFrame : CoreIPC::Arguments4<uint64_t, const WTF::String&, const WTF::String&, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidStartProvisionalLoadForFrameID;
    typedef CoreIPC::Arguments3<uint64_t&, WTF::String&, WTF::String&> DecodeType;
    DidStartProvisionalLoadForFrame(uint64_t frameID, const WTF::String& url, const WTF::String& unreachableURL, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments4<uint64_t, const WTF::String&, const WTF::String&, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, url, unreachableURL, userData)
    {
    }
};

struct DidReceiveTitleForFrame : CoreIPC::Arguments3<uint64_t, const WTF::String&, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidReceiveTitleForFrameID;
    typedef CoreIPC::Arguments2<uint64_t&, WTF::String&> DecodeType;
    DidReceiveTitleForFrame(uint64_t frameID, const WTF::String& title, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments3<uint64_t, const WTF::String&, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, title, userData)
    {
    }
};

struct DidDisplayInsecureContentForFrame : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidDisplayInsecureContentForFrameID;
    typedef CoreIPC::Arguments1<uint64_t&> DecodeType;
    DidDisplayInsecureContentForFrame(uint64_t frameID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, userData)
    {
    }
};

struct DidRunInsecureContentForFrame : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidRunInsecureContentForFrameID;
    typedef CoreIPC::Arguments1<uint64_t&> DecodeType;
    DidRunInsecureContentForFrame(uint64_t frameID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, userData)
    {
    }
};

struct DidDetectXSSForFrame : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidDetectXSSForFrameID;
    typedef CoreIPC::Arguments1<uint64_t&> DecodeType;
    DidDetectXSSForFrame(uint64_t frameID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments2<uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, userData)
    {
    }
};

struct DidSameDocumentNavigationForFrame : CoreIPC::Arguments4<uint64_t, uint32_t, const WTF::String&, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = DidSameDocumentNavigationForFrameID;
    typedef CoreIPC::Arguments3<uint64_t&, uint32_t&, WTF::String&> DecodeType;
    DidSameDocumentNavigationForFrame(uint64_t frameID, uint32_t type, const WTF::String& url, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments4<uint64_t, uint32_t, const WTF::String&, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, type, url, userData)
    {
    }
};

struct FrameDidBecomeFrameSet : CoreIPC::Arguments2<uint64_t, bool> {
    static const Kind messageID = FrameDidBecomeFrameSetID;
    typedef CoreIPC::Arguments2<uint64_t, bool> DecodeType;
    FrameDidBecomeFrameSet(uint64_t frameID, bool value)
        : CoreIPC::Arguments2<uint64_t, bool>(frameID, value)
    {
    }
};

struct DidInitiateLoadForResource : CoreIPC::Arguments4<uint64_t, uint64_t, const WebCore::ResourceRequest&, bool> {
    static const Kind messageID = DidInitiateLoadForResourceID;
    typedef CoreIPC::Arguments4<uint64_t, uint64_t, const WebCore::ResourceRequest&, bool> DecodeType;
    DidInitiateLoadForResource(uint64_t frameID, uint64_t resourceIdentifier, const WebCore::ResourceRequest& request, bool pageIsProvisionallyLoading)
        : CoreIPC::Arguments4<uint64_t, uint64_t, const WebCore::ResourceRequest&, bool>(frameID, resourceIdentifier, request, pageIsProvisionallyLoading)
    {
    }
};

struct DidSendRequestForResource : CoreIPC::Arguments4<uint64_t, uint64_t, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&> {
    static const Kind messageID = DidSendRequestForResourceID;
    typedef CoreIPC::Arguments4<uint64_t, uint64_t, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&> DecodeType;
    DidSendRequestForResource(uint64_t frameID, uint64_t resourceIdentifier, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& redirectResponse)
        : CoreIPC::Arguments4<uint64_t, uint64_t, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&>(frameID, resourceIdentifier, request, redirectResponse)
    {
    }
};

struct DidReceiveResponseForResource : CoreIPC::Arguments3<uint64_t, uint64_t, const WebCore::ResourceResponse&> {
    static const Kind messageID = DidReceiveResponseForResourceID;
    typedef CoreIPC::Arguments3<uint64_t, uint64_t, const WebCore::ResourceResponse&> DecodeType;
    DidReceiveResponseForResource(uint64_t frameID, uint64_t resourceIdentifier, const WebCore::ResourceResponse& response)
        : CoreIPC::Arguments3<uint64_t, uint64_t, const WebCore::ResourceResponse&>(frameID, resourceIdentifier, response)
    {
    }
};

struct DidReceiveContentLengthForResource : CoreIPC::Arguments3<uint64_t, uint64_t, uint64_t> {
    static const Kind messageID = DidReceiveContentLengthForResourceID;
    typedef CoreIPC::Arguments3<uint64_t, uint64_t, uint64_t> DecodeType;
    DidReceiveContentLengthForResource(uint64_t frameID, uint64_t resourceIdentifier, uint64_t contentLength)
        : CoreIPC::Arguments3<uint64_t, uint64_t, uint64_t>(frameID, resourceIdentifier, contentLength)
    {
    }
};

struct DidFinishLoadForResource : CoreIPC::Arguments2<uint64_t, uint64_t> {
    static const Kind messageID = DidFinishLoadForResourceID;
    typedef CoreIPC::Arguments2<uint64_t, uint64_t> DecodeType;
    DidFinishLoadForResource(uint64_t frameID, uint64_t resourceIdentifier)
        : CoreIPC::Arguments2<uint64_t, uint64_t>(frameID, resourceIdentifier)
    {
    }
};

struct DidFailLoadForResource : CoreIPC::Arguments3<uint64_t, uint64_t, const WebCore::ResourceError&> {
    static const Kind messageID = DidFailLoadForResourceID;
    typedef CoreIPC::Arguments3<uint64_t, uint64_t, const WebCore::ResourceError&> DecodeType;
    DidFailLoadForResource(uint64_t frameID, uint64_t resourceIdentifier, const WebCore::ResourceError& error)
        : CoreIPC::Arguments3<uint64_t, uint64_t, const WebCore::ResourceError&>(frameID, resourceIdentifier, error)
    {
    }
};

#if PLATFORM(QT)
struct ResolveApplicationSchemeRequest : CoreIPC::Arguments1<const WebKit::QtNetworkRequestData&> {
    static const Kind messageID = ResolveApplicationSchemeRequestID;
    typedef CoreIPC::Arguments1<const WebKit::QtNetworkRequestData&> DecodeType;
    explicit ResolveApplicationSchemeRequest(const WebKit::QtNetworkRequestData& request)
        : CoreIPC::Arguments1<const WebKit::QtNetworkRequestData&>(request)
    {
    }
};
#endif

struct DidFinishLoadingDataForCustomRepresentation : CoreIPC::Arguments2<const WTF::String&, const CoreIPC::DataReference&> {
    static const Kind messageID = DidFinishLoadingDataForCustomRepresentationID;
    typedef CoreIPC::Arguments2<const WTF::String&, const CoreIPC::DataReference&> DecodeType;
    DidFinishLoadingDataForCustomRepresentation(const WTF::String& suggestedFilename, const CoreIPC::DataReference& data)
        : CoreIPC::Arguments2<const WTF::String&, const CoreIPC::DataReference&>(suggestedFilename, data)
    {
    }
};

struct WillSubmitForm : CoreIPC::Arguments5<uint64_t, uint64_t, const WebKit::StringPairVector&, uint64_t, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = WillSubmitFormID;
    typedef CoreIPC::Arguments4<uint64_t&, uint64_t&, WebKit::StringPairVector&, uint64_t&> DecodeType;
    WillSubmitForm(uint64_t frameID, uint64_t sourceFrameID, const WebKit::StringPairVector& textFieldValues, uint64_t listenerID, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments5<uint64_t, uint64_t, const WebKit::StringPairVector&, uint64_t, const WebKit::InjectedBundleUserMessageEncoder&>(frameID, sourceFrameID, textFieldValues, listenerID, userData)
    {
    }
};

struct VoidCallback : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = VoidCallbackID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit VoidCallback(uint64_t callbackID)
        : CoreIPC::Arguments1<uint64_t>(callbackID)
    {
    }
};

struct DataCallback : CoreIPC::Arguments2<const CoreIPC::DataReference&, uint64_t> {
    static const Kind messageID = DataCallbackID;
    typedef CoreIPC::Arguments2<const CoreIPC::DataReference&, uint64_t> DecodeType;
    DataCallback(const CoreIPC::DataReference& resultData, uint64_t callbackID)
        : CoreIPC::Arguments2<const CoreIPC::DataReference&, uint64_t>(resultData, callbackID)
    {
    }
};

struct StringCallback : CoreIPC::Arguments2<const WTF::String&, uint64_t> {
    static const Kind messageID = StringCallbackID;
    typedef CoreIPC::Arguments2<const WTF::String&, uint64_t> DecodeType;
    StringCallback(const WTF::String& resultString, uint64_t callbackID)
        : CoreIPC::Arguments2<const WTF::String&, uint64_t>(resultString, callbackID)
    {
    }
};

struct ScriptValueCallback : CoreIPC::Arguments2<const CoreIPC::DataReference&, uint64_t> {
    static const Kind messageID = ScriptValueCallbackID;
    typedef CoreIPC::Arguments2<const CoreIPC::DataReference&, uint64_t> DecodeType;
    ScriptValueCallback(const CoreIPC::DataReference& resultData, uint64_t callbackID)
        : CoreIPC::Arguments2<const CoreIPC::DataReference&, uint64_t>(resultData, callbackID)
    {
    }
};

struct ComputedPagesCallback : CoreIPC::Arguments3<const Vector<WebCore::IntRect>&, double, uint64_t> {
    static const Kind messageID = ComputedPagesCallbackID;
    typedef CoreIPC::Arguments3<const Vector<WebCore::IntRect>&, double, uint64_t> DecodeType;
    ComputedPagesCallback(const Vector<WebCore::IntRect>& pageRects, double totalScaleFactorForPrinting, uint64_t callbackID)
        : CoreIPC::Arguments3<const Vector<WebCore::IntRect>&, double, uint64_t>(pageRects, totalScaleFactorForPrinting, callbackID)
    {
    }
};

struct ValidateCommandCallback : CoreIPC::Arguments4<const WTF::String&, bool, int32_t, uint64_t> {
    static const Kind messageID = ValidateCommandCallbackID;
    typedef CoreIPC::Arguments4<const WTF::String&, bool, int32_t, uint64_t> DecodeType;
    ValidateCommandCallback(const WTF::String& command, bool isEnabled, int32_t state, uint64_t callbackID)
        : CoreIPC::Arguments4<const WTF::String&, bool, int32_t, uint64_t>(command, isEnabled, state, callbackID)
    {
    }
};

#if PLATFORM(GTK)
struct PrintFinishedCallback : CoreIPC::Arguments2<const WebCore::ResourceError&, uint64_t> {
    static const Kind messageID = PrintFinishedCallbackID;
    typedef CoreIPC::Arguments2<const WebCore::ResourceError&, uint64_t> DecodeType;
    PrintFinishedCallback(const WebCore::ResourceError& error, uint64_t callbackID)
        : CoreIPC::Arguments2<const WebCore::ResourceError&, uint64_t>(error, callbackID)
    {
    }
};
#endif

struct PageScaleFactorDidChange : CoreIPC::Arguments1<double> {
    static const Kind messageID = PageScaleFactorDidChangeID;
    typedef CoreIPC::Arguments1<double> DecodeType;
    explicit PageScaleFactorDidChange(double scaleFactor)
        : CoreIPC::Arguments1<double>(scaleFactor)
    {
    }
};

#if PLATFORM(GTK)
struct GetEditorCommandsForKeyEvent : CoreIPC::Arguments1<const AtomicString&> {
    static const Kind messageID = GetEditorCommandsForKeyEventID;
    typedef CoreIPC::Arguments1<Vector<WTF::String>&> Reply;
    typedef CoreIPC::Arguments1<const AtomicString&> DecodeType;
    explicit GetEditorCommandsForKeyEvent(const AtomicString& eventType)
        : CoreIPC::Arguments1<const AtomicString&>(eventType)
    {
    }
};
#endif

#if PLATFORM(GTK)
struct BindAccessibilityTree : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = BindAccessibilityTreeID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit BindAccessibilityTree(const WTF::String& plugID)
        : CoreIPC::Arguments1<const WTF::String&>(plugID)
    {
    }
};
#endif

struct BackForwardAddItem : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = BackForwardAddItemID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit BackForwardAddItem(uint64_t itemID)
        : CoreIPC::Arguments1<uint64_t>(itemID)
    {
    }
};

struct BackForwardGoToItem : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = BackForwardGoToItemID;
    typedef CoreIPC::Arguments1<WebKit::SandboxExtension::Handle&> Reply;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit BackForwardGoToItem(uint64_t itemID)
        : CoreIPC::Arguments1<uint64_t>(itemID)
    {
    }
};

struct BackForwardItemAtIndex : CoreIPC::Arguments1<int32_t> {
    static const Kind messageID = BackForwardItemAtIndexID;
    typedef CoreIPC::Arguments1<uint64_t&> Reply;
    typedef CoreIPC::Arguments1<int32_t> DecodeType;
    explicit BackForwardItemAtIndex(int32_t itemIndex)
        : CoreIPC::Arguments1<int32_t>(itemIndex)
    {
    }
};

struct BackForwardBackListCount : CoreIPC::Arguments0 {
    static const Kind messageID = BackForwardBackListCountID;
    typedef CoreIPC::Arguments1<int32_t&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct BackForwardForwardListCount : CoreIPC::Arguments0 {
    static const Kind messageID = BackForwardForwardListCountID;
    typedef CoreIPC::Arguments1<int32_t&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct BackForwardClear : CoreIPC::Arguments0 {
    static const Kind messageID = BackForwardClearID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct ShouldGoToBackForwardListItem : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = ShouldGoToBackForwardListItemID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit ShouldGoToBackForwardListItem(uint64_t itemID)
        : CoreIPC::Arguments1<uint64_t>(itemID)
    {
    }
};

struct WillGoToBackForwardListItem : CoreIPC::Arguments1<uint64_t> {
    static const Kind messageID = WillGoToBackForwardListItemID;
    typedef CoreIPC::Arguments1<uint64_t> DecodeType;
    explicit WillGoToBackForwardListItem(uint64_t itemID)
        : CoreIPC::Arguments1<uint64_t>(itemID)
    {
    }
};

struct RegisterEditCommandForUndo : CoreIPC::Arguments2<uint64_t, uint32_t> {
    static const Kind messageID = RegisterEditCommandForUndoID;
    typedef CoreIPC::Arguments2<uint64_t, uint32_t> DecodeType;
    RegisterEditCommandForUndo(uint64_t commandID, uint32_t editAction)
        : CoreIPC::Arguments2<uint64_t, uint32_t>(commandID, editAction)
    {
    }
};

struct ClearAllEditCommands : CoreIPC::Arguments0 {
    static const Kind messageID = ClearAllEditCommandsID;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct CanUndoRedo : CoreIPC::Arguments1<uint32_t> {
    static const Kind messageID = CanUndoRedoID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<uint32_t> DecodeType;
    explicit CanUndoRedo(uint32_t action)
        : CoreIPC::Arguments1<uint32_t>(action)
    {
    }
};

struct ExecuteUndoRedo : CoreIPC::Arguments1<uint32_t> {
    static const Kind messageID = ExecuteUndoRedoID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<uint32_t> DecodeType;
    explicit ExecuteUndoRedo(uint32_t action)
        : CoreIPC::Arguments1<uint32_t>(action)
    {
    }
};

struct EditorStateChanged : CoreIPC::Arguments1<const WebKit::EditorState&> {
    static const Kind messageID = EditorStateChangedID;
    typedef CoreIPC::Arguments1<const WebKit::EditorState&> DecodeType;
    explicit EditorStateChanged(const WebKit::EditorState& editorState)
        : CoreIPC::Arguments1<const WebKit::EditorState&>(editorState)
    {
    }
};

#if PLATFORM(WIN)
struct DidChangeCompositionSelection : CoreIPC::Arguments1<bool> {
    static const Kind messageID = DidChangeCompositionSelectionID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit DidChangeCompositionSelection(bool hasChanged)
        : CoreIPC::Arguments1<bool>(hasChanged)
    {
    }
};
#endif

struct DidCountStringMatches : CoreIPC::Arguments2<const WTF::String&, uint32_t> {
    static const Kind messageID = DidCountStringMatchesID;
    typedef CoreIPC::Arguments2<const WTF::String&, uint32_t> DecodeType;
    DidCountStringMatches(const WTF::String& string, uint32_t matchCount)
        : CoreIPC::Arguments2<const WTF::String&, uint32_t>(string, matchCount)
    {
    }
};

struct SetFindIndicator : CoreIPC::Arguments6<const WebCore::FloatRect&, const Vector<WebCore::FloatRect>&, float, const WebKit::ShareableBitmap::Handle&, bool, bool> {
    static const Kind messageID = SetFindIndicatorID;
    typedef CoreIPC::Arguments6<const WebCore::FloatRect&, const Vector<WebCore::FloatRect>&, float, const WebKit::ShareableBitmap::Handle&, bool, bool> DecodeType;
    SetFindIndicator(const WebCore::FloatRect& selectionRect, const Vector<WebCore::FloatRect>& textRects, float contentImageScaleFactor, const WebKit::ShareableBitmap::Handle& contentImageHandle, bool fadeOut, bool animate)
        : CoreIPC::Arguments6<const WebCore::FloatRect&, const Vector<WebCore::FloatRect>&, float, const WebKit::ShareableBitmap::Handle&, bool, bool>(selectionRect, textRects, contentImageScaleFactor, contentImageHandle, fadeOut, animate)
    {
    }
};

struct DidFindString : CoreIPC::Arguments2<const WTF::String&, uint32_t> {
    static const Kind messageID = DidFindStringID;
    typedef CoreIPC::Arguments2<const WTF::String&, uint32_t> DecodeType;
    DidFindString(const WTF::String& string, uint32_t matchCount)
        : CoreIPC::Arguments2<const WTF::String&, uint32_t>(string, matchCount)
    {
    }
};

struct DidFailToFindString : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = DidFailToFindStringID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit DidFailToFindString(const WTF::String& string)
        : CoreIPC::Arguments1<const WTF::String&>(string)
    {
    }
};

#if PLATFORM(WIN)
struct DidInstallOrUninstallPageOverlay : CoreIPC::Arguments1<bool> {
    static const Kind messageID = DidInstallOrUninstallPageOverlayID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit DidInstallOrUninstallPageOverlay(bool didInstall)
        : CoreIPC::Arguments1<bool>(didInstall)
    {
    }
};
#endif

struct ShowPopupMenu : CoreIPC::Arguments5<const WebCore::IntRect&, uint64_t, const Vector<WebKit::WebPopupItem>&, int32_t, const WebKit::PlatformPopupMenuData&> {
    static const Kind messageID = ShowPopupMenuID;
    typedef CoreIPC::Arguments5<const WebCore::IntRect&, uint64_t, const Vector<WebKit::WebPopupItem>&, int32_t, const WebKit::PlatformPopupMenuData&> DecodeType;
    ShowPopupMenu(const WebCore::IntRect& rect, uint64_t textDirection, const Vector<WebKit::WebPopupItem>& items, int32_t selectedIndex, const WebKit::PlatformPopupMenuData& data)
        : CoreIPC::Arguments5<const WebCore::IntRect&, uint64_t, const Vector<WebKit::WebPopupItem>&, int32_t, const WebKit::PlatformPopupMenuData&>(rect, textDirection, items, selectedIndex, data)
    {
    }
};

struct HidePopupMenu : CoreIPC::Arguments0 {
    static const Kind messageID = HidePopupMenuID;
    typedef CoreIPC::Arguments0 DecodeType;
};

#if PLATFORM(WIN)
struct SetPopupMenuSelectedIndex : CoreIPC::Arguments1<int32_t> {
    static const Kind messageID = SetPopupMenuSelectedIndexID;
    typedef CoreIPC::Arguments1<int32_t> DecodeType;
    explicit SetPopupMenuSelectedIndex(int32_t selectedIndex)
        : CoreIPC::Arguments1<int32_t>(selectedIndex)
    {
    }
};
#endif

struct ShowContextMenu : CoreIPC::Arguments4<const WebCore::IntPoint&, const WebKit::WebHitTestResult::Data&, const Vector<WebKit::WebContextMenuItemData>&, const WebKit::InjectedBundleUserMessageEncoder&> {
    static const Kind messageID = ShowContextMenuID;
    typedef CoreIPC::Arguments3<WebCore::IntPoint&, WebKit::WebHitTestResult::Data&, Vector<WebKit::WebContextMenuItemData>&> DecodeType;
    ShowContextMenu(const WebCore::IntPoint& menuLocation, const WebKit::WebHitTestResult::Data& hitTestResultData, const Vector<WebKit::WebContextMenuItemData>& items, const WebKit::InjectedBundleUserMessageEncoder& userData)
        : CoreIPC::Arguments4<const WebCore::IntPoint&, const WebKit::WebHitTestResult::Data&, const Vector<WebKit::WebContextMenuItemData>&, const WebKit::InjectedBundleUserMessageEncoder&>(menuLocation, hitTestResultData, items, userData)
    {
    }
};

struct CanAuthenticateAgainstProtectionSpaceInFrame : CoreIPC::Arguments2<uint64_t, const WebCore::ProtectionSpace&> {
    static const Kind messageID = CanAuthenticateAgainstProtectionSpaceInFrameID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments2<uint64_t, const WebCore::ProtectionSpace&> DecodeType;
    CanAuthenticateAgainstProtectionSpaceInFrame(uint64_t frameID, const WebCore::ProtectionSpace& protectionSpace)
        : CoreIPC::Arguments2<uint64_t, const WebCore::ProtectionSpace&>(frameID, protectionSpace)
    {
    }
};

struct DidReceiveAuthenticationChallenge : CoreIPC::Arguments3<uint64_t, const WebCore::AuthenticationChallenge&, uint64_t> {
    static const Kind messageID = DidReceiveAuthenticationChallengeID;
    typedef CoreIPC::Arguments3<uint64_t, const WebCore::AuthenticationChallenge&, uint64_t> DecodeType;
    DidReceiveAuthenticationChallenge(uint64_t frameID, const WebCore::AuthenticationChallenge& challenge, uint64_t challengeID)
        : CoreIPC::Arguments3<uint64_t, const WebCore::AuthenticationChallenge&, uint64_t>(frameID, challenge, challengeID)
    {
    }
};

struct ExceededDatabaseQuota : CoreIPC::Arguments8<uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, uint64_t, uint64_t, uint64_t, uint64_t> {
    static const Kind messageID = ExceededDatabaseQuotaID;
    typedef CoreIPC::Arguments1<uint64_t&> Reply;
    typedef CoreIPC::Arguments8<uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, uint64_t, uint64_t, uint64_t, uint64_t> DecodeType;
    ExceededDatabaseQuota(uint64_t frameID, const WTF::String& originIdentifier, const WTF::String& databaseName, const WTF::String& databaseDisplayName, uint64_t currentQuota, uint64_t currentOriginUsage, uint64_t currentDatabaseUsage, uint64_t expectedUsage)
        : CoreIPC::Arguments8<uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, uint64_t, uint64_t, uint64_t, uint64_t>(frameID, originIdentifier, databaseName, databaseDisplayName, currentQuota, currentOriginUsage, currentDatabaseUsage, expectedUsage)
    {
    }
};

struct RequestGeolocationPermissionForFrame : CoreIPC::Arguments3<uint64_t, uint64_t, const WTF::String&> {
    static const Kind messageID = RequestGeolocationPermissionForFrameID;
    typedef CoreIPC::Arguments3<uint64_t, uint64_t, const WTF::String&> DecodeType;
    RequestGeolocationPermissionForFrame(uint64_t geolocationID, uint64_t frameID, const WTF::String& originIdentifier)
        : CoreIPC::Arguments3<uint64_t, uint64_t, const WTF::String&>(geolocationID, frameID, originIdentifier)
    {
    }
};

struct RequestNotificationPermission : CoreIPC::Arguments2<uint64_t, const WTF::String&> {
    static const Kind messageID = RequestNotificationPermissionID;
    typedef CoreIPC::Arguments2<uint64_t, const WTF::String&> DecodeType;
    RequestNotificationPermission(uint64_t requestID, const WTF::String& originIdentifier)
        : CoreIPC::Arguments2<uint64_t, const WTF::String&>(requestID, originIdentifier)
    {
    }
};

struct ShowNotification : CoreIPC::Arguments6<const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, uint64_t> {
    static const Kind messageID = ShowNotificationID;
    typedef CoreIPC::Arguments6<const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, uint64_t> DecodeType;
    ShowNotification(const WTF::String& title, const WTF::String& body, const WTF::String& iconURL, const WTF::String& replaceID, const WTF::String& originIdentifier, uint64_t notificationID)
        : CoreIPC::Arguments6<const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, uint64_t>(title, body, iconURL, replaceID, originIdentifier, notificationID)
    {
    }
};

#if USE(UNIFIED_TEXT_CHECKING)
struct CheckTextOfParagraph : CoreIPC::Arguments2<const WTF::String&, uint64_t> {
    static const Kind messageID = CheckTextOfParagraphID;
    typedef CoreIPC::Arguments1<Vector<WebCore::TextCheckingResult>&> Reply;
    typedef CoreIPC::Arguments2<const WTF::String&, uint64_t> DecodeType;
    CheckTextOfParagraph(const WTF::String& text, uint64_t checkingTypes)
        : CoreIPC::Arguments2<const WTF::String&, uint64_t>(text, checkingTypes)
    {
    }
};
#endif

struct CheckSpellingOfString : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = CheckSpellingOfStringID;
    typedef CoreIPC::Arguments2<int32_t&, int32_t&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit CheckSpellingOfString(const WTF::String& text)
        : CoreIPC::Arguments1<const WTF::String&>(text)
    {
    }
};

struct CheckGrammarOfString : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = CheckGrammarOfStringID;
    typedef CoreIPC::Arguments3<Vector<WebCore::GrammarDetail>&, int32_t&, int32_t&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit CheckGrammarOfString(const WTF::String& text)
        : CoreIPC::Arguments1<const WTF::String&>(text)
    {
    }
};

struct SpellingUIIsShowing : CoreIPC::Arguments0 {
    static const Kind messageID = SpellingUIIsShowingID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};

struct UpdateSpellingUIWithMisspelledWord : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = UpdateSpellingUIWithMisspelledWordID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit UpdateSpellingUIWithMisspelledWord(const WTF::String& misspelledWord)
        : CoreIPC::Arguments1<const WTF::String&>(misspelledWord)
    {
    }
};

struct UpdateSpellingUIWithGrammarString : CoreIPC::Arguments2<const WTF::String&, const WebCore::GrammarDetail&> {
    static const Kind messageID = UpdateSpellingUIWithGrammarStringID;
    typedef CoreIPC::Arguments2<const WTF::String&, const WebCore::GrammarDetail&> DecodeType;
    UpdateSpellingUIWithGrammarString(const WTF::String& badGrammarPhrase, const WebCore::GrammarDetail& grammarDetail)
        : CoreIPC::Arguments2<const WTF::String&, const WebCore::GrammarDetail&>(badGrammarPhrase, grammarDetail)
    {
    }
};

struct GetGuessesForWord : CoreIPC::Arguments2<const WTF::String&, const WTF::String&> {
    static const Kind messageID = GetGuessesForWordID;
    typedef CoreIPC::Arguments1<Vector<WTF::String>&> Reply;
    typedef CoreIPC::Arguments2<const WTF::String&, const WTF::String&> DecodeType;
    GetGuessesForWord(const WTF::String& word, const WTF::String& context)
        : CoreIPC::Arguments2<const WTF::String&, const WTF::String&>(word, context)
    {
    }
};

struct LearnWord : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = LearnWordID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit LearnWord(const WTF::String& word)
        : CoreIPC::Arguments1<const WTF::String&>(word)
    {
    }
};

struct IgnoreWord : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = IgnoreWordID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit IgnoreWord(const WTF::String& word)
        : CoreIPC::Arguments1<const WTF::String&>(word)
    {
    }
};

struct DidPerformDragControllerAction : CoreIPC::Arguments1<const WebCore::DragSession&> {
    static const Kind messageID = DidPerformDragControllerActionID;
    typedef CoreIPC::Arguments1<const WebCore::DragSession&> DecodeType;
    explicit DidPerformDragControllerAction(const WebCore::DragSession& dragSession)
        : CoreIPC::Arguments1<const WebCore::DragSession&>(dragSession)
    {
    }
};

#if PLATFORM(MAC)
struct SetDragImage : CoreIPC::Arguments3<const WebCore::IntPoint&, const WebKit::ShareableBitmap::Handle&, bool> {
    static const Kind messageID = SetDragImageID;
    typedef CoreIPC::Arguments3<const WebCore::IntPoint&, const WebKit::ShareableBitmap::Handle&, bool> DecodeType;
    SetDragImage(const WebCore::IntPoint& clientPosition, const WebKit::ShareableBitmap::Handle& dragImage, bool linkDrag)
        : CoreIPC::Arguments3<const WebCore::IntPoint&, const WebKit::ShareableBitmap::Handle&, bool>(clientPosition, dragImage, linkDrag)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SetPromisedData : CoreIPC::Arguments10<const WTF::String&, const WebKit::SharedMemory::Handle&, uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, const WebKit::SharedMemory::Handle&, uint64_t> {
    static const Kind messageID = SetPromisedDataID;
    typedef CoreIPC::Arguments10<const WTF::String&, const WebKit::SharedMemory::Handle&, uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, const WebKit::SharedMemory::Handle&, uint64_t> DecodeType;
    SetPromisedData(const WTF::String& pasteboardName, const WebKit::SharedMemory::Handle& imageHandle, uint64_t imageSize, const WTF::String& filename, const WTF::String& extension, const WTF::String& title, const WTF::String& url, const WTF::String& visibleURL, const WebKit::SharedMemory::Handle& archiveHandle, uint64_t archiveSize)
        : CoreIPC::Arguments10<const WTF::String&, const WebKit::SharedMemory::Handle&, uint64_t, const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, const WTF::String&, const WebKit::SharedMemory::Handle&, uint64_t>(pasteboardName, imageHandle, imageSize, filename, extension, title, url, visibleURL, archiveHandle, archiveSize)
    {
    }
};
#endif

#if PLATFORM(WIN)
struct StartDragDrop : CoreIPC::Arguments10<const WebCore::IntPoint&, const WebCore::IntPoint&, uint64_t, const HashMap<UINT,Vector<String> >&, uint64_t, const String&, const WebKit::SharedMemory::Handle&, const WebCore::IntSize&, const WebKit::SharedMemory::Handle&, bool> {
    static const Kind messageID = StartDragDropID;
    typedef CoreIPC::Arguments10<const WebCore::IntPoint&, const WebCore::IntPoint&, uint64_t, const HashMap<UINT,Vector<String> >&, uint64_t, const String&, const WebKit::SharedMemory::Handle&, const WebCore::IntSize&, const WebKit::SharedMemory::Handle&, bool> DecodeType;
    StartDragDrop(const WebCore::IntPoint& imagePoint, const WebCore::IntPoint& dragPoint, uint64_t okEffect, const HashMap<UINT,Vector<String> >& dataMap, uint64_t fileSize, const String& pathname, const WebKit::SharedMemory::Handle& fileContentHandle, const WebCore::IntSize& dragImageSize, const WebKit::SharedMemory::Handle& dragImage, bool linkDrag)
        : CoreIPC::Arguments10<const WebCore::IntPoint&, const WebCore::IntPoint&, uint64_t, const HashMap<UINT,Vector<String> >&, uint64_t, const String&, const WebKit::SharedMemory::Handle&, const WebCore::IntSize&, const WebKit::SharedMemory::Handle&, bool>(imagePoint, dragPoint, okEffect, dataMap, fileSize, pathname, fileContentHandle, dragImageSize, dragImage, linkDrag)
    {
    }
};
#endif

#if PLATFORM(QT) || PLATFORM(GTK)
struct StartDrag : CoreIPC::Arguments2<const WebCore::DragData&, const WebKit::ShareableBitmap::Handle&> {
    static const Kind messageID = StartDragID;
    typedef CoreIPC::Arguments2<const WebCore::DragData&, const WebKit::ShareableBitmap::Handle&> DecodeType;
    StartDrag(const WebCore::DragData& dragData, const WebKit::ShareableBitmap::Handle& dragImage)
        : CoreIPC::Arguments2<const WebCore::DragData&, const WebKit::ShareableBitmap::Handle&>(dragData, dragImage)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct DidPerformDictionaryLookup : CoreIPC::Arguments2<const WTF::String&, const WebKit::DictionaryPopupInfo&> {
    static const Kind messageID = DidPerformDictionaryLookupID;
    typedef CoreIPC::Arguments2<const WTF::String&, const WebKit::DictionaryPopupInfo&> DecodeType;
    DidPerformDictionaryLookup(const WTF::String& text, const WebKit::DictionaryPopupInfo& dictionaryPopupInfo)
        : CoreIPC::Arguments2<const WTF::String&, const WebKit::DictionaryPopupInfo&>(text, dictionaryPopupInfo)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct InterpretQueuedKeyEvent : CoreIPC::Arguments1<const WebKit::EditorState&> {
    static const Kind messageID = InterpretQueuedKeyEventID;
    typedef CoreIPC::Arguments2<bool&, Vector<WebCore::KeypressCommand>&> Reply;
    typedef CoreIPC::Arguments1<const WebKit::EditorState&> DecodeType;
    explicit InterpretQueuedKeyEvent(const WebKit::EditorState& state)
        : CoreIPC::Arguments1<const WebKit::EditorState&>(state)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct ExecuteSavedCommandBySelector : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = ExecuteSavedCommandBySelectorID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit ExecuteSavedCommandBySelector(const WTF::String& selector)
        : CoreIPC::Arguments1<const WTF::String&>(selector)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct RegisterWebProcessAccessibilityToken : CoreIPC::Arguments1<const CoreIPC::DataReference&> {
    static const Kind messageID = RegisterWebProcessAccessibilityTokenID;
    typedef CoreIPC::Arguments1<const CoreIPC::DataReference&> DecodeType;
    explicit RegisterWebProcessAccessibilityToken(const CoreIPC::DataReference& data)
        : CoreIPC::Arguments1<const CoreIPC::DataReference&>(data)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct PluginFocusOrWindowFocusChanged : CoreIPC::Arguments2<uint64_t, bool> {
    static const Kind messageID = PluginFocusOrWindowFocusChangedID;
    typedef CoreIPC::Arguments2<uint64_t, bool> DecodeType;
    PluginFocusOrWindowFocusChanged(uint64_t pluginComplexTextInputIdentifier, bool pluginHasFocusAndWindowHasFocus)
        : CoreIPC::Arguments2<uint64_t, bool>(pluginComplexTextInputIdentifier, pluginHasFocusAndWindowHasFocus)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SetPluginComplexTextInputState : CoreIPC::Arguments2<uint64_t, uint64_t> {
    static const Kind messageID = SetPluginComplexTextInputStateID;
    typedef CoreIPC::Arguments2<uint64_t, uint64_t> DecodeType;
    SetPluginComplexTextInputState(uint64_t pluginComplexTextInputIdentifier, uint64_t complexTextInputState)
        : CoreIPC::Arguments2<uint64_t, uint64_t>(pluginComplexTextInputIdentifier, complexTextInputState)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct GetIsSpeaking : CoreIPC::Arguments0 {
    static const Kind messageID = GetIsSpeakingID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};
#endif

#if PLATFORM(MAC)
struct Speak : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = SpeakID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit Speak(const WTF::String& string)
        : CoreIPC::Arguments1<const WTF::String&>(string)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct StopSpeaking : CoreIPC::Arguments0 {
    static const Kind messageID = StopSpeakingID;
    typedef CoreIPC::Arguments0 DecodeType;
};
#endif

#if PLATFORM(MAC)
struct MakeFirstResponder : CoreIPC::Arguments0 {
    static const Kind messageID = MakeFirstResponderID;
    typedef CoreIPC::Arguments0 DecodeType;
};
#endif

#if PLATFORM(MAC)
struct SearchWithSpotlight : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = SearchWithSpotlightID;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit SearchWithSpotlight(const WTF::String& string)
        : CoreIPC::Arguments1<const WTF::String&>(string)
    {
    }
};
#endif

#if PLATFORM(MAC)
struct SubstitutionsPanelIsShowing : CoreIPC::Arguments0 {
    static const Kind messageID = SubstitutionsPanelIsShowingID;
    typedef CoreIPC::Arguments1<bool&> Reply;
    typedef CoreIPC::Arguments0 DecodeType;
};
#endif

#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
struct ShowCorrectionPanel : CoreIPC::Arguments5<int32_t, const WebCore::FloatRect&, const String&, const String&, const Vector<String>&> {
    static const Kind messageID = ShowCorrectionPanelID;
    typedef CoreIPC::Arguments5<int32_t, const WebCore::FloatRect&, const String&, const String&, const Vector<String>&> DecodeType;
    ShowCorrectionPanel(int32_t panelType, const WebCore::FloatRect& boundingBoxOfReplacedString, const String& replacedString, const String& replacementString, const Vector<String>& alternativeReplacementStrings)
        : CoreIPC::Arguments5<int32_t, const WebCore::FloatRect&, const String&, const String&, const Vector<String>&>(panelType, boundingBoxOfReplacedString, replacedString, replacementString, alternativeReplacementStrings)
    {
    }
};
#endif

#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
struct DismissCorrectionPanel : CoreIPC::Arguments1<int32_t> {
    static const Kind messageID = DismissCorrectionPanelID;
    typedef CoreIPC::Arguments1<int32_t> DecodeType;
    explicit DismissCorrectionPanel(int32_t reason)
        : CoreIPC::Arguments1<int32_t>(reason)
    {
    }
};
#endif

#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
struct DismissCorrectionPanelSoon : CoreIPC::Arguments1<int32_t> {
    static const Kind messageID = DismissCorrectionPanelSoonID;
    typedef CoreIPC::Arguments1<String&> Reply;
    typedef CoreIPC::Arguments1<int32_t> DecodeType;
    explicit DismissCorrectionPanelSoon(int32_t reason)
        : CoreIPC::Arguments1<int32_t>(reason)
    {
    }
};
#endif

#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
struct RecordAutocorrectionResponse : CoreIPC::Arguments3<int32_t, const String&, const String&> {
    static const Kind messageID = RecordAutocorrectionResponseID;
    typedef CoreIPC::Arguments3<int32_t, const String&, const String&> DecodeType;
    RecordAutocorrectionResponse(int32_t responseType, const String& replacedString, const String& replacementString)
        : CoreIPC::Arguments3<int32_t, const String&, const String&>(responseType, replacedString, replacementString)
    {
    }
};
#endif

#if PLATFORM(WIN)
struct SetGestureReachedScrollingLimit : CoreIPC::Arguments1<bool> {
    static const Kind messageID = SetGestureReachedScrollingLimitID;
    typedef CoreIPC::Arguments1<bool> DecodeType;
    explicit SetGestureReachedScrollingLimit(bool limitReached)
        : CoreIPC::Arguments1<bool>(limitReached)
    {
    }
};
#endif

#if PLATFORM(WIN)
struct ScheduleChildWindowGeometryUpdate : CoreIPC::Arguments1<const WebKit::WindowGeometry&> {
    static const Kind messageID = ScheduleChildWindowGeometryUpdateID;
    typedef CoreIPC::Arguments1<const WebKit::WindowGeometry&> DecodeType;
    explicit ScheduleChildWindowGeometryUpdate(const WebKit::WindowGeometry& geometry)
        : CoreIPC::Arguments1<const WebKit::WindowGeometry&>(geometry)
    {
    }
};
#endif

struct SaveRecentSearches : CoreIPC::Arguments2<const WTF::String&, const Vector<String>&> {
    static const Kind messageID = SaveRecentSearchesID;
    typedef CoreIPC::Arguments2<const WTF::String&, const Vector<String>&> DecodeType;
    SaveRecentSearches(const WTF::String& name, const Vector<String>& searchItems)
        : CoreIPC::Arguments2<const WTF::String&, const Vector<String>&>(name, searchItems)
    {
    }
};

struct LoadRecentSearches : CoreIPC::Arguments1<const WTF::String&> {
    static const Kind messageID = LoadRecentSearchesID;
    typedef CoreIPC::Arguments1<Vector<String>&> Reply;
    typedef CoreIPC::Arguments1<const WTF::String&> DecodeType;
    explicit LoadRecentSearches(const WTF::String& name)
        : CoreIPC::Arguments1<const WTF::String&>(name)
    {
    }
};

} // namespace WebPageProxy

} // namespace Messages

namespace CoreIPC {

template<> struct MessageKindTraits<Messages::WebPageProxy::Kind> {
    static const MessageClass messageClass = MessageClassWebPageProxy;
};

} // namespace CoreIPC

#endif // WebPageProxyMessages_h
