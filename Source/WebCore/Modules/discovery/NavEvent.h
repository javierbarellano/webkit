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

#ifndef NAVEVENT_H_
#define NAVEVENT_H_


#include <wtf/text/WTFString.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class NavEvent : public RefCounted<NavEvent> {
public:
    static PassRefPtr<NavEvent> create();

    // Copy Constructor
    NavEvent(const NavEvent &that);

    NavEvent& operator= (const NavEvent &that);

    virtual ~NavEvent(){}

    String propertyset() const {return m_propertyset;}

    String uuid() const {return m_uuid;}

    String friendlyName() const {return m_friendlyName;}

    String serviceType() const {return m_serviceType;}

    void setPropertyset(String props) {m_propertyset  = props;}
    void setUuid(String uuid)         {m_uuid         = uuid;}
    void setFriendlyName(String name) {m_friendlyName = name;}
    void setServiceType(String type)  {m_serviceType  = type;}



private:
    NavEvent(){ }

    String m_propertyset;
    String m_uuid;
    String m_friendlyName;
    String m_serviceType;
};

} // namespace WebCore

#endif /* NAVEVENT_H_ */
