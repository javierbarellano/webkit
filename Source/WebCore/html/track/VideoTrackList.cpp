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

#include "VideoTrackList.h"

#include "EventNames.h"
#include "HTMLMediaElement.h"
#include "HTMLNames.h"
#include "ScriptExecutionContext.h"
#include "VideoTrack.h"
#include "TrackEvent.h"

using namespace WebCore;

static size_t videoTrackElementIndex(VideoTrack *track)
{
	HTMLMediaElement *m_trackElement = track->mediaElement();
    ASSERT(m_trackElement);
    ASSERT(m_trackElement->parentNode());

    size_t index = 0;
    for (Node* node = m_trackElement->parentNode()->firstChild(); node; node = node->nextSibling()) {
        if (!node->hasTagName(HTMLNames::sourceTag) || !node->inDocument())
            continue;
        if (node == m_trackElement)
            return index;
        ++index;
    }
    ASSERT_NOT_REACHED();

    return 0;
}

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
    return m_addTrackTracks.size() + m_elementTracks.size();
}

unsigned VideoTrackList::getTrackIndex(VideoTrack *VideoTrack)
{
    if (VideoTrack->trackType() == VideoTrack::TrackElement)
        return videoTrackElementIndex(VideoTrack);

    if (VideoTrack->trackType() == VideoTrack::AddTrack)
        return m_elementTracks.size() + m_addTrackTracks.find(VideoTrack);

    if (VideoTrack->trackType() == VideoTrack::InBand)
        return m_elementTracks.size() + m_addTrackTracks.size() + m_inbandTracks.find(VideoTrack);

    ASSERT_NOT_REACHED();

    return -1;
}

VideoTrack* VideoTrackList::item(unsigned index)
{
    // 4.8.10.12.1 Text track model
    // The text tracks are sorted as follows:
    // 1. The text tracks corresponding to track element children of the media element, in tree order.
    // 2. Any text tracks added using the addVideoTrack() method, in the order they were added, oldest first.
    // 3. Any media-resource-specific text tracks (text tracks corresponding to data in the media
    // resource), in the order defined by the media resource's format specification.

    if (index < m_elementTracks.size())
        return m_elementTracks[index].get();

    index -= m_elementTracks.size();
    if (index < m_addTrackTracks.size())
        return m_addTrackTracks[index].get();

    index -= m_addTrackTracks.size();
    if (index < m_inbandTracks.size())
        return m_inbandTracks[index].get();

    return 0;
}

void VideoTrackList::append(PassRefPtr<VideoTrack> prpTrack)
{
    RefPtr<VideoTrack> track = prpTrack;

    if (track->trackType() == VideoTrack::AddTrack)
        m_addTrackTracks.append(track);
    else if (track->trackType() == VideoTrack::TrackElement) {
        // Insert tracks added for <track> element in tree order.
        size_t index = videoTrackElementIndex(track.get());
        m_elementTracks.insert(index, track);

        // Invalidate the cached index for all the following tracks.
        for (size_t i = index; i < m_elementTracks.size(); ++i)
            m_elementTracks[i]->invalidateTrackIndex();

        for (size_t i = 0; i < m_addTrackTracks.size(); ++i)
            m_addTrackTracks[i]->invalidateTrackIndex();

    } else if (track->trackType() == VideoTrack::InBand) {
    	m_inbandTracks.append(track);
    } else
        ASSERT_NOT_REACHED();

    ASSERT(!track->mediaElement() || track->mediaElement() == m_owner);
    track->setMediaElement(m_owner);

    scheduleAddTrackEvent(track.release());
}

void VideoTrackList::remove(VideoTrack* track)
{
    Vector<RefPtr<VideoTrack> >* tracks = 0;

    if (track->trackType() == VideoTrack::TrackElement)
        tracks = &m_elementTracks;
    else if (track->trackType() == VideoTrack::AddTrack)
        tracks = &m_addTrackTracks;
    else if (track->trackType() == VideoTrack::InBand)
        tracks = &m_inbandTracks;
    else
        ASSERT_NOT_REACHED();

    size_t index = tracks->find(track);
    if (index == notFound)
        return;

    ASSERT(track->mediaElement() == m_owner);
    track->setMediaElement(0);

    tracks->remove(index);
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

    RefPtr<VideoTrack> trackRef = track;
    TrackEventInit initializer;
    initializer.track = trackRef;
    initializer.bubbles = false;
    initializer.cancelable = false;

    m_pendingEvents.append(TrackEvent::create(eventNames().addtrackEvent, initializer));
    if (!m_pendingEventTimer.isActive())
        m_pendingEventTimer.startOneShot(0);
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

#endif
