/*
 * RenderMediaMenuList.cpp
 *
 *  Created on: Dec 21, 2012
 *      Author: gar
 */

#include "config.h"
#include "RenderMenuList.h"

#include "AXObjectCache.h"
#include "AccessibilityMenuList.h"
#include "CSSFontSelector.h"
#include "Chrome.h"
#include "FontCache.h"
#include "Frame.h"
#include "FrameView.h"
#include "HTMLNames.h"
#include "HTMLOptionElement.h"
#include "HTMLOptGroupElement.h"
#include "NodeRenderStyle.h"
#include "Page.h"
#include "PopupMenu.h"
#include "RenderBR.h"
#include "RenderScrollbar.h"
#include "RenderTheme.h"
#include "Settings.h"
#include "StyleResolver.h"
#include "TextRun.h"
#include <math.h>

#include "RenderMediaMenuList.h"

using namespace std;

namespace WebCore {

RenderMediaMenuList::RenderMediaMenuList(Element* element)
	: RenderMenuList(element)
{
}

RenderMediaMenuList::~RenderMediaMenuList()
{
}

void RenderMediaMenuList::updateOptionsWidth()
{
    float maxOptionWidth = 0;
    const Vector<HTMLElement*>& listItems = mediaSelElement()->listItems();
    int size = listItems.size();
    FontCachePurgePreventer fontCachePurgePreventer;

    for (int i = 0; i < size; ++i) {
        HTMLElement* element = listItems[i];
        if (!element->hasTagName(WebCore::HTMLNames::optionTag))
            continue;

        String text = toHTMLOptionElement(element)->textIndentedToRespectGroupLabel();
        applyTextTransform(style(), text, ' ');
        if (theme()->popupOptionSupportsTextIndent()) {
            // Add in the option's text indent.  We can't calculate percentage values for now.
            float optionWidth = 0;
            if (RenderStyle* optionStyle = element->renderStyle())
                optionWidth += minimumValueForLength(optionStyle->textIndent(), 0, view());
            if (!text.isEmpty())
                optionWidth += style()->font().width(text);
            maxOptionWidth = max(maxOptionWidth, optionWidth);
        } else if (!text.isEmpty())
            maxOptionWidth = max(maxOptionWidth, style()->font().width(text));
    }

    int width = static_cast<int>(ceilf(maxOptionWidth));
    if (m_optionsWidth == width)
        return;

    m_optionsWidth = width;
    if (parent())
        setNeedsLayoutAndPrefWidthsRecalc();
}

void RenderMediaMenuList::updateFromElement()
{
    if (m_optionsChanged) {
        updateOptionsWidth();
        m_optionsChanged = false;
    }

    if (m_popupIsVisible)
        m_popup->updateFromElement();
    else
        setTextFromOption(mediaSelElement()->selectedIndex());
}

void RenderMediaMenuList::setTextFromOption(int optionIndex)
{
	MediaSelectElement* select = mediaSelElement();
    const Vector<HTMLElement*>& listItems = select->listItems();
    int size = listItems.size();

    int i = select->optionToListIndex(optionIndex);
    String text = emptyString();
    if (i >= 0 && i < size) {
        Element* element = listItems[i];
        if (element->hasTagName(WebCore::HTMLNames::optionTag)) {
            text = toHTMLOptionElement(element)->textIndentedToRespectGroupLabel();
            m_optionStyle = element->renderStyle();
        }
    }

    setText(text.stripWhiteSpace());
    didUpdateActiveOption(optionIndex);
}

void RenderMediaMenuList::showPopup()
{
    if (m_popupIsVisible)
        return;

    if (document()->page()->chrome()->hasOpenedPopup())
        return;

    // Create m_innerBlock here so it ends up as the first child.
    // This is important because otherwise we might try to create m_innerBlock
    // inside the showPopup call and it would fail.
    createInnerBlock();
    setPopUp(this);
    m_popupIsVisible = true;

    // Compute the top left taking transforms into account, but use
    // the actual width of the element to size the popup.
    FloatPoint absTopLeft = localToAbsolute(FloatPoint(), false, true);
    IntRect absBounds = absoluteBoundingBoxRectIgnoringTransforms();
    absBounds.setLocation(roundedIntPoint(absTopLeft));
    MediaSelectElement* select = mediaSelElement();
    m_popup->show(absBounds, document()->view(), select->optionToListIndex(select->selectedIndex()));
}

void RenderMediaMenuList::valueChanged(unsigned listIndex, bool fireOnChange)
{
    // Check to ensure a page navigation has not occurred while
    // the popup was up.
    Document* doc = static_cast<Element*>(node())->document();
    if (!doc || doc != doc->frame()->document())
        return;

    MediaSelectElement* select = mediaSelElement();
    select->optionSelectedByUser(select->listToOptionIndex(listIndex), fireOnChange);
}

void RenderMediaMenuList::listBoxSelectItem(int listIndex, bool allowMultiplySelections, bool shift, bool fireOnChangeNow)
{
    mediaSelElement()->listBoxSelectItem(listIndex, allowMultiplySelections, shift, fireOnChangeNow);
}

bool RenderMediaMenuList::multiple() const
{
	MediaSelectElement *ms = mediaSelElement();
    return ms->multiple();
}

void RenderMediaMenuList::didSetSelectedIndex(int listIndex)
{
    didUpdateActiveOption(mediaSelElement()->listToOptionIndex(listIndex));
}

void RenderMediaMenuList::didUpdateActiveOption(int optionIndex)
{
    if (!AXObjectCache::accessibilityEnabled())
        return;

    if (m_lastActiveIndex == optionIndex)
        return;
    m_lastActiveIndex = optionIndex;

    MediaSelectElement* select = mediaSelElement();
    int listIndex = select->optionToListIndex(optionIndex);
    if (listIndex < 0 || listIndex >= static_cast<int>(select->listItems().size()))
        return;

    ASSERT(select->listItems()[listIndex]);

    if (AccessibilityMenuList* menuList = static_cast<AccessibilityMenuList*>(document()->axObjectCache()->get(this)))
        menuList->didUpdateActiveOption(optionIndex);
}

String RenderMediaMenuList::itemText(unsigned listIndex) const
{
	MediaSelectElement* select = mediaSelElement();
    const Vector<HTMLElement*>& listItems = select->listItems();
    if (listIndex >= listItems.size())
        return String();

    String itemString;
    Element* element = listItems[listIndex];
    if (element->hasTagName(WebCore::HTMLNames::optgroupTag))
        itemString = static_cast<const HTMLOptGroupElement*>(element)->groupLabelText();
    else if (element->hasTagName(WebCore::HTMLNames::optionTag))
        itemString = toHTMLOptionElement(element)->textIndentedToRespectGroupLabel();

    applyTextTransform(style(), itemString, ' ');
    return itemString;
}

String RenderMediaMenuList::itemAccessibilityText(unsigned listIndex) const
{
    // Allow the accessible name be changed if necessary.
    const Vector<HTMLElement*>& listItems = mediaSelElement()->listItems();
    if (listIndex >= listItems.size())
        return String();
    return listItems[listIndex]->fastGetAttribute(WebCore::HTMLNames::aria_labelAttr);
}

String RenderMediaMenuList::itemToolTip(unsigned listIndex) const
{
    const Vector<HTMLElement*>& listItems = mediaSelElement()->listItems();
    if (listIndex >= listItems.size())
        return String();
    return listItems[listIndex]->title();
}

bool RenderMediaMenuList::itemIsEnabled(unsigned listIndex) const
{
    const Vector<HTMLElement*>& listItems = mediaSelElement()->listItems();
    if (listIndex >= listItems.size())
        return false;
    HTMLElement* element = listItems[listIndex];
    if (!element->hasTagName(WebCore::HTMLNames::optionTag))
        return false;

    bool groupEnabled = true;
    if (Element* parentElement = element->parentElement()) {
        if (parentElement->hasTagName(WebCore::HTMLNames::optgroupTag))
            groupEnabled = !static_cast<HTMLOptGroupElement*>(parentElement)->disabled();
    }
    if (!groupEnabled)
        return false;

    return element->isEnabledFormControl();
}

PopupMenuStyle RenderMediaMenuList::itemStyle(unsigned listIndex) const
{
    const Vector<HTMLElement*>& listItems = mediaSelElement()->listItems();
    if (listIndex >= listItems.size()) {
        // If we are making an out of bounds access, then we want to use the style
        // of a different option element (index 0). However, if there isn't an option element
        // before at index 0, we fall back to the menu's style.
        if (!listIndex)
            return menuStyle();

        // Try to retrieve the style of an option element we know exists (index 0).
        listIndex = 0;
    }
    HTMLElement* element = listItems[listIndex];

    RenderStyle* style = element->renderStyle() ? element->renderStyle() : element->computedStyle();
    return style ? PopupMenuStyle(style->visitedDependentColor(CSSPropertyColor), itemBackgroundColor(listIndex), style->font(), style->visibility() == VISIBLE,
        style->display() == NONE, style->textIndent(), style->direction(), isOverride(style->unicodeBidi())) : menuStyle();
}

Color RenderMediaMenuList::itemBackgroundColor(unsigned listIndex) const
{
    const Vector<HTMLElement*>& listItems = mediaSelElement()->listItems();
    if (listIndex >= listItems.size())
        return style()->visitedDependentColor(CSSPropertyBackgroundColor);
    HTMLElement* element = listItems[listIndex];

    Color backgroundColor;
    if (element->renderStyle())
        backgroundColor = element->renderStyle()->visitedDependentColor(CSSPropertyBackgroundColor);
    // If the item has an opaque background color, return that.
    if (!backgroundColor.hasAlpha())
        return backgroundColor;

    // Otherwise, the item's background is overlayed on top of the menu background.
    backgroundColor = style()->visitedDependentColor(CSSPropertyBackgroundColor).blend(backgroundColor);
    if (!backgroundColor.hasAlpha())
        return backgroundColor;

    // If the menu background is not opaque, then add an opaque white background behind.
    Color clr(Color::white);
    clr = clr.blend(backgroundColor);
    return clr;
}

int RenderMediaMenuList::listSize() const
{
    return mediaSelElement()->listItems().size();
}

int RenderMediaMenuList::selectedIndex() const
{
	MediaSelectElement* select = mediaSelElement();
    return select->optionToListIndex(select->selectedIndex());
}

void RenderMediaMenuList::popupDidHide()
{
    m_popupIsVisible = false;
}

bool RenderMediaMenuList::itemIsSeparator(unsigned listIndex) const
{
    const Vector<HTMLElement*>& listItems = mediaSelElement()->listItems();
    return listIndex < listItems.size() && listItems[listIndex]->hasTagName(WebCore::HTMLNames::hrTag);
}

bool RenderMediaMenuList::itemIsLabel(unsigned listIndex) const
{
    const Vector<HTMLElement*>& listItems = mediaSelElement()->listItems();
    return listIndex < listItems.size() && listItems[listIndex]->hasTagName(WebCore::HTMLNames::optgroupTag);
}

bool RenderMediaMenuList::itemIsSelected(unsigned listIndex) const
{
    const Vector<HTMLElement*>& listItems = mediaSelElement()->listItems();
    if (listIndex >= listItems.size())
        return false;
    HTMLElement* element = listItems[listIndex];
    return element->hasTagName(WebCore::HTMLNames::optionTag) && toHTMLOptionElement(element)->selected();
}

void RenderMediaMenuList::setTextFromItem(unsigned listIndex)
{
    setTextFromOption(mediaSelElement()->listToOptionIndex(listIndex));
}

inline MediaSelectElement* RenderMediaMenuList::mediaSelElement() const
{
    return static_cast<MediaSelectElement*>(node());
}







} /* namespace WebCore */

