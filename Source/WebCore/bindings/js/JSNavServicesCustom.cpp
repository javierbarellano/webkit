/*
 * JSNavServicesCustom.cpp
 *
 *  Created on: Jan 10, 2012
 *      Author: gar
 */
#include "config.h"

#include <string>

#include "JSNavServices.h"
#include "JSNavService.h"

#include "UString.h"
#include "JSValue.h"
#include "CallFrame.h"


namespace WebCore {

JSC::JSValue JSNavServices::item(JSC::ExecState* exec)
{
    JSC::UString sArg = exec->argument(0).toString(exec)->value(exec);
    uint32_t index = atoi(sArg.ascii().data());

	NavService *srv = impl()->item(index);

	return toJS(exec, globalObject(), srv);
}

void JSNavService::visitChildren(JSCell*, JSC::SlotVisitor&){}


};

