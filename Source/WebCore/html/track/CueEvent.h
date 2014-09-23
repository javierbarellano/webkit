/*
 * Copyright (C) 2011 Apple Inc.  All rights reserved.
 * Copyright (C) 2014 Cable Television Labs Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef CueEvent_h
#define CueEvent_h

#if ENABLE(VIDEO_TRACK)

#include "Event.h"
#include "TextTrackCue.h"
#include <wtf/Vector.h>

namespace WebCore {

struct CueEventInit : public EventInit {
    CueEventInit();

    RefPtr<TextTrackCue> cue;
};

class CueEvent : public Event {
public:
    virtual ~CueEvent();

    static PassRefPtr<CueEvent> create()
    {
        return adoptRef(new CueEvent);
    }

    static PassRefPtr<CueEvent> create(const AtomicString& type, const CueEventInit& initializer)
    {
        return adoptRef(new CueEvent(type, initializer));
    }

    virtual EventInterface eventInterface() const override;

    TextTrackCue* cue() const { return m_cue.get(); }

private:
    CueEvent();
    CueEvent(const AtomicString& type, const CueEventInit& initializer);

    RefPtr<TextTrackCue> m_cue;
};

} // namespace WebCore

#endif
#endif
