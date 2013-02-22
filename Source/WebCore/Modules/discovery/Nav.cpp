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
		devs = nd->startUPnPDiscovery(sType, successcb, errorcb);

	} else if (protoType == ZC_PROTO) {
		if (reset) {
			nd->zcReset();
		}
		zcdevs = nd->startZeroConfDiscovery(sType, successcb, errorcb);
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
}

void Nav::ZCDevAdded(std::string type)
{
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

};

#endif // DISCOVERY

