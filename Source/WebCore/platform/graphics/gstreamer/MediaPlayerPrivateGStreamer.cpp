/*
 * Copyright (C) 2007, 2009 Apple Inc.  All rights reserved.
 * Copyright (C) 2007 Collabora Ltd.  All rights reserved.
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 * Copyright (C) 2009 Gustavo Noronha Silva <gns@gnome.org>
 * Copyright (C) 2009, 2010 Igalia S.L
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

#include "ColorSpace.h"
#include "Document.h"
#include "Frame.h"
#include "FrameView.h"
#include "GStreamerGWorld.h"
#include "GStreamerUtilities.h"
#include "GStreamerVersioning.h"
#include "GraphicsContext.h"
#include "GraphicsTypes.h"
#include "ImageGStreamer.h"
#include "ImageOrientation.h"
#include "IntRect.h"
#include "KURL.h"
#include "MIMETypeRegistry.h"
#include "MediaPlayer.h"
#include "NotImplemented.h"
#include "SecurityOrigin.h"
#include "TimeRanges.h"
#include "VideoSinkGStreamer.h"
#include "WebKitWebSourceGStreamer.h"
#include <gst/gst.h>
#include <gst/video/video.h>
#include <limits>
#include <math.h>
#include <wtf/gobject/GOwnPtr.h>
#include <wtf/text/CString.h>

#ifdef GST_API_VERSION_1
#include <gst/audio/streamvolume.h>
#else
#include <gst/interfaces/streamvolume.h>
#endif

#if ENABLE(VIDEO_TRACK)
#include "VideoTrack.h"
#include "VideoTrackList.h"
#endif

// GstPlayFlags flags from playbin2. It is the policy of GStreamer to
// not publicly expose element-specific enums. That's why this
// GstPlayFlags enum has been copied here.
typedef enum {
    GST_PLAY_FLAG_VIDEO         = 0x00000001,
    GST_PLAY_FLAG_AUDIO         = 0x00000002,
    GST_PLAY_FLAG_TEXT          = 0x00000004,
    GST_PLAY_FLAG_VIS           = 0x00000008,
    GST_PLAY_FLAG_SOFT_VOLUME   = 0x00000010,
    GST_PLAY_FLAG_NATIVE_AUDIO  = 0x00000020,
    GST_PLAY_FLAG_NATIVE_VIDEO  = 0x00000040,
    GST_PLAY_FLAG_DOWNLOAD      = 0x00000080,
    GST_PLAY_FLAG_BUFFERING     = 0x000000100
} GstPlayFlags;

#ifdef GST_API_VERSION_1
static const char* gPlaybinName = "playbin";
#else
static const char* gPlaybinName = "playbin2";
#endif

GST_DEBUG_CATEGORY_STATIC(webkit_media_player_debug);
#define GST_CAT_DEFAULT webkit_media_player_debug

using namespace std;

namespace WebCore {

static int greatestCommonDivisor(int a, int b)
{
    while (b) {
        int temp = a;
        a = b;
        b = temp % b;
    }

    return ABS(a);
}

static gboolean mediaPlayerPrivateMessageCallback(GstBus*, GstMessage* message, MediaPlayerPrivateGStreamer* player)
{
    return player->handleMessage(message);
}

static void mediaPlayerPrivateSourceChangedCallback(GObject*, GParamSpec*, MediaPlayerPrivateGStreamer* player)
{
    player->sourceChanged();
}

static void mediaPlayerPrivateVolumeChangedCallback(GObject*, GParamSpec*, MediaPlayerPrivateGStreamer* player)
{
    // This is called when playbin receives the notify::volume signal.
    player->volumeChanged();
}

static gboolean mediaPlayerPrivateVolumeChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    // This is the callback of the timeout source created in ::volumeChanged.
    player->notifyPlayerOfVolumeChange();
    return FALSE;
}

static void mediaPlayerPrivateMuteChangedCallback(GObject*, GParamSpec*, MediaPlayerPrivateGStreamer* player)
{
    // This is called when playbin receives the notify::mute signal.
    player->muteChanged();
}

static gboolean mediaPlayerPrivateMuteChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    // This is the callback of the timeout source created in ::muteChanged.
    player->notifyPlayerOfMute();
    return FALSE;
}

static void mediaPlayerPrivateVideoSinkCapsChangedCallback(GObject*, GParamSpec*, MediaPlayerPrivateGStreamer* player)
{
    player->videoCapsChanged();
}

static void mediaPlayerPrivateVideoChangedCallback(GObject*, MediaPlayerPrivateGStreamer* player)
{
    player->videoChanged();
    player->videoCapsChanged();
}

static void mediaPlayerPrivateAudioChangedCallback(GObject*, MediaPlayerPrivateGStreamer* player)
{
    player->audioChanged();
}

#if ENABLE(VIDEO_TRACK)
static void mediaPlayerPrivateTextChangedCallback(GObject*, MediaPlayerPrivateGStreamer* player)
{
    player->textChanged();
}

static gboolean mediaPlayerPrivateTextChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    // This is the callback of the timeout source created in ::textChanged.
    player->notifyPlayerOfText();
    return FALSE;
}

static void mediaPlayerPrivateTextTagsChangedCallback(GObject*, gint index, MediaPlayerPrivateGStreamer* player)
{
    player->textTagsChanged();
}

static gboolean mediaPlayerPrivateTextTagChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    // This is the callback of the timeout source created in ::textChanged.
    player->notifyPlayerOfTextTags();
    return FALSE;
}

static void mediaPlayerPrivateTextBufferChangedCallback(GstPad* pad, GstBuffer* buffer, MediaPlayerPrivateGStreamer* player)
{
	player->textBufferChanged(pad, buffer);
}

static void mediaPlayerPrivateTextBufferChangedTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
	player->notifyPlayerOfTextBuffer();
}
#endif

static void mediaPlayerPrivateVideoTagsChangedCallback(GObject* m_playBin, gint i, MediaPlayerPrivateGStreamer* player)
{
    // This checks every video track for new tags, even though we only need to
    // check tag i. I'd like to do this more efficiently, but it would require
    // either a mutex or some complicated code using atomic pointers. It seemed
    // like that complexity wasn't worthwhile.
    player->videoTagsChanged();
}

static void mediaPlayerPrivateAudioTagsChangedCallback(GObject* m_playBin, gint i, MediaPlayerPrivateGStreamer* player)
{
    // This checks every audio track for new tags, even though we only need to
    // check tag i. I'd like to do this more efficiently, but it would require
    // either a mutex or some complicated code using atomic pointers. It seemed
    // like that complexity wasn't worthwhile.
    player->audioTagsChanged();
}

static gboolean mediaPlayerPrivateVideoChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    // This is the callback of the timeout source created in ::videoChanged.
    player->notifyPlayerOfVideo();
    return FALSE;
}

static gboolean mediaPlayerPrivateAudioChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    // This is the callback of the timeout source created in ::audioChanged.
    player->notifyPlayerOfAudio();
    return FALSE;
}

static gboolean mediaPlayerPrivateVideoCapsChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    // This is the callback of the timeout source created in ::videoCapsChanged.
    player->notifyPlayerOfVideoCaps();
    return FALSE;
}

static gboolean mediaPlayerPrivateVideoTagsChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    player->notifyPlayerOfVideoTags();
    return FALSE;
}

static gboolean mediaPlayerPrivateAudioTagsChangeTimeoutCallback(MediaPlayerPrivateGStreamer* player)
{
    player->notifyPlayerOfAudioTags();
    return FALSE;
}

static void mediaPlayerPrivateRepaintCallback(WebKitVideoSink*, GstBuffer *buffer, MediaPlayerPrivateGStreamer* playerPrivate)
{
    playerPrivate->triggerRepaint(buffer);
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

    GRefPtr<GstElementFactory> factory = gst_element_factory_find(gPlaybinName);
    return factory;
}

MediaPlayerPrivateGStreamer::MediaPlayerPrivateGStreamer(MediaPlayer* player)
    : m_player(player)
    , m_playBin(0)
    , m_webkitVideoSink(0)
    , m_fpsSink(0)
    , m_source(0)
    , m_seekTime(0)
    , m_changingRate(false)
    , m_endTime(numeric_limits<float>::infinity())
    , m_isEndReached(false)
    , m_networkState(MediaPlayer::Empty)
    , m_readyState(MediaPlayer::HaveNothing)
    , m_isStreaming(false)
    , m_size(IntSize())
    , m_buffer(0)
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
    , m_preload(MediaPlayer::Auto)
    , m_delayingLoad(false)
    , m_mediaDurationKnown(true)
    , m_maxTimeLoadedAtLastDidLoadingProgress(0)
    , m_volumeTimerHandler(0)
    , m_muteTimerHandler(0)
    , m_hasVideo(false)
#if ENABLE(VIDEO_TRACK)
    , m_hasText(false)
#endif
    , m_hasAudio(false)
    , m_audioTimerHandler(0)
    , m_audioTagsTimerHandler(0)
#if ENABLE(VIDEO_TRACK)
	, m_textBufferTimerHandler(0)
    , m_textTimerHandler(0)
    , m_textTagsTimerHandler(0)
#endif
    , m_videoTimerHandler(0)
    , m_videoCapsTimerHandler(0)
    , m_videoTagsTimerHandler(0)
    , m_webkitAudioSink(0)
    , m_totalBytes(-1)
    , m_originalPreloadWasAutoAndWasOverridden(false)
{
    if (initializeGStreamerAndRegisterWebKitElements())
        createGSTPlayBin();
}

MediaPlayerPrivateGStreamer::~MediaPlayerPrivateGStreamer()
{
    if (m_fillTimer.isActive())
        m_fillTimer.stop();

    if (m_buffer)
        gst_buffer_unref(m_buffer);

    m_buffer = 0;

    if (m_mediaLocations) {
        gst_structure_free(m_mediaLocations);
        m_mediaLocations = 0;
    }

#ifndef GST_API_VERSION_1
    if (m_videoSinkBin) {
        gst_object_unref(m_videoSinkBin);
        m_videoSinkBin = 0;
    }
#endif

    if (m_playBin) {
        gst_element_set_state(m_playBin, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(m_playBin));
        m_playBin = 0;
    }

    m_player = 0;

    if (m_muteTimerHandler)
        g_source_remove(m_muteTimerHandler);

    if (m_volumeTimerHandler)
        g_source_remove(m_volumeTimerHandler);

    if (m_videoTimerHandler)
        g_source_remove(m_videoTimerHandler);

    if (m_videoCapsTimerHandler)
        g_source_remove(m_videoCapsTimerHandler);

    if (m_videoTagsTimerHandler)
        g_source_remove(m_videoTagsTimerHandler);

    if (m_audioTimerHandler)
        g_source_remove(m_audioTimerHandler);

    if (m_audioTagsTimerHandler)
        g_source_remove(m_audioTagsTimerHandler);

    if (m_textBufferTimerHandler)
        g_source_remove(m_textBufferTimerHandler);

    if (m_textTimerHandler)
        g_source_remove(m_textTimerHandler);

    if (m_textTagsTimerHandler)
        g_source_remove(m_textTagsTimerHandler);
}

void MediaPlayerPrivateGStreamer::load(const String& url)
{

    KURL kurl(KURL(), url);
    String cleanUrl(url);

    // Clean out everything after file:// url path.
    if (kurl.isLocalFile())
        cleanUrl = cleanUrl.substring(0, kurl.pathEnd());

    m_url = KURL(KURL(), cleanUrl);
    g_object_set(m_playBin, "uri", cleanUrl.utf8().data(), NULL);

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

    // GStreamer needs to have the pipeline set to a paused state to
    // start providing anything useful.
    gst_element_set_state(m_playBin, GST_STATE_PAUSED);

    if (!m_delayingLoad)
        commitLoad();
}

void MediaPlayerPrivateGStreamer::commitLoad()
{
    ASSERT(!m_delayingLoad);
    LOG_MEDIA_MESSAGE("Committing load.");
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

    GstQuery* query = gst_query_new_position(GST_FORMAT_TIME);
    if (!gst_element_query(m_playBin, query)) {
        LOG_MEDIA_MESSAGE("Position query failed...");
        gst_query_unref(query);
        return ret;
    }

    gint64 position;
    gst_query_parse_position(query, 0, &position);

    // Position is available only if the pipeline is not in GST_STATE_NULL or
    // GST_STATE_READY state.
    if (position != static_cast<gint64>(GST_CLOCK_TIME_NONE))
        ret = static_cast<double>(position) / GST_SECOND;

    LOG_MEDIA_MESSAGE("Position %" GST_TIME_FORMAT, GST_TIME_ARGS(position));

    gst_query_unref(query);

    return ret;
}

bool MediaPlayerPrivateGStreamer::changePipelineState(GstState newState)
{
    ASSERT(newState == GST_STATE_PLAYING || newState == GST_STATE_PAUSED);

    GstState currentState;
    GstState pending;

    gst_element_get_state(m_playBin, &currentState, &pending, 0);
    LOG_MEDIA_MESSAGE("Current state: %s, pending: %s", gst_element_state_get_name(currentState), gst_element_state_get_name(pending));
    if (currentState == newState || pending == newState)
        return true;

    GstStateChangeReturn setStateResult = gst_element_set_state(m_playBin, newState);
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

bool MediaPlayerPrivateGStreamer::isAudioEnabled(int checkTrack) const
{
    GstPlayFlags flags;
    g_object_get(m_playBin, "flags", &flags, NULL);
    if(!(flags & GST_PLAY_FLAG_AUDIO)) {
        return false;
    }
    gint currentTrack;
    g_object_get(m_playBin, "current-audio", &currentTrack, NULL);
    return currentTrack == checkTrack;
}

void MediaPlayerPrivateGStreamer::setAudioEnabled(int track, bool enabled)
{
    // Check if the requested track is already in the right state
    if(enabled == isAudioEnabled(track))
        return;

    int flags;
    g_object_get(m_playBin, "flags", &flags, NULL);
    if(enabled) {
        g_object_set(m_playBin, "current-audio", track, NULL);
        flags |= GST_PLAY_FLAG_AUDIO;
    } else {
        flags &= ~GST_PLAY_FLAG_AUDIO;
    }
    g_object_set(m_playBin, "flags", flags, NULL);
}

bool MediaPlayerPrivateGStreamer::isVideoSelected(int checkTrack) const
{
    GstPlayFlags flags;
    g_object_get(m_playBin, "flags", &flags, NULL);
    if(!(flags & GST_PLAY_FLAG_VIDEO)) {
        return false;
    }
    gint currentTrack;
    g_object_get(m_playBin, "current-video", &currentTrack, NULL);
    return currentTrack == checkTrack;
}

void MediaPlayerPrivateGStreamer::setVideoSelected(int track, bool selected)
{
    // Check if the requested track is already in the right state
    if(selected == isVideoSelected(track))
        return;

    int flags;
    g_object_get(m_playBin, "flags", &flags, NULL);
    if(selected) {
        g_object_set(m_playBin, "current-video", track, NULL);
        flags |= GST_PLAY_FLAG_VIDEO;
    } else {
        // This doesn't seem to work
        flags &= ~GST_PLAY_FLAG_VIDEO;
    }
    g_object_set(m_playBin, "flags", flags, NULL);
}

bool MediaPlayerPrivateGStreamer::isTextEnabled(int checkTrack) const
{
	return false;
//    GstPlayFlags flags;
//    g_object_get(m_playBin, "flags", &flags, NULL);
//    if(!(flags & GST_PLAY_FLAG_TEXT)) {
//        return false;
//    }
//    gint currentTrack;
//    g_object_get(m_playBin, "current-text", &currentTrack, NULL);
//    return currentTrack == checkTrack;
}

void MediaPlayerPrivateGStreamer::setTextEnabled(int track, bool enabled)
{
//    // Check if the requested track is already in the right state
//    if(enabled == isTextEnabled(track))
//        return;
//
//	GstPad* pad = 0;
//	g_signal_emit_by_name(m_playBin, "get-text-pad", track, &pad);
//	if(!pad) {
//		g_print("Couldn't get text pad %d\n", track);
//		return;
//	} else {
//		g_print("Got text pad %p.\n", pad);
//	}
//
//    if(enabled) {
//    	m_textProbes[track] = gst_pad_add_buffer_probe(pad, G_CALLBACK(mediaPlayerPrivateTextNewBufferCallback), this);
//    } else if (m_textProbes[track]){
//    	gst_pad_remove_buffer_probe(pad, m_textProbes[track]);
//    	m_textProbes[track] = 0;
//    }
}

float MediaPlayerPrivateGStreamer::duration() const
{
    if (!m_playBin)
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
    bool failure = !gst_element_query_duration(m_playBin, timeFormat, &timeLength) || static_cast<guint64>(timeLength) == GST_CLOCK_TIME_NONE;
#else
    bool failure = !gst_element_query_duration(m_playBin, &timeFormat, &timeLength) || timeFormat != GST_FORMAT_TIME || static_cast<guint64>(timeLength) == GST_CLOCK_TIME_NONE;
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
    if (!m_playBin)
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
    if (!m_playBin)
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

    if (!gst_element_seek(m_playBin, m_player->rate(),
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
    gst_element_get_state(m_playBin, &state, 0, 0);
    return state == GST_STATE_PAUSED;
}

bool MediaPlayerPrivateGStreamer::seeking() const
{
    return m_seeking;
}

// Returns the size of the video
IntSize MediaPlayerPrivateGStreamer::naturalSize() const
{
    if (!hasVideo())
        return IntSize();

    if (!m_videoSize.isEmpty())
        return m_videoSize;

    GstCaps* caps = webkitGstGetPadCaps(m_videoSinkPad.get());
    if (!caps)
        return IntSize();


    // TODO: handle possible clean aperture data. See
    // https://bugzilla.gnome.org/show_bug.cgi?id=596571
    // TODO: handle possible transformation matrix. See
    // https://bugzilla.gnome.org/show_bug.cgi?id=596326

    // Get the video PAR and original size, if this fails the
    // video-sink has likely not yet negotiated its caps.
    int pixelAspectRatioNumerator, pixelAspectRatioDenominator, stride;
    IntSize originalSize;
    GstVideoFormat format;
    if (!getVideoSizeAndFormatFromCaps(caps, originalSize, format, pixelAspectRatioNumerator, pixelAspectRatioDenominator, stride))
        return IntSize();

    LOG_MEDIA_MESSAGE("Original video size: %dx%d", originalSize.width(), originalSize.height());
    LOG_MEDIA_MESSAGE("Pixel aspect ratio: %d/%d", pixelAspectRatioNumerator, pixelAspectRatioDenominator);

    // Calculate DAR based on PAR and video size.
    int displayWidth = originalSize.width() * pixelAspectRatioNumerator;
    int displayHeight = originalSize.height() * pixelAspectRatioDenominator;

    // Divide display width and height by their GCD to avoid possible overflows.
    int displayAspectRatioGCD = greatestCommonDivisor(displayWidth, displayHeight);
    displayWidth /= displayAspectRatioGCD;
    displayHeight /= displayAspectRatioGCD;

    // Apply DAR to original video size. This is the same behavior as in xvimagesink's setcaps function.
    guint64 width = 0, height = 0;
    if (!(originalSize.height() % displayHeight)) {
        LOG_MEDIA_MESSAGE("Keeping video original height");
        width = gst_util_uint64_scale_int(originalSize.height(), displayWidth, displayHeight);
        height = static_cast<guint64>(originalSize.height());
    } else if (!(originalSize.width() % displayWidth)) {
        LOG_MEDIA_MESSAGE("Keeping video original width");
        height = gst_util_uint64_scale_int(originalSize.width(), displayHeight, displayWidth);
        width = static_cast<guint64>(originalSize.width());
    } else {
        LOG_MEDIA_MESSAGE("Approximating while keeping original video height");
        width = gst_util_uint64_scale_int(originalSize.height(), displayWidth, displayHeight);
        height = static_cast<guint64>(originalSize.height());
    }

    LOG_MEDIA_MESSAGE("Natural size: %" G_GUINT64_FORMAT "x%" G_GUINT64_FORMAT, width, height);
    m_videoSize = IntSize(static_cast<int>(width), static_cast<int>(height));
    return m_videoSize;
}

void MediaPlayerPrivateGStreamer::videoCapsChanged()
{
    if (m_videoCapsTimerHandler)
        g_source_remove(m_videoCapsTimerHandler);
    m_videoCapsTimerHandler = g_idle_add(reinterpret_cast<GSourceFunc>(mediaPlayerPrivateVideoCapsChangeTimeoutCallback), this);
}

/**
 * This signal is emitted whenever the number or order of the video streams has
 * changed. The application will most likely want to select a new video stream.
 */
void MediaPlayerPrivateGStreamer::videoChanged()
{
    if (m_videoTimerHandler)
        g_source_remove(m_videoTimerHandler);
    m_videoTimerHandler = g_idle_add(reinterpret_cast<GSourceFunc>(mediaPlayerPrivateVideoChangeTimeoutCallback), this);
}

void MediaPlayerPrivateGStreamer::audioChanged()
{
    if (m_audioTimerHandler)
        g_source_remove(m_audioTimerHandler);
    m_audioTimerHandler = g_idle_add(reinterpret_cast<GSourceFunc>(mediaPlayerPrivateAudioChangeTimeoutCallback), this);
}

void MediaPlayerPrivateGStreamer::videoTagsChanged()
{
    if (m_videoTagsTimerHandler)
        g_source_remove(m_videoTagsTimerHandler);
    m_videoTagsTimerHandler = g_idle_add(reinterpret_cast<GSourceFunc>(mediaPlayerPrivateVideoTagsChangeTimeoutCallback), this);
}

void MediaPlayerPrivateGStreamer::audioTagsChanged()
{
    if (m_audioTagsTimerHandler)
        g_source_remove(m_audioTagsTimerHandler);
    m_audioTagsTimerHandler = g_idle_add(reinterpret_cast<GSourceFunc>(mediaPlayerPrivateAudioTagsChangeTimeoutCallback), this);
}

void MediaPlayerPrivateGStreamer::notifyPlayerOfVideo()
{
    m_videoTimerHandler = 0;

    gint numVideos = 0;
    if (m_playBin)
        g_object_get(m_playBin, "n-video", &numVideos, NULL);

    m_hasVideo = numVideos > 0;

#if ENABLE(VIDEO_TRACK)
    m_player->mediaPlayerClient()->mediaPlayerClearVideoTracks(m_player);
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

    gint numTracks = 0;
    if (m_playBin)
        g_object_get(m_playBin, "n-audio", &numTracks, NULL);
    m_hasAudio = numTracks > 0;

#if ENABLE(VIDEO_TRACK)
    m_player->mediaPlayerClient()->mediaPlayerClearAudioTracks(m_player);
#endif

    m_player->mediaPlayerClient()->mediaPlayerEngineUpdated(m_player);
}

void MediaPlayerPrivateGStreamer::notifyPlayerOfAudioTags()
{
    m_audioTagsTimerHandler = 0;
    gint numAudioTracks = 0;
    if (m_playBin)
        g_object_get(m_playBin, "n-audio", &numAudioTracks, NULL);

    for(gint i = 0; i < numAudioTracks; ++i) {
        GstTagList *tags = NULL;
        String language;
        String label;
        g_signal_emit_by_name(m_playBin, "get-audio-tags", i, &tags);
        if (tags) {
            gchar *str;
            if (gst_tag_list_get_string(tags, GST_TAG_LANGUAGE_CODE, &str)) {
                language = String(str);

                // The AudioTrack.language and VideoTrack.language attributes
                // must return the BCP 47 language tag of the language of the
                // track, if it has one, or the empty string otherwise.
                if(language == "und") {
                    language = "";
                }
                g_free(str);
            }
            if (gst_tag_list_get_string(tags, GST_TAG_TITLE, &str)) {
                label = String(str);
                g_free(str);
            }
            gst_tag_list_free(tags);
        }
        m_player->mediaPlayerClient()->mediaPlayerAddAudioTrack(m_player, i, isAudioEnabled(i), "", "", label, language);
    }
}

void MediaPlayerPrivateGStreamer::notifyPlayerOfVideoTags()
{
    m_videoTagsTimerHandler = 0;
    gint numVideoTracks = 0;
    if (m_playBin)
        g_object_get(m_playBin, "n-video", &numVideoTracks, NULL);

    for(gint i = 0; i < numVideoTracks; ++i) {
        GstTagList *tags = NULL;
        String language;
        String label;
        g_signal_emit_by_name(m_playBin, "get-video-tags", i, &tags);
        if (tags) {
            gchar *str;
            if (gst_tag_list_get_string(tags, GST_TAG_LANGUAGE_CODE, &str)) {
                language = String(str);

                // The AudioTrack.language and VideoTrack.language attributes
                // must return the BCP 47 language tag of the language of the
                // track, if it has one, or the empty string otherwise.
                if(language == "und") {
                    language = "";
                }
                g_free(str);
            }
            if (gst_tag_list_get_string(tags, GST_TAG_TITLE, &str)) {
                label = String(str);
                g_free(str);
            }
            gst_tag_list_free(tags);
        }
        m_player->mediaPlayerClient()->mediaPlayerAddVideoTrack(m_player, i, isVideoSelected(i), "", "", label, language);
    }
}

#if ENABLE(VIDEO_TRACK)

void MediaPlayerPrivateGStreamer::notifyPlayerOfText()
{
    m_textTimerHandler = 0;

    gint numTracks = 0;
    if (m_playBin)
        g_object_get(m_playBin, "n-text", &numTracks, NULL);

    m_hasText = numTracks > 0;

    // Remove old probes
    for(size_t i = 0; i < m_textProbes.size(); ++i) {
    	if(m_textProbes[i] && m_textPads[i]) {
			gst_pad_remove_buffer_probe(m_textPads[i], m_textProbes[i]);
    	}
    }

    // Setup new probes
    m_textProbes.assign(numTracks, 0);

    // This (size_t) cast is necessary to get the correct version of vector::assign
    m_textPads.assign((size_t)numTracks, 0);
    for(size_t i = 0; i < numTracks; ++i) {
    	g_signal_emit_by_name(m_playBin, "get-text-pad", i, &(m_textPads[i]));
    	m_textProbes[i] = gst_pad_add_buffer_probe(m_textPads[i], G_CALLBACK(mediaPlayerPrivateTextBufferChangedCallback), this);
    }

#if ENABLE(VIDEO_TRACK)
    m_player->mediaPlayerClient()->mediaPlayerClearTextTracks(m_player);
#endif

    m_player->mediaPlayerClient()->mediaPlayerEngineUpdated(m_player);
}

void MediaPlayerPrivateGStreamer::notifyPlayerOfTextTags()
{
    m_textTagsTimerHandler = 0;
    gint numTracks = 0;
    if (m_playBin)
        g_object_get(m_playBin, "n-text", &numTracks, NULL);

    for(gint i = 0; i < numTracks; ++i) {
        GstTagList *tags = NULL;
        String language;
        String label;
        g_signal_emit_by_name(m_playBin, "get-text-tags", i, &tags);
        if (tags) {
            gchar *str;
            if (gst_tag_list_get_string(tags, GST_TAG_LANGUAGE_CODE, &str)) {
                language = String(str);

                // The AudioTrack.language and VideoTrack.language attributes
                // must return the BCP 47 language tag of the language of the
                // track, if it has one, or the empty string otherwise.
                if(language == "und") {
                    language = "";
                }
                g_free(str);
            }
            if (gst_tag_list_get_string(tags, GST_TAG_TITLE, &str)) {
                label = String(str);
                g_free(str);
            }
            gst_tag_list_free(tags);
        }
        m_player->mediaPlayerClient()->mediaPlayerAddTextTrack(m_player, i, "", "", "", label, language);
    }
}

void MediaPlayerPrivateGStreamer::textChanged()
{
    if (m_textTimerHandler)
        g_source_remove(m_textTimerHandler);
    m_textTimerHandler = g_timeout_add(0, reinterpret_cast<GSourceFunc>(mediaPlayerPrivateTextChangeTimeoutCallback), this);
}

void MediaPlayerPrivateGStreamer::textTagsChanged()
{
    if (m_textTagsTimerHandler) {
        g_source_remove(m_textTagsTimerHandler);
    }

    m_textTagsTimerHandler = g_timeout_add(0, reinterpret_cast<GSourceFunc>(mediaPlayerPrivateTextTagChangeTimeoutCallback), this);
}

void MediaPlayerPrivateGStreamer::textBufferChanged(GstPad* pad, GstBuffer* buffer)
{
    if(!buffer) {
        g_print("Buffer is NULL\n");
        return;
    }
    if(!pad) {
    	g_print("Pad is NULL\n");
    	return;
    }

    if (m_textBufferTimerHandler) {
        g_source_remove(m_textBufferTimerHandler);
    }

    for(size_t i = 0; i < m_textPads.size(); ++i) {
    	if(m_textPads[i] == pad) {
    		OwnPtr<TextBufferData> data = adoptPtr(new TextBufferData());
    		data->player = this;
    		data->index = i;
    	    data->data = String(buffer->data, buffer->size);
    	    data->start = ((float)buffer->timestamp) / GST_SECOND;
    	    data->end = data->start + ((float)buffer->duration) / GST_SECOND;
    	    m_textCueQueue.append(data.release());
    	    m_textBufferTimerHandler = g_timeout_add(0, reinterpret_cast<GSourceFunc>(mediaPlayerPrivateTextBufferChangedTimeoutCallback), this);
    		return;
    	}
    }
    g_print("Got buffer for non-existent pad %p\n", pad);
}

void MediaPlayerPrivateGStreamer::notifyPlayerOfTextBuffer()
{
	m_textBufferTimerHandler = 0;

	while(OwnPtr<TextBufferData> data = m_textCueQueue.tryGetMessage()) {
		if(m_player) {
			m_player->mediaPlayerClient()->mediaPlayerAddTextTrackCue(m_player, data->index, data->data, data->start, data->end);
		}
	}
}
#endif

void MediaPlayerPrivateGStreamer::setVolume(float volume)
{
    if (!m_playBin)
        return;

    gst_stream_volume_set_volume(GST_STREAM_VOLUME(m_playBin), GST_STREAM_VOLUME_FORMAT_CUBIC,
                                 static_cast<double>(volume));
}

void MediaPlayerPrivateGStreamer::notifyPlayerOfVolumeChange()
{
    m_volumeTimerHandler = 0;

    if (!m_player || !m_playBin)
        return;
    double volume;
    volume = gst_stream_volume_get_volume(GST_STREAM_VOLUME(m_playBin), GST_STREAM_VOLUME_FORMAT_CUBIC);
    // get_volume() can return values superior to 1.0 if the user
    // applies software user gain via third party application (GNOME
    // volume control for instance).
    volume = CLAMP(volume, 0.0, 1.0);
    m_player->volumeChanged(static_cast<float>(volume));
}

void MediaPlayerPrivateGStreamer::volumeChanged()
{
    if (m_volumeTimerHandler)
        g_source_remove(m_volumeTimerHandler);
    m_volumeTimerHandler = g_timeout_add(0, reinterpret_cast<GSourceFunc>(mediaPlayerPrivateVolumeChangeTimeoutCallback), this);
}

void MediaPlayerPrivateGStreamer::setRate(float rate)
{
    // Avoid useless playback rate update.
    if (m_playbackRate == rate)
        return;

    GstState state;
    GstState pending;

    gst_element_get_state(m_playBin, &state, &pending, 0);
    if ((state != GST_STATE_PLAYING && state != GST_STATE_PAUSED)
        || (pending == GST_STATE_PAUSED))
        return;

    if (isLiveStream())
        return;

    m_playbackRate = rate;
    m_changingRate = true;

    if (!rate) {
        gst_element_set_state(m_playBin, GST_STATE_PAUSED);
        return;
    }

    float currentPosition = static_cast<float>(playbackPosition() * GST_SECOND);
    GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH);
    gint64 start, end;
    bool mute = false;

    LOG_MEDIA_MESSAGE("Set Rate to %f", rate);
    if (rate > 0) {
        // Mute the sound if the playback rate is too extreme.
        // TODO: in other cases we should perform pitch adjustments.
        mute = (bool) (rate < 0.8 || rate > 2);
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

    if (!gst_element_seek(m_playBin, rate, GST_FORMAT_TIME, flags,
                          GST_SEEK_TYPE_SET, start,
                          GST_SEEK_TYPE_SET, end))
        LOG_MEDIA_MESSAGE("Set rate to %f failed", rate);
    else
        g_object_set(m_playBin, "mute", mute, NULL);
}

MediaPlayer::NetworkState MediaPlayerPrivateGStreamer::networkState() const
{
    return m_networkState;
}

MediaPlayer::ReadyState MediaPlayerPrivateGStreamer::readyState() const
{
    return m_readyState;
}

PassRefPtr<TimeRanges> MediaPlayerPrivateGStreamer::buffered() const
{
    RefPtr<TimeRanges> timeRanges = TimeRanges::create();
    if (m_errorOccured || isLiveStream())
        return timeRanges.release();

#if GST_CHECK_VERSION(0, 10, 31)
    float mediaDuration(duration());
    if (!mediaDuration || isinf(mediaDuration))
        return timeRanges.release();

    GstQuery* query = gst_query_new_buffering(GST_FORMAT_PERCENT);

    if (!gst_element_query(m_playBin, query)) {
        gst_query_unref(query);
        return timeRanges.release();
    }

    gint64 rangeStart = 0, rangeStop = 0;
    for (guint index = 0; index < gst_query_get_n_buffering_ranges(query); index++) {
        if (gst_query_parse_nth_buffering_range(query, index, &rangeStart, &rangeStop))
            timeRanges->add(static_cast<float>((rangeStart * mediaDuration) / 100),
                            static_cast<float>((rangeStop * mediaDuration) / 100));
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
        gst_message_parse_error(message, &err.outPtr(), &debug.outPtr());
        LOG_MEDIA_MESSAGE("Error %d: %s (url=%s)", err->code, err->message, m_url.string().utf8().data());

        GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(m_playBin), GST_DEBUG_GRAPH_SHOW_ALL, "webkit-video.error");

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
        if (GST_MESSAGE_SRC(message) == reinterpret_cast<GstObject*>(m_playBin)) {
            updateStates();

            // Construct a filename for the graphviz dot file output.
            GstState oldState, newState;
            gst_message_parse_state_changed(message, &oldState, &newState, 0);

            CString dotFileName = String::format("webkit-video.%s_%s",
                                                 gst_element_state_get_name(oldState),
                                                 gst_element_state_get_name(newState)).utf8();

            GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(m_playBin), GST_DEBUG_GRAPH_SHOW_ALL, dotFileName.data());
        }
        break;
    case GST_MESSAGE_BUFFERING:
        processBufferingStats(message);
        break;
    case GST_MESSAGE_DURATION:
        LOG_MEDIA_MESSAGE("Duration changed");
        durationChanged();
        break;
    default:
        LOG_MEDIA_MESSAGE("Unhandled GStreamer message type: %s",
                    GST_MESSAGE_TYPE_NAME(message));
        break;
    }
    return TRUE;
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
}

void MediaPlayerPrivateGStreamer::fillTimerFired(Timer<MediaPlayerPrivateGStreamer>*)
{
    GstQuery* query = gst_query_new_buffering(GST_FORMAT_PERCENT);

    if (!gst_element_query(m_playBin, query)) {
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
    if (isinf(duration()))
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
    if (!m_playBin || !m_mediaDuration || !totalBytes())
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

unsigned MediaPlayerPrivateGStreamer::decodedFrameCount() const
{
    guint64 decodedFrames = 0;
    if (m_fpsSink)
        g_object_get(m_fpsSink, "frames-rendered", &decodedFrames, NULL);
    return static_cast<unsigned>(decodedFrames);
}

unsigned MediaPlayerPrivateGStreamer::droppedFrameCount() const
{
    guint64 framesDropped = 0;
    if (m_fpsSink)
        g_object_get(m_fpsSink, "frames-dropped", &framesDropped, NULL);
    return static_cast<unsigned>(framesDropped);
}

unsigned MediaPlayerPrivateGStreamer::audioDecodedByteCount() const
{
    GstQuery* query = gst_query_new_position(GST_FORMAT_BYTES);
    gint64 position = 0;

    if (m_webkitAudioSink && gst_element_query(m_webkitAudioSink.get(), query))
        gst_query_parse_position(query, 0, &position);

    gst_query_unref(query);
    return static_cast<unsigned>(position);
}

unsigned MediaPlayerPrivateGStreamer::videoDecodedByteCount() const
{
    GstQuery* query = gst_query_new_position(GST_FORMAT_BYTES);
    gint64 position = 0;

    if (gst_element_query(m_webkitVideoSink, query))
        gst_query_parse_position(query, 0, &position);

    gst_query_unref(query);
    return static_cast<unsigned>(position);
}

void MediaPlayerPrivateGStreamer::updateAudioSink()
{
    if (!m_playBin)
        return;

    GstElement* sinkPtr = 0;

    g_object_get(m_playBin, "audio-sink", &sinkPtr, NULL);
    m_webkitAudioSink = adoptGRef(sinkPtr);

}


void MediaPlayerPrivateGStreamer::sourceChanged()
{
    GstElement* srcPtr = 0;

    g_object_get(m_playBin, "source", &srcPtr, NULL);
    m_source = adoptGRef(srcPtr);

    if (WEBKIT_IS_WEB_SRC(m_source.get()))
        webKitWebSrcSetMediaPlayer(WEBKIT_WEB_SRC(m_source.get()), m_player);
}

void MediaPlayerPrivateGStreamer::cancelLoad()
{
    if (m_networkState < MediaPlayer::Loading || m_networkState == MediaPlayer::Loaded)
        return;

    if (m_playBin)
        gst_element_set_state(m_playBin, GST_STATE_NULL);
}

void MediaPlayerPrivateGStreamer::updateStates()
{
    if (!m_playBin)
        return;

    if (m_errorOccured)
        return;

    MediaPlayer::NetworkState oldNetworkState = m_networkState;
    MediaPlayer::ReadyState oldReadyState = m_readyState;
    GstState state;
    GstState pending;

    GstStateChangeReturn ret = gst_element_get_state(m_playBin,
        &state, &pending, 250 * GST_NSECOND);

    bool shouldUpdateAfterSeek = false;
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
            if (m_buffering && m_bufferingPercentage == 100) {
                m_buffering = false;
                m_bufferingPercentage = 0;
                m_readyState = MediaPlayer::HaveEnoughData;

                LOG_MEDIA_MESSAGE("[Buffering] Complete.");

                if (!m_paused) {
                    LOG_MEDIA_MESSAGE("[Buffering] Restarting playback.");
                    gst_element_set_state(m_playBin, GST_STATE_PLAYING);
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

                gst_element_set_state(m_playBin, GST_STATE_PAUSED);
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
            gst_element_set_state(m_playBin, GST_STATE_NULL);
            gst_element_set_state(m_playBin, GST_STATE_PAUSED);
        }

        // A live stream was paused, reset the pipeline.
        if (state == GST_STATE_PAUSED && pending == GST_STATE_PLAYING && isLiveStream()) {
            gst_element_set_state(m_playBin, GST_STATE_NULL);
            gst_element_set_state(m_playBin, GST_STATE_PLAYING);
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
                gst_element_set_state(m_playBin, GST_STATE_PLAYING);
        } else if (!m_paused)
            gst_element_set_state(m_playBin, GST_STATE_PLAYING);

        m_networkState = MediaPlayer::Loading;
        break;
    default:
        LOG_MEDIA_MESSAGE("Else : %d", ret);
        break;
    }

    if (seeking())
        m_readyState = MediaPlayer::HaveNothing;

    if (shouldUpdateAfterSeek)
        timeChanged();

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

        gchar* currentLocation = 0;
        g_object_get(m_playBin, "uri", &currentLocation, NULL);

        KURL currentUrl(KURL(), currentLocation);
        g_free(currentLocation);

        KURL newUrl;

        if (gst_uri_is_valid(newLocation))
            newUrl = KURL(KURL(), newLocation);
        else
            newUrl = KURL(KURL(), currentUrl.baseAsString() + newLocation);

        RefPtr<SecurityOrigin> securityOrigin = SecurityOrigin::create(currentUrl);
        if (securityOrigin->canRequest(newUrl)) {
            LOG_MEDIA_MESSAGE("New media url: %s", newUrl.string().utf8().data());

            // Reset player states.
            m_networkState = MediaPlayer::Loading;
            m_player->networkStateChanged();
            m_readyState = MediaPlayer::HaveNothing;
            m_player->readyStateChanged();

            // Reset pipeline state.
            m_resetPipeline = true;
            gst_element_set_state(m_playBin, GST_STATE_READY);

            GstState state;
            gst_element_get_state(m_playBin, &state, 0, 0);
            if (state <= GST_STATE_READY) {
                // Set the new uri and start playing.
                g_object_set(m_playBin, "uri", newUrl.string().utf8().data(), NULL);
                gst_element_set_state(m_playBin, GST_STATE_PLAYING);
                return true;
            }
        }
    }
    m_mediaLocationCurrentIndex--;
    return false;

}

void MediaPlayerPrivateGStreamer::loadStateChanged()
{
    updateStates();
}

void MediaPlayerPrivateGStreamer::sizeChanged()
{
    notImplemented();
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
        gst_element_set_state(m_playBin, GST_STATE_NULL);
    }
}

void MediaPlayerPrivateGStreamer::cacheDuration()
{
    // Reset cached media duration
    m_mediaDuration = 0;

    // And re-cache it if possible.
    GstState state;
    gst_element_get_state(m_playBin, &state, 0, 0);
    float newDuration = duration();

    if (state <= GST_STATE_READY) {
        // Don't set m_mediaDurationKnown yet if the pipeline is not
        // paused. This allows duration() query to fail at least once
        // before playback starts and duration becomes known.
        if (!isinf(newDuration))
            m_mediaDuration = newDuration;
    } else {
        m_mediaDurationKnown = !isinf(newDuration);
        if (m_mediaDurationKnown)
            m_mediaDuration = newDuration;
    }

    if (!isinf(newDuration))
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

    if (m_preload == MediaPlayer::None && m_originalPreloadWasAutoAndWasOverridden) {
        m_totalBytes = -1;
        if (totalBytes() && !isLiveStream()) {
            setPreload(MediaPlayer::Auto);
            gst_element_set_state(m_playBin, GST_STATE_NULL);
            gst_element_set_state(m_playBin, GST_STATE_PAUSED);
        }
    }
}

bool MediaPlayerPrivateGStreamer::supportsMuting() const
{
    return true;
}

void MediaPlayerPrivateGStreamer::setMuted(bool muted)
{
    if (!m_playBin)
        return;

    g_object_set(m_playBin, "mute", muted, NULL);
}

void MediaPlayerPrivateGStreamer::notifyPlayerOfMute()
{
    m_muteTimerHandler = 0;

    if (!m_player || !m_playBin)
        return;

    gboolean muted;
    g_object_get(m_playBin, "mute", &muted, NULL);
    m_player->muteChanged(static_cast<bool>(muted));
}

void MediaPlayerPrivateGStreamer::muteChanged()
{
    if (m_muteTimerHandler)
        g_source_remove(m_muteTimerHandler);
    m_muteTimerHandler = g_timeout_add(0, reinterpret_cast<GSourceFunc>(mediaPlayerPrivateMuteChangeTimeoutCallback), this);
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

void MediaPlayerPrivateGStreamer::setSize(const IntSize& size)
{
    m_size = size;
}

void MediaPlayerPrivateGStreamer::setVisible(bool)
{
}


void MediaPlayerPrivateGStreamer::triggerRepaint(GstBuffer* buffer)
{
    g_return_if_fail(GST_IS_BUFFER(buffer));
    gst_buffer_replace(&m_buffer, buffer);
    m_player->repaint();
}

void MediaPlayerPrivateGStreamer::paint(GraphicsContext* context, const IntRect& rect)
{
    if (context->paintingDisabled())
        return;

    if (!m_player->visible())
        return;

    if (!m_buffer)
        return;

    GstCaps* caps = webkitGstGetPadCaps(m_videoSinkPad.get());
    if (!caps)
        return;

    RefPtr<ImageGStreamer> gstImage = ImageGStreamer::createImage(m_buffer, caps);
    if (!gstImage)
        return;

    context->drawImage(reinterpret_cast<Image*>(gstImage->image().get()), ColorSpaceSRGB,
                       rect, gstImage->rect(), CompositeCopy, DoNotRespectImageOrientation, false);
}

static HashSet<String> mimeTypeCache()
{
    initializeGStreamerAndRegisterWebKitElements();

    DEFINE_STATIC_LOCAL(HashSet<String>, cache, ());
    static bool typeListInitialized = false;

    if (typeListInitialized)
        return cache;
    const char* mimeTypes[] = {"application/ogg",
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
                               "audio/qcelp",
                               "audio/riff-midi",
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
                               "video/x-vcd"};

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

bool MediaPlayerPrivateGStreamer::hasSingleSecurityOrigin() const
{
    return true;
}

bool MediaPlayerPrivateGStreamer::supportsFullscreen() const
{
#if PLATFORM(MAC) && !PLATFORM(IOS) && __MAC_OS_X_VERSION_MIN_REQUIRED == 1050
    // See <rdar://problem/7389945>
    return false;
#else
    return true;
#endif
}

PlatformMedia MediaPlayerPrivateGStreamer::platformMedia() const
{
    PlatformMedia p;
#ifndef GST_API_VERSION_1
    p.type = PlatformMedia::GStreamerGWorldType;
    p.media.gstreamerGWorld = m_gstGWorld.get();
#endif
    return p;
}

MediaPlayer::MovieLoadType MediaPlayerPrivateGStreamer::movieLoadType() const
{
    if (m_readyState == MediaPlayer::HaveNothing)
        return MediaPlayer::Unknown;

    if (isLiveStream())
        return MediaPlayer::LiveStream;

    return MediaPlayer::Download;
}

void MediaPlayerPrivateGStreamer::setPreload(MediaPlayer::Preload preload)
{
    m_originalPreloadWasAutoAndWasOverridden = m_preload != preload && m_preload == MediaPlayer::Auto;

    m_preload = preload;

    ASSERT(m_playBin);

    GstPlayFlags flags;
    g_object_get(m_playBin, "flags", &flags, NULL);
    if (m_preload == MediaPlayer::Auto) {
        LOG_MEDIA_MESSAGE("Enabling on-disk buffering");
        g_object_set(m_playBin, "flags", flags | GST_PLAY_FLAG_DOWNLOAD, NULL);
    } else {
        LOG_MEDIA_MESSAGE("Disabling on-disk buffering");
        g_object_set(m_playBin, "flags", flags & ~GST_PLAY_FLAG_DOWNLOAD, NULL);
    }

    if (m_delayingLoad && m_preload != MediaPlayer::None) {
        m_delayingLoad = false;
        commitLoad();
    }
}

void MediaPlayerPrivateGStreamer::createGSTPlayBin()
{
    ASSERT(!m_playBin);
    m_playBin = gst_element_factory_make(gPlaybinName, "play");

#ifndef GST_API_VERSION_1
    m_gstGWorld = GStreamerGWorld::createGWorld(m_playBin);
#endif

    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(m_playBin));
    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "message", G_CALLBACK(mediaPlayerPrivateMessageCallback), this);
    gst_object_unref(bus);

    g_object_set(m_playBin, "mute", m_player->muted(), NULL);

    g_signal_connect(m_playBin, "notify::volume", G_CALLBACK(mediaPlayerPrivateVolumeChangedCallback), this);
    g_signal_connect(m_playBin, "notify::source", G_CALLBACK(mediaPlayerPrivateSourceChangedCallback), this);
    g_signal_connect(m_playBin, "notify::mute", G_CALLBACK(mediaPlayerPrivateMuteChangedCallback), this);
    g_signal_connect(m_playBin, "video-changed", G_CALLBACK(mediaPlayerPrivateVideoChangedCallback), this);
#if ENABLE(VIDEO_TRACK)
    g_signal_connect(m_playBin, "text-changed", G_CALLBACK(mediaPlayerPrivateTextChangedCallback), this);
    g_signal_connect(m_playBin, "text-tags-changed", G_CALLBACK(mediaPlayerPrivateTextTagsChangedCallback), this);
#endif
    g_signal_connect(m_playBin, "audio-changed", G_CALLBACK(mediaPlayerPrivateAudioChangedCallback), this);
    g_signal_connect(m_playBin, "video-tags-changed", G_CALLBACK(mediaPlayerPrivateVideoTagsChangedCallback), this);
    g_signal_connect(m_playBin, "audio-tags-changed", G_CALLBACK(mediaPlayerPrivateAudioTagsChangedCallback), this);

#ifndef GST_API_VERSION_1
    m_webkitVideoSink = webkitVideoSinkNew(m_gstGWorld.get());
#else
    m_webkitVideoSink = webkitVideoSinkNew();
#endif
    m_videoSinkPad = adoptGRef(gst_element_get_static_pad(m_webkitVideoSink, "sink"));

    g_signal_connect(m_webkitVideoSink, "repaint-requested", G_CALLBACK(mediaPlayerPrivateRepaintCallback), this);


#ifndef GST_API_VERSION_1
    m_videoSinkBin = gst_bin_new("sink");

    GstElement* videoTee = gst_element_factory_make("tee", "videoTee");
    GstElement* queue = gst_element_factory_make("queue", 0);
    GstElement* videoConvert = gst_element_factory_make("ffmpegcolorspace", "videoConvert");

    // Take ownership.
    gst_object_ref_sink(m_videoSinkBin);

    // Build a new video sink consisting of a bin containing a tee
    // (meant to distribute data to multiple video sinks) and our
    // internal video sink. For fullscreen we create an autovideosink
    // and initially block the data flow towards it and configure it

    gst_bin_add_many(GST_BIN(m_videoSinkBin), videoTee, queue, videoConvert, NULL);

    // Link a new src pad from tee to queue1.
    GRefPtr<GstPad> srcPad = adoptGRef(gst_element_get_request_pad(videoTee, "src%d"));
    GRefPtr<GstPad> sinkPad = adoptGRef(gst_element_get_static_pad(queue, "sink"));
    gst_pad_link(srcPad.get(), sinkPad.get());

    // Link pad from queue to convert.
    gst_element_link_many(queue, videoConvert, NULL);
#endif

    GstElement* actualVideoSink = 0;
    m_fpsSink = gst_element_factory_make("fpsdisplaysink", "sink");
    if (m_fpsSink) {
        // The verbose property has been added in -bad 0.10.22. Making
        // this whole code depend on it because we don't want
        // fpsdiplaysink to spit data on stdout.
        GstElementFactory* factory = GST_ELEMENT_FACTORY(GST_ELEMENT_GET_CLASS(m_fpsSink)->elementfactory);
        if (gst_plugin_feature_check_version(GST_PLUGIN_FEATURE(factory), 0, 10, 22)) {
            g_object_set(m_fpsSink, "silent", TRUE , NULL);

            // Turn off text overlay unless logging is enabled.
#if LOG_DISABLED
            g_object_set(m_fpsSink, "text-overlay", FALSE , NULL);
#else
            WTFLogChannel* channel = getChannelFromName("Media");
            if (channel->state != WTFLogChannelOn)
                g_object_set(m_fpsSink, "text-overlay", FALSE , NULL);
#endif // LOG_DISABLED

            if (g_object_class_find_property(G_OBJECT_GET_CLASS(m_fpsSink), "video-sink")) {
                g_object_set(m_fpsSink, "video-sink", m_webkitVideoSink, NULL);
#ifndef GST_API_VERSION_1
                gst_bin_add(GST_BIN(m_videoSinkBin), m_fpsSink);
#endif
                actualVideoSink = m_fpsSink;
            } else
                m_fpsSink = 0;
        } else
            m_fpsSink = 0;
    }

    if (!m_fpsSink) {
#ifndef GST_API_VERSION_1
        gst_bin_add(GST_BIN(m_videoSinkBin), m_webkitVideoSink);
#endif
        actualVideoSink = m_webkitVideoSink;
    }

    ASSERT(actualVideoSink);

#ifndef GST_API_VERSION_1
    // Faster elements linking.
    gst_element_link_pads_full(videoConvert, "src", actualVideoSink, "sink", GST_PAD_LINK_CHECK_NOTHING);

    // Add a ghostpad to the bin so it can proxy to tee.
    GRefPtr<GstPad> pad = adoptGRef(gst_element_get_static_pad(videoTee, "sink"));
    gst_element_add_pad(m_videoSinkBin, gst_ghost_pad_new("sink", pad.get()));

    // Set the bin as video sink of playbin.
    g_object_set(m_playBin, "video-sink", m_videoSinkBin, NULL);
#else
    g_object_set(m_playBin, "video-sink", actualVideoSink, NULL);
#endif

    GRefPtr<GstPad> videoSinkPad = adoptGRef(gst_element_get_static_pad(m_webkitVideoSink, "sink"));
    if (videoSinkPad)
        g_signal_connect(videoSinkPad.get(), "notify::caps", G_CALLBACK(mediaPlayerPrivateVideoSinkCapsChangedCallback), this);

    // Don't display subtitles directly on the video
    int flags;
    g_object_get(m_playBin, "flags", &flags, NULL);
	flags &= ~GST_PLAY_FLAG_TEXT;
    g_object_set(m_playBin, "flags", flags, NULL);
}

}

#endif // USE(GSTREAMER)
