/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"

#if ENABLE(VIDEO_TRACK)
#include "JSVideoTrackList.h"

#include "HTMLMediaElement.h"

using namespace JSC;

namespace WebCore {

bool JSVideoTrackListOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSVideoTrackList* jsVideoTrackList = jsCast<JSVideoTrackList*>(handle.get().asCell());
    VideoTrackList* videoTrackList = static_cast<VideoTrackList*>(jsVideoTrackList->impl());

    // If the list is firing event listeners, its wrapper is reachable because
    // the wrapper is responsible for marking those event listeners.
    if (videoTrackList->isFiringEventListeners())
        return true;

    // If the list has no event listeners and has no custom properties, it is not reachable.
    if (!videoTrackList->hasEventListeners() && !jsVideoTrackList->hasCustomProperties())
        return false;

    // It is reachable if the media element parent is reachable.
    //return visitor.containsOpaqueRoot(root(videoTrackList->owner()));
    return visitor.containsOpaqueRoot(videoTrackList->owner());
}

void JSVideoTrackList::visitChildren(JSCell* cell, SlotVisitor& visitor)
{
    JSVideoTrackList* jsVideoTrackList = jsCast<JSVideoTrackList*>(cell);
    ASSERT_GC_OBJECT_INHERITS(jsVideoTrackList, &s_info);
    COMPILE_ASSERT(StructureFlags & OverridesVisitChildren, OverridesVisitChildrenWithoutSettingFlag);
    ASSERT(jsVideoTrackList->structure()->typeInfo().overridesVisitChildren());
    Base::visitChildren(jsVideoTrackList, visitor);
    
    VideoTrackList* videoTrackList = static_cast<VideoTrackList*>(jsVideoTrackList->impl());
    //visitor.addOpaqueRoot(root(videoTrackList->owner()));
    visitor.addOpaqueRoot(videoTrackList->owner());
    videoTrackList->visitJSEventListeners(visitor);
}
    
} // namespace WebCore

#endif
