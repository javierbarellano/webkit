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
#include <wtf/Threading.h>
#include <wtf/MainThread.h>

#include "Nav.h"

namespace WebCore {

Mutex *Nav::m_main = NULL;

Nav::Nav(Frame* frame)
: DOMWindowProperty(frame) {
	m_errorCBIsSet = false;
	m_UPnPserviceAddedCBIsSet = false;
	m_UPnPserviceRemovedCBIsSet = false;
	m_ZCserviceAddedCBIsSet = false;
	m_ZCserviceRemovedCBIsSet = false;

	if (!m_main)
		m_main = new Mutex();
}

Nav::~Nav() {
	delete m_main;
	m_main = NULL;
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

void Nav::getNetworkServices(
		Navigator* n,
		const String& type,
		PassRefPtr<NavServiceOkCB> successcb,
		PassRefPtr<NavServiceErrorCB> errorcb)
{
	printf("Nav::getNetworkServices() Start\n");

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

	if (protoType != UPNP_PROTO && protoType != ZC_PROTO) {
		if (errorcb)
		{
			RefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::UNKNOWN_TYPE);
			errorcb->handleEvent(err.get());
			return;
		}
	}

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
		RefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::UNKNOWN_TYPE);
		errorcb->handleEvent(err.get());
		return;
	}
	else
		return; // Error found and no error callback

	nv->setServices(strType, sType, devs, zcdevs, protoType);

	successcb->handleEvent(nv->m_srvcs.get());

	printf("Nav::getNetworkServices() Done.\n");
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

	m_main->lock();
	callOnMainThread(Nav::sendEventInternal,this);
	m_main->unlock();

}

void Nav::sendEventInternal(void *ptr)
{
	Nav *nv = (Nav*)ptr;
	NavServices* srvs = nv->m_srvcs.get();

	for (int i=0; i<srvs->length(); i++) {
		if (srvs->item(i)->uuid() == nv->m_event->uuid()) {
			NavService* srv = srvs->item(i);
			srv->dispatchEvent(Event::create(eventNames().upnpeventEvent, true, true));
		}
	}
}

void Nav::UPnPDevAdded(std::string type)
{
	m_main->lock();
	m_curType = type;
	printf("Nav::UPnPDevAdded(%s)\n", type.c_str());
	callOnMainThread(Nav::UPnPDevAddedInternal,this);
	m_main->unlock();

}
void Nav::UPnPDevAddedInternal(void *ptr)
{
	Nav *nv = (Nav*)ptr;
	//m_frame->existingDOMWindow()->dispatchEvent(Event::create(eventNames().focusEvent, true, true));

	NavServices* srvs = nv->m_srvcs.get();
	printf("UPnPDevAddedInternal(): sending event.\n");

	bool prevented = srvs->dispatchEvent(Event::create(eventNames().devaddedEvent, true, true));

	printf("UPnPDevAddedInternal(): sent event. ref=%d prevented: %s\n", nv->m_srvcs->refCount(), prevented ? "true":"false");


}

void Nav::ZCDevAdded(std::string type)
{
	m_main->lock();
	m_curType = type;
	printf("Nav::ZCDevAdded(%s)\n", type.c_str());
	callOnMainThread(Nav::ZCDevAddedInternal,this);
	m_main->unlock();

}

void Nav::ZCDevAddedInternal(void *ptr)
{
	UPnPDevAddedInternal(ptr);
}

void Nav::UPnPDevDropped(std::string type)
{
	m_main->lock();
	m_curType = type;
	callOnMainThread(Nav::UPnPDevDroppedInternal,this);
	m_main->unlock();
}
void Nav::UPnPDevDroppedInternal(void *ptr)
{
	Nav *nv = (Nav*)ptr;
	PassRefPtr<NavServices> srvs = nv->m_srvcs;
	srvs->dispatchEvent(Event::create(eventNames().devdroppedEvent, false, false));
}

void Nav::ZCDevDropped(std::string type)
{
	m_main->lock();
	m_curType = type;
	callOnMainThread(Nav::ZCDevDroppedInternal,this);
	m_main->unlock();
}
void Nav::ZCDevDroppedInternal(void *ptr)
{
	UPnPDevDroppedInternal(ptr);
}


Nav::ProtocolType Nav::readRemoveTypePrefix(WTF::CString &cType, char *sType)
{
	ProtocolType protoType = BAD_PROTO;
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

void Nav::setServices(
		std::string strType,
		const char* type,
		std::map<std::string, UPnPDevice> devs,
		std::map<std::string, ZCDevice> zcdevs,
		ProtocolType protoType
		)
{

	if (m_srvcs)
		m_srvcs.release();

	m_srvcs = NavServices::create(m_frame->document(), NavServices::CONNECTED);
	m_srvcs->suspendIfNeeded();

	Vector<RefPtr<NavService> >* vDevs = new Vector<RefPtr<NavService> >();

	if (protoType == UPNP_PROTO)
	{
		std::map<std::string, UPnPDevice>::iterator it;
		for (it=devs.begin(); it!=devs.end(); it++)
		{
			UPnPDevice d((*it).second);
			RefPtr<NavService> srv = NavService::create(m_frame->document());
			srv->suspendIfNeeded();

			srv->setServiceType(WTF::String(type));
			srv->setPType(NavService::UPNP_TYPE);
			srv->setUrl(WTF::String(d.descURL.c_str()));
			srv->setuuid(WTF::String((*it).first.c_str())); // UUID
			srv->setName(WTF::String(d.friendlyName.c_str()));

			vDevs->append(srv);
		}

	}
	else if (protoType == ZC_PROTO)
	{
		std::map<std::string, ZCDevice>::iterator it;
		for (it=zcdevs.begin(); it!=zcdevs.end(); it++)
		{
			ZCDevice d((*it).second);
			RefPtr<NavService> srv = NavService::create(m_frame->document());
			srv->suspendIfNeeded();

			srv->setServiceType(WTF::String(type));
			srv->setPType(NavService::ZCONF_TYPE);
			srv->setUrl(WTF::String(d.url.c_str()));
			srv->setuuid(WTF::String((*it).first.c_str())); // UUID
			srv->setName(WTF::String(d.friendlyName.c_str()));

			vDevs->append(srv);
		}
	}

	// Write devices to service object
	m_srvcs->setServices(vDevs);

	printf("Nav::setServices() DONE. %d services total\n", (int)vDevs->size());
}
};

#endif // DISCOVERY

