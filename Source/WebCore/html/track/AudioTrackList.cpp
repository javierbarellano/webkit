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

#include "AudioTrackList.h"

#include "EventNames.h"
#include "HTMLMediaElement.h"
#include "HTMLNames.h"
#include "ScriptExecutionContext.h"
#include "AudioTrack.h"
#include "TrackEvent.h"

using namespace WebCore;

AudioTrackList::AudioTrackList(HTMLMediaElement* owner, ScriptExecutionContext* context)
    : m_context(context)
    , m_owner(owner)
    , m_pendingEventTimer(this, &AudioTrackList::asyncEventTimerFired)
    , m_dispatchingEvents(0)
{
    ASSERT(context->isDocument());
}

AudioTrackList::~AudioTrackList()
{
}

unsigned AudioTrackList::length() const
{
    return m_tracks.size();
}

unsigned AudioTrackList::getTrackIndex(AudioTrack* track) {
    return m_tracks.find(track);
}

AudioTrack* AudioTrackList::item(unsigned index)
{
    if (index < m_tracks.size())
        return m_tracks[index].get();

    return 0;
}

void AudioTrackList::append(PassRefPtr<AudioTrack> prpTrack)
{
    RefPtr<AudioTrack> track = prpTrack;

    // Insert tracks added for <track> element in tree order.
    //size_t index = track->trackIndex();
    m_tracks.append(track); // TODO: Use order correctly

    printf("AudioTrackList::append(%d) len: %d\n", (int)track->trackIndex(), m_tracks.size());
    ASSERT(!track->mediaElement() || track->mediaElement() == m_owner);
    track->setMediaElement(m_owner);

    scheduleAddTrackEvent(track.release());
}

void AudioTrackList::remove(AudioTrack* track)
{
    size_t index = m_tracks.find(track);
    if (index == notFound)
        return;

    ASSERT(track->mediaElement() == m_owner);
    track->setMediaElement(0);

    m_tracks.remove(index);
}

void AudioTrackList::clear()
{
	//printf("AudioTrackList::clear()\n");
    //m_tracks.clear();
}

const AtomicString& AudioTrackList::interfaceName() const
{
    return eventNames().interfaceForAudioTrackList;
}

void AudioTrackList::scheduleAddTrackEvent(PassRefPtr<AudioTrack> track)
{
    // 4.8.10.12.3 Sourcing out-of-band text tracks
    // 4.8.10.12.3 Sourcing out-of-band text tracks
    // 4.8.10.12.4 Text track API
    // ... then queue a task to fire an event with the name addtrack, that does not 
    // bubble and is not cancelable, and that uses the TrackEvent interface, with 
    // the track attribute initialized to the text track's AudioTrack object, at
    // the media element's AudioTracks attribute's AudioTrackList object.

    RefPtr<AudioTrack> trackRef = track;
    TrackEventInit initializer;
    initializer.track = trackRef;
    initializer.bubbles = false;
    initializer.cancelable = false;

    m_pendingEvents.append(TrackEvent::create(eventNames().addtrackEvent, initializer));
    if (!m_pendingEventTimer.isActive())
        m_pendingEventTimer.startOneShot(0);
}

void AudioTrackList::asyncEventTimerFired(Timer<AudioTrackList>*)
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

Node* AudioTrackList::owner() const
{
    return m_owner;
}

#endif
