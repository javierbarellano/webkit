/*
 * NavEvent.h
 *
 *  Created on: Mar 9, 2012
 *      Author: gar
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
