/*
 * NavServiceErrorCB.h
 *
 *  Created on: Oct 7, 2011
 *      Author: gar
 */

#ifndef NAVIGATORNETWORKSERVICEERRORCALLBACK_H_
#define NAVIGATORNETWORKSERVICEERRORCALLBACK_H_

//#include "CallbackTask.h"
#include "NavServiceError.h"
#include <wtf/RefCounted.h>

namespace WebCore {

class NavServiceError;

class NavServiceErrorCB :
	public RefCounted<NavServiceErrorCB>
//    public CallbackTask1<NavServiceErrorCB, NavServiceError>::Scheduler
{
public:
    virtual ~NavServiceErrorCB() { }
    virtual bool handleEvent(NavServiceError*) = 0;
};

} // namespace WebCore


#endif /* NAVIGATORNETWORKSERVICEERRORCALLBACK_H_ */
