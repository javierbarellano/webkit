/*
 * RenderMediaMenuList.h
 *
 *  Created on: Dec 21, 2012
 *      Author: gar
 */

#ifndef RENDERMEDIAMENULIST_H_
#define RENDERMEDIAMENULIST_H_

#include "RenderMenuList.h"
#include "MediaControlElements.h"

namespace WebCore {

class MediaSelectElement;

class RenderMediaMenuList: public WebCore::RenderMenuList {
public:
	RenderMediaMenuList(Element*);
	virtual ~RenderMediaMenuList();
	virtual void didSetSelectedIndex(int listIndex);
	virtual void showPopup();
protected:
	virtual void updateOptionsWidth();
	virtual void updateFromElement();
	virtual void setTextFromOption(int optionIndex);
	virtual void valueChanged(unsigned listIndex, bool fireOnChange);
	virtual void listBoxSelectItem(int listIndex, bool allowMultiplySelections, bool shift, bool fireOnChangeNow);
	virtual bool multiple() const;
	virtual void didUpdateActiveOption(int optionIndex);
	virtual String itemText(unsigned listIndex) const;
	virtual String itemAccessibilityText(unsigned listIndex) const;
	virtual String itemToolTip(unsigned listIndex) const;
	virtual bool itemIsEnabled(unsigned listIndex) const;
	virtual PopupMenuStyle itemStyle(unsigned listIndex) const;
	virtual Color itemBackgroundColor(unsigned listIndex) const;
	virtual int listSize() const;
	virtual int selectedIndex() const;
	virtual void popupDidHide();
	virtual bool itemIsSeparator(unsigned listIndex) const;
	virtual bool itemIsLabel(unsigned listIndex) const;
	virtual bool itemIsSelected(unsigned listIndex) const;
	virtual void setTextFromItem(unsigned listIndex);

	inline MediaSelectElement *mediaSelElement() const;
};

inline RenderMediaMenuList* toRenderMediaMenuList(RenderObject* object)
{
    ASSERT(!object || object->isMenuList());
    return static_cast<RenderMediaMenuList*>(object);
}



} /* namespace WebCore */
#endif /* RENDERMEDIAMENULIST_H_ */
