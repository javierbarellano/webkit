/* Copyright (C) 2012, 2013 Cable Television Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UDPSocketErrorBase_h
#define UDPSocketErrorBase_h

#include <wtf/text/WTFString.h>

namespace WebCore {

class UDPSocketError;

class UDPSocketErrorBase {
public:
    // Makes a deep copy. Useful for when you need to use a UDPSocketStreamError on another thread.
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

} // namespace WebCore

#endif /* UDPSocketErrorBase_h */
