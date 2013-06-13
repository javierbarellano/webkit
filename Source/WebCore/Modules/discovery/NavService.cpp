/*
 * NamService.cpp
 *
 */

#include "config.h"

#if ENABLE(DISCOVERY)

#include "NavDsc.h"
#include "NavService.h"


namespace WebCore {

// Copy Constructor
NavService::NavService(const NavService &that)
	: RefCounted()
	, ActiveDOMObject(that.scriptExecutionContext())
{
	setName  (that.name());
	setUrl   (that.url());
	setid    (that.id());
	setConfig(that.config());
	setType  (that.type());
	setPType (that.pType());
	setOnline(that.online());
}

NavService& NavService::operator= (const NavService &that)
{
	setName(that.name());
	setUrl(that.url());
	setid(that.id());
	setConfig(that.config());
	setType(that.type());
	setPType(that.pType());
	setOnline(that.online());

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

bool   NavService::hasPermission() const
{
    return m_hasPermission || !NavDsc::m_permissionsEnabled;
}

} // namespace WebCore

#endif // DISCOVERY

