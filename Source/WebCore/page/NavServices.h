/*
 * NavServices.h
 *
 *  Created on: Jan 10, 2012
 *      Author: gar
 */

#ifndef NAVIGATORNETWORKSERVICES_H_
#define NAVIGATORNETWORKSERVICES_H_

#include "Event.h"
#include <wtf/text/WTFString.h>
#include <wtf/RefCounted.h>
#include "NavService.h"


namespace WebCore {

class NavServices : public RefCounted<NavServices> {
public:
    // Should be kept in sync with the values in the idl file.
    enum ReadyState {
    	CONNECTED = 1,
    	DISCONNECTED = 2
    };


    static PassRefPtr<NavServices> create(ReadyState code)
    {
        return adoptRef(new NavServices(code));
    }

    // Copy Constructor
    NavServices(const NavServices &that);

    NavServices& operator= (const NavServices &that);

    virtual ~NavServices();

    int servicesAvailable() const {return m_services.size();}

    int length() const {return m_services.size();}

    NavService* item(unsigned short index) {return m_services.at(index);}

    void setServices(Vector<NavService*>* vDevs)
    {
    	m_services.clear();
    	for (int i=0; i < (int)vDevs->size(); i++)
    		m_services.append(vDevs->at(i));
    }

    void clearSrvs()
    {
    	for (int i=0; i<(int)m_services.size(); i++)
    		delete m_services.at(i);

    	m_services.clear();
    }

private:
    NavServices(ReadyState code) : m_code(code) {
    }

    Vector<NavService*> m_services;
    ReadyState m_code;

};

} // namespace WebCore

#endif /* NAVIGATORNETWORKSERVICES_H_ */
