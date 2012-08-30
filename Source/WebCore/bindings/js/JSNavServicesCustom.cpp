/*
 * JSNavServicesCustom.cpp
 *
 *  Created on: Jan 10, 2012
 *      Author: gar
 */

#include <string>

#include "config.h"
#include "JSNavServices.h"
#include "JSNavService.h"

using namespace JSC;

namespace WebCore {

JSValue JSNavServices::item(ExecState* exec)
{
    bool ok;
    uint32_t index = Identifier::toUInt32(exec->argument(0).toString(exec)->value(exec), ok);
    if (ok)
    {
    	NavService *srv = impl()->item(index);
    	printf("JSNavServices::item(): name: %s", srv->name().ascii().data());

    	return toJS(exec, globalObject(), srv);

    }

	std::string google = "ERROR, index invalid.";
	UString sVal(google.c_str(), google.length());
	JSString *v = jsString(&exec->globalData(), sVal);
	JSValue val = v->toPrimitive(exec, PreferString);

	return val;
}


};

