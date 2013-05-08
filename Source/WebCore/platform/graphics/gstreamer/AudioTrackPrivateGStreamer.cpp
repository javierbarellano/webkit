/*
 * Copyright (C) 2012, 2013 Apple Inc. All rights reserved.
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

#if ENABLE(VIDEO) && USE(GSTREAMER) && ENABLE(VIDEO_TRACK)

#include "AudioTrackPrivateGStreamer.h"

#include "GStreamerUtilities.h"
#include "Logging.h"
#include <gst/gst.h>

namespace WebCore {

static void audioTrackPrivateActivePadChangedCallback(GObject*, GParamSpec*, AudioTrackPrivateGStreamer* track)
{
    track->activePadChanged();
}

#ifdef GST_API_VERSION_1
static GstPadProbeReturn inbandTextTrackPrivateTextTagCallback(GstPad*, GstPadProbeInfo* info, AudioTrackPrivateGStreamer* track)
{
    track->handleTag(gst_pad_probe_info_get_event(info));
    return GST_PAD_PROBE_OK;
}
#else
static gboolean inbandTextTrackPrivateTextTagCallback(GstPad*, GstEvent* event, AudioTrackPrivateGStreamer* track)
{
    track->handleTag(event);
    return true;
}
#endif

static gboolean audioTrackPrivateActivePadChangedTimeoutCallback(AudioTrackPrivateGStreamer* track)
{
    track->notifyPlayerOfActivePad();
    return FALSE;
}

static gboolean inbandTextTrackPrivateTagTimeoutCallback(AudioTrackPrivateGStreamer* track)
{
    track->notifyPlayerOfTag();
    return FALSE;
}

AudioTrackPrivateGStreamer::AudioTrackPrivateGStreamer(GRefPtr<GstElement> adder, GstPad* pad)
    : m_srcPad(pad)
    , m_adder(adder)
    , m_activePadTimerHandler(0)
    , m_tagTimerHandler(0)
    , m_unlinkProbe(0)
#ifndef GST_API_VERSION_1
    , m_tags(0)
#endif
    , m_hasBeenReported(false)
    , m_isDisconnected(false)
{
    m_sinkPad = gst_element_get_request_pad(m_adder.get(), "sink_%u");
    ASSERT(m_sinkPad.get());
    GstPadLinkReturn ret = gst_pad_link(m_srcPad.get(), m_sinkPad.get());
    ASSERT(GST_PAD_LINK_SUCCESSFUL(ret));

    g_signal_connect(m_adder.get(), "notify::active-pad", G_CALLBACK (audioTrackPrivateActivePadChangedCallback), this);

#ifdef GST_API_VERSION_1
    m_tagProbe = gst_pad_add_probe(m_srcPad.get(), GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM,
        reinterpret_cast<GstPadProbeCallback>(inbandTextTrackPrivateTextTagCallback), this, 0);
#else
    m_tagProbe = gst_pad_add_event_probe(m_srcPad.get(), G_CALLBACK(inbandTextTrackPrivateTextTagCallback), this);
#endif
}

AudioTrackPrivateGStreamer::~AudioTrackPrivateGStreamer()
{
    disconnect();
#ifndef GST_API_VERSION_1
    MutexLocker lock(m_tagMutex);
    if (m_tags)
        gst_tag_list_free(m_tags);
#endif
}

void AudioTrackPrivateGStreamer::disconnect()
{
    if (m_isDisconnected)
        return;

    m_isDisconnected = true;

#ifdef GST_API_VERSION_1
    gst_pad_remove_probe(m_srcPad.get(), m_tagProbe);
#else
    gst_pad_remove_event_probe(m_srcPad.get(), m_tagProbe);
#endif
    m_tagProbe = 0;

    g_signal_handlers_disconnect_by_func(m_sinkPad.get(),
        reinterpret_cast<gpointer>(audioTrackPrivateActivePadChangedCallback), this);

    if (m_activePadTimerHandler)
        g_source_remove(m_activePadTimerHandler);

    if (m_tagTimerHandler)
        g_source_remove(m_tagTimerHandler);

    gst_pad_unlink(m_srcPad.get(), m_sinkPad.get());
    gst_element_release_request_pad(m_adder.get(), m_sinkPad.get());

    m_adder.clear();
    m_sinkPad.clear();
    m_srcPad.clear();
}

GstTagList* AudioTrackPrivateGStreamer::tags() const
{
#ifdef GST_API_VERSION_1
    return m_tags.get();
#else
    return m_tags;
#endif
}

void AudioTrackPrivateGStreamer::setEnabled(bool enabled)
{
    if (enabled == this->enabled())
        return;
    AudioTrackPrivate::setEnabled(enabled);

    GstPad* oldSinkpad;
    g_object_get(m_adder.get(), "active-pad", &oldSinkpad, NULL);

    if (enabled && oldSinkpad != m_sinkPad.get()) {
        g_object_set(m_adder.get(), "active-pad", m_sinkPad.get(), NULL);
    } else if (!enabled && oldSinkpad == m_sinkPad.get()) {
        g_object_set(m_adder.get(), "active-pad", NULL, NULL);
    }
}

void AudioTrackPrivateGStreamer::activePadChanged()
{
    if (m_activePadTimerHandler)
        g_source_remove(m_activePadTimerHandler);
    m_activePadTimerHandler = g_timeout_add(0, reinterpret_cast<GSourceFunc>(audioTrackPrivateActivePadChangedTimeoutCallback), this);
}

void AudioTrackPrivateGStreamer::notifyPlayerOfActivePad()
{
    m_activePadTimerHandler = 0;

    if(!m_sinkPad)
        return;

    gboolean active;
    g_object_get(m_sinkPad.get(), "active", &active, NULL);
    setEnabled(active);
}

void AudioTrackPrivateGStreamer::handleTag(GstEvent* event)
{
    if (event->type != GST_EVENT_TAG)
        return;

    if (m_tagTimerHandler)
        g_source_remove(m_tagTimerHandler);

    GstTagList* newTags = 0;
    gst_event_parse_tag(event, &newTags);
    {
        MutexLocker lock(m_tagMutex);
        if (m_isDisconnected)
            return;
        newTags = gst_tag_list_merge(tags(), newTags, GST_TAG_MERGE_REPLACE_ALL);
#ifdef GST_API_VERSION_1
        m_tags = adoptGRef(newTags);
#else
        if (m_tags)
            gst_tag_list_free(m_tags);
        m_tags = newTags;
#endif
    }
    m_tagTimerHandler = g_timeout_add(0, reinterpret_cast<GSourceFunc>(inbandTextTrackPrivateTagTimeoutCallback), this);
}

void AudioTrackPrivateGStreamer::notifyPlayerOfTag()
{
    m_tagTimerHandler = 0;

    String language;
    String label;
    MutexLocker lock(m_tagMutex);
    if (!client())
        return;
    gchar* str;
    if (gst_tag_list_get_string(tags(), GST_TAG_LANGUAGE_CODE, &str)) {
        m_language = str;
        client()->setLanguage(str);
        g_free(str);
    }
    if (gst_tag_list_get_string(tags(), GST_TAG_TITLE, &str)) {
        m_label = str;
        client()->setLabel(str);
        g_free(str);
    }
}

} // namespace WebCore

#endif // ENABLE(VIDEO) && USE(AVFOUNDATION) && HAVE(AVFOUNDATION_TEXT_TRACK_SUPPORT)
