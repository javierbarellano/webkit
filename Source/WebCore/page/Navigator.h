/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef Navigator_h
#define Navigator_h

#include <string>
#include <map>

#include "Modules/discovery/UPnPDevice.h"
#include "Modules/discovery/ZCDevice.h"
#include "Modules/discovery/NavDsc.h"
#include "NavServices.h"

#include "DOMWindowProperty.h"
#include "NavigatorBase.h"
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
class PluginData;

// Home Networking
class NavDscCB;
class NavServiceErrorCB;
class NavServiceOkCB;
class NavEventCB;

typedef int ExceptionCode;

class Navigator : public NavigatorBase, public RefCounted<Navigator>, public DOMWindowProperty, public Supplementable<Navigator> {
public:
    static PassRefPtr<Navigator> create(Frame* frame) { return adoptRef(new Navigator(frame)); }
    virtual ~Navigator();

    String appVersion() const;
    String language() const;
    DOMPluginArray* plugins() const;
    DOMMimeTypeArray* mimeTypes() const;
    bool cookieEnabled() const;
    bool javaEnabled() const;

    virtual String userAgent() const;

    // ----------------------------------
    // Home networking
    // ----------------------------------
public:
	enum ProtocolType
	{
		UPNP_PROTO = 1,
		ZC_PROTO =2
	};
    void getNetworkServices(
            		const String& type,
            		PassRefPtr<NavServiceOkCB> serviceCB,
            		PassRefPtr<NavServiceErrorCB> errorCB );

    void onNetworkServices(
            		const String& type,
            		PassRefPtr<NavDscCB> serviceAddedCB,
            		PassRefPtr<NavServiceOkCB> serviceRemovedCB,
            		PassRefPtr<NavServiceErrorCB> errorCB);

    void onNetworkEvent(
    		const String& type,
    		PassRefPtr<NavEventCB> msgCB,
    		PassRefPtr<NavServiceErrorCB> errorcb);

    void onError(int error);

    void UPnPDevAdded(std::string type);
    void ZCDevAdded(std::string type);

    void UPnPDevDropped(std::string type);
    void ZCDevDropped(std::string type);

    void sendEvent(std::string uuid, std::string stype, std::string body);

private:
   PassRefPtr<NavServices> setServices(
   		const char* type,
   		std::map<std::string, UPnPDevice> devs,
   		std::map<std::string, ZCDevice> zcdevs,
   		ProtocolType protoType);

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

    // ----------------------------------
    // ----------------------------------

public:

#if ENABLE(POINTER_LOCK)
    PointerLock* webkitPointer() const;
#endif

    // Relinquishes the storage lock, if one exists.
    void getStorageUpdates();

private:
    explicit Navigator(Frame*);

    mutable RefPtr<DOMPluginArray> m_plugins;
    mutable RefPtr<DOMMimeTypeArray> m_mimeTypes;
};

}

#endif
