/*
 * NavServices.cpp
 *
 *  Created on: Jan 10, 2012
 *      Author: gar
 */

#include "config.h"
#include "NavServices.h"


namespace WebCore {

// Copy Constructor
NavServices::NavServices(const NavServices &that) :
		RefCounted()
{
	clearSrvs();
	for (int i=0; i < (int)that.m_services.size(); i++)
		m_services.append(that.m_services.at(i));
}

NavServices& NavServices::operator= (const NavServices &that)
{
	clearSrvs();
	for (int i=0; i < (int)that.m_services.size(); i++)
		m_services.append(that.m_services.at(i));

	return *this;
}

NavServices::~NavServices()
{
	clearSrvs();
}



} // namespace WebCore
