/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "JSLayoutTestController.h"
#include "LayoutTestController.h"
#include <JavaScriptCore/JSRetainPtr.h>
#include <wtf/GetPtr.h>

namespace WTR {

LayoutTestController* toLayoutTestController(JSContextRef context, JSValueRef value)
{
    if (!context || !value || !JSLayoutTestController::layoutTestControllerClass() || !JSValueIsObjectOfClass(context, value, JSLayoutTestController::layoutTestControllerClass()))
        return 0;
    return static_cast<LayoutTestController*>(JSWrapper::unwrap(context, value));
}

JSClassRef JSLayoutTestController::layoutTestControllerClass()
{
    static JSClassRef jsClass;
    if (!jsClass) {
        JSClassDefinition definition = kJSClassDefinitionEmpty;
        definition.className = "LayoutTestController";
        definition.parentClass = 0;
        definition.staticValues = staticValues();
        definition.staticFunctions = staticFunctions();
        definition.initialize = initialize;
        definition.finalize = finalize;
        jsClass = JSClassCreate(&definition);
    }
    return jsClass;
}

const JSStaticFunction* JSLayoutTestController::staticFunctions()
{
    static const JSStaticFunction functions[] = {
        { "dumpAsText", dumpAsText, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "dumpChildFramesAsText", dumpChildFramesAsText, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "waitForPolicyDelegate", waitForPolicyDelegate, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "waitUntilDone", waitUntilDone, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "notifyDone", notifyDone, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "dumpBackForwardList", dumpBackForwardList, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "dumpChildFrameScrollPositions", dumpChildFrameScrollPositions, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "dumpEditingCallbacks", dumpEditingCallbacks, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "dumpSelectionRect", dumpSelectionRect, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "dumpStatusCallbacks", dumpStatusCallbacks, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "dumpTitleChanges", dumpTitleChanges, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "dumpFullScreenCallbacks", dumpFullScreenCallbacks, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "dumpConfigurationForViewport", dumpConfigurationForViewport, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "keepWebHistory", keepWebHistory, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setAcceptsEditing", setAcceptsEditing, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setCanOpenWindows", setCanOpenWindows, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setCloseRemainingWindowsWhenComplete", setCloseRemainingWindowsWhenComplete, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setXSSAuditorEnabled", setXSSAuditorEnabled, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setAllowUniversalAccessFromFileURLs", setAllowUniversalAccessFromFileURLs, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setAllowFileAccessFromFileURLs", setAllowFileAccessFromFileURLs, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setFrameFlatteningEnabled", setFrameFlatteningEnabled, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setGeolocationPermission", setGeolocationPermission, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setJavaScriptCanAccessClipboard", setJavaScriptCanAccessClipboard, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setPrivateBrowsingEnabled", setPrivateBrowsingEnabled, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setPopupBlockingEnabled", setPopupBlockingEnabled, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setAuthorAndUserStylesEnabled", setAuthorAndUserStylesEnabled, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "addOriginAccessWhitelistEntry", addOriginAccessWhitelistEntry, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "removeOriginAccessWhitelistEntry", removeOriginAccessWhitelistEntry, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "clearBackForwardList", clearBackForwardList, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "computedStyleIncludingVisitedInfo", computedStyleIncludingVisitedInfo, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "counterValueForElementById", counterValueForElementById, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "execCommand", execCommand, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "isCommandEnabled", isCommandEnabled, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "markerTextForListItem", markerTextForListItem, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "windowCount", windowCount, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "testRepaint", testRepaint, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "repaintSweepHorizontally", repaintSweepHorizontally, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "display", display, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "numberOfPages", numberOfPages, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "pageNumberForElementById", pageNumberForElementById, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "pageSizeAndMarginsInPixels", pageSizeAndMarginsInPixels, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "isPageBoxVisible", isPageBoxVisible, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setValueForUser", setValueForUser, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "numberOfActiveAnimations", numberOfActiveAnimations, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "pauseAnimationAtTimeOnElementWithId", pauseAnimationAtTimeOnElementWithId, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "pauseTransitionAtTimeOnElementWithId", pauseTransitionAtTimeOnElementWithId, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "suspendAnimations", suspendAnimations, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "resumeAnimations", resumeAnimations, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "addUserScript", addUserScript, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "addUserStyleSheet", addUserStyleSheet, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "clearAllDatabases", clearAllDatabases, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setDatabaseQuota", setDatabaseQuota, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "pathToLocalResource", pathToLocalResource, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "clearAllApplicationCaches", clearAllApplicationCaches, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setAppCacheMaximumSize", setAppCacheMaximumSize, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "layerTreeAsText", layerTreeAsText, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "findString", findString, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "evaluateScriptInIsolatedWorld", evaluateScriptInIsolatedWorld, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "showWebInspector", showWebInspector, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "closeWebInspector", closeWebInspector, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "evaluateInWebInspector", evaluateInWebInspector, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setPOSIXLocale", setPOSIXLocale, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setTextDirection", setTextDirection, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setWillSendRequestReturnsNull", setWillSendRequestReturnsNull, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setShouldStayOnPageAfterHandlingBeforeUnload", setShouldStayOnPageAfterHandlingBeforeUnload, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "addChromeInputField", addChromeInputField, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "removeChromeInputField", removeChromeInputField, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "focusWebView", focusWebView, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setBackingScaleFactor", setBackingScaleFactor, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "setWindowIsKey", setWindowIsKey, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { "overridePreference", overridePreference, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { 0, 0, 0 }
    };
    return functions;
}

const JSStaticValue* JSLayoutTestController::staticValues()
{
    static const JSStaticValue values[] = {
        { "globalFlag", globalFlag, setGlobalFlag, kJSPropertyAttributeDontDelete },
        { "platformName", platformName, 0, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
        { 0, 0, 0, 0 }
    };
    return values;
}

// Functions

JSValueRef JSLayoutTestController::dumpAsText(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool dumpPixels = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->dumpAsText(dumpPixels);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::dumpChildFramesAsText(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->dumpChildFramesAsText();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::waitForPolicyDelegate(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->waitForPolicyDelegate();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::waitUntilDone(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->waitUntilDone();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::notifyDone(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->notifyDone();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::dumpBackForwardList(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->dumpBackForwardList();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::dumpChildFrameScrollPositions(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->dumpChildFrameScrollPositions();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::dumpEditingCallbacks(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->dumpEditingCallbacks();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::dumpSelectionRect(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->dumpSelectionRect();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::dumpStatusCallbacks(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->dumpStatusCallbacks();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::dumpTitleChanges(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->dumpTitleChanges();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::dumpFullScreenCallbacks(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->dumpFullScreenCallbacks();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::dumpConfigurationForViewport(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    double deviceDPI = argumentCount > 0 ? JSValueToNumber(context, arguments[0], 0) : 0;
    double deviceWidth = argumentCount > 1 ? JSValueToNumber(context, arguments[1], 0) : 0;
    double deviceHeight = argumentCount > 2 ? JSValueToNumber(context, arguments[2], 0) : 0;
    double availableWidth = argumentCount > 3 ? JSValueToNumber(context, arguments[3], 0) : 0;
    double availableHeight = argumentCount > 4 ? JSValueToNumber(context, arguments[4], 0) : 0;
    impl->dumpConfigurationForViewport(deviceDPI, deviceWidth, deviceHeight, availableWidth, availableHeight);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::keepWebHistory(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->keepWebHistory();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setAcceptsEditing(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setAcceptsEditing(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setCanOpenWindows(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setCanOpenWindows(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setCloseRemainingWindowsWhenComplete(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setCloseRemainingWindowsWhenComplete(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setXSSAuditorEnabled(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setXSSAuditorEnabled(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setAllowUniversalAccessFromFileURLs(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setAllowUniversalAccessFromFileURLs(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setAllowFileAccessFromFileURLs(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setAllowFileAccessFromFileURLs(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setFrameFlatteningEnabled(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setFrameFlatteningEnabled(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setGeolocationPermission(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setGeolocationPermission(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setJavaScriptCanAccessClipboard(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setJavaScriptCanAccessClipboard(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setPrivateBrowsingEnabled(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setPrivateBrowsingEnabled(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setPopupBlockingEnabled(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setPopupBlockingEnabled(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setAuthorAndUserStylesEnabled(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool value = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setAuthorAndUserStylesEnabled(value);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::addOriginAccessWhitelistEntry(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> sourceOrigin = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    JSRetainPtr<JSStringRef> destinationProtocol = argumentCount > 1 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[1], 0)) : JSRetainPtr<JSStringRef>();
    JSRetainPtr<JSStringRef> destinationHost = argumentCount > 2 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[2], 0)) : JSRetainPtr<JSStringRef>();
    bool allowDestinationSubdomains = argumentCount > 3 && JSValueToBoolean(context, arguments[3]);
    impl->addOriginAccessWhitelistEntry(sourceOrigin.get(), destinationProtocol.get(), destinationHost.get(), allowDestinationSubdomains);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::removeOriginAccessWhitelistEntry(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> sourceOrigin = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    JSRetainPtr<JSStringRef> destinationProtocol = argumentCount > 1 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[1], 0)) : JSRetainPtr<JSStringRef>();
    JSRetainPtr<JSStringRef> destinationHost = argumentCount > 2 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[2], 0)) : JSRetainPtr<JSStringRef>();
    bool allowDestinationSubdomains = argumentCount > 3 && JSValueToBoolean(context, arguments[3]);
    impl->removeOriginAccessWhitelistEntry(sourceOrigin.get(), destinationProtocol.get(), destinationHost.get(), allowDestinationSubdomains);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::clearBackForwardList(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->clearBackForwardList();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::computedStyleIncludingVisitedInfo(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSValueRef element = argumentCount > 0 ? arguments[0] : JSValueRef();
    return impl->computedStyleIncludingVisitedInfo(element);
}

JSValueRef JSLayoutTestController::counterValueForElementById(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> elementId = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    return JSValueMakeStringOrNull(context, impl->counterValueForElementById(elementId.get()).get());
}

JSValueRef JSLayoutTestController::execCommand(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> name = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    JSRetainPtr<JSStringRef> argument = argumentCount > 1 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[1], 0)) : JSRetainPtr<JSStringRef>();
    impl->execCommand(name.get(), argument.get());

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::isCommandEnabled(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> name = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    return JSValueMakeBoolean(context, impl->isCommandEnabled(name.get()));
}

JSValueRef JSLayoutTestController::markerTextForListItem(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSValueRef element = argumentCount > 0 ? arguments[0] : JSValueRef();
    return JSValueMakeStringOrNull(context, impl->markerTextForListItem(element).get());
}

JSValueRef JSLayoutTestController::windowCount(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    return JSValueMakeNumber(context, impl->windowCount());
}

JSValueRef JSLayoutTestController::testRepaint(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->testRepaint();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::repaintSweepHorizontally(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->repaintSweepHorizontally();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::display(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->display();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::numberOfPages(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    double pageWidthInPixels = argumentCount > 0 ? JSValueToNumber(context, arguments[0], 0) : 0;
    double pageHeightInPixels = argumentCount > 1 ? JSValueToNumber(context, arguments[1], 0) : 0;
    return JSValueMakeNumber(context, impl->numberOfPages(pageWidthInPixels, pageHeightInPixels));
}

JSValueRef JSLayoutTestController::pageNumberForElementById(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> id = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    double pageWidthInPixels = argumentCount > 1 ? JSValueToNumber(context, arguments[1], 0) : 0;
    double pageHeightInPixels = argumentCount > 2 ? JSValueToNumber(context, arguments[2], 0) : 0;
    return JSValueMakeNumber(context, impl->pageNumberForElementById(id.get(), pageWidthInPixels, pageHeightInPixels));
}

JSValueRef JSLayoutTestController::pageSizeAndMarginsInPixels(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    double pageIndex = argumentCount > 0 ? JSValueToNumber(context, arguments[0], 0) : 0;
    double width = argumentCount > 1 ? JSValueToNumber(context, arguments[1], 0) : 0;
    double height = argumentCount > 2 ? JSValueToNumber(context, arguments[2], 0) : 0;
    double marginTop = argumentCount > 3 ? JSValueToNumber(context, arguments[3], 0) : 0;
    double marginRight = argumentCount > 4 ? JSValueToNumber(context, arguments[4], 0) : 0;
    double marginBottom = argumentCount > 5 ? JSValueToNumber(context, arguments[5], 0) : 0;
    double marginLeft = argumentCount > 6 ? JSValueToNumber(context, arguments[6], 0) : 0;
    return JSValueMakeStringOrNull(context, impl->pageSizeAndMarginsInPixels(pageIndex, width, height, marginTop, marginRight, marginBottom, marginLeft).get());
}

JSValueRef JSLayoutTestController::isPageBoxVisible(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    double pageIndex = argumentCount > 0 ? JSValueToNumber(context, arguments[0], 0) : 0;
    return JSValueMakeBoolean(context, impl->isPageBoxVisible(pageIndex));
}

JSValueRef JSLayoutTestController::setValueForUser(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSValueRef element = argumentCount > 0 ? arguments[0] : JSValueRef();
    JSRetainPtr<JSStringRef> value = argumentCount > 1 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[1], 0)) : JSRetainPtr<JSStringRef>();
    impl->setValueForUser(context, element, value.get());

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::numberOfActiveAnimations(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    return JSValueMakeNumber(context, impl->numberOfActiveAnimations());
}

JSValueRef JSLayoutTestController::pauseAnimationAtTimeOnElementWithId(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> animationName = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    double time = argumentCount > 1 ? JSValueToNumber(context, arguments[1], 0) : 0;
    JSRetainPtr<JSStringRef> elementId = argumentCount > 2 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[2], 0)) : JSRetainPtr<JSStringRef>();
    return JSValueMakeBoolean(context, impl->pauseAnimationAtTimeOnElementWithId(animationName.get(), time, elementId.get()));
}

JSValueRef JSLayoutTestController::pauseTransitionAtTimeOnElementWithId(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> propertyName = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    double time = argumentCount > 1 ? JSValueToNumber(context, arguments[1], 0) : 0;
    JSRetainPtr<JSStringRef> elementId = argumentCount > 2 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[2], 0)) : JSRetainPtr<JSStringRef>();
    return JSValueMakeBoolean(context, impl->pauseTransitionAtTimeOnElementWithId(propertyName.get(), time, elementId.get()));
}

JSValueRef JSLayoutTestController::suspendAnimations(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->suspendAnimations();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::resumeAnimations(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->resumeAnimations();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::addUserScript(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> source = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    bool runAtStart = argumentCount > 1 && JSValueToBoolean(context, arguments[1]);
    bool allFrames = argumentCount > 2 && JSValueToBoolean(context, arguments[2]);
    impl->addUserScript(source.get(), runAtStart, allFrames);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::addUserStyleSheet(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> source = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    bool allFrames = argumentCount > 1 && JSValueToBoolean(context, arguments[1]);
    impl->addUserStyleSheet(source.get(), allFrames);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::clearAllDatabases(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->clearAllDatabases();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setDatabaseQuota(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    double quota = argumentCount > 0 ? JSValueToNumber(context, arguments[0], 0) : 0;
    impl->setDatabaseQuota(quota);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::pathToLocalResource(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> url = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    return JSValueMakeStringOrNull(context, impl->pathToLocalResource(url.get()).get());
}

JSValueRef JSLayoutTestController::clearAllApplicationCaches(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->clearAllApplicationCaches();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setAppCacheMaximumSize(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    double size = argumentCount > 0 ? JSValueToNumber(context, arguments[0], 0) : 0;
    impl->setAppCacheMaximumSize(size);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::layerTreeAsText(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    return JSValueMakeStringOrNull(context, impl->layerTreeAsText().get());
}

JSValueRef JSLayoutTestController::findString(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> target = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    JSValueRef optionsArray = argumentCount > 1 ? arguments[1] : JSValueRef();
    return JSValueMakeBoolean(context, impl->findString(target.get(), optionsArray));
}

JSValueRef JSLayoutTestController::evaluateScriptInIsolatedWorld(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    double worldID = argumentCount > 0 ? JSValueToNumber(context, arguments[0], 0) : 0;
    JSRetainPtr<JSStringRef> script = argumentCount > 1 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[1], 0)) : JSRetainPtr<JSStringRef>();
    impl->evaluateScriptInIsolatedWorld(context, worldID, script.get());

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::showWebInspector(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->showWebInspector();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::closeWebInspector(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    impl->closeWebInspector();

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::evaluateInWebInspector(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    double callID = argumentCount > 0 ? JSValueToNumber(context, arguments[0], 0) : 0;
    JSRetainPtr<JSStringRef> script = argumentCount > 1 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[1], 0)) : JSRetainPtr<JSStringRef>();
    impl->evaluateInWebInspector(callID, script.get());

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setPOSIXLocale(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> locale = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    impl->setPOSIXLocale(locale.get());

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setTextDirection(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> direction = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    impl->setTextDirection(direction.get());

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setWillSendRequestReturnsNull(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool flag = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setWillSendRequestReturnsNull(flag);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setShouldStayOnPageAfterHandlingBeforeUnload(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool flag = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setShouldStayOnPageAfterHandlingBeforeUnload(flag);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::addChromeInputField(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSValueRef callback = argumentCount > 0 ? arguments[0] : JSValueRef();
    impl->addChromeInputField(callback);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::removeChromeInputField(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSValueRef callback = argumentCount > 0 ? arguments[0] : JSValueRef();
    impl->removeChromeInputField(callback);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::focusWebView(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSValueRef callback = argumentCount > 0 ? arguments[0] : JSValueRef();
    impl->focusWebView(callback);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setBackingScaleFactor(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    double backingScaleFactor = argumentCount > 0 ? JSValueToNumber(context, arguments[0], 0) : 0;
    JSValueRef callback = argumentCount > 1 ? arguments[1] : JSValueRef();
    impl->setBackingScaleFactor(backingScaleFactor, callback);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::setWindowIsKey(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    bool isKey = argumentCount > 0 && JSValueToBoolean(context, arguments[0]);
    impl->setWindowIsKey(isKey);

    return JSValueMakeUndefined(context);
}

JSValueRef JSLayoutTestController::overridePreference(JSContextRef context, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, thisObject);
    if (!impl)
        return JSValueMakeUndefined(context);

    JSRetainPtr<JSStringRef> preference = argumentCount > 0 ? JSRetainPtr<JSStringRef>(Adopt, JSValueToStringCopy(context, arguments[0], 0)) : JSRetainPtr<JSStringRef>();
    bool value = argumentCount > 1 && JSValueToBoolean(context, arguments[1]);
    impl->overridePreference(preference.get(), value);

    return JSValueMakeUndefined(context);
}

// Attributes

JSValueRef JSLayoutTestController::globalFlag(JSContextRef context, JSObjectRef object, JSStringRef, JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, object);
    if (!impl)
        return JSValueMakeUndefined(context);

    return JSValueMakeBoolean(context, impl->globalFlag());
}

bool JSLayoutTestController::setGlobalFlag(JSContextRef context, JSObjectRef object, JSStringRef, JSValueRef value, JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, object);
    if (!impl)
        return false;

    impl->setGlobalFlag(JSValueToBoolean(context, value));

    return true;
}

JSValueRef JSLayoutTestController::platformName(JSContextRef context, JSObjectRef object, JSStringRef, JSValueRef* exception)
{
    LayoutTestController* impl = toLayoutTestController(context, object);
    if (!impl)
        return JSValueMakeUndefined(context);

    return JSValueMakeStringOrNull(context, impl->platformName().get());
}

} // namespace WTR

