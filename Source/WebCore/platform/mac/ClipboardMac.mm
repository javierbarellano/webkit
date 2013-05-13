/*
 * Copyright (C) 2004, 2005, 2006, 2008, 2010, 2013 Apple Inc. All rights reserved.
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

#import "config.h"
#import "ClipboardMac.h"

#import "CachedImage.h"
#import "CachedImageClient.h"
#import "DOMElementInternal.h"
#import "DragClient.h"
#import "DragController.h"
#import "DragData.h"
#import "Editor.h"
#import "FileList.h"
#import "Frame.h"
#import "FrameSnapshottingMac.h"
#import "Image.h"
#import "Page.h"
#import "Pasteboard.h"
#import "PasteboardStrategy.h"
#import "PlatformStrategies.h"
#import "RenderImage.h"
#import "ScriptExecutionContext.h"
#import "SecurityOrigin.h"
#import "WebCoreSystemInterface.h"

namespace WebCore {

#if ENABLE(DRAG_SUPPORT)
PassRefPtr<Clipboard> Clipboard::create(ClipboardAccessPolicy policy, DragData* dragData, Frame* frame)
{
    return ClipboardMac::create(DragAndDrop, dragData->pasteboardName(), policy, dragData->containsFiles() ? ClipboardMac::DragAndDropFiles : ClipboardMac::DragAndDropData, frame);
}
#endif

ClipboardMac::ClipboardMac(ClipboardType clipboardType, const String& pasteboardName, ClipboardAccessPolicy policy, ClipboardContents clipboardContents, Frame *frame)
    : Clipboard(policy, clipboardType, Pasteboard::create(pasteboardName), clipboardContents == DragAndDropFiles)
    , m_pasteboardName(pasteboardName)
    , m_frame(frame)
{
    m_changeCount = platformStrategies()->pasteboardStrategy()->changeCount(m_pasteboardName);
}

ClipboardMac::~ClipboardMac()
{
    if (m_dragImage)
        m_dragImage->removeClient(this);
}

// The rest of these getters don't really have any impact on security, so for now make no checks

void ClipboardMac::setDragImage(CachedImage* img, const IntPoint &loc)
{
    setDragImage(img, 0, loc);
}

void ClipboardMac::setDragImageElement(Node *node, const IntPoint &loc)
{
    setDragImage(0, node, loc);
}

void ClipboardMac::setDragImage(CachedImage* image, Node *node, const IntPoint &loc)
{
    if (canSetDragImage()) {
        if (m_dragImage)
            m_dragImage->removeClient(this);
        m_dragImage = image;
        if (m_dragImage)
            m_dragImage->addClient(this);

        m_dragLoc = loc;
        m_dragImageElement = node;
        
        if (dragStarted() && m_changeCount == platformStrategies()->pasteboardStrategy()->changeCount(m_pasteboardName)) {
            NSPoint cocoaLoc;
            NSImage* cocoaImage = dragNSImage(cocoaLoc);
            if (cocoaImage) {
                // Dashboard wants to be able to set the drag image during dragging, but Cocoa does not allow this.
                // Instead we must drop down to the CoreGraphics API.
                wkSetDragImage(cocoaImage, cocoaLoc);

                // Hack: We must post an event to wake up the NSDragManager, which is sitting in a nextEvent call
                // up the stack from us because the CoreFoundation drag manager does not use the run loop by itself.
                // This is the most innocuous event to use, per Kristen Forster.
                NSEvent* ev = [NSEvent mouseEventWithType:NSMouseMoved location:NSZeroPoint
                    modifierFlags:0 timestamp:0 windowNumber:0 context:nil eventNumber:0 clickCount:0 pressure:0];
                [NSApp postEvent:ev atStart:YES];
            }
        }
        // Else either 1) we haven't started dragging yet, so we rely on the part to install this drag image
        // as part of getting the drag kicked off, or 2) Someone kept a ref to the clipboard and is trying to
        // set the image way too late.
    }
}
    
void ClipboardMac::writeRange(Range* range, Frame* frame)
{
    ASSERT(range);
    ASSERT(frame);
    Pasteboard pasteboard(m_pasteboardName);
    pasteboard.writeSelection(range, frame->editor()->smartInsertDeleteEnabled() && frame->selection()->granularity() == WordGranularity, frame, IncludeImageAltTextForClipboard);
}

void ClipboardMac::writePlainText(const String& text)
{
    Pasteboard pasteboard(m_pasteboardName);
    pasteboard.writePlainText(text, Pasteboard::CannotSmartReplace);
}

void ClipboardMac::writeURL(const KURL& url, const String& title, Frame* frame)
{   
    ASSERT(frame);
    ASSERT(m_pasteboardName);
    Pasteboard pasteboard(m_pasteboardName);
    pasteboard.writeURL(url, title, frame);
}
    
#if ENABLE(DRAG_SUPPORT)
void ClipboardMac::declareAndWriteDragImage(Element* element, const KURL& url, const String& title, Frame* frame)
{
    ASSERT(frame);
    if (Page* page = frame->page())
        page->dragController()->client()->declareAndWriteDragImage(m_pasteboardName, kit(element), url, title, frame);
}
#endif // ENABLE(DRAG_SUPPORT)
    
DragImageRef ClipboardMac::createDragImage(IntPoint& loc) const
{
    NSPoint nsloc = NSMakePoint(loc.x(), loc.y());
    DragImageRef result = dragNSImage(nsloc);
    loc = (IntPoint)nsloc;
    return result;
}
    
NSImage *ClipboardMac::dragNSImage(NSPoint& loc) const
{
    NSImage *result = nil;
    if (m_dragImageElement) {
        if (m_frame) {
            NSRect imageRect;
            NSRect elementRect;
            result = snapshotDragImage(m_frame, m_dragImageElement.get(), &imageRect, &elementRect);
            // Client specifies point relative to element, not the whole image, which may include child
            // layers spread out all over the place.
            loc.x = elementRect.origin.x - imageRect.origin.x + m_dragLoc.x();
            loc.y = elementRect.origin.y - imageRect.origin.y + m_dragLoc.y();
            loc.y = imageRect.size.height - loc.y;
        }
    } else if (m_dragImage) {
        result = m_dragImage->image()->getNSImage();
        
        loc = m_dragLoc;
        loc.y = [result size].height - loc.y;
    }
    return result;
}

}
