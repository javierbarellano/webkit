/*
 * NamService.cpp
 *
 */

#include "config.h"
#include "NavService.h"


namespace WebCore {

// Copy Constructor
NavService::NavService(const NavService &that) :
	RefCounted()
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


} // namespace WebCore
