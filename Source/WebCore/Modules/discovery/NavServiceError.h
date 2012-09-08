/*
 * NavServiceError.h
 *
 *  Created on: Oct 7, 2011
 *      Author: gar
 */

#ifndef NAVIGATORNETWORKSERVICEERROR_H_
#define NAVIGATORNETWORKSERVICEERROR_H_
#include <wtf/RefCounted.h>
#include <wtf/PassRefPtr.h>

namespace WebCore {

class NavServiceError : public RefCounted<NavServiceError> {
public:
    // Should be kept in sync with the values in the idl file.
    enum ErrorCode {
        PERMISSION_DENIED_ERR = 1,
        UNKNOWN_TYPE = 2,			// Type must start with "upnp:" or "zeroconf:"
        NETWORK_ERR = 3
    };

    static PassRefPtr<NavServiceError> create(ErrorCode code)
    {
        return adoptRef(new NavServiceError(code));
    }

    virtual ~NavServiceError() { }

    ErrorCode code() const { return m_code; }

private:
    NavServiceError(ErrorCode code) : m_code(code) { }

    ErrorCode m_code;
};

} // namespace WebCore


#endif /* NAVIGATORNETWORKSERVICEERROR_H_ */
