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

#ifndef AudioTrackPrivateGStreamer_h
#define AudioTrackPrivateGStreamer_h

#if ENABLE(VIDEO) && USE(GSTREAMER) && ENABLE(VIDEO_TRACK)

#include "GRefPtrGStreamer.h"
#include "AudioTrackPrivate.h"
#include <gst/gstevent.h>
#include <gst/gsttaglist.h>

namespace WebCore {

class MediaPlayerPrivateGStreamer;

class AudioTrackPrivateGStreamer : public AudioTrackPrivate {
public:
    static PassRefPtr<AudioTrackPrivateGStreamer> create(GRefPtr<GstElement> adder, GstPad* pad)
    {
        return adoptRef(new AudioTrackPrivateGStreamer(adder, pad));
    }

    ~AudioTrackPrivateGStreamer();

    GstPad* pad() const { return m_srcPad.get(); }

    bool isDisconnected() const { return m_isDisconnected; }
    virtual void disconnect() OVERRIDE;

    bool hasBeenReported() const { return m_hasBeenReported; }
    void setHasBeenReported(bool reported) { m_hasBeenReported = reported; }

    void handleTag(GstEvent*);
    void unlinkPad();
    void notifyPlayerOfTag();

    virtual AtomicString label() const OVERRIDE { return m_label; }
    virtual AtomicString language() const OVERRIDE { return m_language; }
    virtual void setEnabled(bool) OVERRIDE;

private:

    GstTagList* tags() const;

    AudioTrackPrivateGStreamer(GRefPtr<GstElement> adder, GstPad*);

    Mutex m_tagMutex;
    GRefPtr<GstPad> m_sinkPad;
    GRefPtr<GstPad> m_srcPad;
    GRefPtr<GstElement> m_adder;
    gulong m_tagProbe;
    guint m_tagTimerHandler;
    gulong m_unlinkProbe;
#ifdef GST_API_VERSION_1
    GRefPtr<GstTagList> m_tags;
#else
    GstTagList* m_tags;
#endif
    String m_label;
    String m_language;
    bool m_hasBeenReported;
    bool m_isDisconnected;
};

} // namespace WebCore

#endif // ENABLE(VIDEO) && USE(AVFOUNDATION) && HAVE(AVFOUNDATION_TEXT_TRACK_SUPPORT)

#endif // AudioTrackPrivateGStreamer_h
