/*
 * UDPSocketErrorBase.h
 *
 *  Created on: Apr 24, 2012
 *      Author: gar
 */

#ifndef UDPSOCKETERRORBASE_H_
#define UDPSOCKETERRORBASE_H_

#include "PlatformString.h"

namespace WebCore {

    class UDPSocketError;

    class UDPSocketErrorBase {
    public:
        // Makes a deep copy.  Useful for when you need to use a UDPSocketStreamError on another thread.
    	UDPSocketError copy() const;

        bool isNull() const { return m_isNull; }

        int errorCode() const { return m_errorCode; }
        const String& failingURL() const { return m_failingURL; }
        const String& localizedDescription() const { return m_localizedDescription; }

        static bool compare(const UDPSocketError&, const UDPSocketError&);

    protected:
        UDPSocketErrorBase()
            : m_errorCode(0)
            , m_isNull(true)
        {
        }

        explicit UDPSocketErrorBase(int errorCode)
            : m_errorCode(errorCode)
            , m_isNull(false)
        {
        }

        UDPSocketErrorBase(int errorCode, const String& failingURL, const String& localizedDescription)
            : m_errorCode(errorCode)
            , m_failingURL(failingURL)
            , m_localizedDescription(localizedDescription)
            , m_isNull(false)
        {
        }

        int m_errorCode;
        String m_failingURL;
        String m_localizedDescription;
        bool m_isNull;
    };

    inline bool operator==(const UDPSocketError& a, const UDPSocketError& b) { return UDPSocketErrorBase::compare(a, b); }
    inline bool operator!=(const UDPSocketError& a, const UDPSocketError& b) { return !(a == b); }

}  // namespace WebCore





#endif /* UDPSOCKETERRORBASE_H_ */
