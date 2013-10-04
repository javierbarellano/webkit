/* Copyright (C) 2013 Cable Television Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#if ENABLE(DISCOVERY)

#include "NavDsc.h"

#include "CSSValueKeywords.h"
#include "Chrome.h"
#include "Document.h"
#include "Frame.h"
#include "FrameLoadRequest.h"
#include "FrameLoaderClient.h"
#include "HTMLBRElement.h"
#include "HTMLDivElement.h"
#include "HTMLElement.h"
#include "HTMLFontElement.h"
#include "HTMLInputElement.h"
#include "HTMLNames.h"
#include "IDiscoveryAPI.h"
#include "NavigatorNetworkService.h"
#include "NodeList.h"
#include "Page.h"
#include "ResourceHandle.h"
#include "ShadowRoot.h"
#include "UPnPDevice.h"
#include "UPnPSearch.h"
#include "ZCDevice.h"
#include "ZeroConf.h"
#include <errno.h>
#include <map>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

// #define LOGGING_NAV 1

#ifdef LOGGING_NAV
#define NAV_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define NAV_LOG(fmt, ...)
#endif
#define ERR_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)

#ifndef IMAX
#define IMAX(a, b) ((a) < (b)) ? (b) : (a)
#endif

namespace WebCore {

// ----------------------------

class PermissionInput : public HTMLInputElement {
public:
    void setNavDsc(NavDsc* nd) { m_navdsc = nd; }
    NavDsc* getNavDsc() { return m_navdsc; }

protected:
    explicit PermissionInput(const QualifiedName&, Document&);
    NavDsc* m_navdsc;
};

// ----------------------------

class PermissionButtonElement FINAL : public PermissionInput {
public:
    static PassRefPtr<PermissionButtonElement> create(Document&, NavDsc*);

    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }

private:
    explicit PermissionButtonElement(Document&);

    virtual void defaultEventHandler(Event*) OVERRIDE;

};

// ----------------------------

class PermissionCheckBoxElement FINAL : public PermissionInput {
public:
    static PassRefPtr<PermissionCheckBoxElement> create(Document&, NavDsc*);

    virtual bool willRespondToMouseClickEvents() OVERRIDE { return true; }
    void setEventData(EventData ed) { m_ed = ed; }
    void setId(int id) { m_id = id; }

private:
    explicit PermissionCheckBoxElement(Document&);

    virtual void defaultEventHandler(Event*) OVERRIDE;
    EventData m_ed;
    int m_id;
};

// ----------------------------

PermissionInput::PermissionInput(const QualifiedName& tag, Document& doc)
    : HTMLInputElement(tag, doc, 0, false)
    , m_navdsc(0)
{
}

// ----------------------------

PassRefPtr<PermissionButtonElement> PermissionButtonElement::create(Document& doc, NavDsc* navdsc)
{
    RefPtr<PermissionButtonElement> button = adoptRef(new PermissionButtonElement(doc));
    button->setType(String("button"));
    button->setNavDsc(navdsc);

    return button.release();
}


PermissionButtonElement::PermissionButtonElement(Document& doc)
    : PermissionInput(HTMLNames::inputTag, doc)
{
}

void PermissionButtonElement::defaultEventHandler(Event* event)
{
    if (event->isMouseEvent() && event->type() == eventNames().mousedownEvent && m_navdsc) {
        Element* child = this->parentElement()->parentElement();

        if (child && child->parentElement())
            child->parentElement()->removeChild(child->toNode(), IGNORE_EXCEPTION);

        callOnMainThread(NavDsc::dispatchServiceOnline, m_navdsc);
    }
}

// ----------------------------

PassRefPtr<PermissionCheckBoxElement> PermissionCheckBoxElement::create(Document& doc, NavDsc* navdsc)
{
    RefPtr<PermissionCheckBoxElement> button = adoptRef(new PermissionCheckBoxElement(doc));
    button->setType(String("checkbox"));
    button->setNavDsc(navdsc);

    return button.release();
}

PermissionCheckBoxElement::PermissionCheckBoxElement(Document& doc)
    : PermissionInput((const QualifiedName)HTMLNames::inputTag, doc)
{
    m_id = -1;
}

void PermissionCheckBoxElement::defaultEventHandler(Event* event)
{
    if (event->isMouseEvent() && event->type() == eventNames().mousedownEvent && m_navdsc) {
        std::vector< NetworkServices* > srvs = m_navdsc->getNavServices(m_ed.type, m_ed.online);

        // This default handler happens before the input handler, so the action hasn't been applied yet
        // srvs is a one element vector, when getNavServices() is called with 'all' for a type, it returns all services
        srvs[0]->m_devs[m_id]->setPermission(!this->checked());
    }
}

// ----------------------------

NavDsc* NavDsc::instance = 0;
#if ENABLE(DISCOVERY_PERMISSIONS)
bool NavDsc::m_permissionsEnabled = true;
#else
bool NavDsc::m_permissionsEnabled = false;
#endif

// Static
NavDsc* NavDsc::create(Frame* frame)
{
    if (!instance)
        instance = new NavDsc(frame);

    if (frame)
        instance->m_frame = frame;

    return instance;
}

NavDsc::NavDsc(Frame* frame)
    : m_frame(frame)
{
    m_resetSet = false;
}

NavDsc::~NavDsc()
{
    if (UPnPSearch::getInstance())
        delete UPnPSearch::getInstance();

    instance = 0;
}


// --------

void NavDsc::updateServices(std::string type, std::map<std::string, UPnPDevice> devs)
{
    NAV_LOG("updateServices(%s) devs.size: %d\n", type.c_str(), devs.size());

    MutexLocker lock(m_lockServices);
    for (std::map<std::string, UPnPDevice>::iterator i = devs.begin(); i!= devs.end(); i++) {
        UPnPDevice ns = i->second;
        std::string uuid = i->first;
        bool found = false;

        if (!m_services[type]) {
            m_services[type] = NetworkServices::create(m_frame->document(), NetworkServices::Connected);
            m_services[type]->suspendIfNeeded();
        }

        NetworkServices* nss = m_services[type].get();

        for (int m = 0; nss && m < nss->length() && !found; m++) {
            if (strstr(nss->item(m)->id().ascii().data(), uuid.c_str()))
                found = true;
        }

        NAV_LOG("updateServices() uuid: %s, found: %s\n", uuid.c_str(), (found ? "true" : "false"));

        if (!found) {
            RefPtr<NetworkService> srv = NetworkService::create(m_frame->document());
            srv->suspendIfNeeded();

            srv->setType(WTF::String(type.c_str()));
            srv->setProtocolType(NetworkService::UPnPType);
            srv->setUrl(WTF::String(ns.descURL.c_str()));
            srv->setid(WTF::String(uuid.c_str())); // UUID
            srv->setName(WTF::String(ns.friendlyName.c_str()));
            m_services[type]->append(srv);
            m_services[type]->m_serviceType = type;

            NAV_LOG("updateServices() uuid: %s, added\n", uuid.c_str());
        }
    }
}

void NavDsc::updateZCServices(std::string type, std::map<std::string, ZCDevice> devs)
{
    NAV_LOG("updateZCServices(%s) devs.size: %d\n", type.c_str(), devs.size());
    MutexLocker lock(m_lockServices);
    for (std::map<std::string, ZCDevice>::iterator i = devs.begin(); i != devs.end(); i++) {
        ZCDevice ns = i->second;
        std::string uuid = i->first;
        bool found = false;

        if (!m_services[type]) {
            m_services[type] = NetworkServices::create(m_frame->document(), NetworkServices::Connected);
            m_services[type]->suspendIfNeeded();
        }

        NetworkServices* nss = m_services[type].get();

        for (int m = 0; nss && m < nss->length() && !found; m++) {
            if (strstr(nss->item(m)->id().ascii().data(), uuid.c_str()))
                found = true;
        }

        NAV_LOG("updateZCServices() uuid: %s, found: %s\n", uuid.c_str(), (found ? "true" : "false"));

        if (!found) {
            RefPtr<NetworkService> srv = NetworkService::create(m_frame->document());
            srv->suspendIfNeeded();

            srv->setType(WTF::String(type.c_str()));
            srv->setProtocolType(NetworkService::UPnPType);
            srv->setUrl(WTF::String(ns.url.c_str()));
            srv->setid(WTF::String(uuid.c_str())); // UUID
            srv->setName(WTF::String(ns.friendlyName.c_str()));
            m_services[type]->append(srv);
            m_services[type]->m_serviceType = type;

            NAV_LOG("updateZCServices() uuid: %s, added\n", uuid.c_str());
        }
    }
}

void NavDsc::setServices(std::string strType, const char* type,
    std::map<std::string, UPnPDevice> devs, std::map<std::string, ZCDevice> zcdevs,
    ProtocolType protoType)
{
    NAV_LOG("setServices(%s)\n", strType.c_str());

    // Just protect m_services
    {
        MutexLocker lock(m_lockServices);
        m_services[strType] = NetworkServices::create(m_frame->document(), NetworkServices::Connected);
        m_services[strType]->suspendIfNeeded();
    }

    Vector<RefPtr<NetworkService> >* vDevs = new Vector<RefPtr<NetworkService> >();

    if (protoType == UPnPProtocol) {
        std::map<std::string, UPnPDevice>::iterator it;
        for (it = devs.begin(); it != devs.end(); it++) {
            UPnPDevice d((*it).second);
            RefPtr<NetworkService> srv = NetworkService::create(m_frame->document());
            srv->suspendIfNeeded();

            srv->setType(WTF::String(type));
            srv->setProtocolType(NetworkService::UPnPType);
            srv->setUrl(WTF::String(d.descURL.c_str()));
            srv->setid(WTF::String((*it).first.c_str())); // UUID
            srv->setName(WTF::String(d.friendlyName.c_str()));

            vDevs->append(srv);
        }

    } else if (protoType == ZeroConfProtocol) {
        std::map<std::string, ZCDevice>::iterator it;
        for (it = zcdevs.begin(); it != zcdevs.end(); it++) {
            ZCDevice d((*it).second);
            RefPtr<NetworkService> srv = NetworkService::create(m_frame->document());
            srv->suspendIfNeeded();

            srv->setType(WTF::String(type));
            srv->setProtocolType(NetworkService::ZConfType);
            srv->setUrl(WTF::String(d.url.c_str()));
            srv->setid(WTF::String((*it).first.c_str())); // UUID
            srv->setName(WTF::String(d.friendlyName.c_str()));

            vDevs->append(srv);
        }
    }

    // Write devices to service object
    MutexLocker lock(m_lockServices);
    m_services[strType]->setServices(vDevs);
    m_services[strType]->m_serviceType = strType;
}

std::vector< NetworkServices* > NavDsc::getNavServices(std::string type, bool isUp)
{
    std::vector<NetworkServices*> pNS;
    RefPtr<NetworkServices>  ns;

    MutexLocker lock(m_lockServices);
    if (type == "all") {
        for (std::map<std::string, RefPtr<NetworkServices> >::iterator it = m_services.begin();
            it != m_services.end(); it++) {

            RefPtr<NetworkServices> typeNS = it->second;
            typeNS->setOnline(isUp);
            pNS.push_back(typeNS.get());
        }
    } else {
        if (!m_services[type]) {
            m_services[type] = NetworkServices::create(m_frame->document(), NetworkServices::Connected);
            m_services[type]->suspendIfNeeded();
        }

        pNS.push_back(m_services[type].get());
    }

    NAV_LOG("getNavServices() number devs: %d for type: %s\n", pNS[0]->length(), type.c_str());

    return pNS;
}

bool NavDsc::has(std::vector<RefPtr<NetworkServices> > srvs, std::string uuid)
{
    String nsUUID(uuid.c_str());

    for (int i = 0; i < srvs.size(); i++) {
        Vector<RefPtr<NetworkService> > ns = srvs.at(i)->m_devs;
        for (int k = 0; k < ns.size(); k++) {
            if (ns.at(k)->id() == nsUUID)
                return true;
        }
    }
    return false;
}

void NavDsc::ZCDevDropped(std::string type, ZCDevice &dev)
{
    NAV_LOG("NavDsc::serviceOffline() url: %s, name: %s\n", dev.url.c_str(),
        dev.friendlyName.c_str());

    EventData ed = { ZeroConfProtocol, false, type, UPnPDevice(), dev, 0 };
    {
        MutexLocker lock(m_mainMutex);
        m_eventData.push(ed);
    }

    callOnMainThread(NavDsc::serviceOfflineInternal, this);
}

void NavDsc::ZCDevAdded(std::string type, ZCDevice &dev)
{
    NAV_LOG("NavDsc::serviceOnline() url: %s, name: %s\n", dev.url.c_str(),
        dev.friendlyName.c_str());

    EventData ed = { ZeroConfProtocol, true, type, UPnPDevice(), dev, 0 };
    {
        MutexLocker lock(m_mainMutex);
        m_eventData.push(ed);
    }

    if (m_permissionsEnabled)
        callOnMainThread(NavDsc::serviceOnlineInternal, this);
    else
        callOnMainThread(NavDsc::dispatchServiceOnline, this);
}

void NavDsc::UPnPDevDropped(std::string type, UPnPDevice &dev)
{
    NAV_LOG("NavDsc::serviceOffline() url: %s, name: %s, id: %s\n",
        dev.descURL.c_str(), dev.friendlyName.c_str(), dev.uuid.c_str());

    UPnPDevMap dm;

    if (m_frame) {
        EventData ed = { UPnPProtocol, false, type, dev, ZCDevice(), 0 };
        {
            MutexLocker lock(m_mainMutex);
            m_eventData.push(ed);
        }

        callOnMainThread(NavDsc::serviceOfflineInternal, this);
    } else {
        UPnPDevMap dm = UPnPSearch::getDevs(type);
        if (dm.devMap.size())
            serverListUpdate(type, &dm.devMap);
    }
}

void NavDsc::UPnPDevAdded(std::string type, UPnPDevice &dev)
{
    NAV_LOG("NavDsc::serviceOnline() url: %s, name: %s, id: %s\n",
        dev.descURL.c_str(), dev.friendlyName.c_str(), dev.uuid.c_str());

    if (m_frame) {
        EventData ed = { UPnPProtocol, true, type, dev, ZCDevice(), 0 };
        {
            MutexLocker lock(m_mainMutex);
            m_eventData.push(ed);
        }

        NAV_LOG("NavDsc::serviceOnlineInternal() m_permissionsEnabled=%s\n", m_permissionsEnabled ? "true":"false");
        if (m_permissionsEnabled)
            callOnMainThread(NavDsc::serviceOnlineInternal, this);
        else
            callOnMainThread(NavDsc::dispatchServiceOnline, this);

    } else {
        UPnPDevMap dm = UPnPSearch::getDevs(type);
        NAV_LOG("UPnPDevAdded(): call serverListUpdate() Size: %d\n", dm.devMap.size());
        if (dm.devMap.size())
            serverListUpdate(type, &dm.devMap);
    }
}

void NavDsc::serviceOfflineInternal(void *ptr)
{
    NAV_LOG("NavDsc::serviceOfflineInternal()\n");
    NavDsc* nd = (NavDsc*)ptr;
    EventData ed;

    if (nd->m_eventData.empty()) {
        NAV_LOG("serviceOfflineInternal: No Types, so cannot add dev\n");
        return;
    }

    {
        MutexLocker lock(nd->m_mainMutex);
        ed = nd->m_eventData.front();
        nd->m_eventData.pop();
    }

    std::string uuid = (ed.proto == ZeroConfProtocol) ? ed.zcdev.friendlyName : ed.dev.uuid;
    std::vector<NetworkServices*> srvs = nd->getNavServices(ed.type, ed.online);
    if (srvs.empty()) {
        NAV_LOG("serviceOfflineInternal: No Devs, so cannot add dev\n");
        return;
    }

    for (int i = 0; i < srvs.size(); i++) {
        bool sendSevicesEvent = false;
        NAV_LOG("serviceOfflineInternal: service: %s, devs: %d\n",
            srvs[i]->m_serviceType.c_str(), srvs[i]->m_devs.size());
        for (int k = 0; k < srvs[i]->m_devs.size(); k++) {
            NAV_LOG("serviceOfflineInternal: service uuid: %s, dev uuid: %s\n",
                srvs[i]->m_devs[k]->m_id.ascii().data(), uuid.c_str());

            if (srvs[i]->m_devs[k]->m_id == String(uuid.c_str())) {
                NAV_LOG("serviceOfflineInternal: Send offline to: %s\n",
                    srvs[i]->m_devs[k]->name().ascii().data());
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
    std::vector< NetworkServices* > srvs = getNavServices(ed.type, ed.online);

    RefPtr<HTMLDivElement> div = HTMLDivElement::create(*m_frame->document());
    div->setInlineStyleProperty(CSSPropertyBorder, ASCIILiteral("2px solid black"));
    div->setInlineStyleProperty(CSSPropertyBoxShadow, ASCIILiteral("10px 10px 5px #888888"));
    div->setInlineStyleProperty(CSSPropertyBackgroundColor, ASCIILiteral("#f0f0f0"));
    div->setInlineStyleProperty(CSSPropertyWidth, ASCIILiteral("480px"));
    div->setInlineStyleProperty(CSSPropertyHeight, ASCIILiteral("240px"));
    div->setInlineStyleProperty(CSSPropertyPosition, ASCIILiteral("absolute"));
    div->setInlineStyleProperty(CSSPropertyLeft, ASCIILiteral("0px"));
    div->setInlineStyleProperty(CSSPropertyTop, ASCIILiteral("0px"));
    div->setInlineStyleProperty(CSSPropertyZIndex, ASCIILiteral("1000px"));

    RefPtr<HTMLDivElement> title = HTMLDivElement::create(*m_frame->document());
    title->setInlineStyleProperty(CSSPropertyBackgroundColor, ASCIILiteral("#505050"));
    title->setInlineStyleProperty(CSSPropertyWidth, ASCIILiteral("480px"));
    title->setInlineStyleProperty(CSSPropertyHeight, ASCIILiteral("26px"));
    title->setInlineStyleProperty(CSSPropertyLeft, ASCIILiteral("0px"));
    title->setInlineStyleProperty(CSSPropertyTop, ASCIILiteral("0px"));

    RefPtr<HTMLFontElement> titleFnt = HTMLFontElement::create((const QualifiedName)HTMLNames::fontTag, *m_frame->document());
    titleFnt->setInlineStyleProperty(CSSPropertyColor, ASCIILiteral("#fff"));
    titleFnt->setInlineStyleProperty(CSSPropertyMarginLeft, ASCIILiteral("3px"));
    titleFnt->setInnerText(ASCIILiteral("Permissions"), IGNORE_EXCEPTION);
    title->appendChild(titleFnt.release());

    div->appendChild(title.release());

    RefPtr<HTMLFontElement> msgFnt = HTMLFontElement::create((const QualifiedName)HTMLNames::fontTag, *m_frame->document());
    msgFnt->setInnerText(ASCIILiteral("Please check all devices you would like this page to use."), IGNORE_EXCEPTION);
    div->appendChild(msgFnt.release());

    for (int k = 0; k < srvs[0]->m_devs.size(); k++) {
        RefPtr<HTMLBRElement> br = HTMLBRElement::create(*m_frame->document());
        div->appendChild(br.release());

        RefPtr<PermissionCheckBoxElement> chk = PermissionCheckBoxElement::create(*m_frame->document(), this);
        chk->setId(k);
        chk->setEventData(ed);
        chk->setChecked(srvs[0]->m_devs[k]->hasPermission(), DispatchNoEvent);
        div->appendChild(chk.release());

        RefPtr<HTMLFontElement> fnt = HTMLFontElement::create((const QualifiedName)HTMLNames::fontTag, *m_frame->document());
        fnt->setInnerText(String(srvs[0]->m_devs[k]->name()), IGNORE_EXCEPTION);
        div->appendChild(fnt.release());
    }

    RefPtr<HTMLDivElement> footer = HTMLDivElement::create(*m_frame->document());
    footer->setInlineStyleProperty(CSSPropertyPosition, ASCIILiteral("absolute"));
    footer->setInlineStyleProperty(CSSPropertyTop, ASCIILiteral("200px"));
    footer->setInlineStyleProperty(CSSPropertyWidth, ASCIILiteral("100px"));
    footer->setInlineStyleProperty(CSSPropertyMarginLeft, ASCIILiteral("190px"));
    footer->setInlineStyleProperty(CSSPropertyHeight, ASCIILiteral("30px"));

    RefPtr<PermissionButtonElement> ok = PermissionButtonElement::create(*m_frame->document(), this);
    ok->setType(ASCIILiteral("button"));
    ok->setValue(ASCIILiteral("OK"), IGNORE_EXCEPTION, DispatchNoEvent);
    ok->setInlineStyleProperty(CSSPropertyWidth, ASCIILiteral("100px"));
    footer->appendChild(ok.release());

    div->appendChild(footer.release());

    RefPtr<NodeList> list = m_frame->document()->documentElement()->getElementsByTagName(AtomicString("body"));
    list->item(0)->appendChild(div.release(), IGNORE_EXCEPTION, AttachLazily);
}

void NavDsc::serviceOnlineInternal(void *ptr)
{
    NAV_LOG("NavDsc::serviceOnlineInternal()\n");
    NavDsc* nd = (NavDsc*)ptr;
    EventData ed;

    if (nd->m_eventData.empty()) {
        NAV_LOG("serviceOnlineInternal: No Types, so cannot add dev\n");
        return;
    }

    {
        MutexLocker lock(nd->m_mainMutex);
        ed = nd->m_eventData.front();
    }

    std::string uuid = (ed.proto == ZeroConfProtocol) ? ed.zcdev.friendlyName : ed.dev.uuid;
    std::vector<NetworkServices*> srvs = nd->getNavServices(ed.type, ed.online);

    if (srvs.empty() || !srvs[0]->find(uuid)) {
        if (ed.proto == ZeroConfProtocol) {
            ZCDevMap* devs = ZeroConf::getDevs(ed.type.c_str());
            if (!devs)
                return;

            nd->updateZCServices(ed.type, devs->devMap);
        } else {
            UPnPDevMap devs = UPnPSearch::getDevs(ed.type);
            if (devs.devMap.empty())
                return;

            nd->updateServices(ed.type, devs.devMap);
        }
    }

    srvs = nd->getNavServices(ed.type, ed.online);
    if (srvs.empty()) {
        ERR_LOG("NavDsc::serviceOnlineInternal(): No Services after add device!\n");
        return;
    }

    nd->createPermissionsDialog(ed);

}

void NavDsc::dispatchServiceOnline(void *ptr)
{
    NAV_LOG("NavDsc::serviceOnlineInternal()\n");
    NavDsc* nd = (NavDsc*)ptr;
    EventData ed;

    if (nd->m_eventData.empty()) {
        NAV_LOG("serviceOnlineInternal: No Types, so cannot add dev\n");
        return;
    }

    {
        MutexLocker lock(nd->m_mainMutex);
        ed = nd->m_eventData.front();
        nd->m_eventData.pop();
    }

    std::string uuid = (ed.proto == ZeroConfProtocol) ? ed.zcdev.friendlyName : ed.dev.uuid;
    std::vector<NetworkServices*> srvs = nd->getNavServices(ed.type, ed.online);

    if (!srvs.size() || !srvs[0]->find(uuid)) {
        if (ed.proto == ZeroConfProtocol) {
            ZCDevMap* devs = ZeroConf::getDevs(ed.type.c_str());
            if (!devs)
                return;

            nd->updateZCServices(ed.type, devs->devMap);
        } else {
            UPnPDevMap devs = UPnPSearch::getDevs(ed.type);
            if (devs.devMap.empty())
                return;

            nd->updateServices(ed.type, devs.devMap);
        }
    }

    srvs = nd->getNavServices(ed.type, ed.online);
    if (srvs.empty()) {
        ERR_LOG("NavDsc::serviceOnlineInternal(): No Services after add device!\n");
        return;
    }

    bool sendSevicesEvent = false;
    NAV_LOG("serviceOnlineInternal: service: %s, devs: %d\n", srvs[0]->m_serviceType.c_str(), srvs[0]->m_devs.size());
    for (int k = 0; k < srvs[0]->m_devs.size(); k++) {
        NAV_LOG("serviceOnlineInternal: service uuid: %s, dev uuid: %s, hasPermission: %s\n",
            srvs[0]->m_devs[k]->m_id.ascii().data(), uuid.c_str(), srvs[0]->m_devs[k]->hasPermission() ? "Ok":"NO");

        if (!srvs[0]->m_devs[k]->hasPermission() && nd->m_permissionsEnabled)
            continue;

        if (srvs[0]->m_devs[k]->m_id.contains(String(uuid.c_str()), true)) {
            NAV_LOG("serviceOnlineInternal: Send online to: %s\n", srvs[0]->m_devs[k]->name().ascii().data());

            if (!sendSevicesEvent) {
                NAV_LOG("serviceOnlineInternal: Send available to: %s\n", srvs[0]->m_serviceType.c_str());
                srvs[0]->dispatchEvent(Event::create(eventNames().serviceavailableEvent, false, false));
                sendSevicesEvent = true;
            }
        }
    }
}

void NavDsc::sendEvent(std::string uuid, std::string stype, std::string body)
{
    std::string name = "";
    UPnPSearch::getInstance()->getUPnPFriendlyName(uuid, stype, name);

    // NAV_LOG("NavDsc::sendEvent(%s)\n",uuid.c_str());
    RefPtr<NavEvent> event = NavEvent::create();

    event->setPropertyset(WTF::String(body.c_str()));
    event->setUuid(WTF::String(uuid.c_str()));
    event->setServiceType(WTF::String(stype.c_str()));
    event->setFriendlyName(WTF::String(name.c_str()));

    EventData ed = { EventProtocol, true, stype, UPnPDevice(), ZCDevice(), event };
    {
        MutexLocker lock(m_mainMutex);
        m_eventData.push(ed);
    }

    callOnMainThread(NavDsc::sendEventInternal, this);
}


void NavDsc::sendEventInternal(void *ptr)
{
    NAV_LOG("NavDsc::sendEventInternal()\n");
    NavDsc* nv = (NavDsc*)ptr;
    EventData ed;

    {
        MutexLocker lock(nv->m_mainMutex);
        ed = nv->m_eventData.front();
        nv->m_eventData.pop();
    }

    RefPtr<NavEvent> evnt = ed.evnt;
    std::vector<NetworkServices*> srvs = nv->getNavServices(ed.type);

    for (int i = 0; i < srvs.size(); i++) {
        NetworkService* srv = srvs[i]->find(std::string(evnt->uuid().ascii().data()));

        if (srv) {
            NAV_LOG("NavDsc::sendEventInternal(%s) SENDING... Name: %s\n", ed.type.c_str(), srv->name().ascii().data());
            srv->dispatchEvent(Event::create());
        } else
            ERR_LOG("NavDsc::sendEventInternal() srv == NULL !!!!!!\n");
    }

    evnt.release();
}

void NavDsc::serverListUpdate(std::string type, std::map<std::string, UPnPDevice> *devs)
{
    if (m_UPnPNav.size()) {
        std::map<std::string, IDiscoveryAPI*>::iterator pair = m_UPnPNav.find(type);
        if (pair != m_UPnPNav.end() && pair->second)
            pair->second->serverListUpdate(type, devs);
    }

}

std::map<std::string, UPnPDevice> NavDsc::startUPnPDiscovery(const char *type,
    PassRefPtr<NavigatorNetworkServiceSuccessCallback> successcb, PassRefPtr<NavigatorNetworkServiceErrorCallback> errorcb)
{
    NAV_LOG("NavDsc::startUPnPDiscovery(%s).\n", type);

    std::map<std::string, UPnPDevice> empty;
    std::map<std::string, UPnPDevice> devs = UPnPSearch::discoverDevs(type, this);

    if (errorcb) {
        if (!UPnPSearch::getInstance()->m_udpSocket || !UPnPSearch::getInstance()->networkIsUp()) {
            // FIXME: Network error doesn't exist in the spec
            //ERR_LOG("NavDsc::startUPnPDiscovery() NETWORK_ERR\n");
            //RefPtr<NavigatorNetworkServiceError> err = NavigatorNetworkServiceError::create(NavigatorNetworkServiceError::NetworkError);
            //errorcb->handleEvent(err.get());
            return devs;
        }
    }

    if (m_resetSet) {
        NAV_LOG("NavDsc::startUPnPDiscovery() resetting...\n");
        UPnPSearch::getInstance()->reset();
        devs.clear();
        m_resetSet = false;

        m_services.clear();
        if (successcb) {
            std::vector<NetworkServices*> srvs = getNavServices(type);
            successcb->handleEvent(srvs.at(0));
        }

        return devs;
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
        updateServices(strType, devs);
    }

    if (successcb) {
        std::vector<NetworkServices*> srvs = getNavServices(type);
        successcb->handleEvent(srvs.at(0));
    }

    return devs;
}

std::map<std::string, ZCDevice> NavDsc::startZeroConfDiscovery(const char* type,
    PassRefPtr<NavigatorNetworkServiceSuccessCallback> successcb, PassRefPtr<NavigatorNetworkServiceErrorCallback> errorcb)
{
    std::map<std::string, ZCDevice> empty;
    std::map<std::string, ZCDevice> zcdevs = ZeroConf::discoverDevs(type, this);

    if (errorcb) {
        if (!ZeroConf::getInstance()->m_udpSocket) {
            // FIXME: Network error doesn't exist in the spec
//            RefPtr<NavigatorNetworkServiceError> err = NavigatorNetworkServiceError::create(NavigatorNetworkServiceError::NetworkError);
//            errorcb->handleEvent(err.get());
            return zcdevs;
        }
    }

    // We have devices to look at
    if (!m_frame)
        return empty;

    Page* page = m_frame->page();
    if (!page)
        return empty;

    std::string strType(type);
    std::map<std::string, UPnPDevice> devs;

    if (m_resetSet) {
        zcdevs.clear();
        m_resetSet = false;
        ZeroConf::getInstance()->reset();
        m_services.clear();
    }

    setServices(strType, type, devs, zcdevs, ZeroConfProtocol);

    std::vector<NetworkServices *> srvs = getNavServices(type);
    successcb->handleEvent(srvs.at(srvs.size()-1));

    return zcdevs;
}

void NavDsc::onError(int error)
{
    if (m_UPnPNav.size()) {
        NAV_LOG("NavDsc::onUPnPError() error: %d\n", error);
        m_UPnPNav.begin()->second->onError(error);
    }
}

void NavDsc::onZCError(int error)
{
    if (m_ZCNav.size()) {
        NAV_LOG("NavDsc::onZCError() error: %d\n", error);
        m_ZCNav.begin()->second->onError(error);
    }
}

} // namespace WebCore

#endif // ENABLE_DISCOVERY
