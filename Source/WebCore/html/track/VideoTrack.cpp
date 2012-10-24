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

VideoTrack::VideoTrack(ScriptExecutionContext* context, VideoTrackClient* client, const String& src, VideoTrackType type)
    : TrackBase(context, TrackBase::TextTrack)
    , m_mediaElement(0)
    , m_selected(false)
    , m_src(src)
    , m_client(client)
    , m_trackType(type)
    , m_readinessState(NotLoaded)
    , m_trackIndex(invalidTrackIndex)
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

void VideoTrack::setKind(const String& kind)
{
    String oldKind = m_kind;

    if (isValidKindKeyword(kind))
        m_kind = kind;
    else
        m_kind = "";

    if (m_client && oldKind != m_kind)
        m_client->videoTrackKindChanged(this);
}

int VideoTrack::trackIndex()
{
    ASSERT(m_mediaElement);

    if (m_trackIndex == invalidTrackIndex)
        m_trackIndex = m_mediaElement->videoTracks()->getTrackIndex(this);

    return m_trackIndex;
}

void VideoTrack::invalidateTrackIndex()
{
    m_trackIndex = invalidTrackIndex;
}

void VideoTrack::setSelected(bool selected) {
    if(!selected) {
        m_selected = false;
        return;
    }

    // Deselect all other videos on this media element
    VideoTrackList *list = m_mediaElement->videoTracks();
    for(unsigned i = 0; i < list->length(); ++i) {
        VideoTrack* item = list->item(i);
        if(item != this) {
            item->setSelected(false);
        }
    }
    // TODO: Actually switch the video playing

    m_selected = true;
    m_mediaElement->setSrc(m_src);
}

} // namespace WebCore
#endif
