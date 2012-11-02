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

#include "VideoTrack.h"

#include "Event.h"
#include "ExceptionCode.h"
#include "HTMLMediaElement.h"
#include "VideoTrackList.h"

namespace WebCore {

static const int invalidTrackIndex = -1;

const AtomicString& VideoTrack::alternativeKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("subtitles", AtomicString::ConstructFromLiteral));
    return subtitles;
}

const AtomicString& VideoTrack::captionsKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("captions", AtomicString::ConstructFromLiteral));
    return subtitles;
}

const AtomicString& VideoTrack::mainKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("main", AtomicString::ConstructFromLiteral));
    return subtitles;
}

const AtomicString& VideoTrack::signKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("sign", AtomicString::ConstructFromLiteral));
    return subtitles;
}

const AtomicString& VideoTrack::subtitlesKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("subtitles", AtomicString::ConstructFromLiteral));
    return subtitles;
}

const AtomicString& VideoTrack::commentaryKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, subtitles, ("commentary", AtomicString::ConstructFromLiteral));
    return subtitles;
}

VideoTrack::VideoTrack(ScriptExecutionContext* context, VideoTrackClient* client, int index, bool selected, const String& id, const String& kind, const String& label, const String& language)
    : TrackBase(context, TrackBase::VideoTrack)
    , m_mediaElement(0)
    , m_id(id)
    , m_kind(kind)
    , m_label(label)
    , m_language(language)
    , m_selected(selected)
    , m_client(client)
    , m_readinessState(NotLoaded)
    , m_trackIndex(index)
{
}

VideoTrack::~VideoTrack()
{
    clearClient();
}

bool VideoTrack::isValidKindKeyword(const String& value)
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

void VideoTrack::setId(const String& id)
{
    m_id = id;
}

void VideoTrack::setKind(const String& kind)
{
    String oldKind = m_kind;

    if (isValidKindKeyword(kind))
        m_kind = kind;
    else
        m_kind = "";

    //if (m_client && oldKind != m_kind)
    //    m_client->videoTrackKindChanged(this);
}

int VideoTrack::trackIndex()
{
    return m_trackIndex;
}

void VideoTrack::setSelected(bool selected) {
    if(selected == m_selected) return;

    // Tell media player which track was selected
    if(m_client)
        m_client->videoTrackSelected(this, selected);

    if(!selected) {
        m_selected = false;
        return;
    }

    // 4.8.10.10.1
    // If the track is in a VideoTrackList, then all the other VideoTrack
    // objects in that list must be unselected. (If the track is no longer in
    // a VideoTrackList object, then the track being selected or unselected
    //has no effect beyond changing the value of the attribute on the
    // VideoTrack object.)
    if(m_mediaElement) {
        VideoTrackList *list = m_mediaElement->videoTracks();
        // TODO: Detect when we're not in the list
        for(unsigned i = 0; i < list->length(); ++i) {
            VideoTrack* item = list->item(i);
            if(item != this && item->selected()) {
                item->setSelected(false);

                // There can only be on selected track
                break;
            }
        }
    }
    m_selected = true;
}

} // namespace WebCore
#endif
