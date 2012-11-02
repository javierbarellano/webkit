/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
 * Copyright (C) 2011 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(VIDEO_TRACK)

#include "AudioTrack.h"

#include "Event.h"
#include "ExceptionCode.h"
#include "HTMLMediaElement.h"
#include "AudioTrackList.h"

namespace WebCore {

static const int invalidTrackIndex = -1;

const AtomicString& AudioTrack::alternativeKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("subtitles", AtomicString::ConstructFromLiteral));
    return subtitles;
}

const AtomicString& AudioTrack::captionsKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("captions", AtomicString::ConstructFromLiteral));
    return subtitles;
}

const AtomicString& AudioTrack::mainKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("main", AtomicString::ConstructFromLiteral));
    return subtitles;
}

const AtomicString& AudioTrack::signKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("sign", AtomicString::ConstructFromLiteral));
    return subtitles;
}

const AtomicString& AudioTrack::subtitlesKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("subtitles", AtomicString::ConstructFromLiteral));
    return subtitles;
}

const AtomicString& AudioTrack::commentaryKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("commentary", AtomicString::ConstructFromLiteral));
    return subtitles;
}

AudioTrack::AudioTrack(ScriptExecutionContext* context, AudioTrackClient* client, int index, bool enabled, const String& id, const String& kind, const String& label, const String& language)
    : TrackBase(context, TrackBase::AudioTrack)
    , m_mediaElement(0)
    , m_id(id)
    , m_kind(kind)
    , m_label(label)
    , m_language(language)
    , m_enabled(enabled)
    , m_client(client)
    , m_readinessState(NotLoaded)
    , m_trackIndex(index)
{
}

AudioTrack::~AudioTrack()
{
    clearClient();
}

bool AudioTrack::isValidKindKeyword(const String& value)
{
    if (equalIgnoringCase(value, alternativeKeyword()))
        return true;
    if (equalIgnoringCase(value, captionsKeyword()))
        return true;
    if (equalIgnoringCase(value, mainKeyword()))
        return true;
    if (equalIgnoringCase(value, signKeyword()))
        return true;
    if (equalIgnoringCase(value, subtitlesKeyword()))
        return true;
    if (equalIgnoringCase(value, commentaryKeyword()))
        return true;

    return false;
}

void AudioTrack::setId(const String& id)
{
    m_id = id;
}

void AudioTrack::setKind(const String& kind)
{
    String oldKind = m_kind;

    if (isValidKindKeyword(kind))
        m_kind = kind;
    else
        m_kind = "";

    //if (m_client && oldKind != m_kind)
    //    m_client->audioTrackKindChanged(this);
}

int AudioTrack::trackIndex()
{
    return m_trackIndex;
}

void AudioTrack::setEnabled(bool enabled) {
    if(enabled == m_enabled) return;

    // Tell media player which track was selected
    if(m_client)
        m_client->audioTrackEnabled(this, enabled);

    if(!enabled) {
        m_enabled = false;
        return;
    }

    // 4.8.10.10.1
    // If the track is in a AudioTrackList, then all the other AudioTrack
    // objects in that list must be unselected. (If the track is no longer in
    // a AudioTrackList object, then the track being selected or unselected
    //has no effect beyond changing the value of the attribute on the
    // AudioTrack object.)
    if(m_mediaElement) {
        AudioTrackList *list = m_mediaElement->audioTracks();
        // TODO: Detect when we're not in the list
        for(unsigned i = 0; i < list->length(); ++i) {
            AudioTrack* item = list->item(i);
            if(item != this && item->enabled()) {
                item->setEnabled(false);

                // There can only be on selected track
                break;
            }
        }
    }
    m_enabled = true;
}

} // namespace WebCore
#endif
