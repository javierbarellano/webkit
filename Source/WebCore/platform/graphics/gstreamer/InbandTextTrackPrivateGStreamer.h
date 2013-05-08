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

#ifndef InbandTextTrackPrivateGStreamer_h
#define InbandTextTrackPrivateGStreamer_h

#if ENABLE(VIDEO) && USE(GSTREAMER) && ENABLE(VIDEO_TRACK)

#include "GRefPtrGStreamer.h"
#include "InbandTextTrackPrivate.h"
#include <gst/gstevent.h>
#include <gst/gsttaglist.h>
#include <wtf/Vector.h>

namespace WebCore {

class MediaPlayerPrivateGStreamer;
class GenericCueData;

class InbandTextTrackPrivateGStreamer : public InbandTextTrackPrivate {
public:
    static PassRefPtr<InbandTextTrackPrivateGStreamer> create(GRefPtr<GstElement> pipeline, GstPad* pad)
    {
        return adoptRef(new InbandTextTrackPrivateGStreamer(pipeline, pad));
    }

    ~InbandTextTrackPrivateGStreamer();

    virtual int textTrackIndex() const OVERRIDE { return m_index; }
    void setTextTrackIndex(int index) { m_index =  index; }
    GstPad* pad() const { return m_pad.get(); }

    bool isDisconnected() const { return m_isDisconnected; }
    virtual void disconnect() OVERRIDE;

    bool hasBeenReported() const { return m_hasBeenReported; }
    void setHasBeenReported(bool reported) { m_hasBeenReported = reported; }

    void newSample();
    void handleTag(GstEvent*);
    void notifyPlayerOfSample();
    void notifyPlayerOfTag();

    virtual AtomicString label() const OVERRIDE { return m_label; }
    virtual AtomicString language() const OVERRIDE { return m_language; }

private:
    struct TextBufferData {
        String data;
        float end;
        float start;
    };

    GstTagList* tags() const;

    InbandTextTrackPrivateGStreamer(GRefPtr<GstElement>, GstPad*);

    Vector<RefPtr<GenericCueData> > m_cues;
    Mutex m_cueMutex;
    Mutex m_tagMutex;
    int m_index;
    GRefPtr<GstPad> m_pad;
    GRefPtr<GstElement> m_pipeline;
    GRefPtr<GstElement> m_queue;
    GRefPtr<GstElement> m_sink;
    gulong m_tagProbe;
    guint m_sampleTimerHandler;
    guint m_tagTimerHandler;
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

#endif // InbandTextTrackPrivateGStreamer_h
