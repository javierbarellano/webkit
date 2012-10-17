/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
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

#if ENABLE(VIDEO_TRACK)

#include "VideoTrackLoader.h"

#include "CachedResourceLoader.h"
//#include "CachedVideoTrack.h"
#include "CrossOriginAccessControl.h"
#include "Document.h"
#include "Logging.h"
#include "ResourceHandle.h"
#include "ScriptCallStack.h"
#include "SecurityOrigin.h"
#include "SharedBuffer.h"
#include "WebVTTParser.h"

namespace WebCore {
    
VideoTrackLoader::VideoTrackLoader(VideoTrackLoaderClient* client, ScriptExecutionContext* context)
    : m_client(client)
    , m_scriptExecutionContext(context)
    , m_cueLoadTimer(this, &VideoTrackLoader::cueLoadTimerFired)
    , m_state(Idle)
    , m_parseOffset(0)
    , m_newCuesAvailable(false)
{
}

VideoTrackLoader::~VideoTrackLoader()
{
}

void VideoTrackLoader::cueLoadTimerFired(Timer<VideoTrackLoader>* timer)
{
    ASSERT_UNUSED(timer, timer == &m_cueLoadTimer);
    
    if (m_newCuesAvailable) {
        m_newCuesAvailable = false;
        m_client->newCuesAvailable(this); 
    }
    
    if (m_state >= Finished)
        m_client->cueLoadingCompleted(this, m_state == Failed);
}

void VideoTrackLoader::cancelLoad()
{
}

void VideoTrackLoader::processNewCueData(CachedResource* resource)
{
    
    if (m_state == Failed || !resource->data())
        return;
    
    SharedBuffer* buffer = resource->data();
    if (m_parseOffset == buffer->size())
        return;

    if (!m_cueParser)
        m_cueParser = WebVTTParser::create(this, m_scriptExecutionContext);

    const char* data;
    unsigned length;

    while ((length = buffer->getSomeData(data, m_parseOffset))) {
        m_cueParser->parseBytes(data, length);
        m_parseOffset += length;
    }
}

void VideoTrackLoader::didReceiveData(CachedResource* resource)
{
    
    if (!resource->data())
        return;
    
    processNewCueData(resource);
}

void VideoTrackLoader::corsPolicyPreventedLoad()
{
    DEFINE_STATIC_LOCAL(String, consoleMessage, (ASCIILiteral("Cross-origin text track load denied by Cross-Origin Resource Sharing policy.")));
    Document* document = static_cast<Document*>(m_scriptExecutionContext);
    document->addConsoleMessage(JSMessageSource, LogMessageType, ErrorMessageLevel, consoleMessage);
    m_state = Failed;
}

void VideoTrackLoader::notifyFinished(CachedResource* resource)
{

    Document* document = static_cast<Document*>(m_scriptExecutionContext);
    if (!m_crossOriginMode.isNull()
        && !document->securityOrigin()->canRequest(resource->response().url())
        && !resource->passesAccessControlCheck(document->securityOrigin())) {

        corsPolicyPreventedLoad();
    }

    if (m_state != Failed) {
        processNewCueData(resource);
        if (m_state != Failed)
            m_state = resource->errorOccurred() ? Failed : Finished;
    }

    if (!m_cueLoadTimer.isActive())
        m_cueLoadTimer.startOneShot(0);
    
    cancelLoad();
}

bool VideoTrackLoader::load(const KURL& url, const String& crossOriginMode)
{
    cancelLoad();

    if (!m_client->shouldLoadCues(this))
        return false;

    ASSERT(m_scriptExecutionContext->isDocument());
    Document* document = static_cast<Document*>(m_scriptExecutionContext);
    ResourceRequest cueRequest(document->completeURL(url));

    if (!crossOriginMode.isNull()) {
        m_crossOriginMode = crossOriginMode;
        StoredCredentials allowCredentials = equalIgnoringCase(crossOriginMode, "use-credentials") ? AllowStoredCredentials : DoNotAllowStoredCredentials;
        updateRequestForAccessControl(cueRequest, document->securityOrigin(), allowCredentials);
    } else {
        // Cross-origin resources that are not suitably CORS-enabled may not load.
        if (!document->securityOrigin()->canRequest(url)) {
            corsPolicyPreventedLoad();
            return false;
        }
    }

    
    return true;
}

void VideoTrackLoader::newCuesParsed()
{
    if (m_cueLoadTimer.isActive())
        return;

    m_newCuesAvailable = true;
    m_cueLoadTimer.startOneShot(0);
}

void VideoTrackLoader::fileFailedToParse()
{
    LOG(Media, "VideoTrackLoader::fileFailedToParse");

    m_state = Failed;

    if (!m_cueLoadTimer.isActive())
        m_cueLoadTimer.startOneShot(0);

    cancelLoad();
}


}

#endif
