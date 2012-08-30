/*
 *  Copyright (C) 2000 Harri Porten (porten@kde.org)
 *  Copyright (c) 2000 Daniel Molkentin (molkentin@kde.org)
 *  Copyright (c) 2000 Stefan Schimanski (schimmi@kde.org)
 *  Copyright (C) 2003, 2004, 2005, 2006 Apple Computer, Inc.
 *  Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"
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

#include "CookieJar.h"
#include "DOMMimeTypeArray.h"
#include "DOMPluginArray.h"
#include "Document.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameLoaderClient.h"
#include "Geolocation.h"
#include "Language.h"
#include "Page.h"
#include "PluginData.h"
#include "SecurityOrigin.h"
#include "Settings.h"
#include "StorageNamespace.h"
#include <wtf/HashSet.h>
#include <wtf/StdLibExtras.h>

namespace WebCore {

Navigator::Navigator(Frame* frame)
    : DOMWindowProperty(frame)
{
	m_errorCBIsSet = false;
	m_UPnPserviceAddedCBIsSet = false;
	m_UPnPserviceRemovedCBIsSet = false;
	m_ZCserviceAddedCBIsSet = false;
	m_ZCserviceRemovedCBIsSet = false;
}

Navigator::~Navigator()
{
}

// If this function returns true, we need to hide the substring "4." that would otherwise
// appear in the appVersion string. This is to avoid problems with old versions of a
// library called OpenCube QuickMenu, which as of this writing is still being used on
// sites such as nwa.com -- the library thinks Safari is Netscape 4 if we don't do this!
static bool shouldHideFourDot(Frame* frame)
{
    const String* sourceURL = frame->script()->sourceURL();
    if (!sourceURL)
        return false;
    if (!(sourceURL->endsWith("/dqm_script.js") || sourceURL->endsWith("/dqm_loader.js") || sourceURL->endsWith("/tdqm_loader.js")))
        return false;
    Settings* settings = frame->settings();
    if (!settings)
        return false;
    return settings->needsSiteSpecificQuirks();
}

String Navigator::appVersion() const
{
    if (!m_frame)
        return String();
    String appVersion = NavigatorBase::appVersion();
    if (shouldHideFourDot(m_frame))
        appVersion.replace("4.", "4_");
    return appVersion;
}

String Navigator::language() const
{
    return defaultLanguage();
}

String Navigator::userAgent() const
{
    if (!m_frame)
        return String();
        
    // If the frame is already detached, FrameLoader::userAgent may malfunction, because it calls a client method
    // that uses frame's WebView (at least, in Mac WebKit).
    if (!m_frame->page())
        return String();
        
    return m_frame->loader()->userAgent(m_frame->document()->url());
}

DOMPluginArray* Navigator::plugins() const
{
    if (!m_plugins)
        m_plugins = DOMPluginArray::create(m_frame);
    return m_plugins.get();
}

DOMMimeTypeArray* Navigator::mimeTypes() const
{
    if (!m_mimeTypes)
        m_mimeTypes = DOMMimeTypeArray::create(m_frame);
    return m_mimeTypes.get();
}

bool Navigator::cookieEnabled() const
{
    if (!m_frame)
        return false;

    if (m_frame->page() && !m_frame->page()->settings()->cookieEnabled())
        return false;

    return cookiesEnabled(m_frame->document());
}

bool Navigator::javaEnabled() const
{
    if (!m_frame || !m_frame->settings())
        return false;

    if (!m_frame->settings()->isJavaEnabled())
        return false;
    if (m_frame->document()->securityOrigin()->isLocal() && !m_frame->settings()->isJavaEnabledForLocalFiles())
        return false;

    return true;
}

void Navigator::getStorageUpdates()
{
    // FIXME: Remove this method or rename to yieldForStorageUpdates.
}

// ---------------------------------------------------
// Home Networking
// ---------------------------------------------------

void Navigator::getNetworkServices(
		const String& type,
		PassRefPtr<NavServiceOkCB> successcb,
		PassRefPtr<NavServiceErrorCB> errorcb)
{
	printf("getNetworkServices()\n");
	
	if (!m_frame)
		return;

	WTF::CString cType(type.ascii());
	NavDsc *nd = NavDsc::create(m_frame);

	// Get service type
	char sType[1000];
	ProtocolType protoType = readRemoveTypePrefix(cType, sType);


	if (errorcb)
	{
		m_errorCB = errorcb;
		m_errorCBIsSet = true;
		if (protoType == UPNP_PROTO)
			nd->onUPnPDiscovery(sType, this);
		else if (protoType == ZC_PROTO)
			nd->onZCDiscovery(sType, this);
	}
	else
		m_errorCBIsSet = false;

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

	PassRefPtr<NavServices> services = setServices(sType, devs, zcdevs, protoType);

	successcb->handleEvent(services.get());

}

void Navigator::onNetworkServices(
		const String& type,
		PassRefPtr<NavDscCB> serviceAddedCB,
		PassRefPtr<NavServiceOkCB> serviceRemovedCB,
		PassRefPtr<NavServiceErrorCB> errorCB)
{
	if (!m_frame)
		return;

	WTF::CString cType(type.ascii());
	NavDsc *nd = NavDsc::create(m_frame);
	char sType[1000];
	ProtocolType protoType = readRemoveTypePrefix(cType, sType);

	if (protoType == UPNP_PROTO)
	{
		if (serviceAddedCB)
		{
			m_UPnPserviceAddedCB = serviceAddedCB;
			m_uacb[std::string(sType)] = m_UPnPserviceAddedCB;
			m_UPnPserviceAddedCBIsSet = true;
		}

		if (serviceRemovedCB)
		{
			m_UPnPserviceRemovedCB = serviceRemovedCB;
			m_urcb[std::string(sType)] = m_UPnPserviceRemovedCB;
			m_UPnPserviceRemovedCBIsSet = true;
		}

		nd->onUPnPDiscovery(sType, this);
	}
	else if (protoType == ZC_PROTO)
	{
		if (serviceAddedCB)
		{
			m_ZCserviceAddedCB = serviceAddedCB;
			m_zacb[std::string(sType)] = m_ZCserviceAddedCB;
			m_ZCserviceAddedCBIsSet = true;
		}

		if (serviceRemovedCB)
		{
			m_ZCserviceRemovedCB = serviceRemovedCB;
			m_zrcb[std::string(sType)] = m_ZCserviceRemovedCB;
			m_ZCserviceRemovedCBIsSet = true;
		}

		nd->onZCDiscovery(sType, this);
	}
	else if (errorCB)
	{
		PassRefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::UNKNOWN_TYPE);
		errorCB->handleEvent(err.get());
	}
}

/*
 * Event messaging
 */

void Navigator::onNetworkEvent(
    		const String& type,
    		PassRefPtr<NavEventCB> eventCB,
    		PassRefPtr<NavServiceErrorCB> errorcb)
{
	m_eventCB = eventCB;

	if (!m_frame)
		return;

	WTF::CString cType(type.ascii());
	char sType[1000];
	readRemoveTypePrefix(cType, sType);

	m_ecb[std::string(sType)] = m_eventCB;

	NavDsc *nd = NavDsc::create(m_frame);
	nd->onUPnPEvent(sType, this);

}

void Navigator::onError(int error)
{
	if (m_errorCBIsSet)
	{
		// Applies to getNetworkServices() only
		PassRefPtr<NavServiceError> err = NavServiceError::create(NavServiceError::NETWORK_ERR);
		m_errorCB->handleEvent(err.get());
	}
}

void Navigator::sendEvent(std::string uuid, std::string stype, std::string body)
{
	std::string name = "";
	UPnPSearch::getInstance()->getUPnPFriendlyName(uuid, stype, name);

	PassRefPtr<NavEvent> event = NavEvent::create();
	event->setPropertyset(WTF::String(body.c_str()));
	event->setUuid(WTF::String(uuid.c_str()));
	event->setServiceType(WTF::String(stype.c_str()));
	event->setFriendlyName(WTF::String(name.c_str()));

	if (m_ecb.find(stype) != m_ecb.end())
		m_ecb[stype]->handleEvent(event.get());
}

void Navigator::UPnPDevAdded(std::string type)
{
	NavDsc *nd = NavDsc::create(m_frame);
	std::map<std::string, UPnPDevice> devs = nd->startUPnPDiscovery(type.c_str());

	PassRefPtr<NavService> srv = NavService::create();
	UPnPDevice d = devs.begin()->second;
	srv->setEventUrl(String(d.eventURL.c_str(), d.eventURL.length()));
	srv->setName(String(d.friendlyName.c_str(), d.friendlyName.length()));
	srv->setServiceType(String(type.c_str(), type.length()));
	srv->setUrl(String(d.descURL.c_str(), d.descURL.length()));
	srv->setuuid(String(d.uuid.c_str(), d.uuid.length()));

	if (m_UPnPserviceAddedCBIsSet && m_uacb.find(type)!=m_uacb.end())
		m_uacb[type]->handleEvent(srv.get());
}
void Navigator::ZCDevAdded(std::string type)
{
	NavDsc *nd = NavDsc::create(m_frame);
	std::map<std::string, ZCDevice> zcdevs = nd->startZeroConfDiscovery(type.c_str());

	PassRefPtr<NavService> srv = NavService::create();
	ZCDevice d = zcdevs.begin()->second;
	srv->setName(String(d.friendlyName.c_str(), d.friendlyName.length()));
	srv->setServiceType(String(type.c_str(), type.length()));
	srv->setUrl(String(d.url.c_str(), d.url.length()));
	srv->setuuid(String(d.friendlyName.c_str(), d.friendlyName.length()));

	if (m_ZCserviceAddedCBIsSet && m_zacb.find(type)!=m_zacb.end())
		m_zacb[type]->handleEvent(srv.get());
}

void Navigator::UPnPDevDropped(std::string type)
{
	std::map<std::string, UPnPDevice> devs;
	std::map<std::string, ZCDevice> zcdevs;

	if (m_UPnPserviceRemovedCBIsSet && m_urcb.find(type)!=m_urcb.end())
	{
		PassRefPtr<NavServices> services = setServices(type.c_str(), devs, zcdevs, UPNP_PROTO);
		m_urcb[type]->handleEvent(services.get());
	}
}
void Navigator::ZCDevDropped(std::string type)
{
	std::map<std::string, UPnPDevice> devs;
	std::map<std::string, ZCDevice> zcdevs;

	if (m_ZCserviceRemovedCBIsSet && m_zrcb.find(type)!=m_zrcb.end())
	{
		PassRefPtr<NavServices> services = setServices(type.c_str(), devs, zcdevs, ZC_PROTO);
		m_zrcb[type]->handleEvent(services.get());
	}
}

Navigator::ProtocolType Navigator::readRemoveTypePrefix(WTF::CString &cType, char *sType)
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

PassRefPtr<NavServices> Navigator::setServices(
		const char* type,
		std::map<std::string, UPnPDevice> devs,
		std::map<std::string, ZCDevice> zcdevs,
		ProtocolType protoType)
{
	PassRefPtr<NavServices> services = NavServices::create(NavServices::CONNECTED);
	Vector<NavService*>* vDevs = new Vector<NavService*>();

	if (protoType == UPNP_PROTO)
	{
		std::map<std::string, UPnPDevice>::iterator it;
		for (it=devs.begin(); it!=devs.end(); it++)
		{
			UPnPDevice d((*it).second);
			if (d.isOkToUse) {
				NavService* nns = new NavService();
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
			ZCDevice d((*it).second);
			if (d.isOkToUse) {
				NavService* nns = new NavService();
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

} // namespace WebCore
