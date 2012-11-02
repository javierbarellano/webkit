/*
 * Copyright (C) 2011 Apple Inc.  All rights reserved.
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

#ifndef VideoTrackList_h
#define VideoTrackList_h

#if ENABLE(VIDEO_TRACK)

#include "EventListener.h"
#include "EventTarget.h"
#include "Timer.h"
#include <algorithm>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>

namespace WebCore {

class HTMLMediaElement;
class VideoTrack;

class VideoTrackList : public RefCounted<VideoTrackList>, public EventTarget {
public:
    static PassRefPtr<VideoTrackList> create(HTMLMediaElement* owner, ScriptExecutionContext* context)
    {
        return adoptRef(new VideoTrackList(owner, context));
    }
    ~VideoTrackList();

    unsigned length() const;
    unsigned getTrackIndex(VideoTrack*);

    VideoTrack* item(unsigned index);
    void append(PassRefPtr<VideoTrack>);
    void remove(VideoTrack*);
    void clear();

    // EventTarget
    virtual const AtomicString& interfaceName() const;
    using RefCounted<VideoTrackList>::ref;
    using RefCounted<VideoTrackList>::deref;
    virtual ScriptExecutionContext* scriptExecutionContext() const { return m_context; }

    DEFINE_ATTRIBUTE_EVENT_LISTENER(addtrack);

    void clearOwner() { m_owner = 0; }
    Node* owner() const;
    
    bool isFiringEventListeners() { return m_dispatchingEvents; }

private:
    VideoTrackList(HTMLMediaElement*, ScriptExecutionContext*);

    // EventTarget
    virtual void refEventTarget() { ref(); }
    virtual void derefEventTarget() { deref(); }
    virtual EventTargetData* eventTargetData() { return &m_eventTargetData; }
    virtual EventTargetData* ensureEventTargetData() { return &m_eventTargetData; }

    void scheduleAddTrackEvent(PassRefPtr<VideoTrack>);
    void asyncEventTimerFired(Timer<VideoTrackList>*);

    ScriptExecutionContext* m_context;
    HTMLMediaElement* m_owner;

    Vector<RefPtr<Event> > m_pendingEvents;
    Timer<VideoTrackList> m_pendingEventTimer;

    EventTargetData m_eventTargetData;
    Vector<RefPtr<VideoTrack> > m_tracks;
    
    int m_dispatchingEvents;
};

} // namespace WebCore

#endif
#endif
