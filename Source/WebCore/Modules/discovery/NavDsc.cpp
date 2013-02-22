
#include "config.h"

#if ENABLE(DISCOVERY)

#include "Page.h"
#include "Chrome.h"
#include "Frame.h"
#include "Document.h"
#include "FrameLoadRequest.h"
#include "FrameLoaderClient.h"
#include "ResourceHandle.h"

#include "Modules/discovery/IDiscoveryAPI.h"
#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/ZCDevice.h"
#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/ZCDevice.h"

#include "Modules/discovery/UPnPSearch.h"
#include "Modules/discovery/ZeroConf.h"
#include "Modules/discovery/UPnPEvent.h"

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <pthread.h>
#include <vector>
#include <map>

#include "Modules/discovery/Nav.h"
#include "NavDsc.h"

#ifndef IMAX
#define IMAX(a,b) ((a) < (b)) ? (b):(a)
#endif

namespace WebCore {

FILE *NavDsc::HN_FD_; // Used for logging
NavDsc *NavDsc::instance = 0;

// Static
NavDsc *NavDsc::create(Frame * frame)
{
	if (!instance)
		instance = new NavDsc(frame);

	if (frame)
		instance->m_frame = frame;

	return instance;
}

NavDsc::NavDsc(Frame * frame) :
		  m_frame(frame)
{
	m_resetSet = false;
	m_main = new Mutex();
}

NavDsc::~NavDsc()
{
	if (UPnPSearch::getInstance())
		delete UPnPSearch::getInstance();

	delete m_main;
	instance = NULL;
}

void NavDsc::setServices(
		std::string strType,
		const char* type,
		std::map<std::string, UPnPDevice> devs,
		std::map<std::string, ZCDevice> zcdevs,
		ProtocolType protoType
		)
{

	//printf("setServices(%s)1\n",strType.c_str());
	RefPtr<NavServices> ns = NavServices::create(m_frame->document(), NavServices::CONNECTED);
	m_services[strType].push_back(ns);
	int lastIndex = IMAX(0, m_services[strType].size() -1);

	m_services[strType].at(lastIndex)->suspendIfNeeded();

	Vector<RefPtr<NavService> >* vDevs = new Vector<RefPtr<NavService> >();

	if (protoType == UPNP_PROTO)
	{
		std::map<std::string, UPnPDevice>::iterator it;
		for (it=devs.begin(); it!=devs.end(); it++)
		{
			UPnPDevice d((*it).second);
			RefPtr<NavService> srv = NavService::create(m_frame->document());
			srv->suspendIfNeeded();

			srv->setType(WTF::String(type));
			srv->setPType(NavService::UPNP_TYPE);
			srv->setUrl(WTF::String(d.descURL.c_str()));
			srv->setid(WTF::String((*it).first.c_str())); // UUID
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

			srv->setType(WTF::String(type));
			srv->setPType(NavService::ZCONF_TYPE);
			srv->setUrl(WTF::String(d.url.c_str()));
			srv->setid(WTF::String((*it).first.c_str())); // UUID
			srv->setName(WTF::String(d.friendlyName.c_str()));

			vDevs->append(srv);
		}
	}

	// Write devices to service object
	m_services[strType].at(lastIndex)->setServices(vDevs);
	m_services[strType].at(lastIndex)->m_serviceType = strType;

	//printf("NavDsc::setServices() DONE. %d services total\n", (int)vDevs->size());
}

std::vector<NavServices*> NavDsc::getNavServices(std::string type, bool isUp) {

	std::vector<RefPtr<NavServices> > ns;
	if (type == "all")
	{
		for (std::map<std::string, std::vector<RefPtr<NavServices> > >::iterator it = m_services.begin();
				it != m_services.end();
				it++) {

			std::vector<RefPtr<NavServices> > typeNS = it->second;
			for (int i=0; i<typeNS.size(); i++) {
				typeNS.at(i)->setOnline(isUp);
				ns.push_back(typeNS.at(i));
			}
		}
	} else
		ns = m_services[type];

	//printf("getNavServices() len: %d, type: %s\n", ns.size(), type.c_str());
	std::vector<NavServices*> pNS;

	for (int i=0; i<ns.size(); i++)
	{
		if (ns.at(i).get())
			pNS.push_back(ns.at(i).get());
	}
	//printf("getNavServices() found: %d devs\n", pNS.size());

	return pNS;
}

bool NavDsc::has(std::vector<RefPtr<NavServices> > srvs, std::string uuid)
{
	String nsUUID(uuid.c_str());

	for (int i=0; i<srvs.size(); i++) {
		Vector<RefPtr<NavService> > ns = srvs.at(i)->m_services;
		for (int k = 0; k < ns.size(); k++) {
			if (ns.at(k)->id() == nsUUID)
				return true;
		}
	}
	return false;
}

void NavDsc::onNetworkChanged(bool isUP)
{
	m_main->lock();
	m_curNetworkStatus.push(isUP);
	callOnMainThread(NavDsc::onNetworkChangedInternal,this);
	m_main->unlock();
}
void NavDsc::onNetworkChangedInternal(void *ptr)
{
	NavDsc *nd = (NavDsc*)ptr;

	if (nd->m_curNetworkStatus.size() == 0) {
		//printf("UPnPDevAddedInternal: No Types, so cannot add dev\n");
		return;
	}

	nd->m_main->lock();
	bool isUp = nd->m_curNetworkStatus.front();
	nd->m_curNetworkStatus.pop();
	nd->m_main->unlock();


	std::vector<NavServices*> srvs = nd->getNavServices(std::string("all"), isUp);
	if (srvs.size() == 0) {
		//printf("UPnPDevAddedInternal: No Devs, so cannot add dev\n");
		return;
	}

	for (int i=0; i<srvs.size(); i++) {
		if (isUp)
			srvs[i]->dispatchEvent(Event::create(eventNames().servicesonlineEvent, true, true));
		else
			srvs[i]->dispatchEvent(Event::create(eventNames().servicesofflineEvent, true, true));
	}
	//printf("UPnPDevAddedInternal(): Add dev. %s\n", type.c_str());
}


void NavDsc::foundUPnPDev(std::string type)
{
	m_main->lock();
	m_curType.push(type);
	callOnMainThread(NavDsc::UPnPDevAddedInternal,this);
	m_main->unlock();
}
void NavDsc::foundZCDev(std::string type)
{
	m_main->lock();
	m_curType.push(type);
	//printf("NavDsc::ZCDevAdded(%s)\n", type.c_str());
	callOnMainThread(NavDsc::ZCDevAddedInternal,this);
	m_main->unlock();
}

void NavDsc::serviceOffline(std::string type, UPnPDevice &dev)
{
	m_main->lock();
	m_curType.push(type);
	m_devs.push(dev);
	callOnMainThread(NavDsc::serviceOfflineInternal,this);
	m_main->unlock();
}
void NavDsc::serviceOfflineInternal(void *ptr)
{
	NavDsc *nd = (NavDsc*)ptr;

	if (nd->m_devs.size() == 0) {
		//printf("UPnPDevAddedInternal: No Types, so cannot add dev\n");
		return;
	}

	nd->m_main->lock();
	UPnPDevice dev(nd->m_devs.front());
	std::string type(nd->m_curType.front());
	nd->m_devs.pop();
	nd->m_curType.pop();
	nd->m_main->unlock();


	std::vector<NavServices*> srvs = nd->getNavServices(type, dev.online);
	if (srvs.size() == 0) {
		//printf("UPnPDevAddedInternal: No Devs, so cannot add dev\n");
		return;
	}

	for (int i=0; i<srvs.size(); i++) {
		for (int k=0; k<srvs[i]->m_services.size(); k++)
		{
			if (srvs[i]->m_services[k]->m_id.contains(String(dev.uuid.c_str()),true))
				srvs[i]->m_services[k]->dispatchEvent(Event::create(eventNames().serviceofflineEvent, true, true));
		}
	}
	//printf("UPnPDevAddedInternal(): Add dev. %s\n", type.c_str());
}
void NavDsc::serviceOnline(std::string type, UPnPDevice &dev)
{
	m_main->lock();
	m_curType.push(type);
	m_devs.push(dev);
	callOnMainThread(NavDsc::serviceOnlineInternal,this);
	m_main->unlock();
}
void NavDsc::serviceOnlineInternal(void *ptr)
{
	NavDsc *nd = (NavDsc*)ptr;

	if (nd->m_devs.size() == 0) {
		//printf("UPnPDevAddedInternal: No Types, so cannot add dev\n");
		return;
	}

	nd->m_main->lock();
	UPnPDevice dev(nd->m_devs.front());
	std::string type(nd->m_curType.front());
	nd->m_devs.pop();
	nd->m_curType.pop();
	nd->m_main->unlock();


	std::vector<NavServices*> srvs = nd->getNavServices(type, dev.online);
	if (srvs.size() == 0) {
		//printf("UPnPDevAddedInternal: No Devs, so cannot add dev\n");
		return;
	}

	for (int i=0; i<srvs.size(); i++) {
		for (int k=0; k<srvs[i]->m_services.size(); k++)
		{
			if (srvs[i]->m_services[k]->m_id.contains(String(dev.uuid.c_str()),true))
				srvs[i]->m_services[k]->dispatchEvent(Event::create(eventNames().serviceonlineEvent, true, true));
		}
	}
	//printf("UPnPDevAddedInternal(): Add dev. %s\n", type.c_str());
}

void NavDsc::lostUPnPDev(std::string type)
{
	m_main->lock();
	m_curType.push(type);
	callOnMainThread(NavDsc::UPnPDevDroppedInternal,this);
	m_main->unlock();
}
void NavDsc::lostZCDev(std::string type)
{
	m_main->lock();
	m_curType.push(type);
	callOnMainThread(NavDsc::ZCDevDroppedInternal,this);
	m_main->unlock();
}

void NavDsc::UPnPDevAddedInternal(void *ptr)
{
	NavDsc *nd = (NavDsc*)ptr;

	if (nd->m_curType.size() == 0) {
		//printf("UPnPDevAddedInternal: No Types, so cannot add dev\n");
		return;
	}

	nd->m_main->lock();
	std::string type(nd->m_curType.front());
	nd->m_curType.pop();
	nd->m_main->unlock();


	std::vector<NavServices*> srvs = nd->getNavServices(type);
	if (srvs.size() == 0) {
		//printf("UPnPDevAddedInternal: No Devs, so cannot add dev\n");
		return;
	}

	for (int i=0; i<srvs.size(); i++) {
		srvs[i]->dispatchEvent(Event::create(eventNames().devaddedEvent, true, true));
	}
	//printf("UPnPDevAddedInternal(): Add dev. %s\n", type.c_str());
}

void NavDsc::ZCDevAddedInternal(void *ptr)
{
	UPnPDevAddedInternal(ptr);
}

void NavDsc::UPnPDevDroppedInternal(void *ptr)
{
	NavDsc *nv = (NavDsc*)ptr;
	nv->m_main->lock();
	std::string type(nv->m_curType.front());
	nv->m_curType.pop();
	nv->m_main->unlock();

	std::vector<NavServices*> srvs = nv->getNavServices(type);
	for (int i=0; i<srvs.size(); i++) {
		srvs[i]->dispatchEvent(Event::create(eventNames().devdroppedEvent, false, false));
	}
}

void NavDsc::ZCDevDroppedInternal(void *ptr)
{
	UPnPDevDroppedInternal(ptr);
}


void NavDsc::sendEvent(std::string uuid, std::string stype, std::string body)
{
	std::string name = "";
	UPnPSearch::getInstance()->getUPnPFriendlyName(uuid, stype, name);

	//printf("NavDsc::sendEvent(%s)\n",uuid.c_str());
	RefPtr<NavEvent> evnt = NavEvent::create();

	evnt->setPropertyset(WTF::String(body.c_str()));
	evnt->setUuid(WTF::String(uuid.c_str()));
	evnt->setServiceType(WTF::String(stype.c_str()));
	evnt->setFriendlyName(WTF::String(name.c_str()));

	m_main->lock();
	m_event.push(evnt);
	m_curType.push(stype);
	callOnMainThread(NavDsc::sendEventInternal,this);
	m_main->unlock();
}


void NavDsc::sendEventInternal(void *ptr)
{
	NavDsc *nv = (NavDsc*)ptr;

	nv->m_main->lock();
	std::string type(nv->m_curType.front());
	nv->m_curType.pop();

	RefPtr<NavEvent> evnt = nv->m_event.front();
	nv->m_event.pop();
	nv->m_main->unlock();

	std::vector<NavServices*> srvs = nv->getNavServices(type);

	for (int i=0; i<srvs.size(); i++) {
		NavService* srv = srvs[i]->find(std::string(evnt->uuid().ascii().data()));

		if (srv) {
			//printf("NavDsc::sendEventInternal(%s) SENDING... Name: %s\n",type.c_str(), srv->name().ascii().data());
			struct UPnPEventInit init;
			init.friendlyName = evnt->friendlyName();
			init.propertyset = evnt->propertyset();
			init.serviceType = evnt->serviceType();
			init.uuid = evnt->uuid();
			srv->dispatchEvent(UPnPEvent::create(eventNames().upnpEvent, init));
		}
		else
			printf("NavDsc::sendEventInternal() srv == NULL !!!!!!\n");
	}

	evnt.release();
}

void NavDsc::startUPnPInternalDiscovery(const char *type, IDiscoveryAPI *api)
{
    //printf("startUPnPInternalDiscovery(%s).\n", type);

	std::map<std::string, UPnPDevice> empty;
	std::map<std::string, UPnPDevice> devs = UPnPSearch::discoverInternalDevs(type, api);

	if (m_resetSet) {
		UPnPSearch::getInstance()->reset();
		m_services.clear();
		devs.clear();
		m_resetSet = false;
	}
	for (std::map<std::string, UPnPDevice>::iterator it = devs.begin(); it != devs.end(); it++)
	{
		UPnPDevice d = (*it).second;
		UPnPSearch::getInstance()->eventServer(type, d.eventURL, d.uuid, d.host, d.port);
		devs[(*it).first] = d;
	}

	std::string strType(type);
	std::map<std::string, ZCDevice> zcdevs;

    return;
}

std::map<std::string, UPnPDevice> NavDsc::startUPnPDiscovery(
		const char *type,
		PassRefPtr<NavServiceOkCB> successcb,
		PassRefPtr<NavServiceErrorCB> errorcb)
{
	std::map<std::string, UPnPDevice> empty;
	std::map<std::string, UPnPDevice> devs = UPnPSearch::discoverDevs(type, this);

	if (!m_frame)
		return empty;

	Page* page = m_frame->page();
	if (!page)
		return empty;
	
	if (m_resetSet) {
		UPnPSearch::getInstance()->reset();
		m_services.clear();
		devs.clear();
		m_resetSet = false;
	}
	for (std::map<std::string, UPnPDevice>::iterator it = devs.begin(); it != devs.end(); it++)
	{
		UPnPDevice d = (*it).second;
		UPnPSearch::getInstance()->eventServer(type, d.eventURL, d.uuid, d.host, d.port);
		devs[(*it).first] = d;
	}


	std::string strType(type);
	std::map<std::string, ZCDevice> zcdevs;
	setServices(strType, type, devs, zcdevs, UPNP_PROTO);

	if (!UPnPSearch::getInstance()->m_udpSocket || !UPnPSearch::getInstance()->networkIsUp()) {
		//printf("NavDsc::startUPnPDiscovery() NETWORK_ERR\n");
		RefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::NETWORK_ERR);
		errorcb->handleEvent(err.get());
		return devs;
	}

	std::vector<NavServices *> srvs = getNavServices(type);
	successcb->handleEvent(srvs.at(srvs.size()-1));

	return devs;

}

std::map<std::string, ZCDevice> NavDsc::startZeroConfDiscovery(
		const char *type,
		PassRefPtr<NavServiceOkCB> successcb,
		PassRefPtr<NavServiceErrorCB> errorcb)
{
	std::map<std::string, ZCDevice> empty;
	std::map<std::string, ZCDevice> zcdevs = ZeroConf::discoverDevs(type, this);

	// We have devices to look at
	if (!m_frame)
		return empty;

	Page* page = m_frame->page();
	if (!page)
		return empty;

	std::string strType(type);
	std::map<std::string, UPnPDevice> devs;
	if (m_resetSet) {
		ZeroConf::getInstance()->reset();
		m_services.clear();
		zcdevs.clear();
		m_resetSet = false;
	}
	setServices(strType, type, devs, zcdevs, ZC_PROTO);

	if (!ZeroConf::getInstance()->m_udpSocket) {
		RefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::NETWORK_ERR);
		errorcb->handleEvent(err.get());
		return zcdevs;
	}

	std::vector<NavServices *> srvs = getNavServices(type);
	successcb->handleEvent(srvs.at(srvs.size()-1));

	return zcdevs;

}

void NavDsc::onUPnPError(int error)
{
	if (m_UPnPnav.size())
	{
		//printf("NavDsc::onUPnPError() error: %d\n", error);
		m_UPnPnav.begin()->second->onError(error);
	}
}

void NavDsc::onZCError(int error)
{
	if (m_ZCnav.size())
	{
		//printf("NavDsc::onZCError() error: %d\n", error);
		m_ZCnav.begin()->second->onError(error);
	}
}


} // namespace WebCore

#endif // ENABLE_DISCOVERY
