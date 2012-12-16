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

long Nav::m_reqID = 0L;

Nav::Nav(Frame* frame)
: DOMWindowProperty(frame) {

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
	//printf("Nav::getNetworkServices() Start\n");

	Nav *nv = from(n);
	IDiscoveryAPI *disAPI = nv;

	//long id = m_reqID++;

	if (!nv->m_frame)
		return;

	WTF::CString cType(type.ascii());
	NavDsc *nd = NavDsc::create(nv->m_frame);

	// Get service type
	char sType[1000];
	bool reset;
	ProtocolType protoType = nv->readRemoveTypePrefix(cType, sType, &reset);
	std::string strType(sType);

	if (protoType != UPNP_PROTO && protoType != ZC_PROTO) {
		if (errorcb)
		{
			RefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::UNKNOWN_TYPE);
			errorcb->handleEvent(err.get());
			return;
		}
	}

	if (protoType == UPNP_PROTO)
		nd->onUPnPDiscovery(sType, disAPI);

	else if (protoType == ZC_PROTO)
		nd->onZCDiscovery(sType, disAPI);

	std::map<std::string, UPnPDevice> devs;
	std::map<std::string, ZCDevice> zcdevs;

	if (protoType == UPNP_PROTO) {
		if (reset) {
			nd->upnpReset();
		}
		devs = nd->startUPnPDiscovery(sType, successcb);
	} else if (protoType == ZC_PROTO) {
		if (reset) {
			nd->zcReset();
		}
		zcdevs = nd->startZeroConfDiscovery(sType, successcb);
	} else if (errorcb) {
		RefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::UNKNOWN_TYPE);
		errorcb->handleEvent(err.get());
		return;
	}
	else
		return; // Error found and no error callback

	//nv->setServices(id, strType, sType, devs, zcdevs, protoType);

	//successcb->handleEvent(nv->getNavServices(id));

	//printf("Nav::getNetworkServices() Done.\n");
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
}

void Nav::ZCDevDropped(std::string type)
{
}


void Nav::sendEvent(std::string uuid, std::string stype, std::string body)
{
}


Nav::ProtocolType Nav::readRemoveTypePrefix(WTF::CString &cType, char *sType, bool *reset)
{
	*reset = false;
	ProtocolType protoType = BAD_PROTO;
	if (!strncmp(cType.data(), "upnp:", 5))	{
		int start = 5;
		if (!strncmp(&cType.data()[5], "reset:", 6)) {
			start = 11;
			*reset = true;
		}
		strcpy(sType, &cType.data()[start]);
		protoType = UPNP_PROTO;
	}
	else if (!strncmp(cType.data(), "zeroconf:", 9)) {
		int start = 9;
		if (!strncmp(&cType.data()[9], "reset:", 6)) {
			start = 15;
			*reset = true;
		}
		strcpy(sType, &cType.data()[start]);
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

