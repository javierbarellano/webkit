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

#include "WebPage.h"

#include "ArgumentCoders.h"
#include "ArgumentDecoder.h"
#if PLATFORM(MAC)
#include "AttributedString.h"
#endif
#include "DataReference.h"
#if PLATFORM(MAC)
#include "EditorState.h"
#endif
#include "HandleMessage.h"
#include "PrintInfo.h"
#if PLATFORM(QT)
#include "QtNetworkReplyData.h"
#endif
#include "SandboxExtension.h"
#include "SessionState.h"
#include "WebContextMenuItemData.h"
#include "WebCoreArgumentCoders.h"
#include "WebEvent.h"
#include "WebPageMessages.h"
#include "WebPreferencesStore.h"
#if PLATFORM(QT) || PLATFORM(GTK)
#include <WebCore/DragData.h>
#endif
#if PLATFORM(MAC) || PLATFORM(QT) || PLATFORM(WIN)
#include <WebCore/Editor.h>
#endif
#if PLATFORM(MAC)
#include <WebCore/FloatPoint.h>
#endif
#include <WebCore/IntPoint.h>
#if PLATFORM(MAC) || PLATFORM(MAC) || PLATFORM(WIN) || PLATFORM(WIN)
#include <WebCore/IntRect.h>
#endif
#include <WebCore/IntSize.h>
#if PLATFORM(MAC)
#include <WebCore/KeyboardEvent.h>
#endif
#include <WebCore/ResourceRequest.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

void WebPage::didReceiveWebPageMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::WebPage::Kind>()) {
    case Messages::WebPage::SetActiveID:
        CoreIPC::handleMessage<Messages::WebPage::SetActive>(arguments, this, &WebPage::setActive);
        return;
    case Messages::WebPage::SetFocusedID:
        CoreIPC::handleMessage<Messages::WebPage::SetFocused>(arguments, this, &WebPage::setFocused);
        return;
    case Messages::WebPage::SetInitialFocusID:
        CoreIPC::handleMessage<Messages::WebPage::SetInitialFocus>(arguments, this, &WebPage::setInitialFocus);
        return;
    case Messages::WebPage::SetIsInWindowID:
        CoreIPC::handleMessage<Messages::WebPage::SetIsInWindow>(arguments, this, &WebPage::setIsInWindow);
        return;
    case Messages::WebPage::SetDrawsBackgroundID:
        CoreIPC::handleMessage<Messages::WebPage::SetDrawsBackground>(arguments, this, &WebPage::setDrawsBackground);
        return;
    case Messages::WebPage::SetDrawsTransparentBackgroundID:
        CoreIPC::handleMessage<Messages::WebPage::SetDrawsTransparentBackground>(arguments, this, &WebPage::setDrawsTransparentBackground);
        return;
    case Messages::WebPage::ViewWillStartLiveResizeID:
        CoreIPC::handleMessage<Messages::WebPage::ViewWillStartLiveResize>(arguments, this, &WebPage::viewWillStartLiveResize);
        return;
    case Messages::WebPage::ViewWillEndLiveResizeID:
        CoreIPC::handleMessage<Messages::WebPage::ViewWillEndLiveResize>(arguments, this, &WebPage::viewWillEndLiveResize);
        return;
    case Messages::WebPage::KeyEventID:
        CoreIPC::handleMessage<Messages::WebPage::KeyEvent>(arguments, this, &WebPage::keyEvent);
        return;
    case Messages::WebPage::MouseEventID:
        CoreIPC::handleMessage<Messages::WebPage::MouseEvent>(arguments, this, &WebPage::mouseEvent);
        return;
#if ENABLE(TOUCH_EVENTS)
    case Messages::WebPage::TouchEventID:
        CoreIPC::handleMessage<Messages::WebPage::TouchEvent>(arguments, this, &WebPage::touchEvent);
        return;
#endif
#if ENABLE(TOUCH_EVENTS) && PLATFORM(QT)
    case Messages::WebPage::HighlightPotentialActivationID:
        CoreIPC::handleMessage<Messages::WebPage::HighlightPotentialActivation>(arguments, this, &WebPage::highlightPotentialActivation);
        return;
#endif
    case Messages::WebPage::ContextMenuHiddenID:
        CoreIPC::handleMessage<Messages::WebPage::ContextMenuHidden>(arguments, this, &WebPage::contextMenuHidden);
        return;
    case Messages::WebPage::ScrollByID:
        CoreIPC::handleMessage<Messages::WebPage::ScrollBy>(arguments, this, &WebPage::scrollBy);
        return;
    case Messages::WebPage::CenterSelectionInVisibleAreaID:
        CoreIPC::handleMessage<Messages::WebPage::CenterSelectionInVisibleArea>(arguments, this, &WebPage::centerSelectionInVisibleArea);
        return;
    case Messages::WebPage::GoBackID:
        CoreIPC::handleMessage<Messages::WebPage::GoBack>(arguments, this, &WebPage::goBack);
        return;
    case Messages::WebPage::GoForwardID:
        CoreIPC::handleMessage<Messages::WebPage::GoForward>(arguments, this, &WebPage::goForward);
        return;
    case Messages::WebPage::GoToBackForwardItemID:
        CoreIPC::handleMessage<Messages::WebPage::GoToBackForwardItem>(arguments, this, &WebPage::goToBackForwardItem);
        return;
    case Messages::WebPage::TryRestoreScrollPositionID:
        CoreIPC::handleMessage<Messages::WebPage::TryRestoreScrollPosition>(arguments, this, &WebPage::tryRestoreScrollPosition);
        return;
    case Messages::WebPage::LoadHTMLStringID:
        CoreIPC::handleMessage<Messages::WebPage::LoadHTMLString>(arguments, this, &WebPage::loadHTMLString);
        return;
    case Messages::WebPage::LoadAlternateHTMLStringID:
        CoreIPC::handleMessage<Messages::WebPage::LoadAlternateHTMLString>(arguments, this, &WebPage::loadAlternateHTMLString);
        return;
    case Messages::WebPage::LoadPlainTextStringID:
        CoreIPC::handleMessage<Messages::WebPage::LoadPlainTextString>(arguments, this, &WebPage::loadPlainTextString);
        return;
    case Messages::WebPage::LoadWebArchiveDataID:
        CoreIPC::handleMessage<Messages::WebPage::LoadWebArchiveData>(arguments, this, &WebPage::loadWebArchiveData);
        return;
    case Messages::WebPage::LoadURLID:
        CoreIPC::handleMessage<Messages::WebPage::LoadURL>(arguments, this, &WebPage::loadURL);
        return;
    case Messages::WebPage::LoadURLRequestID:
        CoreIPC::handleMessage<Messages::WebPage::LoadURLRequest>(arguments, this, &WebPage::loadURLRequest);
        return;
    case Messages::WebPage::LinkClickedID:
        CoreIPC::handleMessage<Messages::WebPage::LinkClicked>(arguments, this, &WebPage::linkClicked);
        return;
    case Messages::WebPage::ReloadID:
        CoreIPC::handleMessage<Messages::WebPage::Reload>(arguments, this, &WebPage::reload);
        return;
    case Messages::WebPage::StopLoadingID:
        CoreIPC::handleMessage<Messages::WebPage::StopLoading>(arguments, this, &WebPage::stopLoading);
        return;
#if PLATFORM(QT)
    case Messages::WebPage::ApplicationSchemeReplyID:
        CoreIPC::handleMessage<Messages::WebPage::ApplicationSchemeReply>(arguments, this, &WebPage::applicationSchemeReply);
        return;
#endif
#if PLATFORM(QT)
    case Messages::WebPage::RegisterApplicationSchemeID:
        CoreIPC::handleMessage<Messages::WebPage::RegisterApplicationScheme>(arguments, this, &WebPage::registerApplicationScheme);
        return;
#endif
    case Messages::WebPage::StopLoadingFrameID:
        CoreIPC::handleMessage<Messages::WebPage::StopLoadingFrame>(arguments, this, &WebPage::stopLoadingFrame);
        return;
    case Messages::WebPage::RestoreSessionID:
        CoreIPC::handleMessage<Messages::WebPage::RestoreSession>(arguments, this, &WebPage::restoreSession);
        return;
    case Messages::WebPage::RestoreSessionAndNavigateToCurrentItemID:
        CoreIPC::handleMessage<Messages::WebPage::RestoreSessionAndNavigateToCurrentItem>(arguments, this, &WebPage::restoreSessionAndNavigateToCurrentItem);
        return;
    case Messages::WebPage::DidRemoveBackForwardItemID:
        CoreIPC::handleMessage<Messages::WebPage::DidRemoveBackForwardItem>(arguments, this, &WebPage::didRemoveBackForwardItem);
        return;
    case Messages::WebPage::SetWillGoToBackForwardItemCallbackEnabledID:
        CoreIPC::handleMessage<Messages::WebPage::SetWillGoToBackForwardItemCallbackEnabled>(arguments, this, &WebPage::setWillGoToBackForwardItemCallbackEnabled);
        return;
    case Messages::WebPage::DidReceivePolicyDecisionID:
        CoreIPC::handleMessage<Messages::WebPage::DidReceivePolicyDecision>(arguments, this, &WebPage::didReceivePolicyDecision);
        return;
    case Messages::WebPage::ClearSelectionID:
        CoreIPC::handleMessage<Messages::WebPage::ClearSelection>(arguments, this, &WebPage::clearSelection);
        return;
    case Messages::WebPage::GetContentsAsStringID:
        CoreIPC::handleMessage<Messages::WebPage::GetContentsAsString>(arguments, this, &WebPage::getContentsAsString);
        return;
    case Messages::WebPage::GetMainResourceDataOfFrameID:
        CoreIPC::handleMessage<Messages::WebPage::GetMainResourceDataOfFrame>(arguments, this, &WebPage::getMainResourceDataOfFrame);
        return;
    case Messages::WebPage::GetResourceDataFromFrameID:
        CoreIPC::handleMessage<Messages::WebPage::GetResourceDataFromFrame>(arguments, this, &WebPage::getResourceDataFromFrame);
        return;
    case Messages::WebPage::GetRenderTreeExternalRepresentationID:
        CoreIPC::handleMessage<Messages::WebPage::GetRenderTreeExternalRepresentation>(arguments, this, &WebPage::getRenderTreeExternalRepresentation);
        return;
    case Messages::WebPage::GetSelectionOrContentsAsStringID:
        CoreIPC::handleMessage<Messages::WebPage::GetSelectionOrContentsAsString>(arguments, this, &WebPage::getSelectionOrContentsAsString);
        return;
    case Messages::WebPage::GetSourceForFrameID:
        CoreIPC::handleMessage<Messages::WebPage::GetSourceForFrame>(arguments, this, &WebPage::getSourceForFrame);
        return;
    case Messages::WebPage::GetWebArchiveOfFrameID:
        CoreIPC::handleMessage<Messages::WebPage::GetWebArchiveOfFrame>(arguments, this, &WebPage::getWebArchiveOfFrame);
        return;
    case Messages::WebPage::RunJavaScriptInMainFrameID:
        CoreIPC::handleMessage<Messages::WebPage::RunJavaScriptInMainFrame>(arguments, this, &WebPage::runJavaScriptInMainFrame);
        return;
    case Messages::WebPage::ForceRepaintID:
        CoreIPC::handleMessage<Messages::WebPage::ForceRepaint>(arguments, this, &WebPage::forceRepaint);
        return;
#if PLATFORM(MAC)
    case Messages::WebPage::PerformDictionaryLookupAtLocationID:
        CoreIPC::handleMessage<Messages::WebPage::PerformDictionaryLookupAtLocation>(arguments, this, &WebPage::performDictionaryLookupAtLocation);
        return;
#endif
    case Messages::WebPage::PreferencesDidChangeID:
        CoreIPC::handleMessage<Messages::WebPage::PreferencesDidChange>(arguments, this, &WebPage::preferencesDidChange);
        return;
    case Messages::WebPage::SetUserAgentID:
        CoreIPC::handleMessage<Messages::WebPage::SetUserAgent>(arguments, this, &WebPage::setUserAgent);
        return;
    case Messages::WebPage::SetCustomTextEncodingNameID:
        CoreIPC::handleMessage<Messages::WebPage::SetCustomTextEncodingName>(arguments, this, &WebPage::setCustomTextEncodingName);
        return;
    case Messages::WebPage::SuspendActiveDOMObjectsAndAnimationsID:
        CoreIPC::handleMessage<Messages::WebPage::SuspendActiveDOMObjectsAndAnimations>(arguments, this, &WebPage::suspendActiveDOMObjectsAndAnimations);
        return;
    case Messages::WebPage::ResumeActiveDOMObjectsAndAnimationsID:
        CoreIPC::handleMessage<Messages::WebPage::ResumeActiveDOMObjectsAndAnimations>(arguments, this, &WebPage::resumeActiveDOMObjectsAndAnimations);
        return;
#if USE(TILED_BACKING_STORE)
    case Messages::WebPage::SetViewportSizeID:
        CoreIPC::handleMessage<Messages::WebPage::SetViewportSize>(arguments, this, &WebPage::setViewportSize);
        return;
#endif
    case Messages::WebPage::CloseID:
        CoreIPC::handleMessage<Messages::WebPage::Close>(arguments, this, &WebPage::close);
        return;
    case Messages::WebPage::TryCloseID:
        CoreIPC::handleMessage<Messages::WebPage::TryClose>(arguments, this, &WebPage::tryClose);
        return;
    case Messages::WebPage::ValidateCommandID:
        CoreIPC::handleMessage<Messages::WebPage::ValidateCommand>(arguments, this, &WebPage::validateCommand);
        return;
    case Messages::WebPage::ExecuteEditCommandID:
        CoreIPC::handleMessage<Messages::WebPage::ExecuteEditCommand>(arguments, this, &WebPage::executeEditCommand);
        return;
    case Messages::WebPage::DidRemoveEditCommandID:
        CoreIPC::handleMessage<Messages::WebPage::DidRemoveEditCommand>(arguments, this, &WebPage::didRemoveEditCommand);
        return;
    case Messages::WebPage::ReapplyEditCommandID:
        CoreIPC::handleMessage<Messages::WebPage::ReapplyEditCommand>(arguments, this, &WebPage::reapplyEditCommand);
        return;
    case Messages::WebPage::UnapplyEditCommandID:
        CoreIPC::handleMessage<Messages::WebPage::UnapplyEditCommand>(arguments, this, &WebPage::unapplyEditCommand);
        return;
    case Messages::WebPage::SetPageAndTextZoomFactorsID:
        CoreIPC::handleMessage<Messages::WebPage::SetPageAndTextZoomFactors>(arguments, this, &WebPage::setPageAndTextZoomFactors);
        return;
    case Messages::WebPage::SetPageZoomFactorID:
        CoreIPC::handleMessage<Messages::WebPage::SetPageZoomFactor>(arguments, this, &WebPage::setPageZoomFactor);
        return;
    case Messages::WebPage::SetTextZoomFactorID:
        CoreIPC::handleMessage<Messages::WebPage::SetTextZoomFactor>(arguments, this, &WebPage::setTextZoomFactor);
        return;
    case Messages::WebPage::WindowScreenDidChangeID:
        CoreIPC::handleMessage<Messages::WebPage::WindowScreenDidChange>(arguments, this, &WebPage::windowScreenDidChange);
        return;
    case Messages::WebPage::ScalePageID:
        CoreIPC::handleMessage<Messages::WebPage::ScalePage>(arguments, this, &WebPage::scalePage);
        return;
    case Messages::WebPage::SetUseFixedLayoutID:
        CoreIPC::handleMessage<Messages::WebPage::SetUseFixedLayout>(arguments, this, &WebPage::setUseFixedLayout);
        return;
    case Messages::WebPage::SetFixedLayoutSizeID:
        CoreIPC::handleMessage<Messages::WebPage::SetFixedLayoutSize>(arguments, this, &WebPage::setFixedLayoutSize);
        return;
    case Messages::WebPage::SetPaginationModeID:
        CoreIPC::handleMessage<Messages::WebPage::SetPaginationMode>(arguments, this, &WebPage::setPaginationMode);
        return;
    case Messages::WebPage::SetPaginationBehavesLikeColumnsID:
        CoreIPC::handleMessage<Messages::WebPage::SetPaginationBehavesLikeColumns>(arguments, this, &WebPage::setPaginationBehavesLikeColumns);
        return;
    case Messages::WebPage::SetPageLengthID:
        CoreIPC::handleMessage<Messages::WebPage::SetPageLength>(arguments, this, &WebPage::setPageLength);
        return;
    case Messages::WebPage::SetGapBetweenPagesID:
        CoreIPC::handleMessage<Messages::WebPage::SetGapBetweenPages>(arguments, this, &WebPage::setGapBetweenPages);
        return;
    case Messages::WebPage::FindStringID:
        CoreIPC::handleMessage<Messages::WebPage::FindString>(arguments, this, &WebPage::findString);
        return;
    case Messages::WebPage::HideFindUIID:
        CoreIPC::handleMessage<Messages::WebPage::HideFindUI>(arguments, this, &WebPage::hideFindUI);
        return;
    case Messages::WebPage::CountStringMatchesID:
        CoreIPC::handleMessage<Messages::WebPage::CountStringMatches>(arguments, this, &WebPage::countStringMatches);
        return;
#if PLATFORM(WIN)
    case Messages::WebPage::PerformDragControllerActionID:
        CoreIPC::handleMessage<Messages::WebPage::PerformDragControllerAction>(arguments, this, &WebPage::performDragControllerAction);
        return;
#endif
#if PLATFORM(QT) || PLATFORM(GTK)
    case Messages::WebPage::PerformDragControllerActionID:
        CoreIPC::handleMessage<Messages::WebPage::PerformDragControllerAction>(arguments, this, &WebPage::performDragControllerAction);
        return;
#endif
#if !PLATFORM(WIN) && !PLATFORM(QT) && !PLATFORM(GTK)
    case Messages::WebPage::PerformDragControllerActionID:
        CoreIPC::handleMessage<Messages::WebPage::PerformDragControllerAction>(arguments, this, &WebPage::performDragControllerAction);
        return;
#endif
    case Messages::WebPage::DragEndedID:
        CoreIPC::handleMessage<Messages::WebPage::DragEnded>(arguments, this, &WebPage::dragEnded);
        return;
    case Messages::WebPage::DidChangeSelectedIndexForActivePopupMenuID:
        CoreIPC::handleMessage<Messages::WebPage::DidChangeSelectedIndexForActivePopupMenu>(arguments, this, &WebPage::didChangeSelectedIndexForActivePopupMenu);
        return;
    case Messages::WebPage::SetTextForActivePopupMenuID:
        CoreIPC::handleMessage<Messages::WebPage::SetTextForActivePopupMenu>(arguments, this, &WebPage::setTextForActivePopupMenu);
        return;
#if PLATFORM(GTK)
    case Messages::WebPage::FailedToShowPopupMenuID:
        CoreIPC::handleMessage<Messages::WebPage::FailedToShowPopupMenu>(arguments, this, &WebPage::failedToShowPopupMenu);
        return;
#endif
    case Messages::WebPage::DidSelectItemFromActiveContextMenuID:
        CoreIPC::handleMessage<Messages::WebPage::DidSelectItemFromActiveContextMenu>(arguments, this, &WebPage::didSelectItemFromActiveContextMenu);
        return;
    case Messages::WebPage::DidChooseFilesForOpenPanelID:
        CoreIPC::handleMessage<Messages::WebPage::DidChooseFilesForOpenPanel>(arguments, this, &WebPage::didChooseFilesForOpenPanel);
        return;
    case Messages::WebPage::DidCancelForOpenPanelID:
        CoreIPC::handleMessage<Messages::WebPage::DidCancelForOpenPanel>(arguments, this, &WebPage::didCancelForOpenPanel);
        return;
#if ENABLE(WEB_PROCESS_SANDBOX)
    case Messages::WebPage::ExtendSandboxForFileFromOpenPanelID:
        CoreIPC::handleMessage<Messages::WebPage::ExtendSandboxForFileFromOpenPanel>(arguments, this, &WebPage::extendSandboxForFileFromOpenPanel);
        return;
#endif
    case Messages::WebPage::AdvanceToNextMisspellingID:
        CoreIPC::handleMessage<Messages::WebPage::AdvanceToNextMisspelling>(arguments, this, &WebPage::advanceToNextMisspelling);
        return;
    case Messages::WebPage::ChangeSpellingToWordID:
        CoreIPC::handleMessage<Messages::WebPage::ChangeSpellingToWord>(arguments, this, &WebPage::changeSpellingToWord);
        return;
#if PLATFORM(MAC)
    case Messages::WebPage::UppercaseWordID:
        CoreIPC::handleMessage<Messages::WebPage::UppercaseWord>(arguments, this, &WebPage::uppercaseWord);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::LowercaseWordID:
        CoreIPC::handleMessage<Messages::WebPage::LowercaseWord>(arguments, this, &WebPage::lowercaseWord);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::CapitalizeWordID:
        CoreIPC::handleMessage<Messages::WebPage::CapitalizeWord>(arguments, this, &WebPage::capitalizeWord);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::SetSmartInsertDeleteEnabledID:
        CoreIPC::handleMessage<Messages::WebPage::SetSmartInsertDeleteEnabled>(arguments, this, &WebPage::setSmartInsertDeleteEnabled);
        return;
#endif
#if ENABLE(GEOLOCATION)
    case Messages::WebPage::DidReceiveGeolocationPermissionDecisionID:
        CoreIPC::handleMessage<Messages::WebPage::DidReceiveGeolocationPermissionDecision>(arguments, this, &WebPage::didReceiveGeolocationPermissionDecision);
        return;
#endif
    case Messages::WebPage::DidReceiveNotificationPermissionDecisionID:
        CoreIPC::handleMessage<Messages::WebPage::DidReceiveNotificationPermissionDecision>(arguments, this, &WebPage::didReceiveNotificationPermissionDecision);
        return;
    case Messages::WebPage::SetWindowResizerSizeID:
        CoreIPC::handleMessage<Messages::WebPage::SetWindowResizerSize>(arguments, this, &WebPage::setWindowResizerSize);
        return;
    case Messages::WebPage::BeginPrintingID:
        CoreIPC::handleMessage<Messages::WebPage::BeginPrinting>(arguments, this, &WebPage::beginPrinting);
        return;
    case Messages::WebPage::EndPrintingID:
        CoreIPC::handleMessage<Messages::WebPage::EndPrinting>(arguments, this, &WebPage::endPrinting);
        return;
    case Messages::WebPage::ComputePagesForPrintingID:
        CoreIPC::handleMessage<Messages::WebPage::ComputePagesForPrinting>(arguments, this, &WebPage::computePagesForPrinting);
        return;
#if PLATFORM(MAC) || PLATFORM(WIN)
    case Messages::WebPage::DrawRectToPDFID:
        CoreIPC::handleMessage<Messages::WebPage::DrawRectToPDF>(arguments, this, &WebPage::drawRectToPDF);
        return;
#endif
#if PLATFORM(MAC) || PLATFORM(WIN)
    case Messages::WebPage::DrawPagesToPDFID:
        CoreIPC::handleMessage<Messages::WebPage::DrawPagesToPDF>(arguments, this, &WebPage::drawPagesToPDF);
        return;
#endif
#if PLATFORM(GTK)
    case Messages::WebPage::DrawPagesForPrintingID:
        CoreIPC::handleMessage<Messages::WebPage::DrawPagesForPrinting>(arguments, this, &WebPage::drawPagesForPrinting);
        return;
#endif
    case Messages::WebPage::SetMediaVolumeID:
        CoreIPC::handleMessage<Messages::WebPage::SetMediaVolume>(arguments, this, &WebPage::setMediaVolume);
        return;
    case Messages::WebPage::SetMemoryCacheMessagesEnabledID:
        CoreIPC::handleMessage<Messages::WebPage::SetMemoryCacheMessagesEnabled>(arguments, this, &WebPage::setMemoryCacheMessagesEnabled);
        return;
    case Messages::WebPage::SetCanRunBeforeUnloadConfirmPanelID:
        CoreIPC::handleMessage<Messages::WebPage::SetCanRunBeforeUnloadConfirmPanel>(arguments, this, &WebPage::setCanRunBeforeUnloadConfirmPanel);
        return;
    case Messages::WebPage::SetCanRunModalID:
        CoreIPC::handleMessage<Messages::WebPage::SetCanRunModal>(arguments, this, &WebPage::setCanRunModal);
        return;
#if PLATFORM(QT)
    case Messages::WebPage::SetCompositionID:
        CoreIPC::handleMessage<Messages::WebPage::SetComposition>(arguments, this, &WebPage::setComposition);
        return;
#endif
#if PLATFORM(QT)
    case Messages::WebPage::ConfirmCompositionID:
        CoreIPC::handleMessage<Messages::WebPage::ConfirmComposition>(arguments, this, &WebPage::confirmComposition);
        return;
#endif
#if PLATFORM(QT)
    case Messages::WebPage::CancelCompositionID:
        CoreIPC::handleMessage<Messages::WebPage::CancelComposition>(arguments, this, &WebPage::cancelComposition);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::SendComplexTextInputToPluginID:
        CoreIPC::handleMessage<Messages::WebPage::SendComplexTextInputToPlugin>(arguments, this, &WebPage::sendComplexTextInputToPlugin);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::SetWindowIsVisibleID:
        CoreIPC::handleMessage<Messages::WebPage::SetWindowIsVisible>(arguments, this, &WebPage::setWindowIsVisible);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::WindowAndViewFramesChangedID:
        CoreIPC::handleMessage<Messages::WebPage::WindowAndViewFramesChanged>(arguments, this, &WebPage::windowAndViewFramesChanged);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::RegisterUIProcessAccessibilityTokensID:
        CoreIPC::handleMessage<Messages::WebPage::RegisterUIProcessAccessibilityTokens>(arguments, this, &WebPage::registerUIProcessAccessibilityTokens);
        return;
#endif
#if PLATFORM(WIN)
    case Messages::WebPage::ConfirmCompositionID:
        CoreIPC::handleMessage<Messages::WebPage::ConfirmComposition>(arguments, this, &WebPage::confirmComposition);
        return;
#endif
#if PLATFORM(WIN)
    case Messages::WebPage::SetCompositionID:
        CoreIPC::handleMessage<Messages::WebPage::SetComposition>(arguments, this, &WebPage::setComposition);
        return;
#endif
#if PLATFORM(WIN)
    case Messages::WebPage::GestureDidScrollID:
        CoreIPC::handleMessage<Messages::WebPage::GestureDidScroll>(arguments, this, &WebPage::gestureDidScroll);
        return;
#endif
#if PLATFORM(WIN)
    case Messages::WebPage::GestureDidEndID:
        CoreIPC::handleMessage<Messages::WebPage::GestureDidEnd>(arguments, this, &WebPage::gestureDidEnd);
        return;
#endif
#if PLATFORM(QT)
    case Messages::WebPage::FindZoomableAreaForPointID:
        CoreIPC::handleMessage<Messages::WebPage::FindZoomableAreaForPoint>(arguments, this, &WebPage::findZoomableAreaForPoint);
        return;
#endif
#if PLATFORM(MAC) && !defined(BUILDING_ON_SNOW_LEOPARD)
    case Messages::WebPage::HandleCorrectionPanelResultID:
        CoreIPC::handleMessage<Messages::WebPage::HandleCorrectionPanelResult>(arguments, this, &WebPage::handleCorrectionPanelResult);
        return;
#endif
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

void WebPage::didReceiveSyncWebPageMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments, OwnPtr<CoreIPC::ArgumentEncoder>& reply)
{
    switch (messageID.get<Messages::WebPage::Kind>()) {
    case Messages::WebPage::KeyEventSyncForTestingID:
        CoreIPC::handleMessage<Messages::WebPage::KeyEventSyncForTesting>(arguments, reply.get(), this, &WebPage::keyEventSyncForTesting);
        return;
    case Messages::WebPage::MouseEventSyncForTestingID:
        CoreIPC::handleMessage<Messages::WebPage::MouseEventSyncForTesting>(arguments, reply.get(), this, &WebPage::mouseEventSyncForTesting);
        return;
    case Messages::WebPage::WheelEventSyncForTestingID:
        CoreIPC::handleMessage<Messages::WebPage::WheelEventSyncForTesting>(arguments, reply.get(), this, &WebPage::wheelEventSyncForTesting);
        return;
#if ENABLE(TOUCH_EVENTS)
    case Messages::WebPage::TouchEventSyncForTestingID:
        CoreIPC::handleMessage<Messages::WebPage::TouchEventSyncForTesting>(arguments, reply.get(), this, &WebPage::touchEventSyncForTesting);
        return;
#endif
    case Messages::WebPage::DummyID:
        CoreIPC::handleMessage<Messages::WebPage::Dummy>(arguments, reply.get(), this, &WebPage::dummy);
        return;
#if PLATFORM(MAC)
    case Messages::WebPage::WriteSelectionToPasteboardID:
        CoreIPC::handleMessage<Messages::WebPage::WriteSelectionToPasteboard>(arguments, reply.get(), this, &WebPage::writeSelectionToPasteboard);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::ReadSelectionFromPasteboardID:
        CoreIPC::handleMessage<Messages::WebPage::ReadSelectionFromPasteboard>(arguments, reply.get(), this, &WebPage::readSelectionFromPasteboard);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::SetCompositionID:
        CoreIPC::handleMessage<Messages::WebPage::SetComposition>(arguments, reply.get(), this, &WebPage::setComposition);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::ConfirmCompositionID:
        CoreIPC::handleMessage<Messages::WebPage::ConfirmComposition>(arguments, reply.get(), this, &WebPage::confirmComposition);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::CancelCompositionID:
        CoreIPC::handleMessage<Messages::WebPage::CancelComposition>(arguments, reply.get(), this, &WebPage::cancelComposition);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::InsertTextID:
        CoreIPC::handleMessage<Messages::WebPage::InsertText>(arguments, reply.get(), this, &WebPage::insertText);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::GetMarkedRangeID:
        CoreIPC::handleMessage<Messages::WebPage::GetMarkedRange>(arguments, reply.get(), this, &WebPage::getMarkedRange);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::GetSelectedRangeID:
        CoreIPC::handleMessage<Messages::WebPage::GetSelectedRange>(arguments, reply.get(), this, &WebPage::getSelectedRange);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::GetAttributedSubstringFromRangeID:
        CoreIPC::handleMessage<Messages::WebPage::GetAttributedSubstringFromRange>(arguments, reply.get(), this, &WebPage::getAttributedSubstringFromRange);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::CharacterIndexForPointID:
        CoreIPC::handleMessage<Messages::WebPage::CharacterIndexForPoint>(arguments, reply.get(), this, &WebPage::characterIndexForPoint);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::FirstRectForCharacterRangeID:
        CoreIPC::handleMessage<Messages::WebPage::FirstRectForCharacterRange>(arguments, reply.get(), this, &WebPage::firstRectForCharacterRange);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::ExecuteKeypressCommandsID:
        CoreIPC::handleMessage<Messages::WebPage::ExecuteKeypressCommands>(arguments, reply.get(), this, &WebPage::executeKeypressCommands);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::ShouldDelayWindowOrderingEventID:
        CoreIPC::handleMessage<Messages::WebPage::ShouldDelayWindowOrderingEvent>(arguments, reply.get(), this, &WebPage::shouldDelayWindowOrderingEvent);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::WebPage::AcceptsFirstMouseID:
        CoreIPC::handleMessage<Messages::WebPage::AcceptsFirstMouse>(arguments, reply.get(), this, &WebPage::acceptsFirstMouse);
        return;
#endif
#if PLATFORM(WIN)
    case Messages::WebPage::FirstRectForCharacterInSelectedRangeID:
        CoreIPC::handleMessage<Messages::WebPage::FirstRectForCharacterInSelectedRange>(arguments, reply.get(), this, &WebPage::firstRectForCharacterInSelectedRange);
        return;
#endif
#if PLATFORM(WIN)
    case Messages::WebPage::GetSelectedTextID:
        CoreIPC::handleMessage<Messages::WebPage::GetSelectedText>(arguments, reply.get(), this, &WebPage::getSelectedText);
        return;
#endif
#if PLATFORM(WIN)
    case Messages::WebPage::GestureWillBeginID:
        CoreIPC::handleMessage<Messages::WebPage::GestureWillBegin>(arguments, reply.get(), this, &WebPage::gestureWillBegin);
        return;
#endif
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit
