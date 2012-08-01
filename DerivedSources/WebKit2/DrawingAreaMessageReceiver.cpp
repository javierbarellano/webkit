/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "DrawingArea.h"

#include "ArgumentDecoder.h"
#include "DrawingAreaMessages.h"
#include "HandleMessage.h"
#include "WebCoreArgumentCoders.h"
#include <WebCore/IntSize.h>

namespace WebKit {

void DrawingArea::didReceiveDrawingAreaMessage(CoreIPC::Connection*, CoreIPC::MessageID messageID, CoreIPC::ArgumentDecoder* arguments)
{
    switch (messageID.get<Messages::DrawingArea::Kind>()) {
    case Messages::DrawingArea::UpdateBackingStoreStateID:
        CoreIPC::handleMessage<Messages::DrawingArea::UpdateBackingStoreState>(arguments, this, &DrawingArea::updateBackingStoreState);
        return;
    case Messages::DrawingArea::DidUpdateID:
        CoreIPC::handleMessage<Messages::DrawingArea::DidUpdate>(arguments, this, &DrawingArea::didUpdate);
        return;
    case Messages::DrawingArea::SuspendPaintingID:
        CoreIPC::handleMessage<Messages::DrawingArea::SuspendPainting>(arguments, this, &DrawingArea::suspendPainting);
        return;
    case Messages::DrawingArea::ResumePaintingID:
        CoreIPC::handleMessage<Messages::DrawingArea::ResumePainting>(arguments, this, &DrawingArea::resumePainting);
        return;
    case Messages::DrawingArea::PageCustomRepresentationChangedID:
        CoreIPC::handleMessage<Messages::DrawingArea::PageCustomRepresentationChanged>(arguments, this, &DrawingArea::pageCustomRepresentationChanged);
        return;
#if PLATFORM(MAC)
    case Messages::DrawingArea::UpdateGeometryID:
        CoreIPC::handleMessage<Messages::DrawingArea::UpdateGeometry>(arguments, this, &DrawingArea::updateGeometry);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::DrawingArea::SetDeviceScaleFactorID:
        CoreIPC::handleMessage<Messages::DrawingArea::SetDeviceScaleFactor>(arguments, this, &DrawingArea::setDeviceScaleFactor);
        return;
#endif
#if PLATFORM(MAC)
    case Messages::DrawingArea::SetLayerHostingModeID:
        CoreIPC::handleMessage<Messages::DrawingArea::SetLayerHostingMode>(arguments, this, &DrawingArea::setLayerHostingMode);
        return;
#endif
    default:
        break;
    }

    ASSERT_NOT_REACHED();
}

} // namespace WebKit
