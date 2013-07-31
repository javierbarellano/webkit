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

#include <glib-object.h>
#include <gst/gsttaglist.h>

namespace WebCore {

static void videoTrackPrivateActiveChangedCallback(GObject*, GParamSpec*, VideoTrackPrivateGStreamer* track)
{
    track->activeChanged();
}

static void videoTrackPrivateTagsChangedCallback(GObject*, GParamSpec*, VideoTrackPrivateGStreamer* track)
{
    track->tagsChanged();
}

static gboolean videoTrackPrivateActiveChangeTimeoutCallback(VideoTrackPrivateGStreamer* track)
{
    track->notifyTrackOfActiveChanged();
    return FALSE;
}

static gboolean videoTrackPrivateTagsChangeTimeoutCallback(VideoTrackPrivateGStreamer* track)
{
    track->notifyTrackOfTagsChanged();
    return FALSE;
}

VideoTrackPrivateGStreamer::VideoTrackPrivateGStreamer(GRefPtr<GstElement> playbin, gint index, GRefPtr<GstPad> pad)
    : m_index(index)
    , m_pad(pad)
    , m_playbin(playbin)
    , m_isDisconnected(false)
    , m_activeTimerHandler(0)
    , m_tagTimerHandler(0)
{
    g_signal_connect(m_playbin.get(), "notify::current-video", G_CALLBACK(videoTrackPrivateActiveChangedCallback), this);
    g_signal_connect(m_pad.get(), "notify::tags", G_CALLBACK(videoTrackPrivateTagsChangedCallback), this);

    gboolean active;
    g_object_get(m_pad.get(), "active", &active, NULL);
    VideoTrackPrivate::setSelected(active);
}

VideoTrackPrivateGStreamer::~VideoTrackPrivateGStreamer()
{
    disconnect();
}

void VideoTrackPrivateGStreamer::disconnect()
{
    if (m_isDisconnected)
        return;

    m_isDisconnected = true;

    g_signal_handlers_disconnect_by_func(m_pad.get(),
        reinterpret_cast<gpointer>(videoTrackPrivateActiveChangedCallback), this);
    g_signal_handlers_disconnect_by_func(m_pad.get(),
        reinterpret_cast<gpointer>(videoTrackPrivateTagsChangedCallback), this);

    if (m_activeTimerHandler)
        g_source_remove(m_activeTimerHandler);

    if (m_tagTimerHandler)
        g_source_remove(m_tagTimerHandler);

    m_pad.clear();
    m_playbin.clear();
}

void VideoTrackPrivateGStreamer::activeChanged()
{
    if (m_activeTimerHandler)
        g_source_remove(m_activeTimerHandler);
    m_activeTimerHandler = g_timeout_add(0,
        reinterpret_cast<GSourceFunc>(videoTrackPrivateActiveChangeTimeoutCallback), this);
}

void VideoTrackPrivateGStreamer::tagsChanged()
{
    if (m_tagTimerHandler)
        g_source_remove(m_tagTimerHandler);
    m_tagTimerHandler = g_timeout_add(0,
        reinterpret_cast<GSourceFunc>(videoTrackPrivateTagsChangeTimeoutCallback), this);
}

void VideoTrackPrivateGStreamer::notifyTrackOfActiveChanged()
{
    if (m_isDisconnected)
        return;

    gboolean active = false;
    if (m_pad)
        g_object_get(m_pad.get(), "active", &active, NULL);

    if (active == selected())
        return;

    VideoTrackPrivate::setSelected(active);
    client()->videoTrackPrivateSelectedChanged(this);
}

void VideoTrackPrivateGStreamer::notifyTrackOfTagsChanged()
{
    if (m_isDisconnected)
        return;

    GstTagList* tags = 0;
    g_object_get(m_pad.get(), "tags", &tags, NULL);

    if (!tags)
        return;

    gchar* tagValue;
    if (gst_tag_list_get_string(tags, GST_TAG_TITLE, &tagValue)) {
        m_label = tagValue;
        g_free(tagValue);
        client()->videoTrackPrivateLabelChanged(this);
    }
    if (gst_tag_list_get_string(tags, GST_TAG_LANGUAGE_CODE, &tagValue)) {
        m_language = tagValue;
        g_free(tagValue);
        client()->videoTrackPrivateLanguageChanged(this);
    }
#ifdef GST_API_VERSION_1
    gst_tag_list_unref(tags);
#else
    gst_tag_list_free(tags);
#endif
}

void VideoTrackPrivateGStreamer::setSelected(bool selected)
{
    if (selected == this->selected())
        return;
    VideoTrackPrivate::setSelected(selected);

    if (selected && m_playbin)
        g_object_set(m_playbin.get(), "current-video", m_index, NULL);
}

} // namespace WebCore

#endif // ENABLE(VIDEO) && USE(GSTREAMER) && ENABLE(VIDEO_TRACK)
