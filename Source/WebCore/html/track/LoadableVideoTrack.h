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

#ifndef LoadableVideoTrack_h
#define LoadableVideoTrack_h

#if ENABLE(VIDEO_TRACK)

#include "VideoTrack.h"
#include "VideoTrackLoader.h"
#include <wtf/PassRefPtr.h>
#include <wtf/Vector.h>

namespace WebCore {

class LoadableVideoTrack;

class LoadableVideoTrackClient : public VideoTrackClient {
public:
    virtual ~LoadableVideoTrackClient() { }
    
    virtual bool canLoadUrl(LoadableVideoTrack*, const KURL&) { return false; }
    virtual void loadingCompleted(LoadableVideoTrack*, bool /* loadingFailed */) { }
    virtual void videoTrackKindChanged(VideoTrack*){}
    virtual void videoTrackModeChanged(VideoTrack*){}
    virtual void videoReadyStateChanged(VideoTrack*){}
};

class LoadableVideoTrack : public VideoTrack, private VideoTrackLoaderClient {
public:
    static PassRefPtr<LoadableVideoTrack> create(HTMLVideoElement* track, const String& kind)
    {
        return adoptRef(new LoadableVideoTrack(track, kind));
    }
    virtual ~LoadableVideoTrack(){}

    void scheduleLoad(const KURL&){}

    virtual void clearClient(){}

    size_t videoElementIndex();
    HTMLVideoElement* videoElement() { return m_videoElement; }

private:
    // VideoTrackLoaderClient
    virtual bool shouldLoadCues(VideoTrackLoader*) { return true; }
    virtual void newCuesAvailable(VideoTrackLoader*){}
    virtual void cueLoadingStarted(VideoTrackLoader*){}
    virtual void cueLoadingCompleted(VideoTrackLoader*, bool loadingFailed){}

    LoadableVideoTrack(HTMLVideoElement* track, const String& kind)
        : VideoTrack(track->document(), track, kind)
        , m_videoElement(track)
    {
    }

    HTMLVideoElement* m_videoElement;
    //Timer<LoadableVideoTrack> m_loadTimer;
    OwnPtr<VideoTrackLoader> m_loader;
    KURL m_url;
};
} // namespace WebCore

#endif
#endif
