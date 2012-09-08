/*
 * UDPSocketError.h
 *
 *  Created on: Dec 7, 2011
 *      Author: gar
 */

#ifndef UDPSOCKETERROR_H_
#define UDPSOCKETERROR_H_

#include "UDPSocketErrorBase.h"

namespace WebCore {

class UDPSocketError : public UDPSocketErrorBase
{
public:
	UDPSocketError() { }
    explicit UDPSocketError(int errorCode, const gchar* description)
        : UDPSocketErrorBase(errorCode, String(), String(description))
    {
      err_ = 0;
    }
    
    int getErr() { return err_;}

private:
	int err_;

};

} // namespace WebKit

#endif /* UDPSOCKETERROR_H_ */
