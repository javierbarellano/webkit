/*
 ----------------------------------
 Home networking
 ----------------------------------
 */

#ifndef NAV_H_
#define NAV_H_

#include <string>
#include <map>
#include <vector>

#include "Modules/discovery/IDiscoveryAPI.h"
#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/ZCDevice.h"
#include "Modules/discovery/NavDsc.h"
#include "Modules/discovery/NavServices.h"
#include "Modules/discovery/NavEvent.h"

#include "DOMWindowProperty.h"
#include "Supplementable.h"

#include <wtf/Forward.h>
#include <wtf/HashMap.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>


namespace WebCore {

class DOMMimeTypeArray;
class DOMPluginArray;
class Frame;
class PointerLock;
class PluginData;

// Home Networking
class NavDscCB;
class NavServiceErrorCB;
class NavServiceOkCB;
class NavEventCB;
class Frame;
class Navigator;

typedef int ExceptionCode;

class Nav : public Supplement<Navigator>, public DOMWindowProperty, public IDiscoveryAPI {
public:
    virtual ~Nav();

    static Nav* from(Navigator*);

public:
	enum ProtocolType
	{
		BAD_PROTO = 0,
		UPNP_PROTO = 1,
		ZC_PROTO =2
	};
    static void getNetworkServices(
			Navigator* n,
			const String& type,
			PassRefPtr<NavServiceOkCB> serviceCB,
			PassRefPtr<NavServiceErrorCB> errorCB );


    virtual void onError(int error){}

    virtual void UPnPDevAdded(std::string type);
    static void UPnPDevAddedInternal(void *ptr);

    virtual void ZCDevAdded(std::string type);
    static void ZCDevAddedInternal(void *ptr);

    virtual void UPnPDevDropped(std::string type);
    static void UPnPDevDroppedInternal(void *ptr);

    virtual void ZCDevDropped(std::string type);
    static void ZCDevDroppedInternal(void *ptr);

    virtual void sendEvent(std::string uuid, std::string stype, std::string body);
    static void sendEventInternal(void *ptr);

    virtual void serverListUpdate(std::string type, std::map<std::string, UPnPDevice> *devs){}

    // Events
	std::string m_curType;
	int m_eventType;
	RefPtr<NavEvent> m_event;

	enum EventType {
		ADDED_UPNP_EVENT,
		DROPPED_UPNP_EVENT,
		SENDEVENT_UPNP_EVENT,
		ADDED_ZC_EVENT,
		DROPPED_ZC_EVENT,
	};

private:
    Nav(Frame*);

    void setServices(
    		std::string strType,
    		const char* type,
    		std::map<std::string, UPnPDevice> devs,
    		std::map<std::string, ZCDevice> zcdevs,
    		ProtocolType protoType);

    ProtocolType readRemoveTypePrefix(WTF::CString &cType, char *sType);
    NavServices* getNavServices(std::string type);

    static Mutex *m_main;

    // Home Networking Data

    std::map<std::string, RefPtr<NavServices> > m_services;

    //mutable RefPtr<NavServices> m_srvcs;
    std::vector<RefPtr<NavService> > m_srv;

};

}

#endif /* NAV_H_ */
