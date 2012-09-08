/*
 * NamService.cpp
 *
 */

#include "config.h"

#if ENABLE(DISCOVERY)

#include "NavService.h"


namespace WebCore {

// Copy Constructor
NavService::NavService(const NavService &that)
	: RefCounted()
	, ActiveDOMObject(that.scriptExecutionContext(), this)
{
	setName(that.name());
	setUrl(that.url());
	setEventUrl(that.eventUrl());
	setuuid(that.uuid());
	setConfig(that.config());
	setServiceType(that.serviceType());
	setPType(that.pType());
}

NavService& NavService::operator= (const NavService &that)
{
	setName(that.name());
	setUrl(that.url());
	setEventUrl(that.eventUrl());
	setuuid(that.uuid());
	setConfig(that.config());
	setServiceType(that.serviceType());
	setPType(that.pType());

	return *this;
}

NavService::~NavService(){}


const AtomicString& NavService::interfaceName() const
{
    return eventNames().interfaceForNavService;
}

ScriptExecutionContext* NavService::scriptExecutionContext() const
{
    return ActiveDOMObject::scriptExecutionContext();
}

EventTargetData* NavService::eventTargetData()
{
    return &m_eventTargetData;
}

EventTargetData* NavService::ensureEventTargetData()
{
    return &m_eventTargetData;
}

} // namespace WebCore

#endif // DISCOVERY

