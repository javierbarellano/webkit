
#include "config.h"

#if ENABLE(DISCOVERY)

#include "Page.h"
#include "Chrome.h"
#include "Frame.h"
#include "Document.h"
#include "HTMLDivElement.h"
#include "FrameLoadRequest.h"
#include "FrameLoaderClient.h"
#include "ResourceHandle.h"
#include "CSSValueKeywords.h"
#include "HTMLElement.h"
#include "HTMLInputElement.h"
#include "HTMLBRElement.h"
#include "HTMLNames.h"
#include "HTMLFontElement.h"
#include "NodeList.h"
#include "ShadowRoot.h"


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

//#define LOGGING_NAV 1

#ifdef LOGGING_NAV
#define NAV_LOG(fmt,...) printf(fmt, ##__VA_ARGS__)
#else
#define NAV_LOG(fmt,...)
#endif
#define ERR_LOG(fmt,...) printf(fmt, ##__VA_ARGS__)

#include "NavDsc.h"

#ifndef IMAX
#define IMAX(a,b) ((a) < (b)) ? (b):(a)
#endif


namespace WebCore {

// ----------------------------

class PermissionInput FINAL : public HTMLInputElement {
public:
    void setNavDsc(NavDsc* nd) {m_navdsc = nd;}
    NavDsc* getNavDsc() {return m_navdsc;}

protected:
    explicit PermissionInput(const QualifiedName &, Document*);
    NavDsc* m_navdsc;
};

// ----------------------------

class PermissionButtonElement FINAL : public PermissionInput {
public:
    static PassRefPtr<PermissionButtonElement> create(Document*, NavDsc *);

    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }

private:
    explicit PermissionButtonElement(Document*);

    //virtual const AtomicString& shadowPseudoId() const OVERRIDE;
    virtual void defaultEventHandler(Event*) OVERRIDE;

};

// ----------------------------

class PermissionCheckBoxElement FINAL : public PermissionInput {
public:
    static PassRefPtr<PermissionCheckBoxElement> create(Document*, NavDsc *);

    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }
    void setEventData(EventData ed) { m_ed = ed; }

private:
    explicit PermissionCheckBoxElement(Document*);

    //virtual const AtomicString& shadowPseudoId() const OVERRIDE;
    virtual void defaultEventHandler(Event*) OVERRIDE;
    EventData m_ed;
};


// ----------------------------

PermissionInput::PermissionInput(const QualifiedName& tag, Document* doc) :
    HTMLInputElement(tag, doc, 0, false)
    , m_navdsc(0)
{

}

// ----------------------------

PassRefPtr<PermissionButtonElement> PermissionButtonElement::create(Document* doc, NavDsc *navdsc)
{
    RefPtr<PermissionButtonElement> button = adoptRef(new PermissionButtonElement(doc));
    button->setType(String("button"));
    button->setNavDsc(navdsc);

    return button.release();
}


PermissionButtonElement::PermissionButtonElement(Document* doc) :
        PermissionInput(HTMLNames::inputTag, doc)
{

}

void PermissionButtonElement::defaultEventHandler(Event* event)
{
    ExceptionCode ec;

    if (event->isMouseEvent() && event->type() == eventNames().mousedownEvent && m_navdsc) {
        RefPtr<NodeList> list = m_navdsc->m_frame->document()->documentElement()->getElementsByTagName(AtomicString("body"));
        Element* n = m_navdsc->m_frame->document()->getElementById(AtomicString("permissionDiv"));
        if (n)
            list->item(0)->removeChild(n->toNode(), ec);
        list.release();
    }
}

// ----------------------------

PassRefPtr<PermissionCheckBoxElement> PermissionCheckBoxElement::create(Document* doc, NavDsc *navdsc)
{
    RefPtr<PermissionCheckBoxElement> button = adoptRef(new PermissionCheckBoxElement(doc));
    button->setType(String("checkbox"));
    button->setNavDsc(navdsc);

    return button.release();
}

PermissionCheckBoxElement::PermissionCheckBoxElement(Document* doc) :
        PermissionInput(HTMLNames::inputTag, doc)
{

}

void PermissionCheckBoxElement::defaultEventHandler(Event* event)
{
    if (event->isMouseEvent() && event->type() == eventNames().mousedownEvent && m_navdsc) {
        const AtomicString id = this->getIdAttribute();
        AtomicString prefix("permissionchkbx");
        int pos = prefix.length();
        UChar c = id[pos++] - 48;
        if (pos < id.length())
            c = c*10 + id[pos] -48;
        std::vector< NavServices* > srvs = m_navdsc->getNavServices(m_ed.type, m_ed.online);

        // This default handler happens before the input handler, so the action hasn't been applied yet
        srvs[0]->m_devs[c]->setPermission(!this->checked());

        printf("CheckBox(%d) %s.\n", (int)c, this->checked() ? "UNChecked":"Checked");
    }
}

// ----------------------------

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
    EventListener(NativeEventListenerType)
    , m_frame(frame)
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

// --------

void NavDsc::handleEvent(ScriptExecutionContext*, Event* event)
{
    if (event->type() == eventNames().clickEvent && event->isMouseEvent()) {
        //MouseEvent* mouseEvent = static_cast<MouseEvent*>(event);
        printf("NavDsc::handleEvent() ok clicked!\n");
    }
}

bool NavDsc::operator==(const EventListener& listener)
{
    return false;
}

// --------

void NavDsc::addUPnPDev(std::string type, std::map<std::string, UPnPDevice> devs)
{
	NAV_LOG("addUPnPDev(%s) devs.size: %d\n",type.c_str(), devs.size());
	for (std::map<std::string, UPnPDevice>::iterator i=devs.begin(); i!= devs.end(); i++) {
		UPnPDevice ns = i->second;
		std::string uuid = i->first;
		bool found = false;

		if (!m_services[type]) {
			m_services[type] = NavServices::create(m_frame->document(), NavServices::CONNECTED);
			m_services[type]->suspendIfNeeded();
		}

		NavServices *nss = m_services[type].get();

		for(int m=0; nss && m<nss->length() && !found; m++) {
			if (strstr(nss->item(m)->id().ascii().data(),uuid.c_str()))
				found = true;
		}

		NAV_LOG("addUPnPDev() uuid: %s, found: %s\n", uuid.c_str(), (found ? "true":"false"));

		if (!found) {
			RefPtr<NavService> srv = NavService::create(m_frame->document());
			srv->suspendIfNeeded();

			srv->setType(WTF::String(type.c_str()));
			srv->setPType(NavService::UPNP_TYPE);
			srv->setUrl(WTF::String(ns.descURL.c_str()));
			srv->setid(WTF::String(uuid.c_str())); // UUID
			srv->setName(WTF::String(ns.friendlyName.c_str()));
			m_services[type]->append(srv);
			m_services[type]->m_serviceType = type;

			NAV_LOG("addUPnPDev() uuid: %s, added\n", uuid.c_str());
		}
	}
}

void NavDsc::addZCDev(std::string type, std::map<std::string, ZCDevice> devs)
{
	NAV_LOG("addZCDev(%s) devs.size: %d\n",type.c_str(), devs.size());
	for (std::map<std::string, ZCDevice>::iterator i=devs.begin(); i!= devs.end(); i++) {
		ZCDevice ns = i->second;
		std::string uuid = i->first;
		bool found = false;

		if (!m_services[type]) {
			m_services[type] = NavServices::create(m_frame->document(), NavServices::CONNECTED);
			m_services[type]->suspendIfNeeded();
		}

		NavServices *nss = m_services[type].get();

		for(int m=0; nss && m<nss->length() && !found; m++) {
			if (strstr(nss->item(m)->id().ascii().data(),uuid.c_str()))
				found = true;
		}

		NAV_LOG("addZCDev() uuid: %s, found: %s\n", uuid.c_str(), (found ? "true":"false"));

		if (!found) {
			RefPtr<NavService> srv = NavService::create(m_frame->document());
			srv->suspendIfNeeded();

			srv->setType(WTF::String(type.c_str()));
			srv->setPType(NavService::UPNP_TYPE);
			srv->setUrl(WTF::String(ns.url.c_str()));
			srv->setid(WTF::String(uuid.c_str())); // UUID
			srv->setName(WTF::String(ns.friendlyName.c_str()));
			m_services[type]->append(srv);
			m_services[type]->m_serviceType = type;

			NAV_LOG("addZCDev() uuid: %s, added\n", uuid.c_str());
		}
	}
}

void NavDsc::setServices(
		std::string strType,
		const char* type,
		std::map<std::string, UPnPDevice> devs,
		std::map<std::string, ZCDevice> zcdevs,
		ProtocolType protoType
		)
{

	NAV_LOG("setServices(%s)\n",strType.c_str());
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
	m_services[strType]->setServices(vDevs);
	m_services[strType]->m_serviceType = strType;

	//NAV_LOG("NavDsc::setServices() DONE. %d services total\n", (int)vDevs->size());
}

std::vector< NavServices* > NavDsc::getNavServices(std::string type, bool isUp) {

	std::vector<NavServices*> pNS;
	RefPtr<NavServices>  ns;
	if (type == "all")
	{
		for (std::map<std::string, RefPtr<NavServices> >::iterator it = m_services.begin();
				it != m_services.end();
				it++) {

			RefPtr<NavServices> typeNS = it->second;
			typeNS->setOnline(isUp);
			pNS.push_back(typeNS.get());
		}
	} else {
		if (!m_services[type]) {
			m_services[type] = NavServices::create(m_frame->document(), NavServices::CONNECTED);
			m_services[type]->suspendIfNeeded();
		}

		pNS.push_back(m_services[type].get());
	}

	NAV_LOG("getNavServices() number devs: %d for type: %s\n", pNS[0]->length(), type.c_str());

	return pNS;
}

bool NavDsc::has(std::vector<RefPtr<NavServices> > srvs, std::string uuid)
{
	String nsUUID(uuid.c_str());

	for (int i=0; i<srvs.size(); i++) {
		Vector<RefPtr<NavService> > ns = srvs.at(i)->m_devs;
		for (int k = 0; k < ns.size(); k++) {
			if (ns.at(k)->id() == nsUUID)
				return true;
		}
	}
	return false;
}

void NavDsc::zcServiceOffline(std::string type, ZCDevice &dev)
{
	NAV_LOG("NavDsc::serviceOffline() url: %s, name: %s\n",
			dev.url.c_str(), dev.friendlyName.c_str());
	m_main->lock();
	EventData ed = {ZC_PROTO, false, type, UPnPDevice(), dev, NULL};
	m_eventData.push(ed);
	callOnMainThread(NavDsc::serviceOfflineInternal,this);
	m_main->unlock();
}

void NavDsc::zcServiceOnline(std::string type, ZCDevice &dev)
{
	NAV_LOG("NavDsc::serviceOnline() url: %s, name: %s\n",
			dev.url.c_str(), dev.friendlyName.c_str());
	m_main->lock();
	EventData ed = {ZC_PROTO, true, type, UPnPDevice(), dev, NULL};
	m_eventData.push(ed);
	callOnMainThread(NavDsc::serviceOnlineInternal,this);
	m_main->unlock();
}

void NavDsc::serviceOffline(std::string type, UPnPDevice &dev)
{
	NAV_LOG("NavDsc::serviceOffline() url: %s, name: %s, id: %s\n",
			dev.descURL.c_str(), dev.friendlyName.c_str(), dev.uuid.c_str());
	m_main->lock();
	EventData ed = {UPNP_PROTO, false, type, dev, ZCDevice(), NULL};
	m_eventData.push(ed);
	callOnMainThread(NavDsc::serviceOfflineInternal,this);
	m_main->unlock();
}

void NavDsc::serviceOnline(std::string type, UPnPDevice &dev)
{
	NAV_LOG("NavDsc::serviceOnline() url: %s, name: %s, id: %s\n",
			dev.descURL.c_str(), dev.friendlyName.c_str(), dev.uuid.c_str());

	m_main->lock();
	EventData ed = {UPNP_PROTO, true, type, dev, ZCDevice(), NULL};
	m_eventData.push(ed);
	callOnMainThread(NavDsc::serviceOnlineInternal,this);
	m_main->unlock();
}

void NavDsc::serviceOfflineInternal(void *ptr)
{
	NAV_LOG("NavDsc::serviceOfflineInternal()\n");
	NavDsc *nd = (NavDsc*)ptr;

	if (nd->m_eventData.size() == 0) {
		NAV_LOG("serviceOfflineInternal: No Types, so cannot add dev\n");
		return;
	}

	nd->m_main->lock();
	EventData ed(nd->m_eventData.front());
	nd->m_eventData.pop();
	nd->m_main->unlock();


	std::string uuid = (ed.proto==ZC_PROTO) ? ed.zcdev.friendlyName : ed.dev.uuid;
	std::vector<NavServices*> srvs = nd->getNavServices(ed.type, ed.online);
	if (srvs.size() == 0) {
		NAV_LOG("serviceOfflineInternal: No Devs, so cannot add dev\n");
		return;
	}

	for (int i=0; i<srvs.size(); i++) {
		bool sendSevicesEvent = false;
		NAV_LOG("serviceOfflineInternal: service: %s, devs: %d\n", srvs[i]->m_serviceType.c_str(), srvs[i]->m_devs.size());
		for (int k=0; k<srvs[i]->m_devs.size(); k++)
		{

			NAV_LOG("serviceOfflineInternal: service uuid: %s, dev uuid: %s\n",
					srvs[i]->m_devs[k]->m_id.ascii().data(), uuid.c_str());

			if (srvs[i]->m_devs[k]->m_id == String(uuid.c_str())) {
				NAV_LOG("serviceOfflineInternal: Send offline to: %s\n", srvs[i]->m_devs[k]->name().ascii().data());
				srvs[i]->m_devs[k]->dispatchEvent(Event::create(eventNames().serviceofflineEvent, false, false));

				if (!sendSevicesEvent) {
					NAV_LOG("serviceOfflineInternal: Send unavailable to: %s\n", srvs[i]->m_serviceType.c_str());
					srvs[i]->dispatchEvent(Event::create(eventNames().serviceunavailableEvent, false, false));
					sendSevicesEvent = true;
				}
			}
		}
	}
}

void NavDsc::createPermissionsDialog(EventData ed)
{
    ExceptionCode ec;

    std::vector< NavServices* > srvs = getNavServices(ed.type, ed.online);

    RefPtr<HTMLDivElement> div = HTMLDivElement::create(m_frame->document());
    div->setIdAttribute(AtomicString("permissionDiv"));
    div->setInlineStyleProperty(CSSPropertyBorder, String("2px solid black"));
    div->setInlineStyleProperty(CSSPropertyBoxShadow, String("10px 10px 5px #888888"));
    div->setInlineStyleProperty(CSSPropertyBackgroundColor, String("#f0f0f0"));
    div->setInlineStyleProperty(CSSPropertyWidth, String("480px"));
    div->setInlineStyleProperty(CSSPropertyHeight, String("240px"));
    div->setInlineStyleProperty(CSSPropertyPosition, String("absolute"));
    div->setInlineStyleProperty(CSSPropertyLeft, String("0px"));
    div->setInlineStyleProperty(CSSPropertyTop, String("0px"));
    div->setInlineStyleProperty(CSSPropertyZIndex, String("1000px"));

    RefPtr<HTMLDivElement> title = HTMLDivElement::create(m_frame->document());
    title->setInlineStyleProperty(CSSPropertyBackgroundColor, String("#505050"));
    title->setInlineStyleProperty(CSSPropertyWidth, String("480px"));
    title->setInlineStyleProperty(CSSPropertyHeight, String("26px"));
    title->setInlineStyleProperty(CSSPropertyLeft, String("0px"));
    title->setInlineStyleProperty(CSSPropertyTop, String("0px"));

    RefPtr<HTMLFontElement> titleFnt = HTMLFontElement::create(HTMLNames::fontTag, m_frame->document());
    titleFnt->setInlineStyleProperty(CSSPropertyColor, String("#fff"));
    titleFnt->setInlineStyleProperty(CSSPropertyMarginLeft, String("3px"));
    titleFnt->setInnerText(String("Permissions"), ec);
    title->appendChild(titleFnt.release());

    div->appendChild(title.release());

    RefPtr<HTMLFontElement> msgFnt = HTMLFontElement::create(HTMLNames::fontTag, m_frame->document());
    msgFnt->setInnerText(String("Please check all devices you would like this page to use."), ec);
    div->appendChild(msgFnt.release());

    for (int k=0; k<srvs[0]->m_devs.size(); k++)
    {

        RefPtr<HTMLBRElement> br = HTMLBRElement::create(m_frame->document());
        div->appendChild(br.release());

        RefPtr<PermissionCheckBoxElement> chk = PermissionCheckBoxElement::create(m_frame->document(), this);
        std::stringstream id;
        id << "permissionchkbx" << k;

        chk->setIdAttribute(AtomicString(id.str().c_str()));
        chk->setEventData(ed);
        chk->setChecked(srvs[0]->m_devs[k]->hasPermission(), DispatchNoEvent);
        div->appendChild(chk.release());

        RefPtr<HTMLFontElement> fnt = HTMLFontElement::create(HTMLNames::fontTag, m_frame->document());
        fnt->setInnerText(String(srvs[0]->m_devs[k]->name()), ec);
        div->appendChild(fnt.release());
    }

    RefPtr<HTMLDivElement> footer = HTMLDivElement::create(m_frame->document());
    footer->setInlineStyleProperty(CSSPropertyPosition, String("absolute"));
    footer->setInlineStyleProperty(CSSPropertyTop, String("200px"));
    footer->setInlineStyleProperty(CSSPropertyWidth, String("100px"));
    footer->setInlineStyleProperty(CSSPropertyMarginLeft, String("190px"));
    footer->setInlineStyleProperty(CSSPropertyHeight, String("30px"));

    RefPtr<PermissionButtonElement> ok = PermissionButtonElement::create(m_frame->document(), this);
    ok->setType(String("button"));
    ok->setValue(String("OK"), ec, DispatchNoEvent);
    ok->setInlineStyleProperty(CSSPropertyWidth, String("100px"));
    footer->appendChild(ok.release());

    div->appendChild(footer.release());

    RefPtr<NodeList> list = m_frame->document()->documentElement()->getElementsByTagName(AtomicString("body"));
    list->item(0)->appendChild(div.release(), ec, AttachLazily);
    list.release();
}

void NavDsc::serviceOnlineInternal(void *ptr)
{
	NAV_LOG("NavDsc::serviceOnlineInternal()\n");
	NavDsc *nd = (NavDsc*)ptr;

	if (nd->m_eventData.size() == 0) {
		NAV_LOG("serviceOnlineInternal: No Types, so cannot add dev\n");
		return;
	}

	nd->m_main->lock();
	EventData ed(nd->m_eventData.front());
	nd->m_eventData.pop();
	nd->m_main->unlock();


	std::string uuid = (ed.proto==ZC_PROTO) ? ed.zcdev.friendlyName : ed.dev.uuid;
	std::vector<NavServices*> srvs = nd->getNavServices(ed.type, ed.online);

	if (!srvs.size() || !srvs[0]->find(uuid)) {
		if (ed.proto == ZC_PROTO) {
			ZCDevMap *devs = ZeroConf::getDevs(ed.type.c_str());
			if (!devs)
				return;

			nd->addZCDev(ed.type, devs->devMap);
		} else {
			UPnPDevMap *devs = UPnPSearch::getDevs(ed.type.c_str());
			if (!devs)
				return;

			nd->addUPnPDev(ed.type, devs->devMap);
		}
	}

	srvs = nd->getNavServices(ed.type, ed.online);
	if (srvs.size() == 0) {
		ERR_LOG("NavDsc::serviceOnlineInternal(): No Services after add device!\n");
		return;
	}

	nd->createPermissionsDialog(ed);

	bool sendSevicesEvent = false;
	NAV_LOG("serviceOnlineInternal: service: %s, devs: %d\n", srvs[0]->m_serviceType.c_str(), srvs[0]->m_devs.size());
	for (int k=0; k<srvs[0]->m_devs.size(); k++)
	{

		NAV_LOG("serviceOnlineInternal: service uuid: %s, dev uuid: %s\n",
				srvs[0]->m_devs[k]->m_id.ascii().data(), uuid.c_str());

		if (srvs[0]->m_devs[k]->m_id.contains(String(uuid.c_str()),true)) {
			NAV_LOG("serviceOnlineInternal: Send online to: %s\n", srvs[0]->m_devs[k]->name().ascii().data());
			//srvs[i]->m_devs[k]->dispatchEvent(Event::create(eventNames().serviceonlineEvent, false, false));

			if (!sendSevicesEvent) {
				NAV_LOG("serviceOnlineInternal: Send available to: %s\n", srvs[0]->m_serviceType.c_str());
				srvs[0]->dispatchEvent(Event::create(eventNames().serviceavailableEvent, false, false));
				sendSevicesEvent = true;
			}
		}
	}
	//NAV_LOG("UPnPDevAddedInternal(): Add dev. %s\n", type.c_str());
}

void NavDsc::sendEvent(std::string uuid, std::string stype, std::string body)
{
	std::string name = "";
	UPnPSearch::getInstance()->getUPnPFriendlyName(uuid, stype, name);

	//NAV_LOG("NavDsc::sendEvent(%s)\n",uuid.c_str());
	RefPtr<NavEvent> evnt = NavEvent::create();

	evnt->setPropertyset(WTF::String(body.c_str()));
	evnt->setUuid(WTF::String(uuid.c_str()));
	evnt->setServiceType(WTF::String(stype.c_str()));
	evnt->setFriendlyName(WTF::String(name.c_str()));

	m_main->lock();
	EventData ed = {EVENT_PROTO, true, stype, UPnPDevice(), ZCDevice(), evnt};
	m_eventData.push(ed);
	callOnMainThread(NavDsc::sendEventInternal,this);
	m_main->unlock();
}


void NavDsc::sendEventInternal(void *ptr)
{
	NAV_LOG("NavDsc::sendEventInternal()\n");
	NavDsc *nv = (NavDsc*)ptr;

	nv->m_main->lock();
	EventData ed = nv->m_eventData.front();
	nv->m_eventData.pop();
	nv->m_main->unlock();

	RefPtr<NavEvent> evnt = ed.evnt;
	std::vector<NavServices*> srvs = nv->getNavServices(ed.type);

	for (int i=0; i<srvs.size(); i++) {
		NavService* srv = srvs[i]->find(std::string(evnt->uuid().ascii().data()));

		if (srv) {
			NAV_LOG("NavDsc::sendEventInternal(%s) SENDING... Name: %s\n",ed.type.c_str(), srv->name().ascii().data());
			struct UPnPEventInit init;
			init.friendlyName = evnt->friendlyName();
			init.propertyset = evnt->propertyset();
			init.serviceType = evnt->serviceType();
			init.uuid = evnt->uuid();
			srv->dispatchEvent(UPnPEvent::create(eventNames().upnpEvent, init));
		}
		else
			ERR_LOG("NavDsc::sendEventInternal() srv == NULL !!!!!!\n");
	}

	evnt.release();
}

void NavDsc::startUPnPInternalDiscovery(const char *type, IDiscoveryAPI *api)
{
    NAV_LOG("startUPnPInternalDiscovery(%s).\n", type);

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
	NAV_LOG("NavDsc::startUPnPDiscovery(%s).\n", type);

	std::map<std::string, UPnPDevice> empty;
	std::map<std::string, UPnPDevice> devs = UPnPSearch::discoverDevs(type, this);

	if (m_resetSet) {
		NAV_LOG("NavDsc::startUPnPDiscovery() resetting...\n");
		UPnPSearch::getInstance()->reset();
		m_services.clear();
		devs.clear();
		m_resetSet = false;
	}

	if (devs.size()) {
		if (!m_frame)
			return empty;

		Page* page = m_frame->page();
		if (!page)
			return empty;

		for (std::map<std::string, UPnPDevice>::iterator it = devs.begin(); it != devs.end(); it++)
			UPnPSearch::getInstance()->eventServer(type, (*it).second.eventURL, (*it).second.uuid, (*it).second.host, (*it).second.port);


		std::string strType(type);
		std::map<std::string, ZCDevice> zcdevs;
		addUPnPDev(strType, devs);
		//setServices(strType, type, devs, zcdevs, UPNP_PROTO);
	}

	if (!UPnPSearch::getInstance()->m_udpSocket || !UPnPSearch::getInstance()->networkIsUp()) {
		ERR_LOG("NavDsc::startUPnPDiscovery() NETWORK_ERR\n");
		RefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::NETWORK_ERR);
		errorcb->handleEvent(err.get());
		return devs;
	}

	std::vector<NavServices *> srvs = getNavServices(type);
	successcb->handleEvent(srvs.at(0));

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
		//NAV_LOG("NavDsc::onUPnPError() error: %d\n", error);
		m_UPnPnav.begin()->second->onError(error);
	}
}

void NavDsc::onZCError(int error)
{
	if (m_ZCnav.size())
	{
		//NAV_LOG("NavDsc::onZCError() error: %d\n", error);
		m_ZCnav.begin()->second->onError(error);
	}
}


} // namespace WebCore

#endif // ENABLE_DISCOVERY
