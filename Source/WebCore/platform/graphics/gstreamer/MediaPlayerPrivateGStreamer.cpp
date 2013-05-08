/*
 * Copyright (C) 2007, 2009 Apple Inc.  All rights reserved.
 * Copyright (C) 2007 Collabora Ltd.  All rights reserved.
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 * Copyright (C) 2009 Gustavo Noronha Silva <gns@gnome.org>
 * Copyright (C) 2009, 2010, 2011, 2012, 2013 Igalia S.L
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * aint with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "MediaPlayerPrivateGStreamer.h"

#if ENABLE(VIDEO) && USE(GSTREAMER)

#include "GStreamerUtilities.h"
#include "GStreamerVersioning.h"
#include "KURL.h"
#include "Logging.h"
#include "MIMETypeRegistry.h"
#include "MediaPlayer.h"
#include "NotImplemented.h"
#include "SecurityOrigin.h"
#include "TimeRanges.h"
#include "WebKitWebSourceGStreamer.h"
#include <gst/gst.h>
#include <gst/pbutils/missing-plugins.h>
#include <limits>
#include <wtf/gobject/GOwnPtr.h>
#include <wtf/text/CString.h>

#if ENABLE(VIDEO_TRACK)
#include "AudioTrackPrivateGStreamer.h"
#include "InbandTextTrackPrivateGStreamer.h"
#include "VideoTrackPrivateGStreamer.h"
#endif

#ifdef GST_API_VERSION_1
#include <gst/audio/streamvolume.h>
#else
#include <gst/interfaces/streamvolume.h>
#endif

// gPercentMax is used when parsing buffering ranges with
// gst_query_parse_nth_buffering_range as there was a bug in GStreamer
// 0.10 that was using 100 instead of GST_FORMAT_PERCENT_MAX. This was
// corrected in 1.0. gst_query_parse_buffering_range worked as
// expected with GST_FORMAT_PERCENT_MAX in both cases.
#ifdef GST_API_VERSION_1
static const gint64 gPercentMax = GST_FORMAT_PERCENT_MAX;
#else
static const gint64 gPercentMax = 100;
#endif

GST_DEBUG_CATEGORY_EXTERN(webkit_media_player_debug);
#define GST_CAT_DEFAULT webkit_media_player_debug

using namespace std;

namespace WebCore {

static gboolean mediaPlayerPrivateMessageCallback(GstBus*, GstMessage* message, MediaPlayerPrivateGStreamer* player)
{
    return player->handleMessage(message);
}

static void mediaPlayerPrivateSourceChangedCallback(GObject*, GParamSpec*, MediaPlayerPrivateGStreamer* player)
{
    player->sourceChanged();
}

static void mediaPlayerPrivatePadAddedCallback(GObject*, GstPad* pad, MediaPlayerPrivateGStreamer* player)
{
    player->padAdded(pad);
}

static void mediaPlayerPrivatePadRemovedCallback(GObject*, GstPad* pad, MediaPlayerPrivateGStreamer* player)
{
    player->padRemoved(pad);
}

static void mediaPlayerPrivateVideoSinkCapsChangedCallback(GObject*, GParamSpec*, MediaPlayerPrivateGStreamer* player)
{
    player->videoCapsChanged();
}

static gboolean mediaPlayerPrivateAudioChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    player->notifyPlayerOfAudio();
    return FALSE;
}

#ifdef GST_API_VERSION_1
static void setAudioStreamPropertiesCallback(GstChildProxy*, GObject* object, gchar*,
    MediaPlayerPrivateGStreamer* player)
#else
static void setAudioStreamPropertiesCallback(GstChildProxy*, GObject* object, MediaPlayerPrivateGStreamer* player)
#endif
{
    player->setAudioStreamProperties(object);
}

static gboolean mediaPlayerPrivateTextChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    player->notifyPlayerOfText();
    return FALSE;
}

static gboolean mediaPlayerPrivateVideoChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    player->notifyPlayerOfVideo();
    return FALSE;
}

static gboolean mediaPlayerPrivateVideoCapsChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    // This is the callback of the timeout source created in ::videoCapsChanged.
    player->notifyPlayerOfVideoCaps();
    return FALSE;
}

static void mediaPlayerPrivatePluginInstallerResultFunction(GstInstallPluginsReturn result, gpointer userData)
{
    MediaPlayerPrivateGStreamer* player = reinterpret_cast<MediaPlayerPrivateGStreamer*>(userData);
    player->handlePluginInstallerResult(result);
}

void MediaPlayerPrivateGStreamer::setAudioStreamProperties(GObject* object)
{
    if (g_strcmp0(G_OBJECT_TYPE_NAME(object), "GstPulseSink"))
        return;

    const char* role = m_player->mediaPlayerClient() && m_player->mediaPlayerClient()->mediaPlayerIsVideo()
        ? "video" : "music";
    GstStructure* structure = gst_structure_new("stream-properties", "media.role", G_TYPE_STRING, role, NULL);
    g_object_set(object, "stream-properties", structure, NULL);
    gst_structure_free(structure);
    GOwnPtr<gchar> elementName(gst_element_get_name(GST_ELEMENT(object)));
    LOG_MEDIA_MESSAGE("Set media.role as %s at %s", role, elementName.get());
}

PassOwnPtr<MediaPlayerPrivateInterface> MediaPlayerPrivateGStreamer::create(MediaPlayer* player)
{
    return adoptPtr(new MediaPlayerPrivateGStreamer(player));
}

void MediaPlayerPrivateGStreamer::registerMediaEngine(MediaEngineRegistrar registrar)
{
    if (isAvailable())
        registrar(create, getSupportedTypes, supportsType, 0, 0, 0);
}

bool initializeGStreamerAndRegisterWebKitElements()
{
    if (!initializeGStreamer())
        return false;

    GRefPtr<GstElementFactory> srcFactory = gst_element_factory_find("webkitwebsrc");
    if (!srcFactory) {
        GST_DEBUG_CATEGORY_INIT(webkit_media_player_debug, "webkitmediaplayer", 0, "WebKit media player");
        return gst_element_register(0, "webkitwebsrc", GST_RANK_PRIMARY + 100, WEBKIT_TYPE_WEB_SRC);
    }

    return true;
}

bool MediaPlayerPrivateGStreamer::isAvailable()
{
    if (!initializeGStreamerAndRegisterWebKitElements())
        return false;

    GRefPtr<GstElementFactory> factory = gst_element_factory_find("uridecodebin");
    return factory;
}

MediaPlayerPrivateGStreamer::MediaPlayerPrivateGStreamer(MediaPlayer* player)
    : MediaPlayerPrivateGStreamerBase(player)
    , m_source(0)
    , m_seekTime(0)
    , m_changingRate(false)
    , m_endTime(numeric_limits<float>::infinity())
    , m_isEndReached(false)
    , m_isStreaming(false)
    , m_mediaLocations(0)
    , m_mediaLocationCurrentIndex(0)
    , m_resetPipeline(false)
    , m_paused(true)
    , m_seeking(false)
    , m_buffering(false)
    , m_playbackRate(1)
    , m_errorOccured(false)
    , m_mediaDuration(0)
    , m_startedBuffering(false)
    , m_fillTimer(this, &MediaPlayerPrivateGStreamer::fillTimerFired)
    , m_maxTimeLoaded(0)
    , m_bufferingPercentage(0)
    , m_preload(player->preload())
    , m_delayingLoad(false)
    , m_mediaDurationKnown(true)
    , m_maxTimeLoadedAtLastDidLoadingProgress(0)
    , m_volumeAndMuteInitialized(false)
    , m_hasVideo(false)
    , m_hasAudio(false)
    , m_hasText(false)
    , m_audioTimerHandler(0)
    , m_textTimerHandler(0)
    , m_videoTimerHandler(0)
    , m_videoCapsTimerHandler(0)
    , m_webkitAudioSink(0)
    , m_totalBytes(-1)
    , m_preservesPitch(false)
    , m_requestedState(GST_STATE_VOID_PENDING)
    , m_missingPlugins(false)
{
}

MediaPlayerPrivateGStreamer::~MediaPlayerPrivateGStreamer()
{
    if (m_fillTimer.isActive())
        m_fillTimer.stop();

    if (m_mediaLocations) {
        gst_structure_free(m_mediaLocations);
        m_mediaLocations = 0;
    }

    if (m_autoAudioSink)
        g_signal_handlers_disconnect_by_func(G_OBJECT(m_autoAudioSink.get()),
            reinterpret_cast<gpointer>(setAudioStreamPropertiesCallback), this);

    if (m_pipeline) {
        GRefPtr<GstBus> bus = webkitGstPipelineGetBus(GST_PIPELINE(m_pipeline.get()));
        ASSERT(bus);
        g_signal_handlers_disconnect_by_func(bus.get(), reinterpret_cast<gpointer>(mediaPlayerPrivateMessageCallback), this);
        gst_bus_remove_signal_watch(bus.get());

        g_signal_handlers_disconnect_by_func(m_decodebin.get(), reinterpret_cast<gpointer>(mediaPlayerPrivateSourceChangedCallback), this);
        g_signal_handlers_disconnect_by_func(m_decodebin.get(), reinterpret_cast<gpointer>(mediaPlayerPrivatePadAddedCallback), this);

        gst_element_set_state(m_pipeline.get(), GST_STATE_NULL);
        m_pipeline.clear();
    }

#if ENABLE(VIDEO_TRACK)
    for (size_t i = 0; i < m_audioTracks.size(); ++i) {
        m_audioTracks[i]->disconnect();
        m_player->removeAudioTrack(m_audioTracks[i].release());
    }
    m_audioTracks.clear();

    for (size_t i = 0; i < m_textTracks.size(); ++i) {
        m_textTracks[i]->disconnect();
        m_player->removeTextTrack(m_textTracks[i].release());
    }
    m_textTracks.clear();

    for (size_t i = 0; i < m_videoTracks.size(); ++i) {
        m_videoTracks[i]->disconnect();
        m_player->removeVideoTrack(m_videoTracks[i].release());
    }
    m_videoTracks.clear();
#endif

    GRefPtr<GstPad> videoSinkPad = adoptGRef(gst_element_get_static_pad(m_webkitVideoSink.get(), "sink"));
    g_signal_handlers_disconnect_by_func(videoSinkPad.get(), reinterpret_cast<gpointer>(mediaPlayerPrivateVideoSinkCapsChangedCallback), this);

    if (m_audioTimerHandler)
        g_source_remove(m_audioTimerHandler);
    if (m_textTimerHandler)
        g_source_remove(m_textTimerHandler);
    if (m_videoCapsTimerHandler)
        g_source_remove(m_videoCapsTimerHandler);
    if (m_videoTimerHandler)
        g_source_remove(m_videoTimerHandler);
}

void MediaPlayerPrivateGStreamer::load(const String& url)
{
    if (!initializeGStreamerAndRegisterWebKitElements())
        return;

    KURL kurl(KURL(), url);
    String cleanUrl(url);

    // Clean out everything after file:// url path.
    if (kurl.isLocalFile())
        cleanUrl = cleanUrl.substring(0, kurl.pathEnd());

    if (!m_pipeline) {
        createPipeline();
        setDownloadBuffering();
    }

    ASSERT(m_pipeline);

    m_url = KURL(KURL(), cleanUrl);
    g_object_set(m_decodebin.get(), "uri", cleanUrl.utf8().data(), NULL);

    LOG_MEDIA_MESSAGE("Load %s", cleanUrl.utf8().data());

    if (m_preload == MediaPlayer::None) {
        LOG_MEDIA_MESSAGE("Delaying load.");
        m_delayingLoad = true;
    }

    // Reset network and ready states. Those will be set properly once
    // the pipeline pre-rolled.
    m_networkState = MediaPlayer::Loading;
    m_player->networkStateChanged();
    m_readyState = MediaPlayer::HaveNothing;
    m_player->readyStateChanged();
    m_volumeAndMuteInitialized = false;

    if (!m_delayingLoad)
        commitLoad();
}

#if ENABLE(MEDIA_SOURCE)
void MediaPlayerPrivateGStreamer::load(const String& url, PassRefPtr<MediaSource>)
{
    notImplemented();
}
#endif

void MediaPlayerPrivateGStreamer::commitLoad()
{
    ASSERT(!m_delayingLoad);
    LOG_MEDIA_MESSAGE("Committing load.");

    // GStreamer needs to have the pipeline set to a paused state to
    // start providing anything useful.
    gst_element_set_state(m_pipeline.get(), GST_STATE_PAUSED);

    updateStates();
}

float MediaPlayerPrivateGStreamer::playbackPosition() const
{
    if (m_isEndReached) {
        // Position queries on a null pipeline return 0. If we're at
        // the end of the stream the pipeline is null but we want to
        // report either the seek time or the duration because this is
        // what the Media element spec expects us to do.
        if (m_seeking)
            return m_seekTime;
        if (m_mediaDuration)
            return m_mediaDuration;
    }

    float ret = 0.0f;

    gint64 position;
    GstFormat timeFormat = GST_FORMAT_TIME;

#ifdef GST_API_VERSION_1
    gboolean failure = !gst_element_query_position(m_pipeline.get(), timeFormat, &position);
#else
    gboolean failure = !gst_element_query_position(m_pipeline.get(), &timeFormat, &position);
#endif
    if (failure) {
        LOG_MEDIA_MESSAGE("Position query failed...");
        return ret;
    }

    // Position is available only if the pipeline is not in GST_STATE_NULL or
    // GST_STATE_READY state.
    if (position != static_cast<gint64>(GST_CLOCK_TIME_NONE))
        ret = static_cast<double>(position) / GST_SECOND;

    LOG_MEDIA_MESSAGE("Position %" GST_TIME_FORMAT, GST_TIME_ARGS(position));

    return ret;
}

bool MediaPlayerPrivateGStreamer::changePipelineState(GstState newState)
{
    ASSERT(newState == GST_STATE_PLAYING || newState == GST_STATE_PAUSED);

    GstState currentState;
    GstState pending;

    gst_element_get_state(m_pipeline.get(), &currentState, &pending, 0);
    if (currentState == newState || pending == newState) {
        LOG_MEDIA_MESSAGE("Rejected state change to %s from %s with %s pending", gst_element_state_get_name(newState),
            gst_element_state_get_name(currentState), gst_element_state_get_name(pending));
        return true;
    }

    LOG_MEDIA_MESSAGE("Changing state change to %s from %s with %s pending", gst_element_state_get_name(newState),
        gst_element_state_get_name(currentState), gst_element_state_get_name(pending));

    GstStateChangeReturn setStateResult = gst_element_set_state(m_pipeline.get(), newState);
    GstState pausedOrPlaying = newState == GST_STATE_PLAYING ? GST_STATE_PAUSED : GST_STATE_PLAYING;
    if (currentState != pausedOrPlaying && setStateResult == GST_STATE_CHANGE_FAILURE) {
        loadingFailed(MediaPlayer::Empty);
        return false;
    }
    return true;
}

void MediaPlayerPrivateGStreamer::prepareToPlay()
{
    m_isEndReached = false;
    m_seeking = false;

    if (m_delayingLoad) {
        m_delayingLoad = false;
        commitLoad();
    }
}

void MediaPlayerPrivateGStreamer::play()
{
    if (changePipelineState(GST_STATE_PLAYING)) {
        m_isEndReached = false;
        LOG_MEDIA_MESSAGE("Play");
    }
}

void MediaPlayerPrivateGStreamer::pause()
{
    if (m_isEndReached)
        return;

    if (changePipelineState(GST_STATE_PAUSED))
        LOG_MEDIA_MESSAGE("Pause");
}

float MediaPlayerPrivateGStreamer::duration() const
{
    if (!m_pipeline)
        return 0.0f;

    if (m_errorOccured)
        return 0.0f;

    // Media duration query failed already, don't attempt new useless queries.
    if (!m_mediaDurationKnown)
        return numeric_limits<float>::infinity();

    if (m_mediaDuration)
        return m_mediaDuration;

    GstFormat timeFormat = GST_FORMAT_TIME;
    gint64 timeLength = 0;

#ifdef GST_API_VERSION_1
    bool failure = !gst_element_query_duration(m_pipeline.get(), timeFormat, &timeLength) || static_cast<guint64>(timeLength) == GST_CLOCK_TIME_NONE;
#else
    bool failure = !gst_element_query_duration(m_pipeline.get(), &timeFormat, &timeLength) || timeFormat != GST_FORMAT_TIME || static_cast<guint64>(timeLength) == GST_CLOCK_TIME_NONE;
#endif
    if (failure) {
        LOG_MEDIA_MESSAGE("Time duration query failed for %s", m_url.string().utf8().data());
        return numeric_limits<float>::infinity();
    }

    LOG_MEDIA_MESSAGE("Duration: %" GST_TIME_FORMAT, GST_TIME_ARGS(timeLength));

    return static_cast<double>(timeLength) / GST_SECOND;
    // FIXME: handle 3.14.9.5 properly
}

float MediaPlayerPrivateGStreamer::currentTime() const
{
    if (!m_pipeline)
        return 0.0f;

    if (m_errorOccured)
        return 0.0f;

    if (m_seeking)
        return m_seekTime;

    // Workaround for
    // https://bugzilla.gnome.org/show_bug.cgi?id=639941 In GStreamer
    // 0.10.35 basesink reports wrong duration in case of EOS and
    // negative playback rate. There's no upstream accepted patch for
    // this bug yet, hence this temporary workaround.
    if (m_isEndReached && m_playbackRate < 0)
        return 0.0f;

    return playbackPosition();

}

void MediaPlayerPrivateGStreamer::seek(float time)
{
    if (!m_pipeline)
        return;

    if (m_errorOccured)
        return;

    LOG_MEDIA_MESSAGE("Seek attempt to %f secs", time);

    // Avoid useless seeking.
    if (time == currentTime())
        return;

    // Extract the integer part of the time (seconds) and the
    // fractional part (microseconds). Attempt to round the
    // microseconds so no floating point precision is lost and we can
    // perform an accurate seek.
    float seconds;
    float microSeconds = modf(time, &seconds) * 1000000;
    GTimeVal timeValue;
    timeValue.tv_sec = static_cast<glong>(seconds);
    timeValue.tv_usec = static_cast<glong>(roundf(microSeconds / 10000) * 10000);

    GstClockTime clockTime = GST_TIMEVAL_TO_TIME(timeValue);
    LOG_MEDIA_MESSAGE("Seek: %" GST_TIME_FORMAT, GST_TIME_ARGS(clockTime));

    if (!gst_element_seek(m_pipeline.get(), m_player->rate(),
            GST_FORMAT_TIME,
            (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
            GST_SEEK_TYPE_SET, clockTime,
            GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
        LOG_MEDIA_MESSAGE("Seek to %f failed", time);
    else {
        m_seeking = true;
        m_seekTime = time;
    }
}

bool MediaPlayerPrivateGStreamer::paused() const
{
    if (m_isEndReached) {
        LOG_MEDIA_MESSAGE("Ignoring pause at EOS");
        return true;
    }

    GstState state;
    gst_element_get_state(m_pipeline.get(), &state, 0, 0);
    return state == GST_STATE_PAUSED;
}

bool MediaPlayerPrivateGStreamer::seeking() const
{
    return m_seeking;
}

void MediaPlayerPrivateGStreamer::padAdded(GstPad* pad)
{
#ifdef GST_API_VERSION_1
    GRefPtr<GstCaps> caps = gst_pad_query_caps(pad, NULL);
#else
    GRefPtr<GstCaps> caps = gst_pad_get_caps_reffed(pad);
#endif
    GstStructure* str = gst_caps_get_structure(caps.get(), 0);
    const gchar* name = gst_structure_get_name(str);

    GstElementClass* klass = GST_ELEMENT_GET_CLASS(m_sink.get());
    GstPadTemplate* templ = 0;
    guint* handler;
    GSourceFunc callback;
    if (g_str_has_prefix(name, "audio")) {
        templ = gst_element_class_get_pad_template(klass, "audio_sink");
        handler = &m_audioTimerHandler;
        callback = reinterpret_cast<GSourceFunc>(mediaPlayerPrivateAudioChangeTimeoutCallback);

#if ENABLE(VIDEO_TRACK)
        if (!m_audioAdder) {
            // Currently this is a copy of videoSelector since adder has some problems
            m_audioAdder = gst_element_factory_make("input-selector", NULL);
            g_object_set(m_audioAdder.get(), "sync-streams", TRUE, NULL);

            gst_bin_add(GST_BIN(m_pipeline.get()), m_audioAdder.get());
            gst_element_sync_state_with_parent(m_audioAdder.get());

            GRefPtr<GstPad> audioSrc = adoptGRef(gst_element_get_static_pad(m_audioAdder.get(), "src"));
            GRefPtr<GstPad> audioSink = adoptGRef(gst_element_get_request_pad(m_sink.get(), "audio_sink"));
            ASSERT(audioSrc);
            ASSERT(audioSink);

            bool ret = gst_pad_link(audioSrc.get(), audioSink.get()) == GST_PAD_LINK_OK;
            ASSERT(ret);
        }

        RefPtr<AudioTrackPrivateGStreamer> track = AudioTrackPrivateGStreamer::create(m_audioAdder, pad);
        MutexLocker lock(m_audioTrackMutex);
        if (m_audioTracks.size() == 0)
            track->setEnabled(true);
        m_audioTracks.append(track.release());
#endif
    } else if (g_str_has_prefix(name, "text")) {
        handler = &m_textTimerHandler;
        callback = reinterpret_cast<GSourceFunc>(mediaPlayerPrivateTextChangeTimeoutCallback);

#if ENABLE(VIDEO_TRACK)
        RefPtr<InbandTextTrackPrivateGStreamer> track = InbandTextTrackPrivateGStreamer::create(m_pipeline, pad);
        MutexLocker lock(m_textTrackMutex);
        m_textTracks.append(track.release());
#endif
    } else if (g_str_has_prefix(name, "video")) {
        templ = gst_element_class_get_pad_template(klass, "video_sink");
        handler = &m_videoTimerHandler;
        callback = reinterpret_cast<GSourceFunc>(mediaPlayerPrivateVideoChangeTimeoutCallback);

#if ENABLE(VIDEO_TRACK)
        if (!m_videoSelector) {
            m_videoSelector = gst_element_factory_make("input-selector", NULL);
            // Sync mode 1 = "clock"
            g_object_set(m_videoSelector.get(), "sync-streams", TRUE, NULL);
            // Maybe block the input selector until we have streams?

            gst_bin_add(GST_BIN(m_pipeline.get()), m_videoSelector.get());
            gst_element_sync_state_with_parent(m_videoSelector.get());

            GRefPtr<GstPad> videoSrc = adoptGRef(gst_element_get_static_pad(m_videoSelector.get(), "src"));
            GRefPtr<GstPad> videoSink = adoptGRef(gst_element_get_request_pad(m_sink.get(), "video_sink"));
            ASSERT(videoSrc);
            ASSERT(videoSink);
            bool ret = gst_pad_link(videoSrc.get(), videoSink.get()) == GST_PAD_LINK_OK;
            ASSERT(ret);
        }
        RefPtr<VideoTrackPrivateGStreamer> track = VideoTrackPrivateGStreamer::create(m_videoSelector, pad);
        MutexLocker lock(m_videoTrackMutex);
        if (m_videoTracks.size() == 0)
            track->setSelected(true);
        m_videoTracks.append(track.release());
#endif
    } else
        return;

    if (templ && !gst_pad_is_linked(pad)) {
        GRefPtr<GstPad> sinkPad = gst_element_request_pad(m_sink.get(), templ, NULL, NULL);
        if (!gst_pad_is_linked(sinkPad.get()))
            gst_pad_link(pad, sinkPad.get());
    }

    if (*handler)
        g_source_remove(*handler);
    *handler = g_timeout_add(0, callback, this);
}

void MediaPlayerPrivateGStreamer::padRemoved(GstPad* pad)
{
#if ENABLE(VIDEO_TRACK)
    {
        MutexLocker lock(m_audioTrackMutex);
        for (size_t i = 0; i < m_audioTracks.size(); ++i) {
            RefPtr<AudioTrackPrivateGStreamer> track = m_audioTracks[i];
            if (track->pad() == pad) {
                track->disconnect();
                if (m_audioTracks.size() == 0) {
                    if (m_audioAdder) {
                        GRefPtr<GstPad> audioSrc = adoptGRef(gst_element_get_static_pad(m_audioAdder.get(), "src"));
                        GRefPtr<GstPad> audioSink = adoptGRef(gst_element_get_request_pad(m_sink.get(), "audio_sink"));
                        ASSERT(audioSrc);
                        ASSERT(audioSink);

                        bool ret = gst_pad_unlink(audioSrc.get(), audioSink.get());
                        ASSERT(ret);

                        gst_bin_remove(GST_BIN(m_pipeline.get()), m_audioAdder.get());
                        gst_element_set_state(m_audioAdder.get(), GST_STATE_NULL);
                        m_audioAdder.clear();
                    }
                }

                if (m_audioTimerHandler)
                    g_source_remove(m_audioTimerHandler);
                m_audioTimerHandler = g_timeout_add(0,
                    reinterpret_cast<GSourceFunc>(mediaPlayerPrivateAudioChangeTimeoutCallback), this);
                return;
            }
        }
    }
    {
        MutexLocker lock(m_textTrackMutex);
        for (size_t i = 0; i < m_textTracks.size(); ++i) {
            RefPtr<InbandTextTrackPrivateGStreamer> track = m_textTracks[i];
            if (track->pad() == pad) {
                track->disconnect();

                if (m_textTimerHandler)
                    g_source_remove(m_textTimerHandler);
                m_textTimerHandler = g_timeout_add(0,
                    reinterpret_cast<GSourceFunc>(mediaPlayerPrivateTextChangeTimeoutCallback), this);
                return;
            }
        }
    }
    {
        MutexLocker lock(m_videoTrackMutex);
        for (size_t i = 0; i < m_videoTracks.size(); ++i) {
            RefPtr<VideoTrackPrivateGStreamer> track = m_videoTracks[i];
            if (track->pad() == pad) {
                track->disconnect();

                if (m_videoTracks.size() == 0) {
                    GRefPtr<GstPad> videoSrc = adoptGRef(gst_element_get_static_pad(m_videoSelector.get(), "src"));
                    GRefPtr<GstPad> videoSink = adoptGRef(gst_element_get_request_pad(m_sink.get(), "video_sink"));
                    ASSERT(videoSrc);
                    ASSERT(videoSink);
                    bool ret = gst_pad_unlink(videoSrc.get(), videoSink.get());
                    ASSERT(ret);

                    gst_bin_remove(GST_BIN(m_pipeline.get()), m_videoSelector.get());
                    gst_element_set_state(m_videoSelector.get(), GST_STATE_NULL);
                    m_videoSelector.clear();
                }

                if (m_videoTimerHandler)
                    g_source_remove(m_videoTimerHandler);
                m_videoTimerHandler = g_timeout_add(0,
                    reinterpret_cast<GSourceFunc>(mediaPlayerPrivateVideoChangeTimeoutCallback), this);
                return;
            }
        }
    }
#endif
}

void MediaPlayerPrivateGStreamer::videoCapsChanged()
{
    if (m_videoCapsTimerHandler)
        g_source_remove(m_videoCapsTimerHandler);
    m_videoCapsTimerHandler = g_timeout_add(0, reinterpret_cast<GSourceFunc>(mediaPlayerPrivateVideoCapsChangeTimeoutCallback), this);
}

void MediaPlayerPrivateGStreamer::notifyPlayerOfVideo()
{
    m_videoTimerHandler = 0;

#if ENABLE(VIDEO_TRACK)
    {
        MutexLocker lock(m_videoTrackMutex);
        m_hasVideo = m_videoTracks.size() > 0;

        size_t i = 0;
        while (i < m_videoTracks.size()) {
            RefPtr<VideoTrackPrivateGStreamer> track = m_videoTracks[i];
            if (track->isDisconnected()) {
                m_player->removeVideoTrack(track.release());
                m_videoTracks.remove(i);
                continue;
            }

            if (!track->hasBeenReported()) {
                m_player->addVideoTrack(track);
                track->setHasBeenReported(true);
            }
            ++i;
        }
    }
#else
    m_hasVideo = true;
#endif

    m_player->mediaPlayerClient()->mediaPlayerEngineUpdated(m_player);
}

void MediaPlayerPrivateGStreamer::notifyPlayerOfVideoCaps()
{
    m_videoCapsTimerHandler = 0;

    m_videoSize = IntSize();

    m_player->mediaPlayerClient()->mediaPlayerEngineUpdated(m_player);
}

void MediaPlayerPrivateGStreamer::notifyPlayerOfAudio()
{
    m_audioTimerHandler = 0;

#if ENABLE(VIDEO_TRACK)
    {
        MutexLocker lock(m_audioTrackMutex);
        m_hasAudio = m_audioTracks.size() > 0;

        size_t i = 0;
        while (i < m_audioTracks.size()) {
            RefPtr<AudioTrackPrivateGStreamer> track = m_audioTracks[i];
            if (track->isDisconnected()) {
                m_player->removeAudioTrack(track.release());
                m_audioTracks.remove(i);
                continue;
            }

            if (!track->hasBeenReported()) {
                m_player->addAudioTrack(track);
                track->setHasBeenReported(true);
            }
            ++i;
        }
    }
#else
    m_hasAudio = true;
#endif

    m_player->mediaPlayerClient()->mediaPlayerEngineUpdated(m_player);
}

void MediaPlayerPrivateGStreamer::notifyPlayerOfText()
{
    m_textTimerHandler = 0;

#if ENABLE(VIDEO_TRACK)
    {
        MutexLocker lock(m_textTrackMutex);
        m_hasText = m_textTracks.size() > 0;

        size_t i = 0;
        while (i < m_textTracks.size()) {
            RefPtr<InbandTextTrackPrivateGStreamer> track = m_textTracks[i];
            if (track->isDisconnected()) {
                m_player->removeTextTrack(track.release());
                m_textTracks.remove(i);
                continue;
            }

            track->setTextTrackIndex(i);

            if (!track->hasBeenReported()) {
                m_player->addTextTrack(track);
                track->setHasBeenReported(true);
            }
            ++i;
        }
    }
#else
    m_hasText = true;
#endif

    m_player->mediaPlayerClient()->mediaPlayerEngineUpdated(m_player);
}

void MediaPlayerPrivateGStreamer::setRate(float rate)
{
    // Avoid useless playback rate update.
    if (m_playbackRate == rate)
        return;

    GstState state;
    GstState pending;

    gst_element_get_state(m_pipeline.get(), &state, &pending, 0);
    if ((state != GST_STATE_PLAYING && state != GST_STATE_PAUSED)
        || (pending == GST_STATE_PAUSED))
        return;

    if (isLiveStream())
        return;

    m_playbackRate = rate;
    m_changingRate = true;

    if (!rate) {
        gst_element_set_state(m_pipeline.get(), GST_STATE_PAUSED);
        return;
    }

    float currentPosition = static_cast<float>(playbackPosition() * GST_SECOND);
    GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH);
    gint64 start, end;
    bool mute = false;

    LOG_MEDIA_MESSAGE("Set Rate to %f", rate);
    if (rate > 0) {
        // Mute the sound if the playback rate is too extreme and
        // audio pitch is not adjusted.
        mute = (!m_preservesPitch && (rate < 0.8 || rate > 2));
        start = currentPosition;
        end = GST_CLOCK_TIME_NONE;
    } else {
        start = 0;
        mute = true;

        // If we are at beginning of media, start from the end to
        // avoid immediate EOS.
        if (currentPosition <= 0)
            end = static_cast<gint64>(duration() * GST_SECOND);
        else
            end = currentPosition;
    }

    LOG_MEDIA_MESSAGE("Need to mute audio: %d", (int) mute);

    if (!gst_element_seek(m_pipeline.get(), rate, GST_FORMAT_TIME, flags,
                          GST_SEEK_TYPE_SET, start,
                          GST_SEEK_TYPE_SET, end))
        LOG_MEDIA_MESSAGE("Set rate to %f failed", rate);
    else
        g_object_set(m_pipeline.get(), "mute", mute, NULL);
}

void MediaPlayerPrivateGStreamer::setPreservesPitch(bool preservesPitch)
{
    m_preservesPitch = preservesPitch;
}

PassRefPtr<TimeRanges> MediaPlayerPrivateGStreamer::buffered() const
{
    RefPtr<TimeRanges> timeRanges = TimeRanges::create();
    if (m_errorOccured || isLiveStream())
        return timeRanges.release();

#if GST_CHECK_VERSION(0, 10, 31)
    float mediaDuration(duration());
    if (!mediaDuration || std::isinf(mediaDuration))
        return timeRanges.release();

    GstQuery* query = gst_query_new_buffering(GST_FORMAT_PERCENT);

    if (!gst_element_query(m_pipeline.get(), query)) {
        gst_query_unref(query);
        return timeRanges.release();
    }

    for (guint index = 0; index < gst_query_get_n_buffering_ranges(query); index++) {
        gint64 rangeStart = 0, rangeStop = 0;
        if (gst_query_parse_nth_buffering_range(query, index, &rangeStart, &rangeStop))
            timeRanges->add(static_cast<float>((rangeStart * mediaDuration) / gPercentMax),
                static_cast<float>((rangeStop * mediaDuration) / gPercentMax));
    }

    // Fallback to the more general maxTimeLoaded() if no range has
    // been found.
    if (!timeRanges->length())
        if (float loaded = maxTimeLoaded())
            timeRanges->add(0, loaded);

    gst_query_unref(query);
#else
    float loaded = maxTimeLoaded();
    if (!m_errorOccured && !isLiveStream() && loaded > 0)
        timeRanges->add(0, loaded);
#endif
    return timeRanges.release();
}

gboolean MediaPlayerPrivateGStreamer::handleMessage(GstMessage* message)
{
    GOwnPtr<GError> err;
    GOwnPtr<gchar> debug;
    MediaPlayer::NetworkState error;
    bool issueError = true;
    bool attemptNextLocation = false;
    const GstStructure* structure = gst_message_get_structure(message);
    GstState requestedState, currentState;

    if (structure) {
        const gchar* messageTypeName = gst_structure_get_name(structure);

        // Redirect messages are sent from elements, like qtdemux, to
        // notify of the new location(s) of the media.
        if (!g_strcmp0(messageTypeName, "redirect")) {
            mediaLocationChanged(message);
            return TRUE;
        }
    }

    LOG_MEDIA_MESSAGE("Message received from element %s", GST_MESSAGE_SRC_NAME(message));
    switch (GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_ERROR:
        if (m_resetPipeline)
            break;
        if (m_missingPlugins)
            break;
        gst_message_parse_error(message, &err.outPtr(), &debug.outPtr());
        LOG_MEDIA_MESSAGE("Error %d: %s (url=%s)", err->code, err->message, m_url.string().utf8().data());

        GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(m_pipeline.get()), GST_DEBUG_GRAPH_SHOW_ALL, "webkit-video.error");

        error = MediaPlayer::Empty;
        if (err->code == GST_STREAM_ERROR_CODEC_NOT_FOUND
            || err->code == GST_STREAM_ERROR_WRONG_TYPE
            || err->code == GST_STREAM_ERROR_FAILED
            || err->code == GST_CORE_ERROR_MISSING_PLUGIN
            || err->code == GST_RESOURCE_ERROR_NOT_FOUND)
            error = MediaPlayer::FormatError;
        else if (err->domain == GST_STREAM_ERROR) {
            // Let the mediaPlayerClient handle the stream error, in
            // this case the HTMLMediaElement will emit a stalled
            // event.
            if (err->code == GST_STREAM_ERROR_TYPE_NOT_FOUND) {
                LOG_MEDIA_MESSAGE("Decode error, let the Media element emit a stalled event.");
                break;
            }
            error = MediaPlayer::DecodeError;
            attemptNextLocation = true;
        } else if (err->domain == GST_RESOURCE_ERROR)
            error = MediaPlayer::NetworkError;

        if (attemptNextLocation)
            issueError = !loadNextLocation();
        if (issueError)
            loadingFailed(error);
        break;
    case GST_MESSAGE_EOS:
        LOG_MEDIA_MESSAGE("End of Stream");
        didEnd();
        break;
    case GST_MESSAGE_STATE_CHANGED:
        // Ignore state changes if load is delayed (preload=none). The
        // player state will be updated once commitLoad() is called.
        if (m_delayingLoad) {
            LOG_MEDIA_MESSAGE("Media load has been delayed. Ignoring state changes for now");
            break;
        }

        // Ignore state changes from internal elements. They are
        // forwarded to playbin2 anyway.
        if (GST_MESSAGE_SRC(message) == reinterpret_cast<GstObject*>(m_pipeline.get())) {
            updateStates();

            // Construct a filename for the graphviz dot file output.
            GstState newState;
            gst_message_parse_state_changed(message, &currentState, &newState, 0);

            CString dotFileName = String::format("webkit-video.%s_%s",
                gst_element_state_get_name(currentState),
                gst_element_state_get_name(newState)).utf8();

            GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(m_pipeline.get()), GST_DEBUG_GRAPH_SHOW_ALL, dotFileName.data());
        }
        break;
    case GST_MESSAGE_BUFFERING:
        processBufferingStats(message);
        break;
#ifdef GST_API_VERSION_1
    case GST_MESSAGE_DURATION_CHANGED:
#else
    case GST_MESSAGE_DURATION:
#endif
        LOG_MEDIA_MESSAGE("Duration changed");
        durationChanged();
        break;
    case GST_MESSAGE_REQUEST_STATE:
        gst_message_parse_request_state(message, &requestedState);
        gst_element_get_state(m_pipeline.get(), &currentState, NULL, 250);
        if (requestedState < currentState) {
            GOwnPtr<gchar> elementName(gst_element_get_name(GST_ELEMENT(message)));
            LOG_MEDIA_MESSAGE("Element %s requested state change to %s", elementName.get(),
                gst_element_state_get_name(requestedState));
            m_requestedState = requestedState;
            changePipelineState(requestedState);
        }
        break;
    case GST_MESSAGE_ELEMENT:
        if (gst_is_missing_plugin_message(message)) {
            gchar* detail = gst_missing_plugin_message_get_installer_detail(message);
            gchar* detailArray[2] = {detail, 0};
            GstInstallPluginsReturn result = gst_install_plugins_async(detailArray, 0, mediaPlayerPrivatePluginInstallerResultFunction, this);
            m_missingPlugins = result == GST_INSTALL_PLUGINS_STARTED_OK;
            g_free(detail);
        }
        break;
    default:
        LOG_MEDIA_MESSAGE("Unhandled GStreamer message type: %s",
                    GST_MESSAGE_TYPE_NAME(message));
        break;
    }
    return TRUE;
}

void MediaPlayerPrivateGStreamer::handlePluginInstallerResult(GstInstallPluginsReturn result)
{
    m_missingPlugins = false;
    if (result == GST_INSTALL_PLUGINS_SUCCESS) {
        gst_element_set_state(m_pipeline.get(), GST_STATE_READY);
        gst_element_set_state(m_pipeline.get(), GST_STATE_PAUSED);
    }
}

void MediaPlayerPrivateGStreamer::processBufferingStats(GstMessage* message)
{
    // This is the immediate buffering that needs to happen so we have
    // enough to play right now.
    m_buffering = true;
    const GstStructure *structure = gst_message_get_structure(message);
    gst_structure_get_int(structure, "buffer-percent", &m_bufferingPercentage);

    LOG_MEDIA_MESSAGE("[Buffering] Buffering: %d%%.", m_bufferingPercentage);

    GstBufferingMode mode;
    gst_message_parse_buffering_stats(message, &mode, 0, 0, 0);
    if (mode != GST_BUFFERING_DOWNLOAD) {
        updateStates();
        return;
    }

    // This is on-disk buffering, that allows us to download much more
    // than needed for right now.
    if (!m_startedBuffering) {
        LOG_MEDIA_MESSAGE("[Buffering] Starting on-disk buffering.");

        m_startedBuffering = true;

        if (m_fillTimer.isActive())
            m_fillTimer.stop();

        m_fillTimer.startRepeating(0.2);
    }

    if (!m_paused && m_bufferingPercentage < 100) {
        LOG_MEDIA_MESSAGE("[Buffering] Download in progress, pausing pipeline.");
        gst_element_set_state(m_pipeline.get(), GST_STATE_PAUSED);
    }
}

void MediaPlayerPrivateGStreamer::fillTimerFired(Timer<MediaPlayerPrivateGStreamer>*)
{
    GstQuery* query = gst_query_new_buffering(GST_FORMAT_PERCENT);

    if (!gst_element_query(m_pipeline.get(), query)) {
        gst_query_unref(query);
        return;
    }

    gint64 start, stop;
    gdouble fillStatus = 100.0;

    gst_query_parse_buffering_range(query, 0, &start, &stop, 0);
    gst_query_unref(query);

    if (stop != -1)
        fillStatus = 100.0 * stop / GST_FORMAT_PERCENT_MAX;

    LOG_MEDIA_MESSAGE("[Buffering] Download buffer filled up to %f%%", fillStatus);

    if (!m_mediaDuration)
        durationChanged();

    // Update maxTimeLoaded only if the media duration is
    // available. Otherwise we can't compute it.
    if (m_mediaDuration) {
        if (fillStatus == 100.0)
            m_maxTimeLoaded = m_mediaDuration;
        else
            m_maxTimeLoaded = static_cast<float>((fillStatus * m_mediaDuration) / 100.0);
        LOG_MEDIA_MESSAGE("[Buffering] Updated maxTimeLoaded: %f", m_maxTimeLoaded);
    }

    if (fillStatus != 100.0) {
        updateStates();
        return;
    }

    // Media is now fully loaded. It will play even if network
    // connection is cut. Buffering is done, remove the fill source
    // from the main loop.
    m_fillTimer.stop();
    m_startedBuffering = false;
    updateStates();
}

float MediaPlayerPrivateGStreamer::maxTimeSeekable() const
{
    if (m_errorOccured)
        return 0.0f;

    LOG_MEDIA_MESSAGE("maxTimeSeekable");
    // infinite duration means live stream
    if (std::isinf(duration()))
        return 0.0f;

    return duration();
}

float MediaPlayerPrivateGStreamer::maxTimeLoaded() const
{
    if (m_errorOccured)
        return 0.0f;

    float loaded = m_maxTimeLoaded;
    if (!loaded && !m_fillTimer.isActive())
        loaded = duration();
    LOG_MEDIA_MESSAGE("maxTimeLoaded: %f", loaded);
    return loaded;
}

bool MediaPlayerPrivateGStreamer::didLoadingProgress() const
{
    if (!m_pipeline || !m_mediaDuration || !totalBytes())
        return false;
    float currentMaxTimeLoaded = maxTimeLoaded();
    bool didLoadingProgress = currentMaxTimeLoaded != m_maxTimeLoadedAtLastDidLoadingProgress;
    m_maxTimeLoadedAtLastDidLoadingProgress = currentMaxTimeLoaded;
    LOG_MEDIA_MESSAGE("didLoadingProgress: %d", didLoadingProgress);
    return didLoadingProgress;
}

unsigned MediaPlayerPrivateGStreamer::totalBytes() const
{
    if (m_errorOccured)
        return 0;

    if (m_totalBytes != -1)
        return m_totalBytes;

    if (!m_source)
        return 0;

    GstFormat fmt = GST_FORMAT_BYTES;
    gint64 length = 0;
#ifdef GST_API_VERSION_1
    if (gst_element_query_duration(m_source.get(), fmt, &length)) {
#else
    if (gst_element_query_duration(m_source.get(), &fmt, &length)) {
#endif
        LOG_MEDIA_MESSAGE("totalBytes %" G_GINT64_FORMAT, length);
        m_totalBytes = static_cast<unsigned>(length);
        m_isStreaming = !length;
        return m_totalBytes;
    }

    // Fall back to querying the source pads manually.
    // See also https://bugzilla.gnome.org/show_bug.cgi?id=638749
    GstIterator* iter = gst_element_iterate_src_pads(m_source.get());
    bool done = false;
    while (!done) {
#ifdef GST_API_VERSION_1
        GValue item = G_VALUE_INIT;
        switch (gst_iterator_next(iter, &item)) {
        case GST_ITERATOR_OK: {
            GstPad* pad = static_cast<GstPad*>(g_value_get_object(&item));
            gint64 padLength = 0;
            if (gst_pad_query_duration(pad, fmt, &padLength) && padLength > length)
                length = padLength;
            break;
        }
#else
        gpointer data;

        switch (gst_iterator_next(iter, &data)) {
        case GST_ITERATOR_OK: {
            GRefPtr<GstPad> pad = adoptGRef(GST_PAD_CAST(data));
            gint64 padLength = 0;
            if (gst_pad_query_duration(pad.get(), &fmt, &padLength) && padLength > length)
                length = padLength;
            break;
        }
#endif
        case GST_ITERATOR_RESYNC:
            gst_iterator_resync(iter);
            break;
        case GST_ITERATOR_ERROR:
            // Fall through.
        case GST_ITERATOR_DONE:
            done = true;
            break;
        }

#ifdef GST_API_VERSION_1
        g_value_unset(&item);
#endif
    }

    gst_iterator_free(iter);

    LOG_MEDIA_MESSAGE("totalBytes %" G_GINT64_FORMAT, length);
    m_totalBytes = static_cast<unsigned>(length);
    m_isStreaming = !length;
    return m_totalBytes;
}

void MediaPlayerPrivateGStreamer::updateAudioSink()
{
    if (!m_sink)
        return;

    GstElement* sinkPtr = 0;

    g_object_get(m_sink.get(), "audio-sink", &sinkPtr, NULL);
#ifdef GST_API_VERSION_1
    m_webkitAudioSink = adoptGRef(sinkPtr);
#else
    m_webkitAudioSink = sinkPtr;
#endif
}

GstElement* MediaPlayerPrivateGStreamer::audioSink() const
{
    return m_webkitAudioSink.get();
}

void MediaPlayerPrivateGStreamer::sourceChanged()
{
    GstElement* srcPtr = 0;

    g_object_get(m_decodebin.get(), "source", &srcPtr, NULL);
    m_source = adoptGRef(srcPtr);

    if (WEBKIT_IS_WEB_SRC(m_source.get()))
        webKitWebSrcSetMediaPlayer(WEBKIT_WEB_SRC(m_source.get()), m_player);
}

void MediaPlayerPrivateGStreamer::cancelLoad()
{
    if (m_networkState < MediaPlayer::Loading || m_networkState == MediaPlayer::Loaded)
        return;

    if (m_pipeline)
        gst_element_set_state(m_pipeline.get(), GST_STATE_NULL);
}

void MediaPlayerPrivateGStreamer::updateStates()
{
    if (!m_pipeline)
        return;

    if (m_errorOccured)
        return;

    MediaPlayer::NetworkState oldNetworkState = m_networkState;
    MediaPlayer::ReadyState oldReadyState = m_readyState;
    GstState state;
    GstState pending;

    GstStateChangeReturn ret = gst_element_get_state(m_pipeline.get(),
        &state, &pending, 250 * GST_NSECOND);

    bool shouldUpdateAfterSeek = false;
    bool shouldUpdatePlaybackState = false;
    switch (ret) {
    case GST_STATE_CHANGE_SUCCESS:
        LOG_MEDIA_MESSAGE("State: %s, pending: %s",
            gst_element_state_get_name(state),
            gst_element_state_get_name(pending));

        m_resetPipeline = state <= GST_STATE_READY;

        // Try to figure out ready and network states.
        if (state == GST_STATE_READY) {
            m_readyState = MediaPlayer::HaveMetadata;
            m_networkState = MediaPlayer::Empty;
            // Cache the duration without emiting the durationchange
            // event because it's taken care of by the media element
            // in this precise case.
            if (!m_isEndReached)
                cacheDuration();
        } else if ((state == GST_STATE_NULL) || (maxTimeLoaded() == duration())) {
            m_networkState = MediaPlayer::Loaded;
            m_readyState = MediaPlayer::HaveEnoughData;
        } else {
            m_readyState = currentTime() < maxTimeLoaded() ? MediaPlayer::HaveFutureData : MediaPlayer::HaveCurrentData;
            m_networkState = MediaPlayer::Loading;
        }

        if (m_buffering && state != GST_STATE_READY) {
            m_readyState = MediaPlayer::HaveCurrentData;
            m_networkState = MediaPlayer::Loading;
        }

        // Now let's try to get the states in more detail using
        // information from GStreamer, while we sync states where
        // needed.
        if (state == GST_STATE_PAUSED) {
            if (!m_webkitAudioSink)
                updateAudioSink();

            if (!m_volumeAndMuteInitialized) {
                notifyPlayerOfVolumeChange();
                notifyPlayerOfMute();
                m_volumeAndMuteInitialized = true;
            }

            if (m_buffering && m_bufferingPercentage == 100) {
                m_buffering = false;
                m_bufferingPercentage = 0;
                m_readyState = MediaPlayer::HaveEnoughData;

                LOG_MEDIA_MESSAGE("[Buffering] Complete.");

                if (!m_paused) {
                    LOG_MEDIA_MESSAGE("[Buffering] Restarting playback.");
                    gst_element_set_state(m_pipeline.get(), GST_STATE_PLAYING);
                }
            } else if (!m_buffering && (currentTime() < duration())) {
                m_paused = true;
            }
        } else if (state == GST_STATE_PLAYING) {
            m_readyState = MediaPlayer::HaveEnoughData;
            m_paused = false;

            if (m_buffering && !isLiveStream()) {
                m_readyState = MediaPlayer::HaveCurrentData;
                m_networkState = MediaPlayer::Loading;

                LOG_MEDIA_MESSAGE("[Buffering] Pausing stream for buffering.");

                gst_element_set_state(m_pipeline.get(), GST_STATE_PAUSED);
            }
        } else
            m_paused = true;

        // Is on-disk buffering in progress?
        if (m_fillTimer.isActive())
            m_networkState = MediaPlayer::Loading;

        if (m_changingRate) {
            m_player->rateChanged();
            m_changingRate = false;
        }

        if (m_seeking) {
            shouldUpdateAfterSeek = true;
            m_seeking = false;
        }

        if (m_requestedState == GST_STATE_PAUSED && state == GST_STATE_PAUSED) {
            shouldUpdatePlaybackState = true;
            LOG_MEDIA_MESSAGE("Requested state change to %s was completed", gst_element_state_get_name(state));
        }

        break;
    case GST_STATE_CHANGE_ASYNC:
        LOG_MEDIA_MESSAGE("Async: State: %s, pending: %s",
            gst_element_state_get_name(state),
            gst_element_state_get_name(pending));
        // Change in progress

        // On-disk buffering was attempted but the media is live. This
        // can't work so disable on-disk buffering and reset the
        // pipeline.
        if (state == GST_STATE_READY && isLiveStream() && m_preload == MediaPlayer::Auto) {
            setPreload(MediaPlayer::None);
            gst_element_set_state(m_pipeline.get(), GST_STATE_NULL);
            gst_element_set_state(m_pipeline.get(), GST_STATE_PAUSED);
        }

        // A live stream was paused, reset the pipeline.
        if (state == GST_STATE_PAUSED && pending == GST_STATE_PLAYING && isLiveStream()) {
            gst_element_set_state(m_pipeline.get(), GST_STATE_NULL);
            gst_element_set_state(m_pipeline.get(), GST_STATE_PLAYING);
        }

        if (!isLiveStream() && !m_buffering)
            return;

        if (m_seeking) {
            shouldUpdateAfterSeek = true;
            m_seeking = false;
        }
        break;
    case GST_STATE_CHANGE_FAILURE:
        LOG_MEDIA_MESSAGE("Failure: State: %s, pending: %s",
            gst_element_state_get_name(state),
            gst_element_state_get_name(pending));
        // Change failed
        return;
    case GST_STATE_CHANGE_NO_PREROLL:
        LOG_MEDIA_MESSAGE("No preroll: State: %s, pending: %s",
            gst_element_state_get_name(state),
            gst_element_state_get_name(pending));

        if (state == GST_STATE_READY)
            m_readyState = MediaPlayer::HaveNothing;
        else if (state == GST_STATE_PAUSED) {
            m_readyState = MediaPlayer::HaveEnoughData;
            m_paused = true;
            // Live pipelines go in PAUSED without prerolling.
            m_isStreaming = true;
        } else if (state == GST_STATE_PLAYING)
            m_paused = false;

        if (m_seeking) {
            shouldUpdateAfterSeek = true;
            m_seeking = false;
            if (!m_paused)
                gst_element_set_state(m_pipeline.get(), GST_STATE_PLAYING);
        } else if (!m_paused)
            gst_element_set_state(m_pipeline.get(), GST_STATE_PLAYING);

        m_networkState = MediaPlayer::Loading;
        break;
    default:
        LOG_MEDIA_MESSAGE("Else : %d", ret);
        break;
    }

    m_requestedState = GST_STATE_VOID_PENDING;

    if (seeking())
        m_readyState = MediaPlayer::HaveNothing;

    if (shouldUpdateAfterSeek)
        timeChanged();

    if (shouldUpdatePlaybackState)
        m_player->playbackStateChanged();

    if (m_networkState != oldNetworkState) {
        LOG_MEDIA_MESSAGE("Network State Changed from %u to %u",
            oldNetworkState, m_networkState);
        m_player->networkStateChanged();
    }
    if (m_readyState != oldReadyState) {
        LOG_MEDIA_MESSAGE("Ready State Changed from %u to %u",
            oldReadyState, m_readyState);
        m_player->readyStateChanged();
    }
}

void MediaPlayerPrivateGStreamer::mediaLocationChanged(GstMessage* message)
{
    if (m_mediaLocations)
        gst_structure_free(m_mediaLocations);

    const GstStructure* structure = gst_message_get_structure(message);
    if (structure) {
        // This structure can contain:
        // - both a new-location string and embedded locations structure
        // - or only a new-location string.
        m_mediaLocations = gst_structure_copy(structure);
        const GValue* locations = gst_structure_get_value(m_mediaLocations, "locations");

        if (locations)
            m_mediaLocationCurrentIndex = static_cast<int>(gst_value_list_get_size(locations)) -1;

        loadNextLocation();
    }
}

bool MediaPlayerPrivateGStreamer::loadNextLocation()
{
    if (!m_mediaLocations)
        return false;

    const GValue* locations = gst_structure_get_value(m_mediaLocations, "locations");
    const gchar* newLocation = 0;

    if (!locations) {
        // Fallback on new-location string.
        newLocation = gst_structure_get_string(m_mediaLocations, "new-location");
        if (!newLocation)
            return false;
    }

    if (!newLocation) {
        if (m_mediaLocationCurrentIndex < 0) {
            m_mediaLocations = 0;
            return false;
        }

        const GValue* location = gst_value_list_get_value(locations,
                                                          m_mediaLocationCurrentIndex);
        const GstStructure* structure = gst_value_get_structure(location);

        if (!structure) {
            m_mediaLocationCurrentIndex--;
            return false;
        }

        newLocation = gst_structure_get_string(structure, "new-location");
    }

    if (newLocation) {
        // Found a candidate. new-location is not always an absolute url
        // though. We need to take the base of the current url and
        // append the value of new-location to it.
        KURL baseUrl = gst_uri_is_valid(newLocation) ? KURL() : m_url;
        KURL newUrl = KURL(baseUrl, newLocation);

        RefPtr<SecurityOrigin> securityOrigin = SecurityOrigin::create(m_url);
        if (securityOrigin->canRequest(newUrl)) {
            LOG_MEDIA_MESSAGE("New media url: %s", newUrl.string().utf8().data());

            // Reset player states.
            m_networkState = MediaPlayer::Loading;
            m_player->networkStateChanged();
            m_readyState = MediaPlayer::HaveNothing;
            m_player->readyStateChanged();

            // Reset pipeline state.
            m_resetPipeline = true;
            gst_element_set_state(m_pipeline.get(), GST_STATE_READY);

            GstState state;
            gst_element_get_state(m_pipeline.get(), &state, 0, 0);
            if (state <= GST_STATE_READY) {
                // Set the new uri and start playing.
                g_object_set(m_pipeline.get(), "uri", newUrl.string().utf8().data(), NULL);
                m_url = newUrl;
                gst_element_set_state(m_pipeline.get(), GST_STATE_PLAYING);
                return true;
            }
        } else
            LOG_MEDIA_MESSAGE("Not allowed to load new media location: %s", newUrl.string().utf8().data());
    }
    m_mediaLocationCurrentIndex--;
    return false;

}

void MediaPlayerPrivateGStreamer::loadStateChanged()
{
    updateStates();
}

void MediaPlayerPrivateGStreamer::timeChanged()
{
    updateStates();
    m_player->timeChanged();
}

void MediaPlayerPrivateGStreamer::didEnd()
{
    // Synchronize position and duration values to not confuse the
    // HTMLMediaElement. In some cases like reverse playback the
    // position is not always reported as 0 for instance.
    float now = currentTime();
    if (now > 0 && now <= duration() && m_mediaDuration != now) {
        m_mediaDurationKnown = true;
        m_mediaDuration = now;
        m_player->durationChanged();
    }

    m_isEndReached = true;
    timeChanged();

    if (!m_player->mediaPlayerClient()->mediaPlayerIsLooping()) {
        m_paused = true;
        gst_element_set_state(m_pipeline.get(), GST_STATE_NULL);
    }
}

void MediaPlayerPrivateGStreamer::cacheDuration()
{
    // Reset cached media duration
    m_mediaDuration = 0;

    // And re-cache it if possible.
    GstState state;
    gst_element_get_state(m_pipeline.get(), &state, 0, 0);
    float newDuration = duration();

    if (state <= GST_STATE_READY) {
        // Don't set m_mediaDurationKnown yet if the pipeline is not
        // paused. This allows duration() query to fail at least once
        // before playback starts and duration becomes known.
        if (!std::isinf(newDuration))
            m_mediaDuration = newDuration;
    } else {
        m_mediaDurationKnown = !std::isinf(newDuration);
        if (m_mediaDurationKnown)
            m_mediaDuration = newDuration;
    }

    if (!std::isinf(newDuration))
        m_mediaDuration = newDuration;
}

void MediaPlayerPrivateGStreamer::durationChanged()
{
    float previousDuration = m_mediaDuration;

    cacheDuration();
    // Avoid emiting durationchanged in the case where the previous
    // duration was 0 because that case is already handled by the
    // HTMLMediaElement.
    if (previousDuration && m_mediaDuration != previousDuration)
        m_player->durationChanged();
}

void MediaPlayerPrivateGStreamer::loadingFailed(MediaPlayer::NetworkState error)
{
    m_errorOccured = true;
    if (m_networkState != error) {
        m_networkState = error;
        m_player->networkStateChanged();
    }
    if (m_readyState != MediaPlayer::HaveNothing) {
        m_readyState = MediaPlayer::HaveNothing;
        m_player->readyStateChanged();
    }
}

static HashSet<String> mimeTypeCache()
{
    initializeGStreamerAndRegisterWebKitElements();

    DEFINE_STATIC_LOCAL(HashSet<String>, cache, ());
    static bool typeListInitialized = false;

    if (typeListInitialized)
        return cache;

    const char* mimeTypes[] = {
        "application/ogg",
        "application/vnd.apple.mpegurl",
        "application/vnd.rn-realmedia",
        "application/x-3gp",
        "application/x-pn-realaudio",
        "audio/3gpp",
        "audio/aac",
        "audio/flac",
        "audio/iLBC-sh",
        "audio/midi",
        "audio/mobile-xmf",
        "audio/mp1",
        "audio/mp2",
        "audio/mp3",
        "audio/mp4",
        "audio/mpeg",
        "audio/ogg",
        "audio/opus",
        "audio/qcelp",
        "audio/riff-midi",
        "audio/speex",
        "audio/wav",
        "audio/webm",
        "audio/x-ac3",
        "audio/x-aiff",
        "audio/x-amr-nb-sh",
        "audio/x-amr-wb-sh",
        "audio/x-au",
        "audio/x-ay",
        "audio/x-celt",
        "audio/x-dts",
        "audio/x-flac",
        "audio/x-gbs",
        "audio/x-gsm",
        "audio/x-gym",
        "audio/x-imelody",
        "audio/x-ircam",
        "audio/x-kss",
        "audio/x-m4a",
        "audio/x-mod",
        "audio/x-mp3",
        "audio/x-mpeg",
        "audio/x-musepack",
        "audio/x-nist",
        "audio/x-nsf",
        "audio/x-paris",
        "audio/x-sap",
        "audio/x-sbc",
        "audio/x-sds",
        "audio/x-shorten",
        "audio/x-sid",
        "audio/x-spc",
        "audio/x-speex",
        "audio/x-svx",
        "audio/x-ttafile",
        "audio/x-vgm",
        "audio/x-voc",
        "audio/x-vorbis+ogg",
        "audio/x-w64",
        "audio/x-wav",
        "audio/x-wavpack",
        "audio/x-wavpack-correction",
        "video/3gpp",
        "video/mj2",
        "video/mp4",
        "video/mpeg",
        "video/mpegts",
        "video/ogg",
        "video/quicktime",
        "video/vivo",
        "video/webm",
        "video/x-cdxa",
        "video/x-dirac",
        "video/x-dv",
        "video/x-fli",
        "video/x-flv",
        "video/x-h263",
        "video/x-ivf",
        "video/x-m4v",
        "video/x-matroska",
        "video/x-mng",
        "video/x-ms-asf",
        "video/x-msvideo",
        "video/x-mve",
        "video/x-nuv",
        "video/x-vcd"
    };

    for (unsigned i = 0; i < (sizeof(mimeTypes) / sizeof(*mimeTypes)); ++i)
        cache.add(String(mimeTypes[i]));

    typeListInitialized = true;
    return cache;
}

void MediaPlayerPrivateGStreamer::getSupportedTypes(HashSet<String>& types)
{
    types = mimeTypeCache();
}

MediaPlayer::SupportsType MediaPlayerPrivateGStreamer::supportsType(const String& type, const String& codecs, const KURL&)
{
    if (type.isNull() || type.isEmpty())
        return MediaPlayer::IsNotSupported;

    // spec says we should not return "probably" if the codecs string is empty
    if (mimeTypeCache().contains(type))
        return codecs.isEmpty() ? MediaPlayer::MayBeSupported : MediaPlayer::IsSupported;
    return MediaPlayer::IsNotSupported;
}

void MediaPlayerPrivateGStreamer::setDownloadBuffering()
{
    if (!m_pipeline)
        return;

    if (m_preload == MediaPlayer::Auto) {
        LOG_MEDIA_MESSAGE("Enabling on-disk buffering");
        g_object_set(m_decodebin.get(), "download", true, NULL);
    } else {
        LOG_MEDIA_MESSAGE("Disabling on-disk buffering");
        g_object_set(m_decodebin.get(), "download", false, NULL);
    }
}

void MediaPlayerPrivateGStreamer::setPreload(MediaPlayer::Preload preload)
{
    if (preload == MediaPlayer::Auto && isLiveStream())
        return;

    m_preload = preload;

    setDownloadBuffering();

    if (m_delayingLoad && m_preload != MediaPlayer::None) {
        m_delayingLoad = false;
        commitLoad();
    }
}

void MediaPlayerPrivateGStreamer::createAudioSink()
{
    // Construct audio sink if pitch preserving is enabled.
    if (!m_preservesPitch)
        return;

    if (!m_pipeline)
        return;

    GstElement* scale = gst_element_factory_make("scaletempo", 0);
    if (!scale) {
        GST_WARNING("Failed to create scaletempo");
        return;
    }

    GstElement* convert = gst_element_factory_make("audioconvert", 0);
    GstElement* resample = gst_element_factory_make("audioresample", 0);
    GstElement* sink = gst_element_factory_make("autoaudiosink", 0);

    m_autoAudioSink = sink;

    g_signal_connect(sink, "child-added", G_CALLBACK(setAudioStreamPropertiesCallback), this);

    GstElement* audioSink = gst_bin_new("audio-sink");
    gst_bin_add_many(GST_BIN(audioSink), scale, convert, resample, sink, NULL);

    if (!gst_element_link_many(scale, convert, resample, sink, NULL)) {
        GST_WARNING("Failed to link audio sink elements");
        gst_object_unref(audioSink);
        return;
    }

    GRefPtr<GstPad> pad = adoptGRef(gst_element_get_static_pad(scale, "sink"));
    gst_element_add_pad(audioSink, gst_ghost_pad_new("sink", pad.get()));

    g_object_set(m_sink.get(), "audio-sink", audioSink, NULL);
}

void MediaPlayerPrivateGStreamer::createPipeline()
{
    ASSERT(!m_pipeline);

    // gst_element_factory_make() returns a floating reference so
    // we should not adopt.
    m_pipeline = gst_pipeline_new("pipeline");

    GRefPtr<GstBus> bus = webkitGstPipelineGetBus(GST_PIPELINE(m_pipeline.get()));
    gst_bus_add_signal_watch(bus.get());
    g_signal_connect(bus.get(), "message", G_CALLBACK(mediaPlayerPrivateMessageCallback), this);

    m_decodebin = gst_element_factory_make("uridecodebin", "decode");
    g_signal_connect(m_decodebin.get(), "pad-added", G_CALLBACK(mediaPlayerPrivatePadAddedCallback), this);
    g_signal_connect(m_decodebin.get(), "pad-removed", G_CALLBACK(mediaPlayerPrivatePadRemovedCallback), this);

    m_sink = gst_element_factory_make("playsink", "sink");
    gst_util_set_object_arg(G_OBJECT(m_sink.get()), "flags", "soft-colorbalance+soft-volume+audio+video");

    gst_bin_add_many(GST_BIN(m_pipeline.get()), m_decodebin.get(), m_sink.get(), NULL);

    setStreamVolumeElement(m_sink.get());

    g_signal_connect(m_decodebin.get(), "source-setup", G_CALLBACK(mediaPlayerPrivateSourceChangedCallback), this);

    GstElement* videoElement = createVideoSink(m_pipeline.get());

    g_object_set(m_sink.get(), "video-sink", videoElement, NULL);

    GRefPtr<GstPad> videoSinkPad = adoptGRef(gst_element_get_static_pad(m_webkitVideoSink.get(), "sink"));
    if (videoSinkPad)
        g_signal_connect(videoSinkPad.get(), "notify::caps", G_CALLBACK(mediaPlayerPrivateVideoSinkCapsChangedCallback), this);

    createAudioSink();
}

void MediaPlayerPrivateGStreamer::simulateAudioInterruption()
{
    GstMessage* message = gst_message_new_request_state(GST_OBJECT(m_pipeline.get()), GST_STATE_PAUSED);
    gst_element_post_message(m_pipeline.get(), message);
}

}

#endif // USE(GSTREAMER)
