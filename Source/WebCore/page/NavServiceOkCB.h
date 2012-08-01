/*
 * NavServiceOkCB.h
 *
 *  Created on: Oct 7, 2011
 *      Author: gar
 */

#ifndef NAVIGATORNETWORKSERVICESUCCESSCALLBACK_H_
#define NAVIGATORNETWORKSERVICESUCCESSCALLBACK_H_

#include <wtf/RefCounted.h>

namespace WebCore {

class NavServices;

class NavServiceOkCB : public RefCounted<NavServiceOkCB>
{
public:
    virtual ~NavServiceOkCB() { }
    virtual bool handleEvent(NavServices* services) = 0;

protected:

};

} // namespace WebCore



#endif /* NAVIGATORNETWORKSERVICESUCCESSCALLBACK_H_ */
