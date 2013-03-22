/*
 * MediaSelControlElement.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: gar
 */

#include "config.h"

#if ENABLE(VIDEO)

#include <string>
#include <sstream>

#include "MediaControlElements.h"
#include "MediaControls.h"
#include "MediaSelControlElements.h"

#include "NodeRenderingContext.h"
#include "AXObjectCache.h"
#include "HTMLDataListElement.h"
#include "Node.h"
#include "HTMLOptionElement.h"
#include "HTMLFormElement.h"
#include "KeyboardEvent.h"
#include "SpatialNavigation.h"
#include "FormController.h"
#include "FormDataList.h"
#include "HTMLFormControlElementWithState.h"
#include "CSSStyleDeclaration.h"
#include "CSSValueKeywords.h"
#include "DOMTokenList.h"
#include "EventNames.h"
#include "FloatConversion.h"
#include "FloatPoint.h"
#include "Frame.h"
#include "HTMLOptionElement.h"
#include "HTMLDivElement.h"
#include "HTMLMediaElement.h"
#include "HTMLNames.h"
#include "HTMLVideoElement.h"
#include "LayoutRepainter.h"
#include "LocalizedStrings.h"
#include "MediaControls.h"
#include "MouseEvent.h"
#include "Page.h"
#include "RenderDeprecatedFlexibleBox.h"
#include "RenderListBox.h"
#include "RenderMediaMenuList.h"
#include "RenderInline.h"
#include "RenderMedia.h"
#include "RenderSlider.h"
#include "RenderText.h"
#include "RenderTheme.h"
#include "RenderVideo.h"
#include "RenderView.h"
#include "ScriptController.h"
#include "Settings.h"
#include "StyleResolver.h"
#include "Text.h"

namespace WebCore
{

using namespace HTMLNames;
using namespace std;

static const DOMTimeStamp typeAheadTimeout = 1000;
// ----------------------------

PassRefPtr<MediaOptionElement> MediaOptionElement::create(Document* document)
{
    return adoptRef(new MediaOptionElement(document));
}

MediaOptionElement::MediaOptionElement(Document* doc) :
		HTMLOptionElement(optionTag, doc)
{
}

Node::InsertionNotificationRequest MediaOptionElement::insertedInto(ContainerNode* insertionPoint)
{
    if (MediaSelectElement* select = ownerMediaElement()) {
        select->setRecalcListItems();
        // Do not call selected() since calling updateListItemSelectedStates()
        // at this time won't do the right thing. (Why, exactly?)
        // FIXME: Might be better to call this unconditionally, always passing m_isSelected,
        // rather than only calling it if we are selected.
        if (m_isSelected)
            select->optionSelectionStateChanged(this, true);
        select->scrollToSelection();
    }

    return HTMLElement::insertedInto(insertionPoint);
}

MediaSelectElement* MediaOptionElement::ownerMediaElement() const
{
    ContainerNode* select = parentNode();
    while (select && !select->hasTagName(selectTag))
        select = select->parentNode();

    if (!select)
        return 0;

    return toMediaSelectElement(select);
}

void MediaOptionElement::setText(const String &text, ExceptionCode& ec)
{
    RefPtr<Node> protectFromMutationEvents(this);

    // Changing the text causes a recalc of a select's items, which will reset the selected
    // index to the first item if the select is single selection with a menu list. We attempt to
    // preserve the selected item.
    RefPtr<MediaSelectElement> select = ownerMediaElement();
    bool selectIsMenuList = select && select->usesMenuList();
    int oldSelectedIndex = selectIsMenuList ? select->selectedIndex() : -1;

    // Handle the common special case where there's exactly 1 child node, and it's a text node.
    Node* child = firstChild();
    if (child && child->isTextNode() && !child->nextSibling())
        toText(child)->setData(text, ec);
    else {
        removeChildren();
        appendChild(Text::create(document(), text), ec);
    }

    if (selectIsMenuList && select->selectedIndex() != oldSelectedIndex)
    	select->setSelectedIndex(oldSelectedIndex);
}

void MediaOptionElement::accessKeyAction(bool)
{
	MediaSelectElement* select = ownerMediaElement();
    if (select)
        select->accessKeySetSelectedIndex(index());
}

int MediaOptionElement::index() const
{
    // It would be faster to cache the index, but harder to get it right in all cases.

	MediaSelectElement* selectElement = ownerMediaElement();
    if (!selectElement)
        return 0;

    int optionIndex = 0;

    const Vector<HTMLElement*>& items = selectElement->listItems();
    size_t length = items.size();
    for (size_t i = 0; i < length; ++i) {
        if (!items[i]->hasTagName(optionTag))
            continue;
        if (items[i] == this)
            return optionIndex;
        ++optionIndex;
    }

    return 0;
}

bool MediaOptionElement::selected()
{
    if (MediaSelectElement* select = ownerMediaElement())
        select->updateListItemSelectedStates();
    return m_isSelected;
}

void MediaOptionElement::setSelected(bool selected)
{
    if (m_isSelected == selected)
        return;

    setSelectedState(selected);

//    if (MediaSelectElement* select = ownerMediaElement())
//        select->optionSelectionStateChanged(this, selected);
}

void MediaOptionElement::setSelectedState(bool selected)
{
    if (m_isSelected == selected)
        return;

    m_isSelected = selected;
    //setNeedsStyleRecalc();

//    if (MediaSelectElement* select = ownerMediaElement())
//        select->invalidateSelectedItems();
}

void MediaOptionElement::childrenChanged(bool changedByParser, Node* beforeChange, Node* afterChange, int childCountDelta)
{
#if ENABLE(DATALIST_ELEMENT)
    if (HTMLDataListElement* dataList = ownerDataListElement())
        dataList->optionElementChildrenChanged();
    else
#endif
    if (MediaSelectElement* select = ownerMediaElement())
        select->optionElementChildrenChanged();
    HTMLElement::childrenChanged(changedByParser, beforeChange, afterChange, childCountDelta);
}

void MediaOptionElement::setRenderStyle(PassRefPtr<RenderStyle> newStyle)
{
    m_style = newStyle;
    if (MediaSelectElement* select = ownerMediaElement()) {
        if (RenderObject* renderer = select->renderer())
            renderer->repaint();
    }
}

// ----------------------------

MediaSelectElement::MediaSelectElement(Document* document, MediaControlElementType displayType, MediaControls* controls)
    : HTMLFormControlElementWithState(selectTag, document, NULL)
	, m_hasFocus(false)
	, m_controls(controls)
    , m_mediaController(0)
{
	m_displayType = displayType;
	m_multiple = false;
}

void MediaSelectElement::setMediaController(MediaControllerInterface* controller)
{
	m_mediaController = controller;
	//m_controls->setMediaController(controller);
}

MediaControllerInterface* MediaSelectElement::mediaController() const
{
	return m_mediaController;
}


void MediaSelectElement::show()
{
    removeInlineStyleProperty(CSSPropertyDisplay);
}

void MediaSelectElement::hide()
{
    setInlineStyleProperty(CSSPropertyDisplay, CSSValueNone);
}

void MediaSelectElement::setDisplayType(MediaControlElementType displayType)
{

    if (displayType == m_displayType)
        return;

    m_displayType = displayType;
    if (RenderObject* object = renderer())
        object->repaint();
}

void MediaSelectElement::fixEventState(Event *event)
{
	if ((event->type() == eventNames().focusEvent
			|| event->type() == eventNames().focusinEvent
			|| event->type() == eventNames().DOMFocusInEvent
			|| event->type() == eventNames().blurEvent
			|| event->type() == eventNames().mousedownEvent
			|| event->type() == eventNames().mouseoutEvent)
			&& event->isMouseEvent()
			&& !m_hasFocus) {

		//printf("VideoTrack:setDefaultHandled() event type: %s\n",event->type().string().ascii().data());
		event->setDefaultHandled();
		if (event->type() == eventNames().mouseoutEvent)
			m_hasFocus = true;

	} else if (m_hasFocus) {
		m_hasFocus = false;
	}
}

void MediaSelectElement::createShadowSubtree()
{
    ASSERT(!shadow());
    ShadowRoot::create(this, ShadowRoot::UserAgentShadowRoot, ASSERT_NO_EXCEPTION);
}

const AtomicString& MediaSelectElement::formControlType() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, selectMultiple, ("select-multiple"));
    DEFINE_STATIC_LOCAL(const AtomicString, selectOne, ("select-one"));
    return m_multiple ? selectMultiple : selectOne;
}

void MediaSelectElement::optionSelectedByUser(int optionIndex, bool fireOnChangeNow, bool allowMultipleSelection)
{
	selectChanged(optionIndex);

    // User interaction such as mousedown events can cause list box select elements to send change events.
    // This produces that same behavior for changes triggered by other code running on behalf of the user.
    if (!usesMenuList()) {
        updateSelectedState(optionIndex, allowMultipleSelection, false);
        setNeedsValidityCheck();
        if (fireOnChangeNow)
            listBoxOnChange();
        return;
    }

    // Bail out if this index is already the selected one, to avoid running unnecessary JavaScript that can mess up
    // autofill when there is no actual change (see https://bugs.webkit.org/show_bug.cgi?id=35256 and <rdar://7467917>).
    // The selectOption function does not behave this way, possibly because other callers need a change event even
    // in cases where the selected option is not change.
    if (optionIndex == selectedIndex())
        return;

    selectOption(optionIndex, DeselectOtherOptions | (fireOnChangeNow ? DispatchChangeEvent : 0) | UserDriven);
}

bool MediaSelectElement::hasPlaceholderLabelOption() const
{
   // The select element has no placeholder label option if it has an attribute "multiple" specified or a display size of non-1.
    //
    // The condition "size() > 1" is not compliant with the HTML5 spec as of Dec 3, 2010. "size() != 1" is correct.
    // Using "size() > 1" here because size() may be 0 in WebKit.
    // See the discussion at https://bugs.webkit.org/show_bug.cgi?id=43887
    //
    // "0 size()" happens when an attribute "size" is absent or an invalid size attribute is specified.
    // In this case, the display size should be assumed as the default.
    // The default display size is 1 for non-multiple select elements, and 4 for multiple select elements.
    //
    // Finally, if size() == 0 and non-multiple, the display size can be assumed as 1.
    if (multiple() || size() > 1)
        return false;

    int listIndex = optionToListIndex(0);
    ASSERT(listIndex >= 0);
    if (listIndex < 0)
        return false;
    HTMLOptionElement* option = static_cast<HTMLOptionElement*>(listItems()[listIndex]);
    return !listIndex && option->value().isEmpty();
}

bool MediaSelectElement::valueMissing() const
{
    if (!willValidate())
        return false;

    if (!isRequiredFormControl())
        return false;

    int firstSelectionIndex = selectedIndex();

    // If a non-placeholer label option is selected (firstSelectionIndex > 0), it's not value-missing.
    return firstSelectionIndex < 0 || (!firstSelectionIndex && hasPlaceholderLabelOption());
}

void MediaSelectElement::listBoxSelectItem(int listIndex, bool allowMultiplySelections, bool shift, bool fireOnChangeNow)
{
    if (!multiple())
        optionSelectedByUser(listToOptionIndex(listIndex), fireOnChangeNow, false);
    else {
        updateSelectedState(listIndex, allowMultiplySelections, shift);
        setNeedsValidityCheck();
        if (fireOnChangeNow)
            listBoxOnChange();
    }
}

bool MediaSelectElement::usesMenuList() const
{
	return true;

//   const Page* page = document()->page();
//    RefPtr<RenderTheme> renderTheme = page ? page->theme() : RenderTheme::defaultTheme();
//    if (renderTheme->delegatesMenuListRendering())
//        return true;
//
//    return !m_multiple && m_size <= 1;
}

void MediaSelectElement::add(HTMLElement* element, HTMLElement* before, ExceptionCode& ec)
{
    // Make sure the element is ref'd and deref'd so we don't leak it.
    RefPtr<HTMLElement> protectNewChild(element);

    if (!element || !(element->hasLocalName(optionTag) || element->hasLocalName(hrTag)))
        return;

    insertBefore(element, before, ec);
    setNeedsValidityCheck();
}

void MediaSelectElement::remove(int optionIndex)
{
    int listIndex = optionToListIndex(optionIndex);
    if (listIndex < 0)
        return;

    ExceptionCode ec;
    listItems()[listIndex]->remove(ec);
}


bool MediaSelectElement::isKeyboardFocusable(KeyboardEvent* event) const
{
    if (renderer())
        return isFocusable();
    return HTMLFormControlElementWithState::isKeyboardFocusable(event);
}


RenderObject* MediaSelectElement::createRenderer(RenderArena* arena, RenderStyle*)
{
    if (usesMenuList())
        return new (arena) RenderMediaMenuList(this);
    return new (arena) RenderListBox(this);
}


PassRefPtr<HTMLOptionsCollection> MediaSelectElement::options()
{
    return static_cast<HTMLOptionsCollection*>(ensureCachedHTMLCollection(SelectOptions).get());
}

void MediaSelectElement::updateListItemSelectedStates()
{
    if (m_shouldRecalcListItems)
        recalcListItems();
}

void MediaSelectElement::optionElementChildrenChanged()
{
    setRecalcListItems();
    setNeedsValidityCheck();

    if (AXObjectCache::accessibilityEnabled() && renderer())
        renderer()->document()->axObjectCache()->childrenChanged(this);
}

void MediaSelectElement::accessKeyAction(bool sendMouseEvents)
{
    focus();
    dispatchSimulatedClick(0, sendMouseEvents ? SendMouseUpDownEvents:SendNoEvents);
}


bool MediaSelectElement::isRequiredFormControl() const
{
    return false;
}

// Returns the 1st valid item |skip| items from |listIndex| in direction |direction| if there is one.
// Otherwise, it returns the valid item closest to that boundary which is past |listIndex| if there is one.
// Otherwise, it returns |listIndex|.
// Valid means that it is enabled and an option element.
int MediaSelectElement::nextValidIndex(int listIndex, SkipDirection direction, int skip) const
{
    ASSERT(direction == -1 || direction == 1);
    const Vector<HTMLElement*>& listItems = this->listItems();
    int lastGoodIndex = listIndex;
    int size = listItems.size();
    for (listIndex += direction; listIndex >= 0 && listIndex < size; listIndex += direction) {
        --skip;
        if (!listItems[listIndex]->disabled() && listItems[listIndex]->hasTagName(optionTag)) {
            lastGoodIndex = listIndex;
            if (skip <= 0)
                break;
        }
    }
    return lastGoodIndex;
}

int MediaSelectElement::nextSelectableListIndex(int startIndex) const
{
    return nextValidIndex(startIndex, SkipForwards, 1);
}

int MediaSelectElement::previousSelectableListIndex(int startIndex) const
{
    if (startIndex == -1)
        startIndex = listItems().size();
    return nextValidIndex(startIndex, SkipBackwards, 1);
}

int MediaSelectElement::firstSelectableListIndex() const
{
    const Vector<HTMLElement*>& items = listItems();
    int index = nextValidIndex(items.size(), SkipBackwards, INT_MAX);
    if (static_cast<size_t>(index) == items.size())
        return -1;
    return index;
}

int MediaSelectElement::lastSelectableListIndex() const
{
    return nextValidIndex(-1, SkipForwards, INT_MAX);
}

// Returns the index of the next valid item one page away from |startIndex| in direction |direction|.
int MediaSelectElement::nextSelectableListIndexPageAway(int startIndex, SkipDirection direction) const
{
    const Vector<HTMLElement*>& items = listItems();
    // Can't use m_size because renderer forces a minimum size.
    int pageSize = 0;
    if (renderer()->isListBox())
        pageSize = toRenderListBox(renderer())->size() - 1; // -1 so we still show context.

    // One page away, but not outside valid bounds.
    // If there is a valid option item one page away, the index is chosen.
    // If there is no exact one page away valid option, returns startIndex or the most far index.
    int edgeIndex = (direction == SkipForwards) ? 0 : (items.size() - 1);
    int skipAmount = pageSize + ((direction == SkipForwards) ? startIndex : (edgeIndex - startIndex));
    return nextValidIndex(edgeIndex, direction, skipAmount);
}

void MediaSelectElement::saveLastSelection()
{
    if (usesMenuList()) {
        m_lastOnChangeIndex = selectedIndex();
        return;
    }

    m_lastOnChangeSelection.clear();
    const Vector<HTMLElement*>& items = listItems();
    for (unsigned i = 0; i < items.size(); ++i) {
        HTMLElement* element = items[i];
        m_lastOnChangeSelection.append(element->hasTagName(optionTag) && toHTMLOptionElement(element)->selected());
    }
}

void MediaSelectElement::setActiveSelectionAnchorIndex(int index)
{
    m_activeSelectionAnchorIndex = index;

    // Cache the selection state so we can restore the old selection as the new
    // selection pivots around this anchor index.
    m_cachedStateForActiveSelection.clear();

    const Vector<HTMLElement*>& items = listItems();
    for (unsigned i = 0; i < items.size(); ++i) {
        HTMLElement* element = items[i];
        m_cachedStateForActiveSelection.append(element->hasTagName(optionTag) && toHTMLOptionElement(element)->selected());
    }
}

void MediaSelectElement::setActiveSelectionEndIndex(int index)
{
    m_activeSelectionEndIndex = index;
}

void MediaSelectElement::updateListBoxSelection(bool deselectOtherOptions)
{
    ASSERT(renderer() && (renderer()->isListBox() || m_multiple));
    ASSERT(!listItems().size() || m_activeSelectionAnchorIndex >= 0);

    unsigned start = min(m_activeSelectionAnchorIndex, m_activeSelectionEndIndex);
    unsigned end = max(m_activeSelectionAnchorIndex, m_activeSelectionEndIndex);

    const Vector<HTMLElement*>& items = listItems();
    for (unsigned i = 0; i < items.size(); ++i) {
        HTMLElement* element = items[i];
        if (!element->hasTagName(optionTag) || toHTMLOptionElement(element)->disabled())
            continue;

        if (i >= start && i <= end)
            toHTMLOptionElement(element)->setSelectedState(m_activeSelectionState);
        else if (deselectOtherOptions || i >= m_cachedStateForActiveSelection.size())
            toHTMLOptionElement(element)->setSelectedState(false);
        else
            toHTMLOptionElement(element)->setSelectedState(m_cachedStateForActiveSelection[i]);
    }

    scrollToSelection();
    setNeedsValidityCheck();
}

void MediaSelectElement::listBoxOnChange()
{
    ASSERT(!usesMenuList() || m_multiple);

    const Vector<HTMLElement*>& items = listItems();

    // If the cached selection list is empty, or the size has changed, then fire
    // dispatchFormControlChangeEvent, and return early.
    if (m_lastOnChangeSelection.isEmpty() || m_lastOnChangeSelection.size() != items.size()) {
        dispatchFormControlChangeEvent();
        return;
    }

    // Update m_lastOnChangeSelection and fire dispatchFormControlChangeEvent.
    bool fireOnChange = false;
    for (unsigned i = 0; i < items.size(); ++i) {
        HTMLElement* element = items[i];
        bool selected = element->hasTagName(optionTag) && toHTMLOptionElement(element)->selected();
        if (selected != m_lastOnChangeSelection[i])
            fireOnChange = true;
        m_lastOnChangeSelection[i] = selected;
    }

    if (fireOnChange)
        dispatchFormControlChangeEvent();
}

void MediaSelectElement::dispatchChangeEventForMenuList()
{
    ASSERT(usesMenuList());

    int selected = selectedIndex();
    if (m_lastOnChangeIndex != selected && m_isProcessingUserDrivenChange) {
        m_lastOnChangeIndex = selected;
        m_isProcessingUserDrivenChange = false;
        dispatchFormControlChangeEvent();
    }
}

void MediaSelectElement::scrollToSelection()
{
    if (usesMenuList())
        return;

    if (RenderObject* renderer = this->renderer())
        toRenderListBox(renderer)->selectionChanged();
}

void MediaSelectElement::setOptionsChangedOnRenderer()
{
    if (RenderObject* renderer = this->renderer()) {
        if (usesMenuList())
            toRenderMediaMenuList(renderer)->setOptionsChanged(true);
        else
            toRenderListBox(renderer)->setOptionsChanged(true);
    }
}

const Vector<HTMLElement*>& MediaSelectElement::listItems() const
{
    recalcListItems();
    return m_listItems;
}

void MediaSelectElement::invalidateSelectedItems()
{
    if (HTMLCollection* collection = cachedHTMLCollection(SelectedOptions))
        collection->invalidateCache();
}

void MediaSelectElement::setRecalcListItems()
{
    m_shouldRecalcListItems = true;
    // Manual selection anchor is reset when manipulating the select programmatically.
    m_activeSelectionAnchorIndex = -1;
    setOptionsChangedOnRenderer();
    setNeedsStyleRecalc();
    if (!inDocument()) {
        if (HTMLCollection* collection = cachedHTMLCollection(SelectOptions))
            collection->invalidateCache();
    }
    if (!inDocument())
        invalidateSelectedItems();
}

void MediaSelectElement::recalcListItems(bool updateSelectedStates) const
{
    m_listItems.clear();

    m_shouldRecalcListItems = false;

    HTMLOptionElement* foundSelected = 0;
    HTMLOptionElement* firstOption = 0;
    for (Node* currentNode = this->firstChild(); currentNode;) {
        if (!currentNode->isHTMLElement()) {
            currentNode = currentNode->traverseNextSibling(this);
            continue;
        }

        HTMLElement* current = toHTMLElement(currentNode);

        // optgroup tags may not nest. However, both FireFox and IE will
        // flatten the tree automatically, so we follow suit.
        // (http://www.w3.org/TR/html401/interact/forms.html#h-17.6)
        if (current->hasTagName(optgroupTag)) {
            m_listItems.append(current);
            if (current->firstChild()) {
                currentNode = current->firstChild();
                continue;
            }
        }

        if (current->hasTagName(optionTag)) {
            m_listItems.append(current);

            if (updateSelectedStates && !m_multiple) {
                HTMLOptionElement* option = toHTMLOptionElement(current);
                if (!firstOption)
                    firstOption = option;
                if (option->selected()) {
                    if (foundSelected)
                        foundSelected->setSelectedState(false);
                    foundSelected = option;
                } else if (m_size <= 1 && !foundSelected && !option->disabled()) {
                    foundSelected = option;
                    foundSelected->setSelectedState(true);
                }
            }
        }

        if (current->hasTagName(hrTag))
            m_listItems.append(current);

        // In conforming HTML code, only <optgroup> and <option> will be found
        // within a <select>. We call traverseNextSibling so that we only step
        // into those tags that we choose to. For web-compat, we should cope
        // with the case where odd tags like a <div> have been added but we
        // handle this because such tags have already been removed from the
        // <select>'s subtree at this point.
        currentNode = currentNode->traverseNextSibling(this);
    }

    if (!foundSelected && m_size <= 1 && firstOption && !firstOption->selected())
        firstOption->setSelectedState(true);
}

int MediaSelectElement::selectedIndex() const
{
    unsigned index = 0;

    // Return the number of the first option selected.
    const Vector<HTMLElement*>& items = listItems();
    //printf("selectedIndex(): len: %d, ", (int)items.size());
    for (size_t i = 0; i < items.size(); ++i) {
        HTMLElement* element = items[i];
        if (element->hasTagName(optionTag)) {
            if (toHTMLOptionElement(element)->selected()) {
            	//printf("SelIndex: %d\n",index);
            	return index;
            }
            ++index;
        }
    }

    //printf("SelIndex: NONE\n");
    return -1;
}

void MediaSelectElement::setSelectedIndex(int index)
{
    selectOption(index, DeselectOtherOptions);
}

void MediaSelectElement::optionSelectionStateChanged(HTMLOptionElement* option, bool optionIsSelected)
{
    //ASSERT(option->ownerSelectElement() == this);
    if (optionIsSelected)
    	selectOption(option->index());
    else if (!usesMenuList())
        selectOption(-1);
    else
        selectOption(nextSelectableListIndex(-1));
}

void MediaSelectElement::selectOption(int optionIndex, SelectOptionFlags flags)
{
    bool shouldDeselect = !m_multiple || (flags & DeselectOtherOptions);

    const Vector<HTMLElement*>& items = listItems();
    int listIndex = optionToListIndex(optionIndex);

    HTMLElement* element = 0;
    if (listIndex >= 0) {
        element = items[listIndex];
        if (element->hasTagName(optionTag)) {
            if (m_activeSelectionAnchorIndex < 0 || shouldDeselect)
                setActiveSelectionAnchorIndex(listIndex);
            if (m_activeSelectionEndIndex < 0 || shouldDeselect)
                setActiveSelectionEndIndex(listIndex);
            toHTMLOptionElement(element)->setSelectedState(true);
        }
    }

    if (shouldDeselect)
        deselectItemsWithoutValidation(element);

    // For the menu list case, this is what makes the selected element appear.
    if (RenderObject* renderer = this->renderer())
    	renderer->updateFromElement();

    scrollToSelection();

    if (usesMenuList()) {
        m_isProcessingUserDrivenChange = flags & UserDriven;
        if (flags & DispatchChangeEvent)
            dispatchChangeEventForMenuList();
        if (RenderObject* renderer = this->renderer()) {
            if (usesMenuList())
                toRenderMediaMenuList(renderer)->didSetSelectedIndex(listIndex);
            else if (renderer->isListBox())
                toRenderListBox(renderer)->selectionChanged();
        }
    }

    setNeedsValidityCheck();
}

int MediaSelectElement::optionToListIndex(int optionIndex) const
{
    const Vector<HTMLElement*>& items = listItems();
    int listSize = static_cast<int>(items.size());
    if (optionIndex < 0 || optionIndex >= listSize)
        return -1;

    int optionIndex2 = -1;
    for (int listIndex = 0; listIndex < listSize; ++listIndex) {
        if (items[listIndex]->hasTagName(optionTag)) {
            ++optionIndex2;
            if (optionIndex2 == optionIndex)
                return listIndex;
        }
    }

    return -1;
}

int MediaSelectElement::listToOptionIndex(int listIndex) const
{
    const Vector<HTMLElement*>& items = listItems();
    if (listIndex < 0 || listIndex >= static_cast<int>(items.size()) || !items[listIndex]->hasTagName(optionTag))
        return -1;

    // Actual index of option not counting OPTGROUP entries that may be in list.
    int optionIndex = 0;
    for (int i = 0; i < listIndex; ++i) {
        if (items[i]->hasTagName(optionTag))
            ++optionIndex;
    }

    return optionIndex;
}

void MediaSelectElement::dispatchFocusEvent(PassRefPtr<Node> oldFocusedNode)
{
    // Save the selection so it can be compared to the new selection when
    // dispatching change events during blur event dispatch.
    if (usesMenuList())
        saveLastSelection();
    HTMLFormControlElementWithState::dispatchFocusEvent(oldFocusedNode);
}

void MediaSelectElement::dispatchBlurEvent(PassRefPtr<Node> newFocusedNode)
{
    // We only need to fire change events here for menu lists, because we fire
    // change events for list boxes whenever the selection change is actually made.
    // This matches other browsers' behavior.
    if (usesMenuList())
        dispatchChangeEventForMenuList();
    HTMLFormControlElementWithState::dispatchBlurEvent(newFocusedNode);
}

void MediaSelectElement::deselectItemsWithoutValidation(HTMLElement* excludeElement)
{
    const Vector<HTMLElement*>& items = listItems();
    for (unsigned i = 0; i < items.size(); ++i) {
        HTMLElement* element = items[i];
        if (element != excludeElement && element->hasTagName(optionTag))
            toHTMLOptionElement(element)->setSelectedState(false);
    }
}

FormControlState MediaSelectElement::saveFormControlState() const
{
    const Vector<HTMLElement*>& items = listItems();
    size_t length = items.size();
    FormControlState state;
    for (unsigned i = 0; i < length; ++i) {
        if (!items[i]->hasTagName(optionTag))
            continue;
        HTMLOptionElement* option = toHTMLOptionElement(items[i]);
        if (!option->selected())
            continue;
        state.append(option->value());
        if (!multiple())
            break;
    }
    return state;
}

size_t MediaSelectElement::searchOptionsForValue(const String& value, size_t listIndexStart, size_t listIndexEnd) const
{
    const Vector<HTMLElement*>& items = listItems();
    size_t loopEndIndex = std::min(items.size(), listIndexEnd);
    for (size_t i = listIndexStart; i < loopEndIndex; ++i) {
        if (!items[i]->hasLocalName(optionTag))
            continue;
        if (static_cast<HTMLOptionElement*>(items[i])->value() == value)
            return i;
    }
    return notFound;
}


#if !PLATFORM(WIN) || OS(WINCE)
bool MediaSelectElement::platformHandleKeydownEvent(KeyboardEvent* event)
{
    const Page* page = document()->page();
    RefPtr<RenderTheme> renderTheme = page ? page->theme() : RenderTheme::defaultTheme();

    if (!renderTheme->popsMenuByArrowKeys())
        return false;

    if (!isSpatialNavigationEnabled(document()->frame())) {
        if (event->keyIdentifier() == "Down" || event->keyIdentifier() == "Up") {
            focus();
            // Calling focus() may cause us to lose our renderer. Return true so
            // that our caller doesn't process the event further, but don't set
            // the event as handled.
            if (!renderer())
                return true;

            // Save the selection so it can be compared to the new selection
            // when dispatching change events during selectOption, which
            // gets called from RenderMenuList::valueChanged, which gets called
            // after the user makes a selection from the menu.
            saveLastSelection();
            if (RenderMediaMenuList* menuList = toRenderMediaMenuList(renderer()))
                menuList->showPopup();
            event->setDefaultHandled();
        }
        return true;
    }

    return false;
}
#endif

void MediaSelectElement::menuListDefaultEventHandler(Event* event)
{
    const Page* page = document()->page();
    RefPtr<RenderTheme> renderTheme = page ? page->theme() : RenderTheme::defaultTheme();

    if (event->type() == eventNames().keydownEvent) {
        if (!renderer() || !event->isKeyboardEvent())
            return;

        if (platformHandleKeydownEvent(static_cast<KeyboardEvent*>(event)))
            return;

        // When using spatial navigation, we want to be able to navigate away
        // from the select element when the user hits any of the arrow keys,
        // instead of changing the selection.
        if (isSpatialNavigationEnabled(document()->frame())) {
            if (!m_activeSelectionState)
                return;
        }

        const String& keyIdentifier = static_cast<KeyboardEvent*>(event)->keyIdentifier();
        bool handled = true;
        const Vector<HTMLElement*>& listItems = this->listItems();
        int listIndex = optionToListIndex(selectedIndex());

        if (keyIdentifier == "Down" || keyIdentifier == "Right")
            listIndex = nextValidIndex(listIndex, SkipForwards, 1);
        else if (keyIdentifier == "Up" || keyIdentifier == "Left")
            listIndex = nextValidIndex(listIndex, SkipBackwards, 1);
        else if (keyIdentifier == "PageDown")
            listIndex = nextValidIndex(listIndex, SkipForwards, 3);
        else if (keyIdentifier == "PageUp")
            listIndex = nextValidIndex(listIndex, SkipBackwards, 3);
        else if (keyIdentifier == "Home")
            listIndex = nextValidIndex(-1, SkipForwards, 1);
        else if (keyIdentifier == "End")
            listIndex = nextValidIndex(listItems.size(), SkipBackwards, 1);
        else
            handled = false;

        if (handled && static_cast<size_t>(listIndex) < listItems.size())
        	selectOption(listToOptionIndex(listIndex), DeselectOtherOptions | DispatchChangeEvent | UserDriven);

        if (handled)
            event->setDefaultHandled();
    }

    // Use key press event here since sending simulated mouse events
    // on key down blocks the proper sending of the key press event.
    if (event->type() == eventNames().keypressEvent) {
        if (!renderer() || !event->isKeyboardEvent())
            return;

        int keyCode = static_cast<KeyboardEvent*>(event)->keyCode();
        bool handled = false;

        if (keyCode == ' ' && isSpatialNavigationEnabled(document()->frame())) {
            // Use space to toggle arrow key handling for selection change or spatial navigation.
            m_activeSelectionState = !m_activeSelectionState;
            event->setDefaultHandled();
            return;
        }

        if (renderTheme->popsMenuBySpaceOrReturn()) {
            if (keyCode == ' ' || keyCode == '\r') {
                focus();

                // Calling focus() may cause us to lose our renderer, in which case
                // do not want to handle the event.
                if (!renderer())
                    return;

                // Save the selection so it can be compared to the new selection
                // when dispatching change events during selectOption, which
                // gets called from RenderMenuList::valueChanged, which gets called
                // after the user makes a selection from the menu.
                saveLastSelection();
                if (RenderMediaMenuList* menuList = toRenderMediaMenuList(renderer()))
                    menuList->showPopup();
                handled = true;
            }
        } else if (renderTheme->popsMenuByArrowKeys()) {
            if (keyCode == ' ') {
                focus();

                // Calling focus() may cause us to lose our renderer, in which case
                // do not want to handle the event.
                if (!renderer())
                    return;

                // Save the selection so it can be compared to the new selection
                // when dispatching change events during selectOption, which
                // gets called from RenderMenuList::valueChanged, which gets called
                // after the user makes a selection from the menu.
                saveLastSelection();
                if (RenderMediaMenuList* menuList = toRenderMediaMenuList(renderer()))
                    menuList->showPopup();
                handled = true;
            } else if (keyCode == '\r') {
                if (form())
                    form()->submitImplicitly(event, false);
                dispatchChangeEventForMenuList();
                handled = true;
            }
        }

        if (handled)
            event->setDefaultHandled();
    }

    if (event->type() == eventNames().mousedownEvent && event->isMouseEvent() && static_cast<MouseEvent*>(event)->button() == LeftButton) {
        focus();
        if (renderer() && renderer()->isMenuList()) {
            if (RenderMediaMenuList* menuList = toRenderMediaMenuList(renderer())) {
                if (menuList->popupIsVisible())
                    menuList->hidePopup();
                else {
                    // Save the selection so it can be compared to the new
                    // selection when we call onChange during selectOption,
                    // which gets called from RenderMenuList::valueChanged,
                    // which gets called after the user makes a selection from
                    // the menu.
                    saveLastSelection();
                    menuList->showPopup();
                }
            }
        }
        event->setDefaultHandled();
    }
}

void MediaSelectElement::updateSelectedState(int listIndex, bool multi, bool shift)
{
    ASSERT(listIndex >= 0);

    // Save the selection so it can be compared to the new selection when
    // dispatching change events during mouseup, or after autoscroll finishes.
    saveLastSelection();

    m_activeSelectionState = true;

    bool shiftSelect = m_multiple && shift;
    bool multiSelect = m_multiple && multi && !shift;

    HTMLElement* clickedElement = listItems()[listIndex];
    if (clickedElement->hasTagName(optionTag)) {
        // Keep track of whether an active selection (like during drag
        // selection), should select or deselect.
        if (toHTMLOptionElement(clickedElement)->selected() && multiSelect)
            m_activeSelectionState = false;
        if (!m_activeSelectionState)
            toHTMLOptionElement(clickedElement)->setSelectedState(false);
    }

    // If we're not in any special multiple selection mode, then deselect all
    // other items, excluding the clicked option. If no option was clicked, then
    // this will deselect all items in the list.
    if (!shiftSelect && !multiSelect)
        deselectItemsWithoutValidation(clickedElement);

    // If the anchor hasn't been set, and we're doing a single selection or a
    // shift selection, then initialize the anchor to the first selected index.
    if (m_activeSelectionAnchorIndex < 0 && !multiSelect)
        setActiveSelectionAnchorIndex(selectedIndex());

    // Set the selection state of the clicked option.
    if (clickedElement->hasTagName(optionTag) && !toHTMLOptionElement(clickedElement)->disabled())
        toHTMLOptionElement(clickedElement)->setSelectedState(true);

    // If there was no selectedIndex() for the previous initialization, or If
    // we're doing a single selection, or a multiple selection (using cmd or
    // ctrl), then initialize the anchor index to the listIndex that just got
    // clicked.
    if (m_activeSelectionAnchorIndex < 0 || !shiftSelect)
        setActiveSelectionAnchorIndex(listIndex);

    setActiveSelectionEndIndex(listIndex);
    updateListBoxSelection(!multiSelect);
}

void MediaSelectElement::listBoxDefaultEventHandler(Event* event)
{
	//printf("listBoxDefaultEventHandler()\n");
    const Vector<HTMLElement*>& listItems = this->listItems();

    if (event->type() == eventNames().mousedownEvent && event->isMouseEvent() && static_cast<MouseEvent*>(event)->button() == LeftButton) {
        focus();
        // Calling focus() may cause us to lose our renderer, in which case do not want to handle the event.
        if (!renderer())
            return;

        // Convert to coords relative to the list box if needed.
        MouseEvent* mouseEvent = static_cast<MouseEvent*>(event);
        IntPoint localOffset = roundedIntPoint(renderer()->absoluteToLocal(mouseEvent->absoluteLocation(), 0));
        int listIndex = toRenderListBox(renderer())->listIndexAtOffset(toSize(localOffset));
        if (listIndex >= 0) {
            if (!disabled()) {
#if PLATFORM(MAC) || (PLATFORM(CHROMIUM) && OS(DARWIN))
                updateSelectedState(listIndex, mouseEvent->metaKey(), mouseEvent->shiftKey());
#else
                updateSelectedState(listIndex, mouseEvent->ctrlKey(), mouseEvent->shiftKey());
#endif
            }
            if (Frame* frame = document()->frame())
                frame->eventHandler()->setMouseDownMayStartAutoscroll();

            event->setDefaultHandled();
        }
    } else if (event->type() == eventNames().mousemoveEvent && event->isMouseEvent() && !toRenderBox(renderer())->canBeScrolledAndHasScrollableArea()) {
        MouseEvent* mouseEvent = static_cast<MouseEvent*>(event);
        if (mouseEvent->button() != LeftButton || !mouseEvent->buttonDown())
            return;

        IntPoint localOffset = roundedIntPoint(renderer()->absoluteToLocal(mouseEvent->absoluteLocation(), 0));
        int listIndex = toRenderListBox(renderer())->listIndexAtOffset(toSize(localOffset));
        if (listIndex >= 0) {
            if (!disabled()) {
                if (m_multiple) {
                    setActiveSelectionEndIndex(listIndex);
                    updateListBoxSelection(false);
                } else {
                    setActiveSelectionAnchorIndex(listIndex);
                    setActiveSelectionEndIndex(listIndex);
                    updateListBoxSelection(true);
                }
            }
            event->setDefaultHandled();
        }
    } else if (event->type() == eventNames().mouseupEvent && event->isMouseEvent() && static_cast<MouseEvent*>(event)->button() == LeftButton && document()->frame()->eventHandler()->autoscrollRenderer() != renderer()) {
        // This makes sure we fire dispatchFormControlChangeEvent for a single
        // click. For drag selection, onChange will fire when the autoscroll
        // timer stops.
        listBoxOnChange();
    } else if (event->type() == eventNames().keydownEvent) {
        if (!event->isKeyboardEvent())
            return;
        const String& keyIdentifier = static_cast<KeyboardEvent*>(event)->keyIdentifier();

        bool handled = false;
        int endIndex = 0;
        if (m_activeSelectionEndIndex < 0) {
            // Initialize the end index
            if (keyIdentifier == "Down" || keyIdentifier == "PageDown") {
                int startIndex = lastSelectedListIndex();
                handled = true;
                if (keyIdentifier == "Down")
                    endIndex = nextSelectableListIndex(startIndex);
                else
                    endIndex = nextSelectableListIndexPageAway(startIndex, SkipForwards);
            } else if (keyIdentifier == "Up" || keyIdentifier == "PageUp") {
                int startIndex = optionToListIndex(selectedIndex());
                handled = true;
                if (keyIdentifier == "Up")
                    endIndex = previousSelectableListIndex(startIndex);
                else
                    endIndex = nextSelectableListIndexPageAway(startIndex, SkipBackwards);
            }
        } else {
            // Set the end index based on the current end index.
            if (keyIdentifier == "Down") {
                endIndex = nextSelectableListIndex(m_activeSelectionEndIndex);
                handled = true;
            } else if (keyIdentifier == "Up") {
                endIndex = previousSelectableListIndex(m_activeSelectionEndIndex);
                handled = true;
            } else if (keyIdentifier == "PageDown") {
                endIndex = nextSelectableListIndexPageAway(m_activeSelectionEndIndex, SkipForwards);
                handled = true;
            } else if (keyIdentifier == "PageUp") {
                endIndex = nextSelectableListIndexPageAway(m_activeSelectionEndIndex, SkipBackwards);
                handled = true;
            }
        }
        if (keyIdentifier == "Home") {
            endIndex = firstSelectableListIndex();
            handled = true;
        } else if (keyIdentifier == "End") {
            endIndex = lastSelectableListIndex();
            handled = true;
        }

        if (isSpatialNavigationEnabled(document()->frame()))
            // Check if the selection moves to the boundary.
            if (keyIdentifier == "Left" || keyIdentifier == "Right" || ((keyIdentifier == "Down" || keyIdentifier == "Up") && endIndex == m_activeSelectionEndIndex))
                return;

        if (endIndex >= 0 && handled) {
            // Save the selection so it can be compared to the new selection
            // when dispatching change events immediately after making the new
            // selection.
            saveLastSelection();

            ASSERT_UNUSED(listItems, !listItems.size() || static_cast<size_t>(endIndex) < listItems.size());
            setActiveSelectionEndIndex(endIndex);

            bool selectNewItem = !m_multiple || static_cast<KeyboardEvent*>(event)->shiftKey() || !isSpatialNavigationEnabled(document()->frame());
            if (selectNewItem)
                m_activeSelectionState = true;
            // If the anchor is unitialized, or if we're going to deselect all
            // other options, then set the anchor index equal to the end index.
            bool deselectOthers = !m_multiple || (!static_cast<KeyboardEvent*>(event)->shiftKey() && selectNewItem);
            if (m_activeSelectionAnchorIndex < 0 || deselectOthers) {
                if (deselectOthers)
                    deselectItemsWithoutValidation();
                setActiveSelectionAnchorIndex(m_activeSelectionEndIndex);
            }

            toRenderListBox(renderer())->scrollToRevealElementAtListIndex(endIndex);
            if (selectNewItem) {
                updateListBoxSelection(deselectOthers);
                listBoxOnChange();
            } else
                scrollToSelection();

            event->setDefaultHandled();
        }
    } else if (event->type() == eventNames().keypressEvent) {
        if (!event->isKeyboardEvent())
            return;
        int keyCode = static_cast<KeyboardEvent*>(event)->keyCode();

        if (keyCode == '\r') {
            if (form())
                form()->submitImplicitly(event, false);
            event->setDefaultHandled();
        } else if (m_multiple && keyCode == ' ' && isSpatialNavigationEnabled(document()->frame())) {
            // Use space to toggle selection change.
            m_activeSelectionState = !m_activeSelectionState;
            updateSelectedState(listToOptionIndex(m_activeSelectionEndIndex), true /*multi*/, false /*shift*/);
            listBoxOnChange();
            event->setDefaultHandled();
        }
    }
}

void MediaSelectElement::defaultEventHandler(Event* event)
{
    if (!renderer())
        return;

    if (usesMenuList())
        menuListDefaultEventHandler(event);
    else
        listBoxDefaultEventHandler(event);
    if (event->defaultHandled())
        return;

    if (event->type() == eventNames().keypressEvent && event->isKeyboardEvent()) {
        KeyboardEvent* keyboardEvent = static_cast<KeyboardEvent*>(event);
        if (!keyboardEvent->ctrlKey() && !keyboardEvent->altKey() && !keyboardEvent->metaKey() ) {
            typeAheadFind(keyboardEvent);
            event->setDefaultHandled();
            return;
        }
    }
    HTMLFormControlElementWithState::defaultEventHandler(event);
}

int MediaSelectElement::lastSelectedListIndex() const
{
    const Vector<HTMLElement*>& items = listItems();
    for (size_t i = items.size(); i;) {
        HTMLElement* element = items[--i];
        if (element->hasTagName(optionTag) && toHTMLOptionElement(element)->selected())
            return i;
    }
    return -1;
}

static String stripLeadingWhiteSpace(const String& string)
{
    int length = string.length();

    int i;
    for (i = 0; i < length; ++i) {
        if ( string[i] != noBreakSpace && !isASCIISpace(string[i]) )
            break;
    }

    return string.substring(i, length - i);
}

void MediaSelectElement::typeAheadFind(KeyboardEvent* event)
{
    if (event->timeStamp() < m_lastCharTime)
        return;

    DOMTimeStamp delta = event->timeStamp() - m_lastCharTime;
    m_lastCharTime = event->timeStamp();

    UChar c = event->charCode();

    String prefix;
    int searchStartOffset = 1;
    if (delta > typeAheadTimeout) {
        prefix = String(&c, 1);
        m_typedString = prefix;
        m_repeatingChar = c;
    } else {
        m_typedString.append(c);

        if (c == m_repeatingChar) {
            // The user is likely trying to cycle through all the items starting
            // with this character, so just search on the character.
            prefix = String(&c, 1);
        } else {
            m_repeatingChar = 0;
            prefix = m_typedString;
            searchStartOffset = 0;
        }
    }

    const Vector<HTMLElement*>& items = listItems();
    int itemCount = items.size();
    if (itemCount < 1)
        return;

    int selected = selectedIndex();
    int index = (optionToListIndex(selected >= 0 ? selected : 0) + searchStartOffset) % itemCount;
    ASSERT(index >= 0);

    // Compute a case-folded copy of the prefix string before beginning the search for
    // a matching element. This code uses foldCase to work around the fact that
    // String::startWith does not fold non-ASCII characters. This code can be changed
    // to use startWith once that is fixed.
    String prefixWithCaseFolded(prefix.foldCase());
    for (int i = 0; i < itemCount; ++i, index = (index + 1) % itemCount) {
        HTMLElement* element = items[index];
        if (!element->hasTagName(optionTag) || toHTMLOptionElement(element)->disabled())
            continue;

        // Fold the option string and check if its prefix is equal to the folded prefix.
        String text = toHTMLOptionElement(element)->textIndentedToRespectGroupLabel();
        if (stripLeadingWhiteSpace(text).foldCase().startsWith(prefixWithCaseFolded)) {
        	selectOption(listToOptionIndex(index), DeselectOtherOptions | DispatchChangeEvent | UserDriven);
            if (!usesMenuList())
                listBoxOnChange();

            setOptionsChangedOnRenderer();
            setNeedsStyleRecalc();
            return;
        }
    }
}

Node::InsertionNotificationRequest MediaSelectElement::insertedInto(ContainerNode* insertionPoint)
{
    // When the element is created during document parsing, it won't have any
    // items yet - but for innerHTML and related methods, this method is called
    // after the whole subtree is constructed.
    recalcListItems();
    HTMLFormControlElementWithState::insertedInto(insertionPoint);
    return InsertionDone;
}

void MediaSelectElement::accessKeySetSelectedIndex(int index)
{
    // First bring into focus the list box.
    if (!focused())
        accessKeyAction(false);

    // If this index is already selected, unselect. otherwise update the selected index.
    const Vector<HTMLElement*>& items = listItems();
    int listIndex = optionToListIndex(index);
    if (listIndex >= 0) {
        HTMLElement* element = items[listIndex];
        if (element->hasTagName(optionTag)) {
            if (toHTMLOptionElement(element)->selected())
                toHTMLOptionElement(element)->setSelectedState(false);
            else
            	selectOption(index, DispatchChangeEvent | UserDriven);
        }
    }

    if (usesMenuList())
        dispatchChangeEventForMenuList();
    else
        listBoxOnChange();

    scrollToSelection();
}

}

#endif /* #if ENABLE(VIDEO) */
