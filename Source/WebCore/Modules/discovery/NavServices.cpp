/*
 * NavServices.cpp
 *
 *  Created on: Jan 10, 2012
 *      Author: gar
 */

#include "config.h"

#if ENABLE(DISCOVERY)

#include "NavServices.h"


namespace WebCore {

// Copy Constructor
NavServices::NavServices(const NavServices &that) :
		RefCounted()
		, ActiveDOMObject(that.scriptExecutionContext(), this)
{
	clearSrvs();
	for (int i=0; i < (int)that.m_services.size(); i++)
		m_services.append(that.m_services.at(i));
}

NavServices& NavServices::operator= (const NavServices &that)
{
	clearSrvs();
	for (int i=0; i < (int)that.m_services.size(); i++)
		m_services.append(that.m_services[i]);

	return *this;
}

NavServices::~NavServices()
{
	clearSrvs();
}

const AtomicString& NavServices::interfaceName() const
{
    return eventNames().interfaceForNavServices;
}

ScriptExecutionContext* NavServices::scriptExecutionContext() const
{
    return ActiveDOMObject::scriptExecutionContext();
}

EventTargetData* NavServices::eventTargetData()
{
    return &m_eventTargetData;
}

EventTargetData* NavServices::ensureEventTargetData()
{
    return &m_eventTargetData;
}

} // namespace WebCore

#endif // DISCOVERY

