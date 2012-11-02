/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef VideoTrack_h
#define VideoTrack_h

#if ENABLE(VIDEO_TRACK)

#include "ExceptionCode.h"
#include "TrackBase.h"
#include <wtf/PassOwnPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class HTMLMediaElement;
class VideoTrack;

class VideoTrackClient {
public:
    virtual ~VideoTrackClient() { }
    virtual void videoTrackSelected(VideoTrack*, bool) = 0;
};

class VideoTrack : public TrackBase {
public:
    static PassRefPtr<VideoTrack> create(ScriptExecutionContext* context, VideoTrackClient* client, int index, bool selected, const String& id, const String& kind, const String& label, const String& language)
    {
        return adoptRef(new VideoTrack(context, client, index, selected, id, kind, label, language));
    }
    virtual ~VideoTrack();

    void setMediaElement(HTMLMediaElement* element) { m_mediaElement = element; }
    HTMLMediaElement* mediaElement() { return m_mediaElement; }

    String id() const { return m_id; }
    void setId(const String&);

    String kind() const { return m_kind; }
    void setKind(const String&);

    static const AtomicString& alternativeKeyword();
    static const AtomicString& captionsKeyword();
    static const AtomicString& mainKeyword();
    static const AtomicString& signKeyword();
    static const AtomicString& subtitlesKeyword();
    static const AtomicString& commentaryKeyword();
    static bool isValidKindKeyword(const String&);

    String label() const { return m_label; }
    void setLabel(const String& label) { m_label = label; }

    String language() const { return m_language; }
    void setLanguage(const String& language) { m_language = language; }

    bool selected() const { return m_selected; }
    void setSelected(bool selected);

    enum ReadinessState { NotLoaded = 0, Loading = 1, Loaded = 2, FailedToLoad = 3 };
    ReadinessState readinessState() const { return m_readinessState; }
    void setReadinessState(ReadinessState state) { m_readinessState = state; }

    virtual void clearClient() { m_client = 0; }
    VideoTrackClient* client() { return m_client; }

    int trackIndex();

    bool isRendered();

protected:
    VideoTrack(ScriptExecutionContext*, VideoTrackClient*, int index, bool selected, const String& id, const String& kind, const String& label, const String& language);

private:
    HTMLMediaElement* m_mediaElement;
    String m_id;
    String m_kind;
    String m_label;
    String m_language;
    bool m_selected;
    VideoTrackClient* m_client;
    ReadinessState m_readinessState;
    int m_trackIndex;
};

} // namespace WebCore

#endif
#endif
