/*
 * Nav.cpp
 *
 */

#include "config.h"

#if ENABLE(DISCOVERY)

#include "Frame.h"
#include "Navigator.h"
#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/UPnPSearch.h"
#include "Modules/discovery/NavDsc.h"

#include "NavServiceError.h"
#include "NavServiceErrorCB.h"
#include "NavServiceOkCB.h"
#include "NavDscCB.h"
#include "NavEvent.h"
#include "NavEventCB.h"

#include "Nav.h"

namespace WebCore {

void eventHandlerThread(void *context)
{
	usleep(1000*100);
	Nav* nav = (Nav*)context;

	//if (true) return;

	switch (nav->m_eventType) {
	case Nav::ADDED_UPNP_EVENT:
		nav->UPnPDevAddedInternal(nav->m_curType);
		break;

	case Nav::DROPPED_UPNP_EVENT:
		nav->UPnPDevDroppedInternal(nav->m_curType);
		break;

	case Nav::ADDED_ZC_EVENT:
		nav->ZCDevAddedInternal(nav->m_curType);
		break;

	case Nav::DROPPED_ZC_EVENT:
		nav->ZCDevDroppedInternal(nav->m_curType);
		break;
	}
}

Nav::Nav(Frame* frame)
: DOMWindowProperty(frame) {
	m_errorCBIsSet = false;
	m_UPnPserviceAddedCBIsSet = false;
	m_UPnPserviceRemovedCBIsSet = false;
	m_ZCserviceAddedCBIsSet = false;
	m_ZCserviceRemovedCBIsSet = false;
}

Nav::~Nav() {
}

// Static
Nav* Nav::from(Navigator* navigator)
{
    DEFINE_STATIC_LOCAL(AtomicString, name, ("Nav"));
    Nav* supplement = static_cast<Nav*>(Supplement<Navigator>::from(navigator, name));
    if (!supplement) {
        supplement = new Nav(navigator->frame());
        provideTo(navigator, name, adoptPtr(supplement));
    }
    return supplement;
}

// ---------------------------------------------------
// Home Networking
// ---------------------------------------------------

void Nav::getNetworkServices(
		Navigator* n,
		const String& type,
		PassRefPtr<NavServiceOkCB> successcb,
		PassRefPtr<NavServiceErrorCB> errorcb)
{
	printf("getNetworkServices()\n");

	Nav *nv = from(n);
	IDiscoveryAPI *disAPI = nv;

	if (!nv->m_frame)
		return;

	WTF::CString cType(type.ascii());
	NavDsc *nd = NavDsc::create(nv->m_frame);

	// Get service type
	char sType[1000];
	ProtocolType protoType = nv->readRemoveTypePrefix(cType, sType);
	std::string strType(sType);


	if (errorcb)
	{
		nv->m_errorCB = errorcb;
		nv->m_errorCBIsSet = true;
		if (protoType == UPNP_PROTO)
			nd->onUPnPDiscovery(sType, disAPI);
		else if (protoType == ZC_PROTO)
			nd->onZCDiscovery(sType, disAPI);
	}
	else
		nv->m_errorCBIsSet = false;

	std::map<std::string, UPnPDevice> devs;
	std::map<std::string, ZCDevice> zcdevs;

	if (protoType == UPNP_PROTO)
		devs = nd->startUPnPDiscovery(sType);
	else if (protoType == ZC_PROTO)
		zcdevs = nd->startZeroConfDiscovery(sType);
	else if (errorcb)
	{
		PassRefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::UNKNOWN_TYPE);
		errorcb->handleEvent(err.get());
		return;
	}
	else
		return; // Error found and no error callback

	RefPtr<NavServices> services = NULL;
	if (nv->m_services.find(strType) == nv->m_services.end()) {
		services = nv->setServices(sType, devs, zcdevs, protoType, NULL);
		services->turnOffVerifier();
		services->suspendIfNeeded();
	} else {
		services = nv->setServices(sType, devs, zcdevs, protoType, nv->m_services[strType]);
	}

	nv->m_services[strType] = services;
	successcb->handleEvent(nv->m_services[strType].get());

	//PassRefPtr<NavServices> services = nv->setServices(sType, devs, zcdevs, protoType);
	//successcb->handleEvent(services.get());

}


void Nav::onError(int error)
{
	if (m_errorCBIsSet)
	{
		// Applies to getNetworkServices() only
		PassRefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::NETWORK_ERR);
		m_errorCB->handleEvent(err.get());
	}
}

void Nav::sendEvent(std::string uuid, std::string stype, std::string body)
{
	std::string name = "";
	UPnPSearch::getInstance()->getUPnPFriendlyName(uuid, stype, name);

	m_event->setPropertyset(WTF::String(body.c_str()));
	m_event->setUuid(WTF::String(uuid.c_str()));
	m_event->setServiceType(WTF::String(stype.c_str()));
	m_event->setFriendlyName(WTF::String(name.c_str()));

	m_curType = stype;
	m_eventType = SENDEVENT_UPNP_EVENT;
	WTF::createThread(eventHandlerThread, this, "EventHandlerThread");

}

void Nav::sendEventInternal()
{
	if (m_services.find(m_curType) == m_services.end()) {
		printf("sendEventInternal() - UPnP Device NOT FOUND!!!\n");
		return;
	}

	NavServices* srvs = m_services[m_curType].get();

	for (int i=0; i<srvs->length(); i++) {
		if (srvs->m_services.at(i)->uuid() == m_event->uuid()) {
			NavService* srv = srvs->m_services.at(i);
			srv->dispatchEvent(Event::create(eventNames().upnpeventEvent, true, true));
		}
	}
}

void Nav::UPnPDevAdded(std::string type)
{
	m_curType = type;
	m_eventType = ADDED_UPNP_EVENT;
	WTF::createThread(eventHandlerThread, this, "EventHandlerThread");
}
void Nav::UPnPDevAddedInternal(std::string type)
{
	if (m_services.find(type) == m_services.end()) {
		printf("UPnPDevAddedInternal() - UPnP Device NOT FOUND!!!\n");
		return;
	}

	//m_frame->existingDOMWindow()->dispatchEvent(Event::create(eventNames().focusEvent, true, true));

	NavServices* srvs = m_services[type].get();
	printf("UPnPDevAddedInternal(): sending event.\n");

	srvs->dispatchEvent(Event::create(eventNames().devaddedEvent, true, true));

	printf("UPnPDevAddedInternal(): sent event. ref=%d\n", m_services[type]->refCount());


}

void Nav::ZCDevAdded(std::string type)
{
	m_curType = type;
	m_eventType = ADDED_ZC_EVENT;
	WTF::createThread(eventHandlerThread, this, "EventHandlerThread");
}

void Nav::ZCDevAddedInternal(std::string type)
{
	UPnPDevAddedInternal(type);
}

void Nav::UPnPDevDropped(std::string type)
{
	m_curType = type;
	m_eventType = DROPPED_UPNP_EVENT;
	WTF::createThread(eventHandlerThread, this, "EventHandlerThread");
}
void Nav::UPnPDevDroppedInternal(std::string type)
{
	if (m_services.find(type) == m_services.end())
		return;

	PassRefPtr<NavServices> srvs = m_services[type];
	srvs->dispatchEvent(Event::create(eventNames().devdroppedEvent, false, false));
}

void Nav::ZCDevDropped(std::string type)
{
	m_curType = type;
	m_eventType = DROPPED_ZC_EVENT;
	WTF::createThread(eventHandlerThread, this, "EventHandlerThread");
}
void Nav::ZCDevDroppedInternal(std::string type)
{
	UPnPDevDroppedInternal(type);
}


Nav::ProtocolType Nav::readRemoveTypePrefix(WTF::CString &cType, char *sType)
{
	ProtocolType protoType = UPNP_PROTO;
	if (!strncmp(cType.data(), "upnp:", 5))
	{
		strcpy(sType, &cType.data()[5]);
		protoType = UPNP_PROTO;
	}
	else if (!strncmp(cType.data(), "zeroconf:", 9))
	{
		strcpy(sType, &cType.data()[9]);
		protoType = ZC_PROTO;
	}

	return protoType;
}

PassRefPtr<NavServices> Nav::setServices(
		const char* type,
		std::map<std::string, UPnPDevice> devs,
		std::map<std::string, ZCDevice> zcdevs,
		ProtocolType protoType,
		PassRefPtr<NavServices> servicesParm
		)
{
	RefPtr<NavServices> services = NavServices::create(m_frame->document(), NavServices::CONNECTED);

	if (servicesParm) {
		services = servicesParm;
	}

	Vector<NavService*>* vDevs = new Vector<NavService*>();

	if (protoType == UPNP_PROTO)
	{
		std::map<std::string, UPnPDevice>::iterator it;
		for (it=devs.begin(); it!=devs.end(); it++)
		{
			if (services->find((*it).first)) { // UUID
				vDevs->append(services->find((*it).first));
				continue;
			}

			UPnPDevice d((*it).second);
			if (d.isOkToUse) {
				NavService* nns = new NavService(m_frame->document());
				nns->setServiceType(WTF::String(type));
				nns->setPType(NavService::UPNP_TYPE);
				nns->setUrl(WTF::String(d.descURL.c_str()));
				nns->setuuid(WTF::String((*it).first.c_str())); // UUID
				nns->setName(WTF::String(d.friendlyName.c_str()));
				vDevs->append(nns);
			}
		}

	}
	else if (protoType == ZC_PROTO)
	{
		std::map<std::string, ZCDevice>::iterator it;
		for (it=zcdevs.begin(); it!=zcdevs.end(); it++)
		{
			if (services->find((*it).first)) { // ID
				vDevs->append(services->find((*it).first));
				continue;
			}

			ZCDevice d((*it).second);
			if (d.isOkToUse) {
				NavService* nns = new NavService(m_frame->document());
				nns->setServiceType(WTF::String(type));
				nns->setPType(NavService::ZCONF_TYPE);
				nns->setUrl(WTF::String(d.url.c_str()));
				nns->setuuid(WTF::String((*it).first.c_str())); // UUID
				nns->setName(WTF::String(d.friendlyName.c_str()));
				vDevs->append(nns);
			}
		}
	}

	// Write devices to service object
	services->setServices(vDevs);

	return services;
}
// -------------------------------------------------------
// -------------------------------------------------------
};

#endif // DISCOVERY

