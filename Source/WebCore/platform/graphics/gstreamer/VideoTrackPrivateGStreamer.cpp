/*
 * Copyright (C) 2013 Cable Television Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(VIDEO) && USE(GSTREAMER) && ENABLE(VIDEO_TRACK)

#include "VideoTrackPrivateGStreamer.h"
#include "VideoTrack.h"

#include <glib-object.h>

namespace WebCore {

VideoTrackPrivateGStreamer::VideoTrackPrivateGStreamer(GRefPtr<GstElement> playbin, gint index, GRefPtr<GstPad> pad)
    : TrackPrivateBaseGStreamer(this, index, pad)
    , m_playbin(playbin)
    , m_kind(None)
{
    notifyTrackOfActiveChanged();
}

void VideoTrackPrivateGStreamer::disconnect()
{
    m_playbin.clear();
    TrackPrivateBaseGStreamer::disconnect();
}

void VideoTrackPrivateGStreamer::setSelected(bool selected)
{
    if (selected == this->selected())
        return;
    VideoTrackPrivate::setSelected(selected);

    if (selected && m_playbin)
        g_object_set(m_playbin.get(), "current-video", m_index, NULL);
}

void VideoTrackPrivateGStreamer::kindChanged()
{
    Kind kind;

    if (m_kindKeyword == VideoTrack::alternativeKeyword())
        kind = Alternative;
    else if (m_kindKeyword == VideoTrack::captionsKeyword())
        kind = Captions;
    else if (m_kindKeyword == VideoTrack::mainKeyword())
        kind = Main;
    else if (m_kindKeyword == VideoTrack::signKeyword())
        kind = Sign;
    else if (m_kindKeyword == VideoTrack::subtitlesKeyword())
        kind = Subtitles;
    else if (m_kindKeyword == VideoTrack::commentaryKeyword())
        kind = Commentary;
    else
        kind = None;

    setKind(kind);
}

void VideoTrackPrivateGStreamer::setKind(Kind kind)
{
    if (m_kind == kind)
        return;

    m_kind = kind;
    if (client())
        client()->kindChanged(this);
}

} // namespace WebCore

#endif // ENABLE(VIDEO) && USE(GSTREAMER) && ENABLE(VIDEO_TRACK)
