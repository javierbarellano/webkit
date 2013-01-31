/*
 * Copyright (C) 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MediaControlElements_h
#define MediaControlElements_h

#if ENABLE(VIDEO)

#include "HTMLOptionElement.h"
#include "HTMLDivElement.h"
#include "HTMLInputElement.h"
#include "MediaControllerInterface.h"
#include "RenderBlock.h"

#include "Event.h"
#include "HTMLFormControlElementWithState.h"
#include "HTMLOptionsCollection.h"
#include <wtf/Vector.h>

// These are the shadow elements used in RenderMedia

namespace WebCore {

class Event;
class Frame;
class HTMLMediaElement;
class MediaControls;
class MouseEvent;
class MediaSelectElement;

// Must match WebKitSystemInterface.h
enum MediaControlElementType {
    MediaEnterFullscreenButton = 0,
    MediaMuteButton,
    MediaPlayButton,
    MediaSeekBackButton,
    MediaSeekForwardButton,
    MediaSlider,
    MediaSliderThumb,
    MediaRewindButton,
    MediaReturnToRealtimeButton,
    MediaShowClosedCaptionsButton,
    MediaHideClosedCaptionsButton,
    MediaUnMuteButton,
    MediaPauseButton,
    MediaTimelineContainer,
    MediaCurrentTimeDisplay,
    MediaTimeRemainingDisplay,
    MediaStatusDisplay,
    MediaControlsPanel,
    MediaVolumeSliderContainer,
    MediaVolumeSlider,
    MediaVolumeSliderThumb,
    MediaFullScreenVolumeSlider,
    MediaFullScreenVolumeSliderThumb,
    MediaVolumeSliderMuteButton,
    MediaTextTrackDisplayContainer,
    MediaTextTrackDisplay,
    MediaExitFullscreenButton,
    MediaOverlayPlayButton,
    MediaVideoTrackSelButton,
    MediaAudioTrackSelButton,
    MediaTextTrackSelButton,
    MediaFFButton,
    MediaRevButton,
};

HTMLMediaElement* toParentMediaElement(Node*);
inline HTMLMediaElement* toParentMediaElement(RenderObject* renderer) { return toParentMediaElement(renderer->node()); }

MediaControlElementType mediaControlElementType(Node*);

// ----------------------------

class MediaControlElement : public HTMLDivElement {
public:
    void hide();
    void show();

    virtual MediaControlElementType displayType() const = 0;

    void setMediaController(MediaControllerInterface* controller) { m_mediaController = controller; }
    MediaControllerInterface* mediaController() const { return m_mediaController; }

protected:
    MediaControlElement(Document*);

private:
    virtual bool isMediaControlElement() const { return true; }

    MediaControllerInterface* m_mediaController;   
};

// ----------------------------
class MediaOptionElement : public HTMLOptionElement {
public:
	static PassRefPtr<MediaOptionElement> create(Document*);

	virtual MediaSelectElement* ownerMediaElement() const;

	virtual void setText(const String &text, ExceptionCode& ec);

	virtual int index() const;
	virtual bool selected();
	virtual void setSelected(bool selected);
	virtual void setSelectedState(bool selected);

protected:
	virtual Node::InsertionNotificationRequest insertedInto(ContainerNode* insertionPoint);
	virtual void setRenderStyle(PassRefPtr<RenderStyle> newStyle);
	virtual void childrenChanged(bool changedByParser, Node* beforeChange, Node* afterChange, int childCountDelta);
	virtual void accessKeyAction(bool);
	MediaOptionElement(Document* doc);

};

// ----------------------------

class MediaSelectElement : public HTMLFormControlElementWithState {
public:
    void hide();
    void show();

    virtual MediaControlElementType displayType() const = 0;
    void setDisplayType(MediaControlElementType displayType);

    void createShadowSubtree();

    virtual void setMediaController(MediaControllerInterface* controller) { m_mediaController = controller; }
    MediaControllerInterface* mediaController() const { return m_mediaController; }

    int selectedIndex() const;
    void setSelectedIndex(int);

    void optionSelectedByUser(int index, bool dispatchChangeEvent, bool allowMultipleSelection = false);

    // For ValidityState
    virtual bool valueMissing() const OVERRIDE;

    int size() const { return m_size; }
    bool multiple() const { return m_multiple; }

    bool usesMenuList() const;

    void add(HTMLElement*, HTMLElement* beforeElement, ExceptionCode&);
    void remove(int index);
    PassRefPtr<HTMLOptionsCollection> options();

    void optionElementChildrenChanged();

    void setRecalcListItems();
    void invalidateSelectedItems();
    void updateListItemSelectedStates();

    const Vector<HTMLElement*>& listItems() const;

    virtual void accessKeyAction(bool sendMouseEvents);
    void accessKeySetSelectedIndex(int);

    void scrollToSelection();

    void listBoxSelectItem(int listIndex, bool allowMultiplySelections, bool shift, bool fireOnChangeNow = true);

    int listToOptionIndex(int listIndex) const;
    void listBoxOnChange();
    int optionToListIndex(int optionIndex) const;
    void setActiveSelectionAnchorIndex(int);
    void setActiveSelectionEndIndex(int);
    void updateListBoxSelection(bool deselectOtherOptions);

    // For use in the implementation of HTMLOptionElement.
    void optionSelectionStateChanged(HTMLOptionElement*, bool optionIsSelected);
protected:
    MediaSelectElement(Document*, MediaControlElementType displayType);
    virtual void selectChanged(int newIndex) {}

    virtual void defaultEventHandler(Event*);
    void fixEventState(Event *event);
    bool m_hasFocus;

private:
    virtual bool isMediaControlElement() const { return false; }
    virtual const AtomicString& formControlType() const;

    virtual bool isKeyboardFocusable(KeyboardEvent*) const;

    virtual void dispatchFocusEvent(PassRefPtr<Node> oldFocusedNode);
    virtual void dispatchBlurEvent(PassRefPtr<Node> newFocusedNode);

    virtual bool canStartSelection() const { return false; }

    virtual bool isEnumeratable() const { return true; }
    virtual bool supportLabels() const OVERRIDE { return true; }

    virtual FormControlState saveFormControlState() const OVERRIDE;
    virtual RenderObject* createRenderer(RenderArena*, RenderStyle *);

    void dispatchChangeEventForMenuList();

    void recalcListItems(bool updateSelectedStates = true) const;

    void typeAheadFind(KeyboardEvent*);
    void saveLastSelection();

    virtual InsertionNotificationRequest insertedInto(ContainerNode*) OVERRIDE;

    virtual bool isOptionalFormControl() const { return !isRequiredFormControl(); }
    virtual bool isRequiredFormControl() const;

    bool hasPlaceholderLabelOption() const;

    enum SelectOptionFlag {
        DeselectOtherOptions = 1 << 0,
        DispatchChangeEvent = 1 << 1,
        UserDriven = 1 << 2,
    };
    typedef unsigned SelectOptionFlags;
    void selectOption(int optionIndex, SelectOptionFlags = 0);
    void deselectItemsWithoutValidation(HTMLElement* elementToExclude = 0);
    //void parseMultipleAttribute(const Attribute&);
    int lastSelectedListIndex() const;
    void updateSelectedState(int listIndex, bool multi, bool shift);
    void menuListDefaultEventHandler(Event*);
    bool platformHandleKeydownEvent(KeyboardEvent*);
    void listBoxDefaultEventHandler(Event*);
    void setOptionsChangedOnRenderer();
    size_t searchOptionsForValue(const String&, size_t listIndexStart, size_t listIndexEnd) const;

    enum SkipDirection {
        SkipBackwards = -1,
        SkipForwards = 1
    };
    int nextValidIndex(int listIndex, SkipDirection, int skip) const;
    int nextSelectableListIndex(int startIndex) const;
    int previousSelectableListIndex(int startIndex) const;
    int firstSelectableListIndex() const;
    int lastSelectableListIndex() const;
    int nextSelectableListIndexPageAway(int startIndex, SkipDirection) const;

    virtual bool areAuthorShadowsAllowed() const OVERRIDE { return false; }

    mutable Vector<HTMLElement*> m_listItems;
    Vector<bool> m_lastOnChangeSelection;
    Vector<bool> m_cachedStateForActiveSelection;
    DOMTimeStamp m_lastCharTime;
    String m_typedString;
    int m_size;
    int m_lastOnChangeIndex;
    int m_activeSelectionAnchorIndex;
    int m_activeSelectionEndIndex;
    UChar m_repeatingChar;
    bool m_isProcessingUserDrivenChange;
    bool m_multiple;
    bool m_activeSelectionState;
    mutable bool m_shouldRecalcListItems;
    MediaControlElementType m_displayType;


    MediaControllerInterface* m_mediaController;
};

inline bool isMediaSelectElement(const Node* node)
{
    return node->hasTagName(HTMLNames::selectTag);
}

inline MediaSelectElement* toMediaSelectElement(Node* node)
{
    ASSERT(!node || isMediaSelectElement(node));
    return static_cast<MediaSelectElement*>(node);
}

inline const MediaSelectElement* toMediaSelectElement(const Node* node)
{
    ASSERT(!node || isMediaSelectElement(node));
    return static_cast<const MediaSelectElement*>(node);
}

void toMediaSelectElement(const MediaSelectElement*); // This overload will catch anyone doing an unnecessary cast.

// ----------------------------

class MediaControlPanelElement : public MediaControlElement {
public:
    static PassRefPtr<MediaControlPanelElement> create(Document*);

    void setCanBeDragged(bool);
    void setIsDisplayed(bool);

    void resetPosition();
    void makeOpaque();
    void makeTransparent();

    virtual bool willRespondToMouseMoveEvents() OVERRIDE { return true; }
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }

private:
    MediaControlPanelElement(Document*);
    virtual MediaControlElementType displayType() const;
    virtual const AtomicString& shadowPseudoId() const;
    virtual void defaultEventHandler(Event*);

    void startDrag(const LayoutPoint& eventLocation);
    void continueDrag(const LayoutPoint& eventLocation);
    void endDrag();

    void startTimer();
    void stopTimer();
    void transitionTimerFired(Timer<MediaControlPanelElement>*);

    void setPosition(const LayoutPoint&);

    bool m_canBeDragged;
    bool m_isBeingDragged;
    bool m_isDisplayed;
    bool m_opaque;
    LayoutPoint m_dragStartEventLocation;

    Timer<MediaControlPanelElement> m_transitionTimer;
};

// ----------------------------

class MediaControlTimelineContainerElement : public MediaControlElement {
public:
    static PassRefPtr<MediaControlTimelineContainerElement> create(Document*);

private:
    MediaControlTimelineContainerElement(Document*);
    virtual const AtomicString& shadowPseudoId() const;

    virtual MediaControlElementType displayType() const;
};

// ----------------------------

class MediaControlVolumeSliderContainerElement : public MediaControlElement {
public:
    static PassRefPtr<MediaControlVolumeSliderContainerElement> create(Document*);

    virtual bool willRespondToMouseMoveEvents() OVERRIDE { return true; }

private:
    MediaControlVolumeSliderContainerElement(Document*);
    virtual RenderObject* createRenderer(RenderArena*, RenderStyle*);
    virtual void defaultEventHandler(Event*);
    virtual MediaControlElementType displayType() const;
    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlStatusDisplayElement : public MediaControlElement {
public:
    static PassRefPtr<MediaControlStatusDisplayElement> create(Document*);

    void update();

private:
    MediaControlStatusDisplayElement(Document*);

    virtual MediaControlElementType displayType() const;
    virtual const AtomicString& shadowPseudoId() const;

    enum StateBeingDisplayed { Nothing, Loading, LiveBroadcast };
    StateBeingDisplayed m_stateBeingDisplayed;
};

// ----------------------------

class MediaControlInputElement : public HTMLInputElement {
public:
    void hide();
    void show();

    MediaControlElementType displayType() const { return m_displayType; }

    virtual void setMediaController(MediaControllerInterface* controller) { m_mediaController = controller; }
    MediaControllerInterface* mediaController() const { return m_mediaController; }

protected:
    MediaControlInputElement(Document*, MediaControlElementType);

    void setDisplayType(MediaControlElementType);

private:
    virtual bool isMediaControlElement() const { return true; }

    virtual void updateDisplayType() { }

    MediaControllerInterface* m_mediaController;
    MediaControlElementType m_displayType;
};

// ----------------------------

class MediaControlMuteButtonElement : public MediaControlInputElement {
public:
    void changedMute();

    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }

protected:
    MediaControlMuteButtonElement(Document*, MediaControlElementType);
    virtual void defaultEventHandler(Event*);

private:
    virtual void updateDisplayType();
};

// ----------------------------

class MediaControlPanelMuteButtonElement : public MediaControlMuteButtonElement {
public:
    static PassRefPtr<MediaControlPanelMuteButtonElement> create(Document*, MediaControls*);

    virtual bool willRespondToMouseMoveEvents() OVERRIDE { return true; }

private:
    MediaControlPanelMuteButtonElement(Document*, MediaControls*);

    virtual void defaultEventHandler(Event*);
    virtual const AtomicString& shadowPseudoId() const;

    MediaControls* m_controls;
};

// ----------------------------

class MediaControlVolumeSliderMuteButtonElement : public MediaControlMuteButtonElement {
public:
    static PassRefPtr<MediaControlVolumeSliderMuteButtonElement> create(Document*);

private:
    MediaControlVolumeSliderMuteButtonElement(Document*);

    virtual const AtomicString& shadowPseudoId() const;
};


// ----------------------------

class MediaControlPlayButtonElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlPlayButtonElement> create(Document*);

    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }
    virtual void updateDisplayType();

private:
    MediaControlPlayButtonElement(Document*);

    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlFFButtonElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlFFButtonElement> create(Document*);

    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }
    MediaControlElementType displayType();
    virtual void updateDisplayType();

private:
    MediaControlFFButtonElement(Document*);

    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlRevButtonElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlRevButtonElement> create(Document*);

    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }
    MediaControlElementType displayType() const;
    virtual void updateDisplayType();

private:
    MediaControlRevButtonElement(Document*);

    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlOverlayPlayButtonElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlOverlayPlayButtonElement> create(Document*);

    virtual void defaultEventHandler(Event*);
    virtual void updateDisplayType();

private:
    explicit MediaControlOverlayPlayButtonElement(Document*);

    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------
class MediaControlSeekButtonElement : public MediaControlInputElement {
public:
    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }

protected:
    MediaControlSeekButtonElement(Document*, MediaControlElementType);

private:
    virtual bool isForwardButton() const = 0;
    virtual void setActive(bool /*flag*/ = true, bool /*pause*/ = false);

    void startTimer();
    void stopTimer();
    float nextRate() const;
    void seekTimerFired(Timer<MediaControlSeekButtonElement>*);

    enum ActionType { Nothing, Play, Pause };
    ActionType m_actionOnStop;
    enum SeekType { Skip, Scan };
    SeekType m_seekType;
    Timer<MediaControlSeekButtonElement> m_seekTimer;
};

// ----------------------------

class MediaControlSeekForwardButtonElement : public MediaControlSeekButtonElement {
public:
    static PassRefPtr<MediaControlSeekForwardButtonElement> create(Document*);

private:
    MediaControlSeekForwardButtonElement(Document*);

    virtual bool isForwardButton() const { return true; }
    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlSeekBackButtonElement : public MediaControlSeekButtonElement {
public:
    static PassRefPtr<MediaControlSeekBackButtonElement> create(Document*);

private:
    MediaControlSeekBackButtonElement(Document*);

    virtual bool isForwardButton() const { return false; }
    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlRewindButtonElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlRewindButtonElement> create(Document*);

    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }

private:
    MediaControlRewindButtonElement(Document*);

    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlReturnToRealtimeButtonElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlReturnToRealtimeButtonElement> create(Document*);

    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }

private:
    MediaControlReturnToRealtimeButtonElement(Document*);

    virtual const AtomicString& shadowPseudoId() const;
};    

// ----------------------------

class MediaControlToggleClosedCaptionsButtonElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlToggleClosedCaptionsButtonElement> create(Document*);

    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }
    virtual void updateDisplayType();

private:
    MediaControlToggleClosedCaptionsButtonElement(Document*);

    virtual const AtomicString& shadowPseudoId() const;
};    

// ----------------------------

class MediaControlTimelineElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlTimelineElement> create(Document*, MediaControls*);

    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseClickEvents() OVERRIDE;
    void setPosition(float);
    void setDuration(float);

private:
    MediaControlTimelineElement(Document*, MediaControls*);

    virtual const AtomicString& shadowPseudoId() const;

    MediaControls* m_controls;
};

// ----------------------------

class MediaControlVolumeSliderElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlVolumeSliderElement> create(Document*);

    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseMoveEvents() OVERRIDE;
    virtual bool willRespondToMouseClickEvents() OVERRIDE;
    void setVolume(float);
    void setClearMutedOnUserInteraction(bool);

protected:
    MediaControlVolumeSliderElement(Document*);

private:
    virtual const AtomicString& shadowPseudoId() const;
    bool m_clearMutedOnUserInteraction;
};

// ----------------------------

class MediaControlFullscreenButtonElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlFullscreenButtonElement> create(Document*, MediaControls*);

    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }
    void setIsFullscreen(bool);

private:
    MediaControlFullscreenButtonElement(Document*, MediaControls*);

    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlVideoTrackSelButtonElement : public MediaSelectElement {
public:
    static PassRefPtr<MediaControlVideoTrackSelButtonElement> create(Document*, MediaControls* controls);
    void changedVideoTrack();

    virtual void defaultEventHandler(Event* event);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }
    virtual void updateDisplayType();
    virtual MediaControlElementType displayType() const {return MediaVideoTrackSelButton;}

    bool hasTracks() { int index; return mediaController()->getSelVideoTrackNames(&index).size()>1; }

    void display();

protected:
    virtual void selectChanged(int newIndex);

private:
    MediaControlVideoTrackSelButtonElement(Document* doc, MediaControls* controls);

    virtual const AtomicString& shadowPseudoId() const;

    MediaControls* m_controls;
};

// ----------------------------

class MediaControlAudioTrackSelButtonElement : public MediaSelectElement {
public:
    static PassRefPtr<MediaControlAudioTrackSelButtonElement> create(Document*, MediaControls* controls);
    void changedAudioTrack();

    virtual void defaultEventHandler(Event* event);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }
    virtual void updateDisplayType();
    virtual MediaControlElementType displayType() const {return MediaAudioTrackSelButton;}

    bool hasTracks() { int index; return mediaController()->getSelAudioTrackNames(&index).size()>1; }

    void display();

protected:
    virtual void selectChanged(int newIndex);

private:
    MediaControlAudioTrackSelButtonElement(Document* doc, MediaControls* controls);

    virtual const AtomicString& shadowPseudoId() const;

    MediaControls* m_controls;
};

// ----------------------------

class MediaControlTextTrackSelButtonElement : public MediaSelectElement {
public:
    static PassRefPtr<MediaControlTextTrackSelButtonElement> create(Document*, MediaControls* controls);
    void changedTextTrack();

    virtual void defaultEventHandler(Event* event);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }
    virtual void updateDisplayType();
    virtual MediaControlElementType displayType() const {return MediaTextTrackSelButton;}

    bool hasTracks() { int index; return mediaController()->getSelTextTrackNames(&index).size()>0; }  // We add none so compare against 0

    void display();

protected:
    virtual void selectChanged(int newIndex);

private:
    MediaControlTextTrackSelButtonElement(Document* doc, MediaControls* controls);

    virtual const AtomicString& shadowPseudoId() const;

    MediaControls* m_controls;
};

// ----------------------------

class MediaControlFullscreenVolumeSliderElement : public MediaControlVolumeSliderElement {
public:
    static PassRefPtr<MediaControlFullscreenVolumeSliderElement> create(Document*);
    
private:
    MediaControlFullscreenVolumeSliderElement(Document*);
    
    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlFullscreenVolumeMinButtonElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlFullscreenVolumeMinButtonElement> create(Document*);
    
    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }

private:
    MediaControlFullscreenVolumeMinButtonElement(Document*);
    
    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlFullscreenVolumeMaxButtonElement : public MediaControlInputElement {
public:
    static PassRefPtr<MediaControlFullscreenVolumeMaxButtonElement> create(Document*);
    
    virtual void defaultEventHandler(Event*);
    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }

private:
    MediaControlFullscreenVolumeMaxButtonElement(Document*);
    
    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlTimeDisplayElement : public MediaControlElement {
public:
    void setCurrentValue(float);
    float currentValue() const { return m_currentValue; }

protected:
    MediaControlTimeDisplayElement(Document*);

private:
    virtual RenderObject* createRenderer(RenderArena*, RenderStyle*);

    float m_currentValue;
};

// ----------------------------

class MediaControlTimeRemainingDisplayElement : public MediaControlTimeDisplayElement {
public:
    static PassRefPtr<MediaControlTimeRemainingDisplayElement> create(Document*);

private:
    MediaControlTimeRemainingDisplayElement(Document*);

    virtual MediaControlElementType displayType() const;
    virtual const AtomicString& shadowPseudoId() const;
};

// ----------------------------

class MediaControlCurrentTimeDisplayElement : public MediaControlTimeDisplayElement {
public:
    static PassRefPtr<MediaControlCurrentTimeDisplayElement> create(Document*);

private:
    MediaControlCurrentTimeDisplayElement(Document*);

    virtual MediaControlElementType displayType() const;
    virtual const AtomicString& shadowPseudoId() const;
};
 
// ----------------------------

#if ENABLE(VIDEO_TRACK)
class MediaControlTextTrackContainerElement : public MediaControlElement {
public:
    static PassRefPtr<MediaControlTextTrackContainerElement> create(Document*);

    void updateDisplay();
    void updateSizes();

private:
    MediaControlTextTrackContainerElement(Document*);

    virtual RenderObject* createRenderer(RenderArena*, RenderStyle*);
    virtual MediaControlElementType displayType() const { return MediaTextTrackDisplayContainer; }
    virtual const AtomicString& shadowPseudoId() const;

    IntRect m_videoDisplaySize;
    float m_fontSize;
};

#endif
// ----------------------------

} // namespace WebCore

#endif // ENABLE(VIDEO)

#endif // MediaControlElements_h
