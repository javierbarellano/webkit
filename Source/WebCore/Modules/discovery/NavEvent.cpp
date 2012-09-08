/*
 * NavEvent.cpp
 *
 *  Created on: Jun 6, 2012
 *      Author: gar
 */

#include "config.h"

#if ENABLE(DISCOVERY)

#include "NavEvent.h"

namespace WebCore {

// static
PassRefPtr<NavEvent> NavEvent::create()
{
	return adoptRef(new NavEvent());
}

// Copy Constructor
NavEvent::NavEvent(const NavEvent &that) :
	RefCounted()
{
	m_propertyset = that.m_propertyset;
	m_uuid = that.m_uuid;
	m_friendlyName = that.m_friendlyName;
	m_serviceType = that.m_serviceType;
}

NavEvent& NavEvent::operator= (const NavEvent &that)
{
	m_propertyset = that.m_propertyset;
	m_uuid = that.m_uuid;
	m_friendlyName = that.m_friendlyName;
	m_serviceType = that.m_serviceType;

	return *this;
}

};

#endif //ENABLE(DISCOVERY)



