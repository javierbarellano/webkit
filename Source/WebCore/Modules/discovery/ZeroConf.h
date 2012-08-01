/*
 * ZeroConf.h
 *
 *  Created on: Dec 27, 2011
 *      Author: gar
 */

#ifndef ZEROCONF_H_
#define ZEROCONF_H_

#include <vector>
#include <map>

#ifdef LOG
#undef LOG
#endif
//#include "base/memory/weak_ptr.h"

#include "../../../../Source/WebCore/platform/network/soup/UDPSocketHandle.h"
#include "Modules/discovery/ZCDevice.h"
#include "Modules/discovery/DiscoveryBase.h"

namespace WebCore {

class NavDsc;

typedef struct sZCDevMap
{
	std::map<std::string, ZCDevice> devMap;
} ZCDevMap;



class ZeroConf : public DiscoveryBase
//				 ,public base::SupportsWeakPtr<ZeroConf>
{
public:
	static std::map<std::string, ZCDevice> discoverDevs(const char *type, NavDsc *navDsc);

	static ZeroConf* getInstance();

    // Called when Socket Stream is opened.
    virtual void UDPdidOpenStream(UDPSocketHandle*);

    // Called when |amountSent| bytes are sent.
    virtual void UDPdidSendData(UDPSocketHandle*, int /* amountSent */);

    // Called when data are received.
    virtual void UDPdidReceiveData(UDPSocketHandle*, const char* data, int dLen, const char *hostPort, int hpLen);

    // Called when Socket Stream is closed.
    virtual void UDPdidClose(UDPSocketHandle*);

    // Called when Socket Stream has an error.
    virtual void UDPdidFail(UDPSocketHandle*, UDPSocketError&);

	void initQuery();

	// White list imp
	virtual bool hostPortOk(const char* host, int port);

	void checkForDroppedDevs();

	char* query_;
	int queryLen_;

protected:

    virtual bool parseDev(const char* resp, size_t respLen, const char* hostPort);

private:
	static ZeroConf* instance_;
	static char prefix_[];
	static int prfixLen_;

	static char postfix_[];
	static int postfixLen_;

	ZeroConf(const char *type);
	virtual ~ZeroConf();

	// Return pos
	int parseRec(const char* resp, size_t respLen, int pos, int numRecs, std::string& name);

	// Return pos
	int cxSupport(const char* resp, int pos, std::string& subName, int curPos);

	// Return pos
	int setName(const char* resp, int pos, std::string& name);

	// key == service type
	// dev key == UUID
	std::map<std::string, ZCDevMap> devs_;

	long lastSend_;

	bool sendPending_;

};

}

#endif /* ZEROCONF_H_ */
