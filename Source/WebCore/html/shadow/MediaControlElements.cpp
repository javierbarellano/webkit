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

#include "config.h"

#if ENABLE(VIDEO)

#include <string>
#include <sstream>

#include "MediaControlElements.h"

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

namespace WebCore {

using namespace HTMLNames;
using namespace std;

// FIXME: These constants may need to be tweaked to better match the seeking in the QuickTime plug-in.
static const float cSkipRepeatDelay = 0.1f;
static const float cSkipTime = 0.2f;
static const float cScanRepeatDelay = 1.5f;
static const float cScanMaximumRate = 8;

static const unsigned maxSelectItems=30;
static const DOMTimeStamp typeAheadTimeout = 1000;

HTMLMediaElement* toParentMediaElement(Node* node)
{
    if (!node)
        return 0;
    Node* mediaNode = node->shadowHost();
    if (!mediaNode)
        mediaNode = node;
    if (!mediaNode || !mediaNode->isElementNode() || !static_cast<Element*>(mediaNode)->isMediaElement())
        return 0;

    return static_cast<HTMLMediaElement*>(mediaNode);
}

MediaControlElementType mediaControlElementType(Node* node)
{
    ASSERT(node->isMediaControlElement());
    HTMLElement* element = toHTMLElement(node);
   if (element->hasTagName(inputTag))
        return static_cast<MediaControlInputElement*>(element)->displayType();
    return static_cast<MediaControlElement*>(element)->displayType();
}

// ----------------------------

MediaControlElement::MediaControlElement(Document* document)
    : HTMLDivElement(divTag, document)
    , m_mediaController(0)
{
}

void MediaControlElement::show()
{
    removeInlineStyleProperty(CSSPropertyDisplay);
}

void MediaControlElement::hide()
{
    setInlineStyleProperty(CSSPropertyDisplay, CSSValueNone);
}

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

    if (MediaSelectElement* select = ownerMediaElement())
        select->optionSelectionStateChanged(this, selected);
}

void MediaOptionElement::setSelectedState(bool selected)
{
    if (m_isSelected == selected)
        return;

    m_isSelected = selected;
    setNeedsStyleRecalc();

    if (MediaSelectElement* select = ownerMediaElement())
        select->invalidateSelectedItems();
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

MediaSelectElement::MediaSelectElement(Document* document, MediaControlElementType displayType)
    : HTMLFormControlElementWithState(selectTag, document, NULL)
	, m_hasFocus(false)
    , m_mediaController(0)
{
	m_displayType = displayType;
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

//void MediaSelectElement::deselectItems(HTMLOptionElement* excludeElement)
//{
//	printf("deselectItems()\n");
//    deselectItemsWithoutValidation(excludeElement);
//    setNeedsValidityCheck();
//}

void MediaSelectElement::optionSelectedByUser(int optionIndex, bool fireOnChangeNow, bool allowMultipleSelection)
{
	//printf("optionSelectedByUser(%d,%s,%s)\n", optionIndex, fireOnChangeNow ? "true":"false", allowMultipleSelection ? "true":"false");
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
	//printf("hasPlaceholderLabelOption()\n");
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

//String MediaSelectElement::validationMessage() const
//{
//	printf("validationMessage()\n");
//    if (!willValidate())
//        return String();
//
//    if (customError())
//        return customValidationMessage();
//
//    return valueMissing() ? validationMessageValueMissingForSelectText() : String();
//}

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
   const Page* page = document()->page();
    RefPtr<RenderTheme> renderTheme = page ? page->theme() : RenderTheme::defaultTheme();
    if (renderTheme->delegatesMenuListRendering())
        return true;

    return !m_multiple && m_size <= 1;
}

//int MediaSelectElement::activeSelectionStartListIndex() const
//{
//	printf("activeSelectionStartListIndex()\n");
//   if (m_activeSelectionAnchorIndex >= 0)
//        return m_activeSelectionAnchorIndex;
//    return optionToListIndex(selectedIndex());
//}

//int MediaSelectElement::activeSelectionEndListIndex() const
//{
//	printf("activeSelectionEndListIndex()\n");
//    if (m_activeSelectionEndIndex >= 0)
//        return m_activeSelectionEndIndex;
//    return lastSelectedListIndex();
//}

void MediaSelectElement::add(HTMLElement* element, HTMLElement* before, ExceptionCode& ec)
{
	//printf("add()\n");
    // Make sure the element is ref'd and deref'd so we don't leak it.
    RefPtr<HTMLElement> protectNewChild(element);

    if (!element || !(element->hasLocalName(optionTag) || element->hasLocalName(hrTag)))
        return;

    insertBefore(element, before, ec);
    setNeedsValidityCheck();
}

void MediaSelectElement::remove(int optionIndex)
{
	//printf("remove()\n");
    int listIndex = optionToListIndex(optionIndex);
    if (listIndex < 0)
        return;

    ExceptionCode ec;
    listItems()[listIndex]->remove(ec);
}

//String MediaSelectElement::value() const
//{
//	printf("value()\n");
//    const Vector<HTMLElement*>& items = listItems();
//    for (unsigned i = 0; i < items.size(); i++) {
//        if (items[i]->hasLocalName(optionTag) && static_cast<HTMLOptionElement*>(items[i])->selected())
//            return static_cast<HTMLOptionElement*>(items[i])->value();
//    }
//    return "";
//}

//void MediaSelectElement::setValue(const String &value)
//{
//	printf("setValue()\n");
//    // We clear the previously selected option(s) when needed, to guarantee calling setSelectedIndex() only once.
//    if (value.isNull()) {
//        setSelectedIndex(-1);
//        return;
//    }
//
//    // Find the option with value() matching the given parameter and make it the current selection.
//    const Vector<HTMLElement*>& items = listItems();
//    unsigned optionIndex = 0;
//    for (unsigned i = 0; i < items.size(); i++) {
//        if (items[i]->hasLocalName(optionTag)) {
//            if (static_cast<HTMLOptionElement*>(items[i])->value() == value) {
//                setSelectedIndex(optionIndex);
//                return;
//            }
//            optionIndex++;
//        }
//    }
//
//    setSelectedIndex(-1);
//}

//bool MediaSelectElement::isPresentationAttribute(const QualifiedName& name) const
//{
//	printf("isPresentationAttribute()\n");
//    if (name == alignAttr) {
//        // Don't map 'align' attribute. This matches what Firefox, Opera and IE do.
//        // See http://bugs.webkit.org/show_bug.cgi?id=12072
//        return false;
//    }
//
//    return HTMLFormControlElementWithState::isPresentationAttribute(name);
//}

//void MediaSelectElement::parseAttribute(const Attribute& attribute)
//{
//	printf("parseAttribute()\n");
//    if (attribute.name() == sizeAttr) {
//        int oldSize = m_size;
//        // Set the attribute value to a number.
//        // This is important since the style rules for this attribute can determine the appearance property.
//        int size = attribute.value().toInt();
//        String attrSize = String::number(size);
//        if (attrSize != attribute.value()) {
//            // FIXME: This is horribly factored.
//            if (Attribute* sizeAttribute = getAttributeItem(sizeAttr))
//                sizeAttribute->setValue(attrSize);
//        }
//        size = max(size, 1);
//
//        // Ensure that we've determined selectedness of the items at least once prior to changing the size.
//        if (oldSize != size)
//            updateListItemSelectedStates();
//
//        m_size = size;
//        setNeedsValidityCheck();
//        if (m_size != oldSize && attached()) {
//            reattach();
//            setRecalcListItems();
//        }
//    } else if (attribute.name() == multipleAttr)
//        parseMultipleAttribute(attribute);
//    else if (attribute.name() == accesskeyAttr) {
//        // FIXME: ignore for the moment.
//    } else if (attribute.name() == onchangeAttr) {
//        //setAttributeEventListener(eventNames().changeEvent, createAttributeEventListener(this, attribute));
//    } else
//        HTMLFormControlElementWithState::parseAttribute(attribute);
//}

bool MediaSelectElement::isKeyboardFocusable(KeyboardEvent* event) const
{
    if (renderer())
        return isFocusable();
    return HTMLFormControlElementWithState::isKeyboardFocusable(event);
}

//bool MediaSelectElement::isMouseFocusable() const
//{
//	printf("isMouseFocusable()\n");
//    if (renderer())
//        return isFocusable();
//    return HTMLFormControlElementWithState::isMouseFocusable();
//}

//bool MediaSelectElement::canSelectAll() const
//{
//	printf("canSelectAll()\n");
//    return !usesMenuList();
//}

RenderObject* MediaSelectElement::createRenderer(RenderArena* arena, RenderStyle*)
{
    if (usesMenuList())
        return new (arena) RenderMediaMenuList(this);
    return new (arena) RenderListBox(this);
}

//bool MediaSelectElement::childShouldCreateRenderer(const NodeRenderingContext& childContext) const
//{
//	printf("childShouldCreateRenderer()\n");
//    if (!HTMLFormControlElementWithState::childShouldCreateRenderer(childContext))
//        return false;
//    if (!usesMenuList())
//        return true;
//    return validationMessageShadowTreeContains(childContext.node());
//}

//PassRefPtr<HTMLCollection> MediaSelectElement::selectedOptions()
//{
//	printf("selectedOptions()\n");
//    return ensureCachedHTMLCollection(SelectedOptions);
//}

PassRefPtr<HTMLOptionsCollection> MediaSelectElement::options()
{
	//printf("options()\n");
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
	//printf("accessKeyAction()\n");
    focus();
    dispatchSimulatedClick(0, sendMouseEvents);
}

//void MediaSelectElement::setMultiple(bool multiple)
//{
//	printf("setMultiple()\n");
//    bool oldMultiple = this->multiple();
//    int oldSelectedIndex = selectedIndex();
//    setAttribute(multipleAttr, multiple ? "" : 0);
//
//    // Restore selectedIndex after changing the multiple flag to preserve
//    // selection as single-line and multi-line has different defaults.
//    if (oldMultiple != this->multiple())
//        setSelectedIndex(oldSelectedIndex);
//}

//void MediaSelectElement::setSize(int size)
//{
//	printf("setSize()\n");
//    setAttribute(sizeAttr, String::number(size));
//}

//Node* MediaSelectElement::namedItem(const AtomicString& name)
//{
//	printf("namedItem()\n");
//    return options()->namedItem(name);
//}

//Node* MediaSelectElement::item(unsigned index)
//{
//	printf("item()\n");
//    return options()->item(index);
//}

//void MediaSelectElement::setOption(unsigned index, HTMLOptionElement* option, ExceptionCode& ec)
//{
//	printf("setOption()\n");
//    ec = 0;
//    if (index > maxSelectItems - 1)
//        index = maxSelectItems - 1;
//    int diff = index - length();
//    RefPtr<HTMLElement> before = 0;
//    // Out of array bounds? First insert empty dummies.
//    if (diff > 0) {
//        setLength(index, ec);
//        // Replace an existing entry?
//    } else if (diff < 0) {
//        before = toHTMLElement(options()->item(index+1));
//        remove(index);
//    }
//    // Finally add the new element.
//    if (!ec) {
//        add(option, before.get(), ec);
//        if (diff >= 0 && option->selected())
//            optionSelectionStateChanged(option, true);
//    }
//}

//void MediaSelectElement::setLength(unsigned newLen, ExceptionCode& ec)
//{
//	printf("setLength()\n");
//    ec = 0;
//    if (newLen > maxSelectItems)
//        newLen = maxSelectItems;
//    int diff = length() - newLen;
//
//    if (diff < 0) { // Add dummy elements.
//        do {
//            RefPtr<Element> option = document()->createElement(optionTag, false);
//            ASSERT(option);
//            add(toHTMLElement(option.get()), 0, ec);
//            if (ec)
//                break;
//        } while (++diff);
//    } else {
//        const Vector<HTMLElement*>& items = listItems();
//
//        // Removing children fires mutation events, which might mutate the DOM further, so we first copy out a list
//        // of elements that we intend to remove then attempt to remove them one at a time.
//        Vector<RefPtr<Element> > itemsToRemove;
//        size_t optionIndex = 0;
//        for (size_t i = 0; i < items.size(); ++i) {
//            Element* item = items[i];
//            if (item->hasLocalName(optionTag) && optionIndex++ >= newLen) {
//                ASSERT(item->parentNode());
//                itemsToRemove.append(item);
//            }
//        }
//
//        for (size_t i = 0; i < itemsToRemove.size(); ++i) {
//            Element* item = itemsToRemove[i].get();
//            if (item->parentNode())
//                item->parentNode()->removeChild(item, ec);
//        }
//    }
//    setNeedsValidityCheck();
//}

bool MediaSelectElement::isRequiredFormControl() const
{
    return required();
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
	//printf("nextSelectableListIndex()\n");
    return nextValidIndex(startIndex, SkipForwards, 1);
}

int MediaSelectElement::previousSelectableListIndex(int startIndex) const
{
	//printf("previousSelectableListIndex()\n");
    if (startIndex == -1)
        startIndex = listItems().size();
    return nextValidIndex(startIndex, SkipBackwards, 1);
}

int MediaSelectElement::firstSelectableListIndex() const
{
	//printf("firstSelectableListIndex()\n");
    const Vector<HTMLElement*>& items = listItems();
    int index = nextValidIndex(items.size(), SkipBackwards, INT_MAX);
    if (static_cast<size_t>(index) == items.size())
        return -1;
    return index;
}

int MediaSelectElement::lastSelectableListIndex() const
{
	//printf("lastSelectableListIndex()\n");
    return nextValidIndex(-1, SkipForwards, INT_MAX);
}

// Returns the index of the next valid item one page away from |startIndex| in direction |direction|.
int MediaSelectElement::nextSelectableListIndexPageAway(int startIndex, SkipDirection direction) const
{
	//printf("nextSelectableListIndexPageAway()\n");
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

//void MediaSelectElement::selectAll()
//{
//	printf("selectAll()\n");
//    ASSERT(!usesMenuList());
//    if (!renderer() || !m_multiple)
//        return;
//
//    // Save the selection so it can be compared to the new selectAll selection
//    // when dispatching change events.
//    saveLastSelection();
//
//    m_activeSelectionState = true;
//    setActiveSelectionAnchorIndex(nextSelectableListIndex(-1));
//    setActiveSelectionEndIndex(previousSelectableListIndex(-1));
//
//    updateListBoxSelection(false);
//    listBoxOnChange();
//    setNeedsValidityCheck();
//}

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
	//printf("updateListBoxSelection()\n");
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
	//printf("listBoxOnChange()\n");
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
    if (m_shouldRecalcListItems)
        recalcListItems();
    else {
#if !ASSERT_DISABLED
        Vector<HTMLElement*> items = m_listItems;
        recalcListItems(false);
        ASSERT(items == m_listItems);
#endif
    }

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
    for (size_t i = 0; i < items.size(); ++i) {
        HTMLElement* element = items[i];
        if (element->hasTagName(optionTag)) {
            if (toHTMLOptionElement(element)->selected())
                return index;
            ++index;
        }
    }

    return -1;
}

void MediaSelectElement::setSelectedIndex(int index)
{
    selectOption(index, DeselectOtherOptions);
}

void MediaSelectElement::optionSelectionStateChanged(HTMLOptionElement* option, bool optionIsSelected)
{
	//printf("optionSelectionStateChanged()\n");
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
//    if (Frame* frame = document()->frame())
//        frame->page()->chrome()->client()->formStateDidChange(this);
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
	//printf("searchOptionsForValue()\n");
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

//void MediaSelectElement::restoreFormControlState(const FormControlState& state)
//{
//	printf("restoreFormControlState()\n");
//    recalcListItems();
//
//    const Vector<HTMLElement*>& items = listItems();
//    size_t itemsSize = items.size();
//    if (!itemsSize)
//        return;
//
//    for (size_t i = 0; i < itemsSize; ++i) {
//        if (!items[i]->hasLocalName(optionTag))
//            continue;
//        static_cast<HTMLOptionElement*>(items[i])->setSelectedState(false);
//    }
//
//    if (!multiple()) {
//        size_t foundIndex = searchOptionsForValue(state[0], 0, itemsSize);
//        if (foundIndex != notFound)
//            toHTMLOptionElement(items[foundIndex])->setSelectedState(true);
//    } else {
//        size_t startIndex = 0;
//        for (size_t i = 0; i < state.valueSize(); ++i) {
//            const String& value = state[i];
//            size_t foundIndex = searchOptionsForValue(value, startIndex, itemsSize);
//            if (foundIndex == notFound)
//                foundIndex = searchOptionsForValue(value, 0, startIndex);
//            if (foundIndex == notFound)
//                continue;
//            toHTMLOptionElement(items[foundIndex])->setSelectedState(true);
//            startIndex = foundIndex + 1;
//        }
//    }
//
//    setOptionsChangedOnRenderer();
//    setNeedsValidityCheck();
//}

//void MediaSelectElement::parseMultipleAttribute(const Attribute& attribute)
//{
//	printf("parseMultipleAttribute()\n");
//    bool oldUsesMenuList = usesMenuList();
//    m_multiple = !attribute.isNull();
//    setNeedsValidityCheck();
//    if (oldUsesMenuList != usesMenuList())
//        reattachIfAttached();
//}

//bool MediaSelectElement::appendFormData(FormDataList& list, bool)
//{
//	printf("appendFormData()\n");
//    const AtomicString& name = this->name();
//    if (name.isEmpty())
//        return false;
//
//    bool successful = false;
//    const Vector<HTMLElement*>& items = listItems();
//
//    for (unsigned i = 0; i < items.size(); ++i) {
//        HTMLElement* element = items[i];
//        if (element->hasTagName(optionTag) && toHTMLOptionElement(element)->selected() && !toHTMLOptionElement(element)->disabled()) {
//            list.appendData(name, toHTMLOptionElement(element)->value());
//            successful = true;
//        }
//    }
//
//    // It's possible that this is a menulist with multiple options and nothing
//    // will be submitted (!successful). We won't send a unselected non-disabled
//    // option as fallback. This behavior matches to other browsers.
//    return successful;
//}

//void MediaSelectElement::reset()
//{
//	printf("reset()\n");
//    HTMLOptionElement* firstOption = 0;
//    HTMLOptionElement* selectedOption = 0;
//
//    const Vector<HTMLElement*>& items = listItems();
//    for (unsigned i = 0; i < items.size(); ++i) {
//        HTMLElement* element = items[i];
//        if (!element->hasTagName(optionTag))
//            continue;
//
//        if (items[i]->fastHasAttribute(selectedAttr)) {
//            if (selectedOption && !m_multiple)
//                selectedOption->setSelectedState(false);
//            toHTMLOptionElement(element)->setSelectedState(true);
//            selectedOption = toHTMLOptionElement(element);
//        } else
//            toHTMLOptionElement(element)->setSelectedState(false);
//
//        if (!firstOption)
//            firstOption = toHTMLOptionElement(element);
//    }
//
//    if (!selectedOption && firstOption && !m_multiple && m_size <= 1)
//        firstOption->setSelectedState(true);
//
//    setOptionsChangedOnRenderer();
//    setNeedsStyleRecalc();
//    setNeedsValidityCheck();
//}

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
	//printf("updateSelectedState()\n");
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
        IntPoint localOffset = roundedIntPoint(renderer()->absoluteToLocal(mouseEvent->absoluteLocation(), false, true));
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

        IntPoint localOffset = roundedIntPoint(renderer()->absoluteToLocal(mouseEvent->absoluteLocation(), false, true));
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
	//printf("lastSelectedListIndex()\n");
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
	//printf("accessKeySetSelectedIndex()\n");
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

//unsigned MediaSelectElement::length() const
//{
//	printf("length()\n");
//    unsigned options = 0;
//
//    const Vector<HTMLElement*>& items = listItems();
//    for (unsigned i = 0; i < items.size(); ++i) {
//        if (items[i]->hasTagName(optionTag))
//            ++options;
//    }
//
//    return options;
//}
// ----------------------------

inline MediaControlPanelElement::MediaControlPanelElement(Document* document)
    : MediaControlElement(document)
    , m_canBeDragged(false)
    , m_isBeingDragged(false)
    , m_isDisplayed(false)
    , m_opaque(true)
    , m_transitionTimer(this, &MediaControlPanelElement::transitionTimerFired)
{
}

PassRefPtr<MediaControlPanelElement> MediaControlPanelElement::create(Document* document)
{
    return adoptRef(new MediaControlPanelElement(document));
}

MediaControlElementType MediaControlPanelElement::displayType() const
{
    return MediaControlsPanel;
}

const AtomicString& MediaControlPanelElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-panel"));
    return id;
}

void MediaControlPanelElement::startDrag(const LayoutPoint& eventLocation)
{
    if (!m_canBeDragged)
        return;

    if (m_isBeingDragged)
        return;

    RenderObject* renderer = this->renderer();
    if (!renderer || !renderer->isBox())
        return;

    Frame* frame = document()->frame();
    if (!frame)
        return;

    m_dragStartEventLocation = eventLocation;

    frame->eventHandler()->setCapturingMouseEventsNode(this);

    m_isBeingDragged = true;
}

void MediaControlPanelElement::continueDrag(const LayoutPoint& eventLocation)
{
    if (!m_isBeingDragged)
        return;

    LayoutSize distanceDragged = eventLocation - m_dragStartEventLocation;
    setPosition(LayoutPoint(distanceDragged.width(), distanceDragged.height()));
}

void MediaControlPanelElement::endDrag()
{
    if (!m_isBeingDragged)
        return;

    m_isBeingDragged = false;

    Frame* frame = document()->frame();
    if (!frame)
        return;

    frame->eventHandler()->setCapturingMouseEventsNode(0);
}

void MediaControlPanelElement::startTimer()
{
    stopTimer();

    // The timer is required to set the property display:'none' on the panel,
    // such that captions are correctly displayed at the bottom of the video
    // at the end of the fadeout transition.
    double duration = document()->page() ? document()->page()->theme()->mediaControlsFadeOutDuration() : 0;
    m_transitionTimer.startOneShot(duration);
}

void MediaControlPanelElement::stopTimer()
{
    if (m_transitionTimer.isActive())
        m_transitionTimer.stop();
}


void MediaControlPanelElement::transitionTimerFired(Timer<MediaControlPanelElement>*)
{
    if (!m_opaque)
        hide();

    stopTimer();
}

void MediaControlPanelElement::setPosition(const LayoutPoint& position)
{
    double left = position.x();
    double top = position.y();

    // Set the left and top to control the panel's position; this depends on it being absolute positioned.
    // Set the margin to zero since the position passed in will already include the effect of the margin.
    setInlineStyleProperty(CSSPropertyLeft, left, CSSPrimitiveValue::CSS_PX);
    setInlineStyleProperty(CSSPropertyTop, top, CSSPrimitiveValue::CSS_PX);
    setInlineStyleProperty(CSSPropertyMarginLeft, 0.0, CSSPrimitiveValue::CSS_PX);
    setInlineStyleProperty(CSSPropertyMarginTop, 0.0, CSSPrimitiveValue::CSS_PX);

    ExceptionCode ignored;
    classList()->add("dragged", ignored);
}

void MediaControlPanelElement::resetPosition()
{
    removeInlineStyleProperty(CSSPropertyLeft);
    removeInlineStyleProperty(CSSPropertyTop);
    removeInlineStyleProperty(CSSPropertyMarginLeft);
    removeInlineStyleProperty(CSSPropertyMarginTop);

    ExceptionCode ignored;
    classList()->remove("dragged", ignored);
}

void MediaControlPanelElement::makeOpaque()
{
	//printf("makeOpaque()\n");
    if (m_opaque)
        return;

    double duration = document()->page() ? document()->page()->theme()->mediaControlsFadeInDuration() : 0;

    setInlineStyleProperty(CSSPropertyWebkitTransitionProperty, CSSPropertyOpacity);
    setInlineStyleProperty(CSSPropertyWebkitTransitionDuration, duration, CSSPrimitiveValue::CSS_S);
    setInlineStyleProperty(CSSPropertyOpacity, 1.0, CSSPrimitiveValue::CSS_NUMBER);

    m_opaque = true;

    if (m_isDisplayed)
        show();
}

void MediaControlPanelElement::makeTransparent()
{
	//printf("makeTransparent()\n");

    if (!m_opaque)
        return;

    setInlineStyleProperty(CSSPropertyWebkitTransitionProperty, CSSPropertyOpacity);
    setInlineStyleProperty(CSSPropertyWebkitTransitionDuration, document()->page()->theme()->mediaControlsFadeOutDuration(), CSSPrimitiveValue::CSS_S);
    setInlineStyleProperty(CSSPropertyOpacity, 0.0, CSSPrimitiveValue::CSS_NUMBER);

    m_opaque = false;
    startTimer();
}

void MediaControlPanelElement::defaultEventHandler(Event* event)
{
    MediaControlElement::defaultEventHandler(event);

    if (event->isMouseEvent()) {
        LayoutPoint location = static_cast<MouseEvent*>(event)->absoluteLocation();
        if (event->type() == eventNames().mousedownEvent && event->target() == this) {
            startDrag(location);
            event->setDefaultHandled();
        } else if (event->type() == eventNames().mousemoveEvent && m_isBeingDragged)
            continueDrag(location);
        else if (event->type() == eventNames().mouseupEvent && m_isBeingDragged) {
            continueDrag(location);
            endDrag();
            event->setDefaultHandled();
        }
    }
}

void MediaControlPanelElement::setCanBeDragged(bool canBeDragged)
{
    if (m_canBeDragged == canBeDragged)
        return;

    m_canBeDragged = canBeDragged;

    if (!canBeDragged)
        endDrag();
}

void MediaControlPanelElement::setIsDisplayed(bool isDisplayed)
{
    m_isDisplayed = isDisplayed;
}

// ----------------------------

inline MediaControlTimelineContainerElement::MediaControlTimelineContainerElement(Document* document)
    : MediaControlElement(document)
{
}

PassRefPtr<MediaControlTimelineContainerElement> MediaControlTimelineContainerElement::create(Document* document)
{
    RefPtr<MediaControlTimelineContainerElement> element = adoptRef(new MediaControlTimelineContainerElement(document));
    element->hide();
    return element.release();
}

MediaControlElementType MediaControlTimelineContainerElement::displayType() const
{
    return MediaTimelineContainer;
}

const AtomicString& MediaControlTimelineContainerElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-timeline-container"));
    return id;
}

// ----------------------------

class RenderMediaVolumeSliderContainer : public RenderBlock {
public:
    RenderMediaVolumeSliderContainer(Node*);

private:
    virtual void layout();
};

RenderMediaVolumeSliderContainer::RenderMediaVolumeSliderContainer(Node* node)
    : RenderBlock(node)
{
}

void RenderMediaVolumeSliderContainer::layout()
{
    RenderBlock::layout();

    if (style()->display() == NONE || !nextSibling() || !nextSibling()->isBox())
        return;

    RenderBox* buttonBox = toRenderBox(nextSibling());
    int absoluteOffsetTop = buttonBox->localToAbsolute(FloatPoint(0, -size().height())).y();

    LayoutStateDisabler layoutStateDisabler(view());

    // If the slider would be rendered outside the page, it should be moved below the controls.
    if (UNLIKELY(absoluteOffsetTop < 0))
        setY(buttonBox->offsetTop() + theme()->volumeSliderOffsetFromMuteButton(buttonBox, pixelSnappedSize()).y());
}

inline MediaControlVolumeSliderContainerElement::MediaControlVolumeSliderContainerElement(Document* document)
    : MediaControlElement(document)
{
}

PassRefPtr<MediaControlVolumeSliderContainerElement> MediaControlVolumeSliderContainerElement::create(Document* document)
{
    RefPtr<MediaControlVolumeSliderContainerElement> element = adoptRef(new MediaControlVolumeSliderContainerElement(document));
    element->hide();
    return element.release();
}

RenderObject* MediaControlVolumeSliderContainerElement::createRenderer(RenderArena* arena, RenderStyle*)
{
    return new (arena) RenderMediaVolumeSliderContainer(this);
}

void MediaControlVolumeSliderContainerElement::defaultEventHandler(Event* event)
{
    if (!event->isMouseEvent() || event->type() != eventNames().mouseoutEvent)
        return;

    // Poor man's mouseleave event detection.
    MouseEvent* mouseEvent = static_cast<MouseEvent*>(event);
    if (!mouseEvent->relatedTarget() || !mouseEvent->relatedTarget()->toNode())
        return;

    if (this->containsIncludingShadowDOM(mouseEvent->relatedTarget()->toNode()))
        return;

    hide();
}

MediaControlElementType MediaControlVolumeSliderContainerElement::displayType() const
{
    return MediaVolumeSliderContainer;
}

const AtomicString& MediaControlVolumeSliderContainerElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-volume-slider-container"));
    return id;
}

// ----------------------------

inline MediaControlStatusDisplayElement::MediaControlStatusDisplayElement(Document* document)
    : MediaControlElement(document)
    , m_stateBeingDisplayed(Nothing)
{
}

PassRefPtr<MediaControlStatusDisplayElement> MediaControlStatusDisplayElement::create(Document* document)
{
    RefPtr<MediaControlStatusDisplayElement> element = adoptRef(new MediaControlStatusDisplayElement(document));
    element->hide();
    return element.release();
}

void MediaControlStatusDisplayElement::update()
{
    // Get the new state that we'll have to display.
    StateBeingDisplayed newStateToDisplay = Nothing;

    if (mediaController()->readyState() <= MediaControllerInterface::HAVE_METADATA && mediaController()->hasCurrentSrc())
        newStateToDisplay = Loading;
    else if (mediaController()->isLiveStream())
        newStateToDisplay = LiveBroadcast;

    if (newStateToDisplay == m_stateBeingDisplayed)
        return;

    ExceptionCode e;

    if (m_stateBeingDisplayed == Nothing)
        show();
    else if (newStateToDisplay == Nothing)
        hide();

    m_stateBeingDisplayed = newStateToDisplay;

    switch (m_stateBeingDisplayed) {
    case Nothing:
        setInnerText("", e);
        break;
    case Loading:
        setInnerText(mediaElementLoadingStateText(), e);
        break;
    case LiveBroadcast:
        setInnerText(mediaElementLiveBroadcastStateText(), e);
        break;
    }
}

MediaControlElementType MediaControlStatusDisplayElement::displayType() const
{
    return MediaStatusDisplay;
}

const AtomicString& MediaControlStatusDisplayElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-status-display"));
    return id;
}

// ----------------------------
    
MediaControlInputElement::MediaControlInputElement(Document* document, MediaControlElementType displayType)
    : HTMLInputElement(inputTag, document, 0, false)
    , m_mediaController(0)
    , m_displayType(displayType)
{
}

void MediaControlInputElement::show()
{
    removeInlineStyleProperty(CSSPropertyDisplay);
}

void MediaControlInputElement::hide()
{
    setInlineStyleProperty(CSSPropertyDisplay, CSSValueNone);
}


void MediaControlInputElement::setDisplayType(MediaControlElementType displayType)
{

    if (displayType == m_displayType)
        return;

    m_displayType = displayType;
    if (RenderObject* object = renderer()) {

        object->repaint();
    }
}

// ----------------------------

inline MediaControlMuteButtonElement::MediaControlMuteButtonElement(Document* document, MediaControlElementType displayType)
    : MediaControlInputElement(document, displayType)
{
}

void MediaControlMuteButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == eventNames().clickEvent) {
        mediaController()->setMuted(!mediaController()->muted());
        event->setDefaultHandled();
    }

    HTMLInputElement::defaultEventHandler(event);
}

void MediaControlMuteButtonElement::changedMute()
{
    updateDisplayType();
}

void MediaControlMuteButtonElement::updateDisplayType()
{
    setDisplayType(mediaController()->muted() ? MediaUnMuteButton : MediaMuteButton);
}

// ----------------------------

inline MediaControlPanelMuteButtonElement::MediaControlPanelMuteButtonElement(Document* document, MediaControls* controls)
    : MediaControlMuteButtonElement(document, MediaMuteButton)
    , m_controls(controls)
{
}

PassRefPtr<MediaControlPanelMuteButtonElement> MediaControlPanelMuteButtonElement::create(Document* document, MediaControls* controls)
{
    ASSERT(controls);

    RefPtr<MediaControlPanelMuteButtonElement> button = adoptRef(new MediaControlPanelMuteButtonElement(document, controls));
    button->createShadowSubtree();
    button->setType("button");
    return button.release();
}

void MediaControlPanelMuteButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == eventNames().mouseoverEvent)
        m_controls->showVolumeSlider();

    MediaControlMuteButtonElement::defaultEventHandler(event);
}

const AtomicString& MediaControlPanelMuteButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-mute-button"));
    return id;
}

// ----------------------------

inline MediaControlVolumeSliderMuteButtonElement::MediaControlVolumeSliderMuteButtonElement(Document* document)
    : MediaControlMuteButtonElement(document, MediaMuteButton)
{
}

PassRefPtr<MediaControlVolumeSliderMuteButtonElement> MediaControlVolumeSliderMuteButtonElement::create(Document* document)
{
    RefPtr<MediaControlVolumeSliderMuteButtonElement> button = adoptRef(new MediaControlVolumeSliderMuteButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    return button.release();
}

const AtomicString& MediaControlVolumeSliderMuteButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-volume-slider-mute-button"));
    return id;
}

// ----------------------------

inline MediaControlPlayButtonElement::MediaControlPlayButtonElement(Document* document)
    : MediaControlInputElement(document, MediaPlayButton)
{
}

PassRefPtr<MediaControlPlayButtonElement> MediaControlPlayButtonElement::create(Document* document)
{
    RefPtr<MediaControlPlayButtonElement> button = adoptRef(new MediaControlPlayButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    return button.release();
}

void MediaControlPlayButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == eventNames().clickEvent) {
        if (mediaController()->canPlay())
            mediaController()->play();
        else
            mediaController()->pause();
        updateDisplayType();
        event->setDefaultHandled();
    }
    HTMLInputElement::defaultEventHandler(event);
}

void MediaControlPlayButtonElement::updateDisplayType()
{
    setDisplayType(mediaController()->canPlay() ? MediaPlayButton : MediaPauseButton);
}

const AtomicString& MediaControlPlayButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-play-button"));
    return id;
}

// ----------------------------

inline MediaControlFFButtonElement::MediaControlFFButtonElement(Document* document)
    : MediaControlInputElement(document, MediaFFButton)
{
}

PassRefPtr<MediaControlFFButtonElement> MediaControlFFButtonElement::create(Document* document)
{
    RefPtr<MediaControlFFButtonElement> button = adoptRef(new MediaControlFFButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    return button.release();
}

void MediaControlFFButtonElement::defaultEventHandler(Event* event)
{
    updateDisplayType();
    if (event->type() == eventNames().clickEvent) {
        event->setDefaultHandled();
    }
    HTMLInputElement::defaultEventHandler(event);
}

MediaControlElementType MediaControlFFButtonElement::displayType()
{
    return MediaFFButton;
}

void MediaControlFFButtonElement::updateDisplayType()
{
	setDisplayType(MediaFFButton);
}

const AtomicString& MediaControlFFButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-ff-button"));
    return id;
}

// ----------------------------

inline MediaControlRevButtonElement::MediaControlRevButtonElement(Document* document)
    : MediaControlInputElement(document, MediaRevButton)
{
}

PassRefPtr<MediaControlRevButtonElement> MediaControlRevButtonElement::create(Document* document)
{
    RefPtr<MediaControlRevButtonElement> button = adoptRef(new MediaControlRevButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    return button.release();
}

void MediaControlRevButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == eventNames().clickEvent) {
        updateDisplayType();
        event->setDefaultHandled();
    }
    HTMLInputElement::defaultEventHandler(event);
}

MediaControlElementType MediaControlRevButtonElement::displayType() const
{
    return MediaRevButton;
}

void MediaControlRevButtonElement::updateDisplayType()
{
	setDisplayType(MediaRevButton);
}

const AtomicString& MediaControlRevButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-rev-button"));
    return id;
}

// ----------------------------

inline MediaControlOverlayPlayButtonElement::MediaControlOverlayPlayButtonElement(Document* document)
    : MediaControlInputElement(document, MediaOverlayPlayButton)
{
}

PassRefPtr<MediaControlOverlayPlayButtonElement> MediaControlOverlayPlayButtonElement::create(Document* document)
{
    RefPtr<MediaControlOverlayPlayButtonElement> button = adoptRef(new MediaControlOverlayPlayButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    return button.release();
}

void MediaControlOverlayPlayButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == eventNames().clickEvent && mediaController()->canPlay()) {
        mediaController()->play();
        updateDisplayType();
        event->setDefaultHandled();
    }
    HTMLInputElement::defaultEventHandler(event);
}

void MediaControlOverlayPlayButtonElement::updateDisplayType()
{
    if (mediaController()->canPlay()) {
        show();
        setDisplayType(MediaOverlayPlayButton);
    } else
        hide();
}

const AtomicString& MediaControlOverlayPlayButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-overlay-play-button"));
    return id;
}

// ----------------------------

inline MediaControlSeekButtonElement::MediaControlSeekButtonElement(Document* document, MediaControlElementType displayType)
    : MediaControlInputElement(document, displayType)
    , m_actionOnStop(Nothing)
    , m_seekType(Skip)
    , m_seekTimer(this, &MediaControlSeekButtonElement::seekTimerFired)
{
}

void MediaControlSeekButtonElement::defaultEventHandler(Event* event)
{
    // Set the mousedown and mouseup events as defaultHandled so they
    // do not trigger drag start or end actions in MediaControlPanelElement.
    if (event->type() == eventNames().mousedownEvent || event->type() == eventNames().mouseupEvent)
        event->setDefaultHandled();
}

void MediaControlSeekButtonElement::setActive(bool flag, bool pause)
{
    if (flag == active())
        return;

    if (flag)
        startTimer();
    else
        stopTimer();

    MediaControlInputElement::setActive(flag, pause);
}

void MediaControlSeekButtonElement::startTimer()
{
    m_seekType = mediaController()->supportsScanning() ? Scan : Skip;

    if (m_seekType == Skip) {
        // Seeking by skipping requires the video to be paused during seeking.
        m_actionOnStop = mediaController()->paused() ? Nothing : Play;
        mediaController()->pause();
    } else {
        // Seeking by scanning requires the video to be playing during seeking.
        m_actionOnStop = mediaController()->paused() ? Pause : Nothing;
        mediaController()->play();
        mediaController()->setPlaybackRate(nextRate());
    }

    m_seekTimer.start(0, m_seekType == Skip ? cSkipRepeatDelay : cScanRepeatDelay);
}

void MediaControlSeekButtonElement::stopTimer()
{
    if (m_seekType == Scan)
        mediaController()->setPlaybackRate(mediaController()->defaultPlaybackRate());

    if (m_actionOnStop == Play)
        mediaController()->play();
    else if (m_actionOnStop == Pause)
        mediaController()->pause();

    if (m_seekTimer.isActive())
        m_seekTimer.stop();
}

float MediaControlSeekButtonElement::nextRate() const
{
    float rate = std::min(cScanMaximumRate, fabsf(mediaController()->playbackRate() * 2));
    if (!isForwardButton())
        rate *= -1;
    return rate;
}

void MediaControlSeekButtonElement::seekTimerFired(Timer<MediaControlSeekButtonElement>*)
{
    if (m_seekType == Skip) {
        ExceptionCode ec;
        float skipTime = isForwardButton() ? cSkipTime : -cSkipTime;
        mediaController()->setCurrentTime(mediaController()->currentTime() + skipTime, ec);
    } else
        mediaController()->setPlaybackRate(nextRate());
}

// ----------------------------

inline MediaControlSeekForwardButtonElement::MediaControlSeekForwardButtonElement(Document* document)
    : MediaControlSeekButtonElement(document, MediaSeekForwardButton)
{
}

PassRefPtr<MediaControlSeekForwardButtonElement> MediaControlSeekForwardButtonElement::create(Document* document)
{
    RefPtr<MediaControlSeekForwardButtonElement> button = adoptRef(new MediaControlSeekForwardButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    return button.release();
}

const AtomicString& MediaControlSeekForwardButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-seek-forward-button"));
    return id;
}

// ----------------------------

inline MediaControlSeekBackButtonElement::MediaControlSeekBackButtonElement(Document* document)
    : MediaControlSeekButtonElement(document, MediaSeekBackButton)
{
}

PassRefPtr<MediaControlSeekBackButtonElement> MediaControlSeekBackButtonElement::create(Document* document)
{
    RefPtr<MediaControlSeekBackButtonElement> button = adoptRef(new MediaControlSeekBackButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    return button.release();
}

const AtomicString& MediaControlSeekBackButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-seek-back-button"));
    return id;
}

// ----------------------------

inline MediaControlRewindButtonElement::MediaControlRewindButtonElement(Document* document)
    : MediaControlInputElement(document, MediaRewindButton)
{
}

PassRefPtr<MediaControlRewindButtonElement> MediaControlRewindButtonElement::create(Document* document)
{
    RefPtr<MediaControlRewindButtonElement> button = adoptRef(new MediaControlRewindButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    return button.release();
}

void MediaControlRewindButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == eventNames().clickEvent) {
        ExceptionCode ignoredCode;
        mediaController()->setCurrentTime(max(0.0f, mediaController()->currentTime() - 30), ignoredCode);
        event->setDefaultHandled();
    }    
    HTMLInputElement::defaultEventHandler(event);
}

const AtomicString& MediaControlRewindButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-rewind-button"));
    return id;
}

// ----------------------------

inline MediaControlReturnToRealtimeButtonElement::MediaControlReturnToRealtimeButtonElement(Document* document)
    : MediaControlInputElement(document, MediaReturnToRealtimeButton)
{
}

PassRefPtr<MediaControlReturnToRealtimeButtonElement> MediaControlReturnToRealtimeButtonElement::create(Document* document)
{
    RefPtr<MediaControlReturnToRealtimeButtonElement> button = adoptRef(new MediaControlReturnToRealtimeButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    button->hide();
    return button.release();
}

void MediaControlReturnToRealtimeButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == eventNames().clickEvent) {
        mediaController()->returnToRealtime();
        event->setDefaultHandled();
    }
    HTMLInputElement::defaultEventHandler(event);
}

const AtomicString& MediaControlReturnToRealtimeButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-return-to-realtime-button"));
    return id;
}

// ----------------------------

inline MediaControlToggleClosedCaptionsButtonElement::MediaControlToggleClosedCaptionsButtonElement(Document* document)
    : MediaControlInputElement(document, MediaShowClosedCaptionsButton)
{
}

PassRefPtr<MediaControlToggleClosedCaptionsButtonElement> MediaControlToggleClosedCaptionsButtonElement::create(Document* document)
{
    RefPtr<MediaControlToggleClosedCaptionsButtonElement> button = adoptRef(new MediaControlToggleClosedCaptionsButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    button->hide();
    return button.release();
}

void MediaControlToggleClosedCaptionsButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == eventNames().clickEvent) {
        mediaController()->setClosedCaptionsVisible(!mediaController()->closedCaptionsVisible());
        setChecked(mediaController()->closedCaptionsVisible());
        updateDisplayType();
        event->setDefaultHandled();
    }

    HTMLInputElement::defaultEventHandler(event);
}

void MediaControlToggleClosedCaptionsButtonElement::updateDisplayType()
{
    setDisplayType(mediaController()->closedCaptionsVisible() ? MediaHideClosedCaptionsButton : MediaShowClosedCaptionsButton);
}

const AtomicString& MediaControlToggleClosedCaptionsButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-toggle-closed-captions-button"));
    return id;
}

// ----------------------------

MediaControlTimelineElement::MediaControlTimelineElement(Document* document, MediaControls* controls)
    : MediaControlInputElement(document, MediaSlider)
    , m_controls(controls)
{
}

PassRefPtr<MediaControlTimelineElement> MediaControlTimelineElement::create(Document* document, MediaControls* controls)
{
    ASSERT(controls);

    RefPtr<MediaControlTimelineElement> timeline = adoptRef(new MediaControlTimelineElement(document, controls));
    timeline->createShadowSubtree();
    timeline->setType("range");
    timeline->setAttribute(precisionAttr, "float");
    return timeline.release();
}

void MediaControlTimelineElement::defaultEventHandler(Event* event)
{
    // Left button is 0. Rejects mouse events not from left button.
    if (event->isMouseEvent() && static_cast<MouseEvent*>(event)->button())
        return;

    if (!attached())
        return;

    if (event->type() == eventNames().mousedownEvent)
        mediaController()->beginScrubbing();

    if (event->type() == eventNames().mouseupEvent)
        mediaController()->endScrubbing();

    MediaControlInputElement::defaultEventHandler(event);

    if (event->type() == eventNames().mouseoverEvent || event->type() == eventNames().mouseoutEvent || event->type() == eventNames().mousemoveEvent)
        return;

    float time = narrowPrecisionToFloat(value().toDouble());
    if (event->type() == eventNames().inputEvent && time != mediaController()->currentTime()) {
        ExceptionCode ec;
        mediaController()->setCurrentTime(time, ec);
    }

    RenderSlider* slider = toRenderSlider(renderer());
    if (slider && slider->inDragMode())
        m_controls->updateTimeDisplay();
}

bool MediaControlTimelineElement::willRespondToMouseClickEvents()
{
    if (!attached())
        return false;

    return true;
}

void MediaControlTimelineElement::setPosition(float currentTime)
{
    setValue(String::number(currentTime));
}

void MediaControlTimelineElement::setDuration(float duration)
{
    setAttribute(maxAttr, String::number(isfinite(duration) ? duration : 0));
}


const AtomicString& MediaControlTimelineElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-timeline"));
    return id;
}

// ----------------------------

inline MediaControlVolumeSliderElement::MediaControlVolumeSliderElement(Document* document)
    : MediaControlInputElement(document, MediaVolumeSlider)
    , m_clearMutedOnUserInteraction(false)
{
}

PassRefPtr<MediaControlVolumeSliderElement> MediaControlVolumeSliderElement::create(Document* document)
{
    RefPtr<MediaControlVolumeSliderElement> slider = adoptRef(new MediaControlVolumeSliderElement(document));
    slider->createShadowSubtree();
    slider->setType("range");
    slider->setAttribute(precisionAttr, "float");
    slider->setAttribute(maxAttr, "1");
    return slider.release();
}

void MediaControlVolumeSliderElement::defaultEventHandler(Event* event)
{
    // Left button is 0. Rejects mouse events not from left button.
    if (event->isMouseEvent() && static_cast<MouseEvent*>(event)->button())
        return;

    if (!attached())
        return;

    MediaControlInputElement::defaultEventHandler(event);

    if (event->type() == eventNames().mouseoverEvent || event->type() == eventNames().mouseoutEvent || event->type() == eventNames().mousemoveEvent)
        return;

    float volume = narrowPrecisionToFloat(value().toDouble());
    if (volume != mediaController()->volume()) {
        ExceptionCode ec = 0;
        mediaController()->setVolume(volume, ec);
        ASSERT(!ec);
    }
    if (m_clearMutedOnUserInteraction)
        mediaController()->setMuted(false);
}

bool MediaControlVolumeSliderElement::willRespondToMouseMoveEvents()
{
    if (!attached())
        return false;

    return MediaControlInputElement::willRespondToMouseMoveEvents();
}

bool MediaControlVolumeSliderElement::willRespondToMouseClickEvents()
{
    if (!attached())
        return false;

    return MediaControlInputElement::willRespondToMouseClickEvents();
}

void MediaControlVolumeSliderElement::setVolume(float volume)
{
    if (value().toFloat() != volume)
        setValue(String::number(volume));
}

void MediaControlVolumeSliderElement::setClearMutedOnUserInteraction(bool clearMute)
{
    m_clearMutedOnUserInteraction = clearMute;
}

const AtomicString& MediaControlVolumeSliderElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-volume-slider"));
    return id;
}

// ----------------------------

inline MediaControlFullscreenVolumeSliderElement::MediaControlFullscreenVolumeSliderElement(Document* document)
    : MediaControlVolumeSliderElement(document)
{
}

PassRefPtr<MediaControlFullscreenVolumeSliderElement> MediaControlFullscreenVolumeSliderElement::create(Document* document)
{
    RefPtr<MediaControlFullscreenVolumeSliderElement> slider = adoptRef(new MediaControlFullscreenVolumeSliderElement(document));
    slider->createShadowSubtree();
    slider->setType("range");
    slider->setAttribute(precisionAttr, "float");
    slider->setAttribute(maxAttr, "1");
    return slider.release();
}

const AtomicString& MediaControlFullscreenVolumeSliderElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-fullscreen-volume-slider"));
    return id;
}

// ----------------------------

inline MediaControlFullscreenButtonElement::MediaControlFullscreenButtonElement(Document* document, MediaControls*)
    : MediaControlInputElement(document, MediaEnterFullscreenButton)
{
}

PassRefPtr<MediaControlFullscreenButtonElement> MediaControlFullscreenButtonElement::create(Document* document, MediaControls* controls)
{
    ASSERT(controls);

    RefPtr<MediaControlFullscreenButtonElement> button = adoptRef(new MediaControlFullscreenButtonElement(document, controls));
    button->createShadowSubtree();
    button->setType("button");
    button->hide();
    return button.release();
}

void MediaControlFullscreenButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == eventNames().clickEvent) {
#if ENABLE(FULLSCREEN_API)
        // Only use the new full screen API if the fullScreenEnabled setting has 
        // been explicitly enabled. Otherwise, use the old fullscreen API. This
        // allows apps which embed a WebView to retain the existing full screen
        // video implementation without requiring them to implement their own full 
        // screen behavior.
        if (document()->settings() && document()->settings()->fullScreenEnabled()) {
            if (document()->webkitIsFullScreen() && document()->webkitCurrentFullScreenElement() == toParentMediaElement(this))
                document()->webkitCancelFullScreen();
            else
                document()->requestFullScreenForElement(toParentMediaElement(this), 0, Document::ExemptIFrameAllowFullScreenRequirement);
        } else
#endif
            mediaController()->enterFullscreen();
        event->setDefaultHandled();
    }
    HTMLInputElement::defaultEventHandler(event);
}

const AtomicString& MediaControlFullscreenButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-fullscreen-button"));
    return id;
}

void MediaControlFullscreenButtonElement::setIsFullscreen(bool isFullscreen)
{
    setDisplayType(isFullscreen ? MediaExitFullscreenButton : MediaEnterFullscreenButton);
}

// ----------------------------

inline MediaControlVideoTrackSelButtonElement::MediaControlVideoTrackSelButtonElement(Document* document, MediaControls* controls)
    : MediaSelectElement(document, MediaVideoTrackSelButton)
	, m_controls(controls)
{
}

PassRefPtr<MediaControlVideoTrackSelButtonElement> MediaControlVideoTrackSelButtonElement::create(Document* document, MediaControls* controls)
{
	RefPtr<MediaControlVideoTrackSelButtonElement> button = adoptRef(new MediaControlVideoTrackSelButtonElement(document, controls));
	button->createShadowSubtree();
    button->hide();

	return button.release();
}

void MediaControlVideoTrackSelButtonElement::defaultEventHandler(Event* event) {

	MediaSelectElement::defaultEventHandler(event);
	fixEventState(event);
}

void MediaControlVideoTrackSelButtonElement::changedVideoTrack()
{
    updateDisplayType();
}

void MediaControlVideoTrackSelButtonElement::updateDisplayType()
{
	setDisplayType(MediaVideoTrackSelButton);
}

void MediaControlVideoTrackSelButtonElement::display()
{
	if (renderer()) {
		//setInlineStyleProperty(CSSPropertyBackgroundColor, CSSValueBlack);
		//setInlineStyleProperty(CSSPropertyColor, String("#ff7835"));
		int index = this->selectedIndex();
		std::vector<std::string> names = mediaController()->getSelVideoTrackNames();
		if (names.size() > 0) {
			names.push_back(std::string("Video Dummy1"));
			int len = 0;
			for (int i=0; i<(int)names.size(); i++)
			{
				if (names[i].length()>len) len = names[i].length();
			}

			// Set up Select control
			ExceptionCode ec = 0;
			removeAllChildren();
			std::stringstream ss;
			for (size_t i = 0; i < names.size(); ++i) {
				std::string nam = names[i];

				if (nam.length() == 0) {
					ss.clear();
					ss << "Video" << i;
					nam = std::string(ss.str());
				}

				RefPtr<MediaOptionElement> option = MediaOptionElement::create(document());

				appendChild(option, ec);
				option->setInlineStyleProperty(CSSPropertyBackgroundColor, CSSValueBlack);
				option->setInlineStyleProperty(CSSPropertyColor, String("#ff7835"));
				String sOpt(nam.c_str());
				option->appendChild(Text::create(document(), sOpt), ec);
			}
			if (index >= 0)
				setSelectedIndex(index);
		}
	}

}

void MediaControlVideoTrackSelButtonElement::selectChanged(int newIndex)
{
	mediaController()->selectVideoTrack(newIndex);
}


const AtomicString& MediaControlVideoTrackSelButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-video-track-sel-button"));
    return id;
}

// ----------------------------

inline MediaControlAudioTrackSelButtonElement::MediaControlAudioTrackSelButtonElement(Document* document, MediaControls* controls)
    : MediaSelectElement(document, MediaAudioTrackSelButton)
	, m_controls(controls)
{
}

PassRefPtr<MediaControlAudioTrackSelButtonElement> MediaControlAudioTrackSelButtonElement::create(Document* document, MediaControls* controls)
{
	RefPtr<MediaControlAudioTrackSelButtonElement> button = adoptRef(new MediaControlAudioTrackSelButtonElement(document, controls));
	button->createShadowSubtree();
    button->hide();

	return button.release();
}


void MediaControlAudioTrackSelButtonElement::defaultEventHandler(Event* event) {

	MediaSelectElement::defaultEventHandler(event);
	fixEventState(event);
}

void MediaControlAudioTrackSelButtonElement::changedAudioTrack()
{
    updateDisplayType();
}

void MediaControlAudioTrackSelButtonElement::updateDisplayType()
{
	setDisplayType(MediaAudioTrackSelButton);

}

void MediaControlAudioTrackSelButtonElement::display()
{
	if (renderer()) {
		//setInlineStyleProperty(CSSPropertyBackgroundColor, CSSValueBlack);
		//setInlineStyleProperty(CSSPropertyColor, String("#ff7835"));
		int index = this->selectedIndex();
		std::vector<std::string> names = mediaController()->getSelAudioTrackNames();

		if (names.size() > 0) {
			names.push_back(std::string("Audio Dummy1"));
			int len = 0;
			for (int i=0; i<(int)names.size(); i++)
			{
				if (names[i].length()>len) len = names[i].length();
			}

			// Set up Select control
			ExceptionCode ec = 0;
			removeAllChildren();
			std::stringstream ss;
			for (size_t i = 0; i < names.size(); ++i) {
				std::string nam = names[i];

				if (nam.length() == 0) {
					ss.clear();
					ss << "Audio" << i;
					nam = std::string(ss.str());
				}

				RefPtr<MediaOptionElement> option = MediaOptionElement::create(document());
				appendChild(option, ec);
				option->setInlineStyleProperty(CSSPropertyBackgroundColor, CSSValueBlack);
				option->setInlineStyleProperty(CSSPropertyColor, String("#ff7835"));
				String sOpt(nam.c_str());
				option->appendChild(Text::create(document(), sOpt), ec);
			}
			if (index >= 0)
				setSelectedIndex(index);
		}
	}

}


void MediaControlAudioTrackSelButtonElement::selectChanged(int newIndex)
{
	mediaController()->selectAudioTrack(newIndex);
}

const AtomicString& MediaControlAudioTrackSelButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-audio-track-sel-button"));
    return id;
}

// ----------------------------

inline MediaControlTextTrackSelButtonElement::MediaControlTextTrackSelButtonElement(Document* document, MediaControls* controls)
    : MediaSelectElement(document, MediaTextTrackSelButton)
	, m_controls(controls)
{
}

PassRefPtr<MediaControlTextTrackSelButtonElement> MediaControlTextTrackSelButtonElement::create(Document* document, MediaControls* controls)
{
	RefPtr<MediaControlTextTrackSelButtonElement> button = adoptRef(new MediaControlTextTrackSelButtonElement(document, controls));
	button->createShadowSubtree();
    button->hide();

	return button.release();
}


void MediaControlTextTrackSelButtonElement::defaultEventHandler(Event* event) {

	MediaSelectElement::defaultEventHandler(event);
	fixEventState(event);
}

void MediaControlTextTrackSelButtonElement::changedTextTrack()
{
    updateDisplayType();
}

void MediaControlTextTrackSelButtonElement::updateDisplayType()
{
	setDisplayType(MediaTextTrackSelButton);

}

void MediaControlTextTrackSelButtonElement::display()
{
	if (renderer()) {
		//setInlineStyleProperty(CSSPropertyBackgroundColor, CSSValueBlack);
		//setInlineStyleProperty(CSSPropertyColor, String("#ff7835"));
		std::vector<std::string> names = mediaController()->getSelTextTrackNames();

		if (names.size() > 0) {
			int len = 0;
			for (int i=0; i<(int)names.size(); i++)
			{
				if (names[i].length()>len) len = names[i].length();
			}

			// Set up Select control
			ExceptionCode ec = 0;
			removeAllChildren();
			for (size_t i = 0; i < names.size(); ++i) {
				RefPtr<MediaOptionElement> option = MediaOptionElement::create(document());
				appendChild(option, ec);
				option->setInlineStyleProperty(CSSPropertyBackgroundColor, CSSValueBlack);
				option->setInlineStyleProperty(CSSPropertyColor, String("#ff7835"));
				String sOpt(names[i].c_str());
				option->appendChild(Text::create(document(), sOpt), ec);
			}
		}
	}

}


void MediaControlTextTrackSelButtonElement::selectChanged(int newIndex)
{
	mediaController()->selectTextTrack(newIndex);
}

const AtomicString& MediaControlTextTrackSelButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-text-track-sel-button"));
    return id;
}

// ----------------------------

inline MediaControlFullscreenVolumeMinButtonElement::MediaControlFullscreenVolumeMinButtonElement(Document* document)
    : MediaControlInputElement(document, MediaUnMuteButton)
{
}

PassRefPtr<MediaControlFullscreenVolumeMinButtonElement> MediaControlFullscreenVolumeMinButtonElement::create(Document* document)
{
    RefPtr<MediaControlFullscreenVolumeMinButtonElement> button = adoptRef(new MediaControlFullscreenVolumeMinButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    return button.release();
}

void MediaControlFullscreenVolumeMinButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == eventNames().clickEvent) {
        ExceptionCode code = 0;
        mediaController()->setVolume(0, code);
        event->setDefaultHandled();
    }
    HTMLInputElement::defaultEventHandler(event);
}

const AtomicString& MediaControlFullscreenVolumeMinButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-fullscreen-volume-min-button"));
    return id;
}

// ----------------------------

inline MediaControlFullscreenVolumeMaxButtonElement::MediaControlFullscreenVolumeMaxButtonElement(Document* document)
: MediaControlInputElement(document, MediaMuteButton)
{
}

PassRefPtr<MediaControlFullscreenVolumeMaxButtonElement> MediaControlFullscreenVolumeMaxButtonElement::create(Document* document)
{
    RefPtr<MediaControlFullscreenVolumeMaxButtonElement> button = adoptRef(new MediaControlFullscreenVolumeMaxButtonElement(document));
    button->createShadowSubtree();
    button->setType("button");
    return button.release();
}

void MediaControlFullscreenVolumeMaxButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == eventNames().clickEvent) {
        ExceptionCode code = 0;
        mediaController()->setVolume(1, code);
        event->setDefaultHandled();
    }
    HTMLInputElement::defaultEventHandler(event);
}

const AtomicString& MediaControlFullscreenVolumeMaxButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-fullscreen-volume-max-button"));
    return id;
}

// ----------------------------

class RenderMediaControlTimeDisplay : public RenderDeprecatedFlexibleBox {
public:
    RenderMediaControlTimeDisplay(Node*);

private:
    virtual void layout();
};

RenderMediaControlTimeDisplay::RenderMediaControlTimeDisplay(Node* node)
    : RenderDeprecatedFlexibleBox(node)
{
}

// We want the timeline slider to be at least 100 pixels wide.
// FIXME: Eliminate hard-coded widths altogether.
static const int minWidthToDisplayTimeDisplays = 45 + 100 + 45;

void RenderMediaControlTimeDisplay::layout()
{
    RenderDeprecatedFlexibleBox::layout();
    RenderBox* timelineContainerBox = parentBox();
    while (timelineContainerBox && timelineContainerBox->isAnonymous())
        timelineContainerBox = timelineContainerBox->parentBox();

    if (timelineContainerBox && timelineContainerBox->width() < minWidthToDisplayTimeDisplays)
        setWidth(0);
}

inline MediaControlTimeDisplayElement::MediaControlTimeDisplayElement(Document* document)
    : MediaControlElement(document)
    , m_currentValue(0)
{
}

void MediaControlTimeDisplayElement::setCurrentValue(float time)
{
    m_currentValue = time;
}

RenderObject* MediaControlTimeDisplayElement::createRenderer(RenderArena* arena, RenderStyle*)
{
    return new (arena) RenderMediaControlTimeDisplay(this);
}

// ----------------------------

PassRefPtr<MediaControlTimeRemainingDisplayElement> MediaControlTimeRemainingDisplayElement::create(Document* document)
{
    return adoptRef(new MediaControlTimeRemainingDisplayElement(document));
}

MediaControlTimeRemainingDisplayElement::MediaControlTimeRemainingDisplayElement(Document* document)
    : MediaControlTimeDisplayElement(document)
{
}

MediaControlElementType MediaControlTimeRemainingDisplayElement::displayType() const
{
    return MediaTimeRemainingDisplay;
}

const AtomicString& MediaControlTimeRemainingDisplayElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-time-remaining-display"));
    return id;
}

// ----------------------------

PassRefPtr<MediaControlCurrentTimeDisplayElement> MediaControlCurrentTimeDisplayElement::create(Document* document)
{
    return adoptRef(new MediaControlCurrentTimeDisplayElement(document));
}

MediaControlCurrentTimeDisplayElement::MediaControlCurrentTimeDisplayElement(Document* document)
    : MediaControlTimeDisplayElement(document)
{
}

MediaControlElementType MediaControlCurrentTimeDisplayElement::displayType() const
{
    return MediaCurrentTimeDisplay;
}

const AtomicString& MediaControlCurrentTimeDisplayElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-controls-current-time-display"));
    return id;
}

// ----------------------------

#if ENABLE(VIDEO_TRACK)

class RenderTextTrackContainerElement : public RenderBlock {
public:
    RenderTextTrackContainerElement(Node*);

private:
    virtual void layout();
};

RenderTextTrackContainerElement::RenderTextTrackContainerElement(Node* node)
    : RenderBlock(node)
{
}

void RenderTextTrackContainerElement::layout()
{
    RenderBlock::layout();
    if (style()->display() == NONE)
        return;

    ASSERT(mediaControlElementType(node()) == MediaTextTrackDisplayContainer);

    LayoutStateDisabler layoutStateDisabler(view());
    static_cast<MediaControlTextTrackContainerElement*>(node())->updateSizes();
}

inline MediaControlTextTrackContainerElement::MediaControlTextTrackContainerElement(Document* document)
    : MediaControlElement(document)
    , m_fontSize(0)
{
}

PassRefPtr<MediaControlTextTrackContainerElement> MediaControlTextTrackContainerElement::create(Document* document)
{
    RefPtr<MediaControlTextTrackContainerElement> element = adoptRef(new MediaControlTextTrackContainerElement(document));
    element->hide();
    return element.release();
}

RenderObject* MediaControlTextTrackContainerElement::createRenderer(RenderArena* arena, RenderStyle*)
{
    return new (arena) RenderTextTrackContainerElement(this);
}

const AtomicString& MediaControlTextTrackContainerElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id, ("-webkit-media-text-track-container"));
    return id;
}

void MediaControlTextTrackContainerElement::updateDisplay()
{
    HTMLMediaElement* mediaElement = toParentMediaElement(this);

    // 1. If the media element is an audio element, or is another playback
    // mechanism with no rendering area, abort these steps. There is nothing to
    // render.
    if (!mediaElement->isVideo())
        return;

    // 2. Let video be the media element or other playback mechanism.
    HTMLVideoElement* video = static_cast<HTMLVideoElement*>(mediaElement);

    // 3. Let output be an empty list of absolutely positioned CSS block boxes.
    Vector<RefPtr<HTMLDivElement> > output;

    // 4. If the user agent is exposing a user interface for video, add to
    // output one or more completely transparent positioned CSS block boxes that
    // cover the same region as the user interface.

    // 5. If the last time these rules were run, the user agent was not exposing
    // a user interface for video, but now it is, let reset be true. Otherwise,
    // let reset be false.

    // There is nothing to be done explicitly for 4th and 5th steps, as
    // everything is handled through CSS. The caption box is on top of the
    // controls box, in a container set with the -webkit-box display property.

    // 6. Let tracks be the subset of video's list of text tracks that have as
    // their rules for updating the text track rendering these rules for
    // updating the display of WebVTT text tracks, and whose text track mode is
    // showing or showing by default.
    // 7. Let cues be an empty list of text track cues.
    // 8. For each track track in tracks, append to cues all the cues from
    // track's list of cues that have their text track cue active flag set.
    CueList activeCues = video->currentlyActiveCues();

    // 9. If reset is false, then, for each text track cue cue in cues: if cue's
    // text track cue display state has a set of CSS boxes, then add those boxes
    // to output, and remove cue from cues.

    // There is nothing explicitly to be done here, as all the caching occurs
    // within the TextTrackCue instance itself. If parameters of the cue change,
    // the display tree is cleared.

    // 10. For each text track cue cue in cues that has not yet had
    // corresponding CSS boxes added to output, in text track cue order, run the
    // following substeps:
    for (size_t i = 0; i < activeCues.size(); ++i) {
        TextTrackCue* cue = activeCues[i].data();

        ASSERT(cue->isActive());
        if (!cue->track() || !cue->track()->isRendered())
            continue;

        RefPtr<TextTrackCueBox> displayBox = cue->getDisplayTree();

        if (displayBox->hasChildNodes() && !contains(static_cast<Node*>(displayBox.get())))
            // Note: the display tree of a cue is removed when the active flag of the cue is unset.
            appendChild(displayBox, ASSERT_NO_EXCEPTION, false);
    }

    // 11. Return output.
    hasChildNodes() ? show() : hide();
}

static const float mimimumFontSize = 16;
static const float videoHeightFontSizePercentage = .05;
static const float trackBottomMultiplier = .9;

void MediaControlTextTrackContainerElement::updateSizes()
{
    HTMLMediaElement* mediaElement = toParentMediaElement(this);
    if (!mediaElement || !mediaElement->renderer() || !mediaElement->renderer()->isVideo())
        return;

    IntRect videoBox = toRenderVideo(mediaElement->renderer())->videoBox();
    if (m_videoDisplaySize == videoBox)
        return;
    m_videoDisplaySize = videoBox;

    float fontSize = m_videoDisplaySize.size().height() * videoHeightFontSizePercentage;
    if (fontSize != m_fontSize) {
        m_fontSize = fontSize;
        setInlineStyleProperty(CSSPropertyFontSize, String::number(fontSize) + "px");
    }
}


#endif // ENABLE(VIDEO_TRACK)

// ----------------------------

} // namespace WebCore

#endif // ENABLE(VIDEO)
