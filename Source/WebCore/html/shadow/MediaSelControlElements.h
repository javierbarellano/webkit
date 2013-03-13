/*
 * MediaSelControlElement.h
 *
 *  Created on: Mar 12, 2013
 *      Author: gar
 */

#ifndef MEDIASELCONTROLELEMENT_H_
#define MEDIASELCONTROLELEMENT_H_

#if ENABLE(VIDEO)

#include "HTMLOptionElement.h"
#include "MediaControlElements.h"

#include "Event.h"
#include "HTMLFormControlElementWithState.h"
#include "HTMLOptionsCollection.h"
#include <wtf/Vector.h>

namespace WebCore
{

class MediaSelectElement;
class MouseEvent;

class MediaSelControlElements: public MediaControlElement {
public:
	virtual ~MediaSelControlElements(){}

protected:
	MediaSelControlElements(Document* doc) : MediaControlElement(doc) {}
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

}

#endif /* ENABLE(VIDEO) */

#endif /* MEDIASELCONTROLELEMENT_H_ */
