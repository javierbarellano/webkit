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

#include "config.h"

#if ENABLE(VIDEO_TRACK)

#include "EventNames.h"
#include "HTMLMediaElement.h"
#include "HTMLNames.h"
#include "ScriptExecutionContext.h"
#include "VideoTrack.h"
#include "TrackEvent.h"

#include "VideoTrackList.h"

namespace WebCore
{

VideoTrackList::VideoTrackList(HTMLMediaElement* owner, ScriptExecutionContext* context)
    : m_context(context)
    , m_owner(owner)
    , m_pendingEventTimer(this, &VideoTrackList::asyncEventTimerFired)
    , m_dispatchingEvents(0)
{
    ASSERT(context->isDocument());
}

VideoTrackList::~VideoTrackList()
{
}

unsigned VideoTrackList::length() const
{
    return m_tracks.size();
}

unsigned VideoTrackList::getTrackIndex(VideoTrack* track) {
    return m_tracks.find(track);
}

VideoTrack* VideoTrackList::item(unsigned index)
{
	RefPtr<VideoTrack> vt = itemRef(index);
	if (vt)
		return vt.get();

	return 0;
}

RefPtr<VideoTrack> VideoTrackList::itemRef(unsigned index)
{
    if (index < m_tracks.size())
        return m_tracks[index];

    return 0;
}

void VideoTrackList::selectTrack(unsigned index)
{
	m_trackSelected = itemRef(index);
	callOnMainThread(VideoTrackList::selectTrackEventOnContextThread,this);
}


void VideoTrackList::append(PassRefPtr<VideoTrack> prpTrack)
{
    RefPtr<VideoTrack> track = prpTrack;

    // Insert tracks added in the correct order
    int index = track->trackIndex();
    bool inserted = false;
    for(size_t i = 0; i < m_tracks.size(); ++i) {
        if(m_tracks[i]->trackIndex() > index) {
            m_tracks.insert(i, track);
            inserted = true;
            break;
        }
    }
    if(!inserted) {
        m_tracks.append(track);
    }

    ASSERT(!track->mediaElement() || track->mediaElement() == m_owner);
    track->setMediaElement(m_owner);

    scheduleAddTrackEvent(track.release());
}

void VideoTrackList::remove(VideoTrack* track)
{
    size_t index = m_tracks.find(track);
    if (index == notFound)
        return;

    ASSERT(track->mediaElement() == m_owner);
    track->setMediaElement(0);

    m_tracks.remove(index);
}

void VideoTrackList::clear()
{
    m_tracks.clear();
}

const AtomicString& VideoTrackList::interfaceName() const
{
    return eventNames().interfaceForVideoTrackList;
}

void VideoTrackList::scheduleAddTrackEvent(PassRefPtr<VideoTrack> track)
{
    // 4.8.10.12.3 Sourcing out-of-band text tracks
    // 4.8.10.12.3 Sourcing out-of-band text tracks
    // 4.8.10.12.4 Text track API
    // ... then queue a task to fire an event with the name addtrack, that does not 
    // bubble and is not cancelable, and that uses the TrackEvent interface, with 
    // the track attribute initialized to the text track's VideoTrack object, at
    // the media element's VideoTracks attribute's VideoTrackList object.

	m_trackAdded = track;
	callOnMainThread(VideoTrackList::addTrackEventOnContextThread,this);
}


// static
void VideoTrackList::addTrackEventOnContextThread(void* ptr)
{
	VideoTrackList *vtl = (VideoTrackList *)ptr;

    TrackEventInit initializer;
    initializer.track = vtl->m_trackAdded;
    initializer.bubbles = false;
    initializer.cancelable = false;

//    vtl->m_pendingEvents.append(TrackEvent::create(eventNames().addtrackEvent, initializer));
//    if (!vtl->m_pendingEventTimer.isActive())
//    	vtl->m_pendingEventTimer.startOneShot(0);
    vtl->dispatchEvent(TrackEvent::create(eventNames().addtrackEvent, initializer));
}

// static
void VideoTrackList::selectTrackEventOnContextThread(void* ptr)
{
	VideoTrackList *vtl = (VideoTrackList *)ptr;

    TrackEventInit initializer;
    initializer.track = vtl->m_trackSelected;
    initializer.bubbles = false;
    initializer.cancelable = false;

//    vtl->m_pendingEvents.append(TrackEvent::create(eventNames().addtrackEvent, initializer));
//    if (!vtl->m_pendingEventTimer.isActive())
//    	vtl->m_pendingEventTimer.startOneShot(0);
    vtl->dispatchEvent(TrackEvent::create(eventNames().trackselectedEvent, initializer));
}

void VideoTrackList::asyncEventTimerFired(Timer<VideoTrackList>*)
{
    Vector<RefPtr<Event> > pendingEvents;
    ExceptionCode ec = 0;
    
    ++m_dispatchingEvents;
    m_pendingEvents.swap(pendingEvents);
    size_t count = pendingEvents.size();
    for (size_t index = 0; index < count; ++index)
        dispatchEvent(pendingEvents[index].release(), ec);
    --m_dispatchingEvents;
}

Node* VideoTrackList::owner() const
{
    return m_owner;
}

} // namespace WebCore

#endif
