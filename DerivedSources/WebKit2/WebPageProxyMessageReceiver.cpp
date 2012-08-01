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

#include "WebPageProxy.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#include "DataReference.h"
#if PLATFORM(MAC)
#include "DictionaryPopupInfo.h"
#endif
#include "EditorState.h"
#include "HandleMessage.h"
#include "InjectedBundleUserMessageCoders.h"
#include "PlatformCertificateInfo.h"
#include "PlatformPopupMenuData.h"
#if PLATFORM(QT)
#include "QtNetworkRequestData.h"
#endif
#include "SandboxExtension.h"
#include "ShareableBitmap.h"
#if PLATFORM(MAC) || PLATFORM(WIN)
#include "SharedMemory.h"
#endif
#include "StringPairVector.h"
#include "WebContextMenuItemData.h"
#include "WebCoreArgumentCoders.h"
#include "WebHitTestResult.h"
#include "WebPageCreationParameters.h"
#include "WebPageProxyMessages.h"
#include "WebPopupItem.h"
#if PLATFORM(WIN)
#include "WindowGeometry.h"
#endif
#include <WebCore/AuthenticationChallenge.h>
#include <WebCore/Cursor.h>
#if PLATFORM(QT) || PLATFORM(GTK)
#include <WebCore/DragData.h>
#endif
#include <WebCore/DragSession.h>
#include <WebCore/FileChooser.h>
#include <WebCore/FloatRect.h>
#include <WebCore/IntPoint.h>
#include <WebCore/IntRect.h>
#if PLATFORM(QT) || PLATFORM(WIN)
#include <WebCore/IntSize.h>
#endif
#if PLATFORM(MAC)
#include <WebCore/KeyboardEvent.h>
#endif
#include <WebCore/ProtectionSpace.h>
#include <WebCore/ResourceError.h>
#include <WebCore/ResourceRequest.h>
#include <WebCore/ResourceResponse.h>
#include <WebCore/TextCheckerClient.h>
#include <WebCore/ViewportArguments.h>
#include <WebCore/WindowFeatures.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

void WebPageProxy::didReceiveWebPageProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::WebPageProxy::Kind>()) {
    case Messages::WebPageProxy::ShowPageID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ShowPage>(arguments, this, &WebPageProxy::showPage);
        return;
    case Messages::WebPageProxy::ClosePageID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ClosePage>(arguments, this, &WebPageProxy::closePage);
        return;
    case Messages::WebPageProxy::MouseDidMoveOverElementID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::MouseDidMoveOverElement>(arguments, this, &WebPageProxy::mouseDidMoveOverElement);
        return;
    case Messages::WebPageProxy::MissingPluginButtonClickedID:
        CoreIPC::handleMessage<Messages::WebPageProxy::MissingPluginButtonClicked>(arguments, this, &WebPageProxy::missingPluginButtonClicked);
        return;
    case Messages::WebPageProxy::DidChangeViewportPropertiesID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidChangeViewportProperties>(arguments, this, &WebPageProxy::didChangeViewportProperties);
        return;
    case Messages::WebPageProxy::DidReceiveEventID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidReceiveEvent>(arguments, this, &WebPageProxy::didReceiveEvent);
        return;
    case Messages::WebPageProxy::StopResponsivenessTimerID:
        CoreIPC::handleMessage<Messages::WebPageProxy::StopResponsivenessTimer>(arguments, this, &WebPageProxy::stopResponsivenessTimer);
        return;
    case Messages::WebPageProxy::SetCursorID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetCursor>(arguments, this, &WebPageProxy::setCursor);
        return;
    case Messages::WebPageProxy::SetCursorHiddenUntilMouseMovesID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetCursorHiddenUntilMouseMoves>(arguments, this, &WebPageProxy::setCursorHiddenUntilMouseMoves);
        return;
    case Messages::WebPageProxy::SetStatusTextID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetStatusText>(arguments, this, &WebPageProxy::setStatusText);
        return;
    case Messages::WebPageProxy::SetToolTipID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetToolTip>(arguments, this, &WebPageProxy::setToolTip);
        return;
    case Messages::WebPageProxy::SetFocusID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetFocus>(arguments, this, &WebPageProxy::setFocus);
        return;
    case Messages::WebPageProxy::TakeFocusID:
        CoreIPC::handleMessage<Messages::WebPageProxy::TakeFocus>(arguments, this, &WebPageProxy::takeFocus);
        return;
    case Messages::WebPageProxy::FocusedFrameChangedID:
        CoreIPC::handleMessage<Messages::WebPageProxy::FocusedFrameChanged>(arguments, this, &WebPageProxy::focusedFrameChanged);
        return;
    case Messages::WebPageProxy::FrameSetLargestFrameChangedID:
        CoreIPC::handleMessage<Messages::WebPageProxy::FrameSetLargestFrameChanged>(arguments, this, &WebPageProxy::frameSetLargestFrameChanged);
        return;
    case Messages::WebPageProxy::SetRenderTreeSizeID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetRenderTreeSize>(arguments, this, &WebPageProxy::setRenderTreeSize);
        return;
    case Messages::WebPageProxy::SetToolbarsAreVisibleID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetToolbarsAreVisible>(arguments, this, &WebPageProxy::setToolbarsAreVisible);
        return;
    case Messages::WebPageProxy::SetMenuBarIsVisibleID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetMenuBarIsVisible>(arguments, this, &WebPageProxy::setMenuBarIsVisible);
        return;
    case Messages::WebPageProxy::SetStatusBarIsVisibleID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetStatusBarIsVisible>(arguments, this, &WebPageProxy::setStatusBarIsVisible);
        return;
    case Messages::WebPageProxy::SetIsResizableID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetIsResizable>(arguments, this, &WebPageProxy::setIsResizable);
        return;
    case Messages::WebPageProxy::SetWindowFrameID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetWindowFrame>(arguments, this, &WebPageProxy::setWindowFrame);
        return;
    case Messages::WebPageProxy::PageDidScrollID:
        CoreIPC::handleMessage<Messages::WebPageProxy::PageDidScroll>(arguments, this, &WebPageProxy::pageDidScroll);
        return;
    case Messages::WebPageProxy::RunOpenPanelID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RunOpenPanel>(arguments, this, &WebPageProxy::runOpenPanel);
        return;
    case Messages::WebPageProxy::RunModalID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RunModal>(arguments, this, &WebPageProxy::runModal);
        return;
    case Messages::WebPageProxy::NotifyScrollerThumbIsVisibleInRectID:
        CoreIPC::handleMessage<Messages::WebPageProxy::NotifyScrollerThumbIsVisibleInRect>(arguments, this, &WebPageProxy::notifyScrollerThumbIsVisibleInRect);
        return;
    case Messages::WebPageProxy::RecommendedScrollbarStyleDidChangeID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RecommendedScrollbarStyleDidChange>(arguments, this, &WebPageProxy::recommendedScrollbarStyleDidChange);
        return;
    case Messages::WebPageProxy::DidChangeScrollbarsForMainFrameID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidChangeScrollbarsForMainFrame>(arguments, this, &WebPageProxy::didChangeScrollbarsForMainFrame);
        return;
    case Messages::WebPageProxy::DidChangeScrollOffsetPinningForMainFrameID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidChangeScrollOffsetPinningForMainFrame>(arguments, this, &WebPageProxy::didChangeScrollOffsetPinningForMainFrame);
        return;
    case Messages::WebPageProxy::DidChangePageCountID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidChangePageCount>(arguments, this, &WebPageProxy::didChangePageCount);
        return;
    case Messages::WebPageProxy::DidFailToInitializePluginID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidFailToInitializePlugin>(arguments, this, &WebPageProxy::didFailToInitializePlugin);
        return;
    case Messages::WebPageProxy::SetCanShortCircuitHorizontalWheelEventsID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetCanShortCircuitHorizontalWheelEvents>(arguments, this, &WebPageProxy::setCanShortCircuitHorizontalWheelEvents);
        return;
#if USE(TILED_BACKING_STORE)
    case Messages::WebPageProxy::PageDidRequestScrollID:
        CoreIPC::handleMessage<Messages::WebPageProxy::PageDidRequestScroll>(arguments, this, &WebPageProxy::pageDidRequestScroll);
        return;
#endif
#if PLATFORM(QT)
    case Messages::WebPageProxy::DidChangeContentsSizeID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidChangeContentsSize>(arguments, this, &WebPageProxy::didChangeContentsSize);
        return;
#endif
#if PLATFORM(QT)
    case Messages::WebPageProxy::DidFindZoomableAreaID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidFindZoomableArea>(arguments, this, &WebPageProxy::didFindZoomableArea);
        return;
#endif
#if ENABLE(TOUCH_EVENTS)
    case Messages::WebPageProxy::NeedTouchEventsID:
        CoreIPC::handleMessage<Messages::WebPageProxy::NeedTouchEvents>(arguments, this, &WebPageProxy::needTouchEvents);
        return;
#endif
    case Messages::WebPageProxy::DecidePolicyForNewWindowActionID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DecidePolicyForNewWindowAction>(arguments, this, &WebPageProxy::decidePolicyForNewWindowAction);
        return;
    case Messages::WebPageProxy::UnableToImplementPolicyID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::UnableToImplementPolicy>(arguments, this, &WebPageProxy::unableToImplementPolicy);
        return;
    case Messages::WebPageProxy::DidChangeProgressID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidChangeProgress>(arguments, this, &WebPageProxy::didChangeProgress);
        return;
    case Messages::WebPageProxy::DidFinishProgressID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidFinishProgress>(arguments, this, &WebPageProxy::didFinishProgress);
        return;
    case Messages::WebPageProxy::DidStartProgressID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidStartProgress>(arguments, this, &WebPageProxy::didStartProgress);
        return;
    case Messages::WebPageProxy::DidCreateMainFrameID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidCreateMainFrame>(arguments, this, &WebPageProxy::didCreateMainFrame);
        return;
    case Messages::WebPageProxy::DidCreateSubframeID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidCreateSubframe>(arguments, this, &WebPageProxy::didCreateSubframe);
        return;
    case Messages::WebPageProxy::DidSaveFrameToPageCacheID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidSaveFrameToPageCache>(arguments, this, &WebPageProxy::didSaveFrameToPageCache);
        return;
    case Messages::WebPageProxy::DidRestoreFrameFromPageCacheID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidRestoreFrameFromPageCache>(arguments, this, &WebPageProxy::didRestoreFrameFromPageCache);
        return;
    case Messages::WebPageProxy::DidCommitLoadForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidCommitLoadForFrame>(arguments, this, &WebPageProxy::didCommitLoadForFrame);
        return;
    case Messages::WebPageProxy::DidFailLoadForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidFailLoadForFrame>(arguments, this, &WebPageProxy::didFailLoadForFrame);
        return;
    case Messages::WebPageProxy::DidFailProvisionalLoadForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidFailProvisionalLoadForFrame>(arguments, this, &WebPageProxy::didFailProvisionalLoadForFrame);
        return;
    case Messages::WebPageProxy::DidFinishDocumentLoadForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidFinishDocumentLoadForFrame>(arguments, this, &WebPageProxy::didFinishDocumentLoadForFrame);
        return;
    case Messages::WebPageProxy::DidFinishLoadForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidFinishLoadForFrame>(arguments, this, &WebPageProxy::didFinishLoadForFrame);
        return;
    case Messages::WebPageProxy::DidFirstLayoutForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidFirstLayoutForFrame>(arguments, this, &WebPageProxy::didFirstLayoutForFrame);
        return;
    case Messages::WebPageProxy::DidFirstVisuallyNonEmptyLayoutForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidFirstVisuallyNonEmptyLayoutForFrame>(arguments, this, &WebPageProxy::didFirstVisuallyNonEmptyLayoutForFrame);
        return;
    case Messages::WebPageProxy::DidNewFirstVisuallyNonEmptyLayoutID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidNewFirstVisuallyNonEmptyLayout>(arguments, this, &WebPageProxy::didNewFirstVisuallyNonEmptyLayout);
        return;
    case Messages::WebPageProxy::DidReceiveServerRedirectForProvisionalLoadForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidReceiveServerRedirectForProvisionalLoadForFrame>(arguments, this, &WebPageProxy::didReceiveServerRedirectForProvisionalLoadForFrame);
        return;
    case Messages::WebPageProxy::DidRemoveFrameFromHierarchyID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidRemoveFrameFromHierarchy>(arguments, this, &WebPageProxy::didRemoveFrameFromHierarchy);
        return;
    case Messages::WebPageProxy::DidStartProvisionalLoadForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidStartProvisionalLoadForFrame>(arguments, this, &WebPageProxy::didStartProvisionalLoadForFrame);
        return;
    case Messages::WebPageProxy::DidReceiveTitleForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidReceiveTitleForFrame>(arguments, this, &WebPageProxy::didReceiveTitleForFrame);
        return;
    case Messages::WebPageProxy::DidDisplayInsecureContentForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidDisplayInsecureContentForFrame>(arguments, this, &WebPageProxy::didDisplayInsecureContentForFrame);
        return;
    case Messages::WebPageProxy::DidRunInsecureContentForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidRunInsecureContentForFrame>(arguments, this, &WebPageProxy::didRunInsecureContentForFrame);
        return;
    case Messages::WebPageProxy::DidDetectXSSForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidDetectXSSForFrame>(arguments, this, &WebPageProxy::didDetectXSSForFrame);
        return;
    case Messages::WebPageProxy::DidSameDocumentNavigationForFrameID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DidSameDocumentNavigationForFrame>(arguments, this, &WebPageProxy::didSameDocumentNavigationForFrame);
        return;
    case Messages::WebPageProxy::FrameDidBecomeFrameSetID:
        CoreIPC::handleMessage<Messages::WebPageProxy::FrameDidBecomeFrameSet>(arguments, this, &WebPageProxy::frameDidBecomeFrameSet);
        return;
    case Messages::WebPageProxy::DidInitiateLoadForResourceID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidInitiateLoadForResource>(arguments, this, &WebPageProxy::didInitiateLoadForResource);
        return;
    case Messages::WebPageProxy::DidSendRequestForResourceID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidSendRequestForResource>(arguments, this, &WebPageProxy::didSendRequestForResource);
        return;
    case Messages::WebPageProxy::DidReceiveResponseForResourceID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidReceiveResponseForResource>(arguments, this, &WebPageProxy::didReceiveResponseForResource);
        return;
    case Messages::WebPageProxy::DidReceiveContentLengthForResourceID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidReceiveContentLengthForResource>(arguments, this, &WebPageProxy::didReceiveContentLengthForResource);
        return;
    case Messages::WebPageProxy::DidFinishLoadForResourceID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidFinishLoadForResource>(arguments, this, &WebPageProxy::didFinishLoadForResource);
        return;
    case Messages::WebPageProxy::DidFailLoadForResourceID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidFailLoadForResource>(arguments, this, &WebPageProxy::didFailLoadForResource);
        return;
#if PLATFORM(QT)
    case Messages::WebPageProxy::ResolveApplicationSchemeRequestID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ResolveApplicationSchemeRequest>(arguments, this, &WebPageProxy::resolveApplicationSchemeRequest);
        return;
#endif
    case Messages::WebPageProxy::DidFinishLoadingDataForCustomRepresentationID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidFinishLoadingDataForCustomRepresentation>(arguments, this, &WebPageProxy::didFinishLoadingDataForCustomRepresentation);
        return;
    case Messages::WebPageProxy::WillSubmitFormID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::WillSubmitForm>(arguments, this, &WebPageProxy::willSubmitForm);
        return;
    case Messages::WebPageProxy::VoidCallbackID:
        CoreIPC::handleMessage<Messages::WebPageProxy::VoidCallback>(arguments, this, &WebPageProxy::voidCallback);
        return;
    case Messages::WebPageProxy::DataCallbackID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DataCallback>(arguments, this, &WebPageProxy::dataCallback);
        return;
    case Messages::WebPageProxy::StringCallbackID:
        CoreIPC::handleMessage<Messages::WebPageProxy::StringCallback>(arguments, this, &WebPageProxy::stringCallback);
        return;
    case Messages::WebPageProxy::ScriptValueCallbackID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ScriptValueCallback>(arguments, this, &WebPageProxy::scriptValueCallback);
        return;
    case Messages::WebPageProxy::ComputedPagesCallbackID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ComputedPagesCallback>(arguments, this, &WebPageProxy::computedPagesCallback);
        return;
    case Messages::WebPageProxy::ValidateCommandCallbackID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ValidateCommandCallback>(arguments, this, &WebPageProxy::validateCommandCallback);
        return;
#if PLATFORM(GTK)
    case Messages::WebPageProxy::PrintFinishedCallbackID:
        CoreIPC::handleMessage<Messages::WebPageProxy::PrintFinishedCallback>(arguments, this, &WebPageProxy::printFinishedCallback);
        return;
#endif
    case Messages::WebPageProxy::PageScaleFactorDidChangeID:
        CoreIPC::handleMessage<Messages::WebPageProxy::PageScaleFactorDidChange>(arguments, this, &WebPageProxy::pageScaleFactorDidChange);
        return;
#if PLATFORM(GTK)
    case Messages::WebPageProxy::BindAccessibilityTreeID:
        CoreIPC::handleMessage<Messages::WebPageProxy::BindAccessibilityTree>(arguments, this, &WebPageProxy::bindAccessibilityTree);
        return;
#endif
    case Messages::WebPageProxy::BackForwardAddItemID:
        CoreIPC::handleMessage<Messages::WebPageProxy::BackForwardAddItem>(arguments, this, &WebPageProxy::backForwardAddItem);
        return;
    case Messages::WebPageProxy::BackForwardClearID:
        CoreIPC::handleMessage<Messages::WebPageProxy::BackForwardClear>(arguments, this, &WebPageProxy::backForwardClear);
        return;
    case Messages::WebPageProxy::WillGoToBackForwardListItemID:
        CoreIPC::handleMessage<Messages::WebPageProxy::WillGoToBackForwardListItem>(arguments, this, &WebPageProxy::willGoToBackForwardListItem);
        return;
    case Messages::WebPageProxy::RegisterEditCommandForUndoID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RegisterEditCommandForUndo>(arguments, this, &WebPageProxy::registerEditCommandForUndo);
        return;
    case Messages::WebPageProxy::ClearAllEditCommandsID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ClearAllEditCommands>(arguments, this, &WebPageProxy::clearAllEditCommands);
        return;
    case Messages::WebPageProxy::EditorStateChangedID:
        CoreIPC::handleMessage<Messages::WebPageProxy::EditorStateChanged>(arguments, this, &WebPageProxy::editorStateChanged);
        return;
#if PLATFORM(WIN)
    case Messages::WebPageProxy::DidChangeCompositionSelectionID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidChangeCompositionSelection>(arguments, this, &WebPageProxy::didChangeCompositionSelection);
        return;
#endif
    case Messages::WebPageProxy::DidCountStringMatchesID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidCountStringMatches>(arguments, this, &WebPageProxy::didCountStringMatches);
        return;
    case Messages::WebPageProxy::SetFindIndicatorID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetFindIndicator>(arguments, this, &WebPageProxy::setFindIndicator);
        return;
    case Messages::WebPageProxy::DidFindStringID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidFindString>(arguments, this, &WebPageProxy::didFindString);
        return;
    case Messages::WebPageProxy::DidFailToFindStringID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidFailToFindString>(arguments, this, &WebPageProxy::didFailToFindString);
        return;
#if PLATFORM(WIN)
    case Messages::WebPageProxy::DidInstallOrUninstallPageOverlayID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidInstallOrUninstallPageOverlay>(arguments, this, &WebPageProxy::didInstallOrUninstallPageOverlay);
        return;
#endif
    case Messages::WebPageProxy::ShowPopupMenuID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ShowPopupMenu>(arguments, this, &WebPageProxy::showPopupMenu);
        return;
    case Messages::WebPageProxy::HidePopupMenuID:
        CoreIPC::handleMessage<Messages::WebPageProxy::HidePopupMenu>(arguments, this, &WebPageProxy::hidePopupMenu);
        return;
#if PLATFORM(WIN)
    case Messages::WebPageProxy::SetPopupMenuSelectedIndexID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetPopupMenuSelectedIndex>(arguments, this, &WebPageProxy::setPopupMenuSelectedIndex);
        return;
#endif
    case Messages::WebPageProxy::ShowContextMenuID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::ShowContextMenu>(arguments, this, &WebPageProxy::showContextMenu);
        return;
    case Messages::WebPageProxy::DidReceiveAuthenticationChallengeID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidReceiveAuthenticationChallenge>(arguments, this, &WebPageProxy::didReceiveAuthenticationChallenge);
        return;
    case Messages::WebPageProxy::RequestGeolocationPermissionForFrameID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RequestGeolocationPermissionForFrame>(arguments, this, &WebPageProxy::requestGeolocationPermissionForFrame);
        return;
    case Messages::WebPageProxy::RequestNotificationPermissionID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RequestNotificationPermission>(arguments, this, &WebPageProxy::requestNotificationPermission);
        return;
    case Messages::WebPageProxy::ShowNotificationID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ShowNotification>(arguments, this, &WebPageProxy::showNotification);
        return;
    case Messages::WebPageProxy::UpdateSpellingUIWithMisspelledWordID:
        CoreIPC::handleMessage<Messages::WebPageProxy::UpdateSpellingUIWithMisspelledWord>(arguments, this, &WebPageProxy::updateSpellingUIWithMisspelledWord);
        return;
    case Messages::WebPageProxy::UpdateSpellingUIWithGrammarStringID:
        CoreIPC::handleMessage<Messages::WebPageProxy::UpdateSpellingUIWithGrammarString>(arguments, this, &WebPageProxy::updateSpellingUIWithGrammarString);
        return;
    case Messages::WebPageProxy::LearnWordID:
        CoreIPC::handleMessage<Messages::WebPageProxy::LearnWord>(arguments, this, &WebPageProxy::learnWord);
        return;
    case Messages::WebPageProxy::IgnoreWordID:
        CoreIPC::handleMessage<Messages::WebPageProxy::IgnoreWord>(arguments, this, &WebPageProxy::ignoreWord);
        return;
    case Messages::WebPageProxy::DidPerformDragControllerActionID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidPerformDragControllerAction>(arguments, this, &WebPageProxy::didPerformDragControllerAction);
        return;
#if PLATFORM(MAC)
    case Messages::WebPageProxy::SetDragImageID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetDragImage>(arguments, this, &WebPageProxy::setDragImage);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::SetPromisedDataID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetPromisedData>(arguments, this, &WebPageProxy::setPromisedData);
        return;
#endif
#if PLATFORM(WIN)
    case Messages::WebPageProxy::StartDragDropID:
        CoreIPC::handleMessage<Messages::WebPageProxy::StartDragDrop>(arguments, this, &WebPageProxy::startDragDrop);
        return;
#endif
#if PLATFORM(QT) || PLATFORM(GTK)
    case Messages::WebPageProxy::StartDragID:
        CoreIPC::handleMessage<Messages::WebPageProxy::StartDrag>(arguments, this, &WebPageProxy::startDrag);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::DidPerformDictionaryLookupID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DidPerformDictionaryLookup>(arguments, this, &WebPageProxy::didPerformDictionaryLookup);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::RegisterWebProcessAccessibilityTokenID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RegisterWebProcessAccessibilityToken>(arguments, this, &WebPageProxy::registerWebProcessAccessibilityToken);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::PluginFocusOrWindowFocusChangedID:
        CoreIPC::handleMessage<Messages::WebPageProxy::PluginFocusOrWindowFocusChanged>(arguments, this, &WebPageProxy::pluginFocusOrWindowFocusChanged);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::SetPluginComplexTextInputStateID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetPluginComplexTextInputState>(arguments, this, &WebPageProxy::setPluginComplexTextInputState);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::SpeakID:
        CoreIPC::handleMessage<Messages::WebPageProxy::Speak>(arguments, this, &WebPageProxy::speak);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::StopSpeakingID:
        CoreIPC::handleMessage<Messages::WebPageProxy::StopSpeaking>(arguments, this, &WebPageProxy::stopSpeaking);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::MakeFirstResponderID:
        CoreIPC::handleMessage<Messages::WebPageProxy::MakeFirstResponder>(arguments, this, &WebPageProxy::makeFirstResponder);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::SearchWithSpotlightID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SearchWithSpotlight>(arguments, this, &WebPageProxy::searchWithSpotlight);
        return;
#endif
#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
    case Messages::WebPageProxy::ShowCorrectionPanelID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ShowCorrectionPanel>(arguments, this, &WebPageProxy::showCorrectionPanel);
        return;
#endif
#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
    case Messages::WebPageProxy::DismissCorrectionPanelID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DismissCorrectionPanel>(arguments, this, &WebPageProxy::dismissCorrectionPanel);
        return;
#endif
#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
    case Messages::WebPageProxy::RecordAutocorrectionResponseID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RecordAutocorrectionResponse>(arguments, this, &WebPageProxy::recordAutocorrectionResponse);
        return;
#endif
#if PLATFORM(WIN)
    case Messages::WebPageProxy::SetGestureReachedScrollingLimitID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SetGestureReachedScrollingLimit>(arguments, this, &WebPageProxy::setGestureReachedScrollingLimit);
        return;
#endif
#if PLATFORM(WIN)
    case Messages::WebPageProxy::ScheduleChildWindowGeometryUpdateID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ScheduleChildWindowGeometryUpdate>(arguments, this, &WebPageProxy::scheduleChildWindowGeometryUpdate);
        return;
#endif
    case Messages::WebPageProxy::SaveRecentSearchesID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SaveRecentSearches>(arguments, this, &WebPageProxy::saveRecentSearches);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

void WebPageProxy::didReceiveSyncWebPageProxyMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    switch (messageID.get<Messages::WebPageProxy::Kind>()) {
    case Messages::WebPageProxy::CreateNewPageID:
        CoreIPC::handleMessage<Messages::WebPageProxy::CreateNewPage>(arguments, reply.get(), this, &WebPageProxy::createNewPage);
        return;
    case Messages::WebPageProxy::RunJavaScriptAlertID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RunJavaScriptAlert>(arguments, reply.get(), this, &WebPageProxy::runJavaScriptAlert);
        return;
    case Messages::WebPageProxy::RunJavaScriptConfirmID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RunJavaScriptConfirm>(arguments, reply.get(), this, &WebPageProxy::runJavaScriptConfirm);
        return;
    case Messages::WebPageProxy::RunJavaScriptPromptID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RunJavaScriptPrompt>(arguments, reply.get(), this, &WebPageProxy::runJavaScriptPrompt);
        return;
    case Messages::WebPageProxy::ShouldInterruptJavaScriptID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ShouldInterruptJavaScript>(arguments, reply.get(), this, &WebPageProxy::shouldInterruptJavaScript);
        return;
    case Messages::WebPageProxy::GetToolbarsAreVisibleID:
        CoreIPC::handleMessage<Messages::WebPageProxy::GetToolbarsAreVisible>(arguments, reply.get(), this, &WebPageProxy::getToolbarsAreVisible);
        return;
    case Messages::WebPageProxy::GetMenuBarIsVisibleID:
        CoreIPC::handleMessage<Messages::WebPageProxy::GetMenuBarIsVisible>(arguments, reply.get(), this, &WebPageProxy::getMenuBarIsVisible);
        return;
    case Messages::WebPageProxy::GetStatusBarIsVisibleID:
        CoreIPC::handleMessage<Messages::WebPageProxy::GetStatusBarIsVisible>(arguments, reply.get(), this, &WebPageProxy::getStatusBarIsVisible);
        return;
    case Messages::WebPageProxy::GetIsResizableID:
        CoreIPC::handleMessage<Messages::WebPageProxy::GetIsResizable>(arguments, reply.get(), this, &WebPageProxy::getIsResizable);
        return;
    case Messages::WebPageProxy::GetWindowFrameID:
        CoreIPC::handleMessage<Messages::WebPageProxy::GetWindowFrame>(arguments, reply.get(), this, &WebPageProxy::getWindowFrame);
        return;
    case Messages::WebPageProxy::ScreenToWindowID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ScreenToWindow>(arguments, reply.get(), this, &WebPageProxy::screenToWindow);
        return;
    case Messages::WebPageProxy::WindowToScreenID:
        CoreIPC::handleMessage<Messages::WebPageProxy::WindowToScreen>(arguments, reply.get(), this, &WebPageProxy::windowToScreen);
        return;
    case Messages::WebPageProxy::RunBeforeUnloadConfirmPanelID:
        CoreIPC::handleMessage<Messages::WebPageProxy::RunBeforeUnloadConfirmPanel>(arguments, reply.get(), this, &WebPageProxy::runBeforeUnloadConfirmPanel);
        return;
    case Messages::WebPageProxy::PrintFrameID:
        CoreIPC::handleMessage<Messages::WebPageProxy::PrintFrame>(arguments, reply.get(), this, &WebPageProxy::printFrame);
        return;
#if PLATFORM(QT)
    case Messages::WebPageProxy::AuthenticationRequiredRequestID:
        CoreIPC::handleMessage<Messages::WebPageProxy::AuthenticationRequiredRequest>(arguments, reply.get(), this, &WebPageProxy::authenticationRequiredRequest);
        return;
#endif
#if PLATFORM(QT)
    case Messages::WebPageProxy::CertificateVerificationRequestID:
        CoreIPC::handleMessage<Messages::WebPageProxy::CertificateVerificationRequest>(arguments, reply.get(), this, &WebPageProxy::certificateVerificationRequest);
        return;
#endif
#if PLATFORM(QT)
    case Messages::WebPageProxy::ProxyAuthenticationRequiredRequestID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ProxyAuthenticationRequiredRequest>(arguments, reply.get(), this, &WebPageProxy::proxyAuthenticationRequiredRequest);
        return;
#endif
    case Messages::WebPageProxy::DecidePolicyForResponseID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DecidePolicyForResponse>(arguments, reply.get(), this, &WebPageProxy::decidePolicyForResponse);
        return;
    case Messages::WebPageProxy::DecidePolicyForNavigationActionID:
        CoreIPC::handleMessageVariadic<Messages::WebPageProxy::DecidePolicyForNavigationAction>(arguments, reply.get(), this, &WebPageProxy::decidePolicyForNavigationAction);
        return;
#if PLATFORM(GTK)
    case Messages::WebPageProxy::GetEditorCommandsForKeyEventID:
        CoreIPC::handleMessage<Messages::WebPageProxy::GetEditorCommandsForKeyEvent>(arguments, reply.get(), this, &WebPageProxy::getEditorCommandsForKeyEvent);
        return;
#endif
    case Messages::WebPageProxy::BackForwardGoToItemID:
        CoreIPC::handleMessage<Messages::WebPageProxy::BackForwardGoToItem>(arguments, reply.get(), this, &WebPageProxy::backForwardGoToItem);
        return;
    case Messages::WebPageProxy::BackForwardItemAtIndexID:
        CoreIPC::handleMessage<Messages::WebPageProxy::BackForwardItemAtIndex>(arguments, reply.get(), this, &WebPageProxy::backForwardItemAtIndex);
        return;
    case Messages::WebPageProxy::BackForwardBackListCountID:
        CoreIPC::handleMessage<Messages::WebPageProxy::BackForwardBackListCount>(arguments, reply.get(), this, &WebPageProxy::backForwardBackListCount);
        return;
    case Messages::WebPageProxy::BackForwardForwardListCountID:
        CoreIPC::handleMessage<Messages::WebPageProxy::BackForwardForwardListCount>(arguments, reply.get(), this, &WebPageProxy::backForwardForwardListCount);
        return;
    case Messages::WebPageProxy::ShouldGoToBackForwardListItemID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ShouldGoToBackForwardListItem>(arguments, reply.get(), this, &WebPageProxy::shouldGoToBackForwardListItem);
        return;
    case Messages::WebPageProxy::CanUndoRedoID:
        CoreIPC::handleMessage<Messages::WebPageProxy::CanUndoRedo>(arguments, reply.get(), this, &WebPageProxy::canUndoRedo);
        return;
    case Messages::WebPageProxy::ExecuteUndoRedoID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ExecuteUndoRedo>(arguments, reply.get(), this, &WebPageProxy::executeUndoRedo);
        return;
    case Messages::WebPageProxy::CanAuthenticateAgainstProtectionSpaceInFrameID:
        CoreIPC::handleMessage<Messages::WebPageProxy::CanAuthenticateAgainstProtectionSpaceInFrame>(arguments, reply.get(), this, &WebPageProxy::canAuthenticateAgainstProtectionSpaceInFrame);
        return;
    case Messages::WebPageProxy::ExceededDatabaseQuotaID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ExceededDatabaseQuota>(arguments, reply.get(), this, &WebPageProxy::exceededDatabaseQuota);
        return;
#if USE(UNIFIED_TEXT_CHECKING)
    case Messages::WebPageProxy::CheckTextOfParagraphID:
        CoreIPC::handleMessage<Messages::WebPageProxy::CheckTextOfParagraph>(arguments, reply.get(), this, &WebPageProxy::checkTextOfParagraph);
        return;
#endif
    case Messages::WebPageProxy::CheckSpellingOfStringID:
        CoreIPC::handleMessage<Messages::WebPageProxy::CheckSpellingOfString>(arguments, reply.get(), this, &WebPageProxy::checkSpellingOfString);
        return;
    case Messages::WebPageProxy::CheckGrammarOfStringID:
        CoreIPC::handleMessage<Messages::WebPageProxy::CheckGrammarOfString>(arguments, reply.get(), this, &WebPageProxy::checkGrammarOfString);
        return;
    case Messages::WebPageProxy::SpellingUIIsShowingID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SpellingUIIsShowing>(arguments, reply.get(), this, &WebPageProxy::spellingUIIsShowing);
        return;
    case Messages::WebPageProxy::GetGuessesForWordID:
        CoreIPC::handleMessage<Messages::WebPageProxy::GetGuessesForWord>(arguments, reply.get(), this, &WebPageProxy::getGuessesForWord);
        return;
#if PLATFORM(MAC)
    case Messages::WebPageProxy::InterpretQueuedKeyEventID:
        CoreIPC::handleMessage<Messages::WebPageProxy::InterpretQueuedKeyEvent>(arguments, reply.get(), this, &WebPageProxy::interpretQueuedKeyEvent);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::ExecuteSavedCommandBySelectorID:
        CoreIPC::handleMessage<Messages::WebPageProxy::ExecuteSavedCommandBySelector>(arguments, reply.get(), this, &WebPageProxy::executeSavedCommandBySelector);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::GetIsSpeakingID:
        CoreIPC::handleMessage<Messages::WebPageProxy::GetIsSpeaking>(arguments, reply.get(), this, &WebPageProxy::getIsSpeaking);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPageProxy::SubstitutionsPanelIsShowingID:
        CoreIPC::handleMessage<Messages::WebPageProxy::SubstitutionsPanelIsShowing>(arguments, reply.get(), this, &WebPageProxy::substitutionsPanelIsShowing);
        return;
#endif
#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
    case Messages::WebPageProxy::DismissCorrectionPanelSoonID:
        CoreIPC::handleMessage<Messages::WebPageProxy::DismissCorrectionPanelSoon>(arguments, reply.get(), this, &WebPageProxy::dismissCorrectionPanelSoon);
        return;
#endif
    case Messages::WebPageProxy::LoadRecentSearchesID:
        CoreIPC::handleMessage<Messages::WebPageProxy::LoadRecentSearches>(arguments, reply.get(), this, &WebPageProxy::loadRecentSearches);
        return;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit
