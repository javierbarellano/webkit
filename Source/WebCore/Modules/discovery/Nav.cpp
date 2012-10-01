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
#include "Modules/discovery/UPnPEvent.h"

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

NavServices* Nav::getNavServices(std::string type) {
	return m_services[type].get();
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
		if (protoType == UPNP_PROTO)
			nd->onUPnPDiscovery(sType, disAPI);
		else if (protoType == ZC_PROTO)
			nd->onZCDiscovery(sType, disAPI);
	}

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

	successcb->handleEvent(nv->getNavServices(strType));

	printf("Nav::getNetworkServices() Done.\n");
}


void Nav::UPnPDevAdded(std::string type)
{
	m_main->lock();
	m_curType.push(type);
	callOnMainThread(Nav::UPnPDevAddedInternal,this);
	m_main->unlock();

}
void Nav::UPnPDevAddedInternal(void *ptr)
{
	Nav *nv = (Nav*)ptr;

	nv->m_main->lock();
	std::string type(nv->m_curType.front());
	nv->m_curType.pop();
	nv->m_main->unlock();


	NavServices* srvs = nv->getNavServices(type);
	srvs->dispatchEvent(Event::create(eventNames().devaddedEvent, true, true));
	printf("UPnPDevAddedInternal(): sent event. %s\n", type.c_str());
}

void Nav::ZCDevAdded(std::string type)
{
	m_main->lock();
	m_curType.push(type);
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
	m_curType.push(type);
	callOnMainThread(Nav::UPnPDevDroppedInternal,this);
	m_main->unlock();
}
void Nav::UPnPDevDroppedInternal(void *ptr)
{
	Nav *nv = (Nav*)ptr;
	nv->m_main->lock();
	std::string type(nv->m_curType.front());
	nv->m_curType.pop();
	nv->m_main->unlock();

	PassRefPtr<NavServices> srvs = nv->m_services[type];
	srvs->dispatchEvent(Event::create(eventNames().devdroppedEvent, false, false));
}

void Nav::ZCDevDropped(std::string type)
{
	m_main->lock();
	m_curType.push(type);
	callOnMainThread(Nav::ZCDevDroppedInternal,this);
	m_main->unlock();
}
void Nav::ZCDevDroppedInternal(void *ptr)
{
	UPnPDevDroppedInternal(ptr);
}


void Nav::sendEvent(std::string uuid, std::string stype, std::string body)
{
	std::string name = "";
	UPnPSearch::getInstance()->getUPnPFriendlyName(uuid, stype, name);

	//printf("Nav::sendEvent(%s)\n",uuid.c_str());
	RefPtr<NavEvent> evnt = NavEvent::create();

	evnt->setPropertyset(WTF::String(body.c_str()));
	evnt->setUuid(WTF::String(uuid.c_str()));
	evnt->setServiceType(WTF::String(stype.c_str()));
	evnt->setFriendlyName(WTF::String(name.c_str()));

	m_main->lock();
	m_event.push(evnt);
	m_curType.push(stype);
	callOnMainThread(Nav::sendEventInternal,this);
	m_main->unlock();
}

void Nav::sendEventInternal(void *ptr)
{
	Nav *nv = (Nav*)ptr;

	nv->m_main->lock();
	std::string type(nv->m_curType.front());
	nv->m_curType.pop();

	RefPtr<NavEvent> evnt = nv->m_event.front();
	nv->m_event.pop();
	nv->m_main->unlock();

	NavServices* srvs = nv->getNavServices(type);
	NavService* srv = srvs->find(std::string(evnt->uuid().ascii().data()));

	if (srv) {
		//printf("Nav::sendEventInternal(%s) SENDING... Name: %s\n",type.c_str(), srv->name().ascii().data());
		struct UPnPEventInit init;
		init.friendlyName = evnt->friendlyName();
		init.propertyset = evnt->propertyset();
		init.serviceType = evnt->serviceType();
		init.uuid = evnt->uuid();
		srv->dispatchEvent(UPnPEvent::create(eventNames().upnpEvent, init));
	}
	else
		printf("Nav::sendEventInternal() srv == NULL !!!!!!\n");

	evnt.release();
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

	printf("setServices(%s)\n",strType.c_str());

	if (m_services[strType])
		m_services[strType].release();

	m_services[strType] = NavServices::create(m_frame->document(), NavServices::CONNECTED);
	m_services[strType]->suspendIfNeeded();

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
	m_services[strType]->setServices(vDevs);

	printf("Nav::setServices() DONE. %d services total\n", (int)vDevs->size());
}
};

#endif // DISCOVERY

