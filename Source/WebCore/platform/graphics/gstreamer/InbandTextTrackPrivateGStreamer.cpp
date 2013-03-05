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

#include "InbandTextTrackPrivateGStreamer.h"

#include "GStreamerUtilities.h"
#include "InbandTextTrackPrivateClient.h"
#include "Logging.h"
#include <gst/gst.h>

namespace WebCore {

#ifdef GST_API_VERSION_1
static const char* gSampleSignal = "new-sample";
#else
static const char* gSampleSignal = "new-buffer";
#endif

#ifdef GST_API_VERSION_1
static GstFlowReturn inbandTextTrackPrivateNewSampleCallback(GstPad*, InbandTextTrackPrivateGStreamer* track)
{
    track->newSample();
    return GST_FLOW_OK;
}

static GstPadProbeReturn inbandTextTrackPrivateTextTagCallback(GstPad*, GstPadProbeInfo* info, InbandTextTrackPrivateGStreamer* track)
{
    track->handleTag(gst_pad_probe_info_get_event(info));
    return GST_PAD_PROBE_OK;
}
#else
static void inbandTextTrackPrivateNewSampleCallback(GstPad*, InbandTextTrackPrivateGStreamer* track)
{
    track->newSample();
}

static gboolean inbandTextTrackPrivateTextTagCallback(GstPad*, GstEvent* event, InbandTextTrackPrivateGStreamer* track)
{
    track->handleTag(event);
    return true;
}
#endif

static gboolean inbandTextTrackPrivateBufferTimeoutCallback(InbandTextTrackPrivateGStreamer* track)
{
    track->notifyPlayerOfSample();
    return FALSE;
}

static gboolean inbandTextTrackPrivateTagTimeoutCallback(InbandTextTrackPrivateGStreamer* track)
{
    track->notifyPlayerOfTag();
    return FALSE;
}

InbandTextTrackPrivateGStreamer::InbandTextTrackPrivateGStreamer(GRefPtr<GstElement> pipeline, GstPad* pad)
    : m_pad(pad)
    , m_pipeline(pipeline)
    , m_queue(gst_element_factory_make("queue", NULL))
    , m_sink(gst_element_factory_make("appsink", NULL))
    , m_sampleTimerHandler(0)
    , m_tagTimerHandler(0)
#ifndef GST_API_VERSION_1
    , m_tags(0)
#endif
    , m_hasBeenReported(false)
    , m_isDisconnected(false)
{
    gst_bin_add_many(GST_BIN(m_pipeline.get()), m_queue.get(), m_sink.get(), NULL);
    gst_element_sync_state_with_parent(m_queue.get());
    gst_element_sync_state_with_parent(m_sink.get());

    GstPad* queuePad = gst_element_get_static_pad(m_queue.get(), "sink");
    bool ret = gst_pad_link(pad, queuePad) == GST_PAD_LINK_OK;
    UNUSED_PARAM(ret);
    ASSERT(ret);
    ret = gst_element_link(m_queue.get(), m_sink.get());
    ASSERT(ret);

    g_object_set(m_sink.get(), "emit-signals", true, "sync", true, NULL);
    g_signal_connect(m_sink.get(), gSampleSignal, G_CALLBACK(inbandTextTrackPrivateNewSampleCallback), this);

    GstPad* sinkPad = gst_element_get_static_pad(m_sink.get(), "sink");
    ASSERT(sinkPad);
#ifdef GST_API_VERSION_1
    m_tagProbe = gst_pad_add_probe(sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM,
        reinterpret_cast<GstPadProbeCallback>(inbandTextTrackPrivateTextTagCallback), this, 0);
#else
    m_tagProbe = gst_pad_add_event_probe(sinkPad, G_CALLBACK(inbandTextTrackPrivateTextTagCallback), this);
#endif
}

InbandTextTrackPrivateGStreamer::~InbandTextTrackPrivateGStreamer()
{
    disconnect();
#ifndef GST_API_VERSION_1
    MutexLocker lock(m_tagMutex);
    if (m_tags)
        gst_tag_list_free(m_tags);
#endif
}

void InbandTextTrackPrivateGStreamer::disconnect()
{
    if (m_isDisconnected)
        return;

    m_isDisconnected = true;

    GstPad* sinkPad = gst_element_get_static_pad(m_sink.get(), "sink");
    ASSERT(sinkPad);

#ifdef GST_API_VERSION_1
    gst_pad_remove_probe(sinkPad, m_tagProbe);
#else
    gst_pad_remove_event_probe(sinkPad, m_tagProbe);
#endif
    m_tagProbe = 0;

    gst_element_set_state(m_queue.get(), GST_STATE_NULL);
    gst_element_set_state(m_sink.get(), GST_STATE_NULL);

    gst_bin_remove_many(GST_BIN(m_pipeline.get()), m_queue.get(), m_sink.get(), NULL);

    m_pipeline.clear();
    m_queue.clear();
    m_sink.clear();

    m_pad = 0;

    if (m_sampleTimerHandler)
        g_source_remove(m_sampleTimerHandler);
    if (m_tagTimerHandler)
        g_source_remove(m_tagTimerHandler);
    m_index = 0;
}

GstTagList* InbandTextTrackPrivateGStreamer::tags() const
{
#ifdef GST_API_VERSION_1
    return m_tags.get();
#else
    return m_tags;
#endif
}

void InbandTextTrackPrivateGStreamer::newSample()
{
    if (m_sampleTimerHandler)
        g_source_remove(m_sampleTimerHandler);

    RefPtr<GenericCueData> data = GenericCueData::create();
    GstBuffer* buffer;
#ifdef GST_API_VERSION_1
    GstSample* sample;
    g_signal_emit_by_name(m_sink.get(), "pull-sample", &sample, NULL);
    if (!sample)
        return; // EOS
    buffer = gst_sample_get_buffer(sample);
    GstMapInfo mapInfo;
    if (!gst_buffer_map(buffer, &mapInfo, GST_MAP_READ)) {
        LOG_MEDIA_MESSAGE("Unable to memory map cue data.\n");
        return;
    }
    data->setContent(String::fromUTF8(mapInfo.data, mapInfo.size));
    gst_buffer_unmap(buffer, &mapInfo);
    data->setStartTime(((double)buffer->pts) / GST_SECOND);
    data->setEndTime(data->startTime() + ((double)buffer->duration) / GST_SECOND);
    gst_sample_unref(sample);
#else
    g_signal_emit_by_name(m_sink.get(), "pull-buffer", &buffer, NULL);
    if (!buffer)
        return; // EOS
    data->setContent(String::fromUTF8(buffer->data, buffer->size));
    data->setStartTime(((double)buffer->timestamp) / GST_SECOND);
    data->setEndTime(data->startTime() + ((double)buffer->duration) / GST_SECOND);
    gst_buffer_unref(buffer);
#endif
    data->setStatus(GenericCueData::Complete);
    MutexLocker lock(m_cueMutex);
    m_cues.append(data.release());
    m_sampleTimerHandler = g_timeout_add(0,
        reinterpret_cast<GSourceFunc>(inbandTextTrackPrivateBufferTimeoutCallback), this);
}

void InbandTextTrackPrivateGStreamer::notifyPlayerOfSample()
{
    m_sampleTimerHandler = 0;

    MutexLocker lock(m_cueMutex);
    for (size_t i = 0; i < m_cues.size() && client(); ++i)
        client()->addGenericCue(this, m_cues[i].release());
    m_cues.clear();
}

void InbandTextTrackPrivateGStreamer::handleTag(GstEvent* event)
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

void InbandTextTrackPrivateGStreamer::notifyPlayerOfTag()
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
