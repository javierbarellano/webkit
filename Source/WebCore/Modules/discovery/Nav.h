/*
 ----------------------------------
 Home networking
 ----------------------------------
 */

#ifndef NAV_H_
#define NAV_H_

#include <string>
#include <map>

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
		UPNP_PROTO = 1,
		ZC_PROTO =2
	};
    static void getNetworkServices(
			Navigator* n,
			const String& type,
			PassRefPtr<NavServiceOkCB> serviceCB,
			PassRefPtr<NavServiceErrorCB> errorCB );


    virtual void onError(int error);

    virtual void UPnPDevAdded(std::string type);
    virtual void UPnPDevAddedInternal(std::string type);
    virtual void ZCDevAdded(std::string type);
    virtual void ZCDevAddedInternal(std::string type);

    virtual void UPnPDevDropped(std::string type);
    virtual void UPnPDevDroppedInternal(std::string type);
    virtual void ZCDevDropped(std::string type);
    virtual void ZCDevDroppedInternal(std::string type);

    virtual void sendEvent(std::string uuid, std::string stype, std::string body);
    virtual void sendEventInternal();

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

    PassRefPtr<NavServices> setServices(
    		const char* type,
    		std::map<std::string, UPnPDevice> devs,
    		std::map<std::string, ZCDevice> zcdevs,
    		ProtocolType protoType,
    		PassRefPtr<NavServices> servicesParm);

    ProtocolType readRemoveTypePrefix(WTF::CString &cType, char *sType);

    // Home Networking Data
    mutable RefPtr<NavServiceErrorCB> m_errorCB;
    bool m_errorCBIsSet;

    std::map<std::string, RefPtr<NavDscCB> > m_uacb;
    mutable RefPtr<NavDscCB> m_UPnPserviceAddedCB;
    bool m_UPnPserviceAddedCBIsSet;

    std::map<std::string, RefPtr<NavServiceOkCB> > m_urcb;
    mutable RefPtr<NavServiceOkCB> m_UPnPserviceRemovedCB;
    bool m_UPnPserviceRemovedCBIsSet;

    std::map<std::string, RefPtr<NavDscCB> > m_zacb;
    mutable RefPtr<NavDscCB> m_ZCserviceAddedCB;
    bool m_ZCserviceAddedCBIsSet;

    std::map<std::string, RefPtr<NavServiceOkCB> > m_zrcb;
    mutable RefPtr<NavServiceOkCB> m_ZCserviceRemovedCB;
    bool m_ZCserviceRemovedCBIsSet;

    std::map<std::string, RefPtr<NavEventCB> > m_ecb;
    mutable RefPtr<NavEventCB> m_eventCB;

    std::map<std::string, RefPtr<NavServices> > m_services;


};

}

#endif /* NAV_H_ */
