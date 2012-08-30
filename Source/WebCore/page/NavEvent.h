/*
 * NavEvent.h
 *
 *  Created on: Mar 9, 2012
 *      Author: gar
 */

#ifndef NAVIGATORNETWORKEVENT_H_
#define NAVIGATORNETWORKEVENT_H_


#include <wtf/text/WTFString.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class NavEvent : public RefCounted<NavEvent> {
public:
    static PassRefPtr<NavEvent> create()
    {
        return adoptRef(new NavEvent());
    }

    // Copy Constructor
    NavEvent(const NavEvent &that) :
    	RefCounted()
    {
        m_propertyset = that.m_propertyset;
        m_uuid = that.m_uuid;
        m_friendlyName = that.m_friendlyName;
        m_serviceType = that.m_serviceType;
    }

    NavEvent& operator= (const NavEvent &that)
    {
        m_propertyset = that.m_propertyset;
        m_uuid = that.m_uuid;
        m_friendlyName = that.m_friendlyName;
        m_serviceType = that.m_serviceType;

        return *this;
    }

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

#endif /* NAVIGATORNETWORKEVENT_H_ */
