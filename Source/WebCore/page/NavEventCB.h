/*
 * NavEventCallback.h
 *
 *  Created on: Mar 9, 2012
 *      Author: gar
 */

#ifndef NAVIGATORNETWORKEVENTCALLBACK_H_
#define NAVIGATORNETWORKEVENTCALLBACK_H_

//#include "CallbackTask.h"
#include <wtf/RefCounted.h>

namespace WebCore {

class NavEvent;

class NavEventCB :
	public RefCounted<NavEventCB>
//    public CallbackTask1<NavEventCB, NavEvent>::Scheduler
{
public:
    virtual ~NavEventCB() { }
    virtual bool handleEvent(NavEvent* event) = 0;

protected:

};

} // namespace WebCore

#endif /* NAVIGATORNETWORKEVENTCALLBACK_H_ */
