/*
 * NavDscCallback.h
 *
 *  Created on: Mar 12, 2012
 *      Author: gar
 */

#ifndef NAVIGATORNETWORKDISCOVERYCALLBACK_H_
#define NAVIGATORNETWORKDISCOVERYCALLBACK_H_

#include <wtf/RefCounted.h>

namespace WebCore {

class NavService;

class NavDscCB :
	public RefCounted<NavDscCB>
{
public:
    virtual ~NavDscCB() { }
    virtual bool handleEvent(NavService* service) = 0;

protected:

};

} // namespace WebCore

#endif /* NAVIGATORNETWORKDISCOVERYCALLBACK_H_ */
