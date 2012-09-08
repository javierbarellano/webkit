/*
 * DiscoveryBase.h
 *
 *  Created on: Dec 27, 2011
 *      Author: gar
 */

#ifndef DISCOVERYBASE_H_
#define DISCOVERYBASE_H_

#include <sstream>
#include <string>

#include "platform/network/UDPSocketHandleClient.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketHandle.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketError.h"

#include "platform/network/HNEventServerHandle.h"

namespace WebCore
{

class NavDsc;

class DiscoveryBase : public UDPSocketHandleClient
{
public:
	DiscoveryBase(const char *type);
	virtual ~DiscoveryBase();

	static void socketSend(const char *url, int port, const char *toSend, size_t sLen, char *bf, size_t *len);
	static void HTTPget(const char *host, int port, const char *path, char *bf, size_t *len);
	static void HTTPget(KURL &url, char *bf, size_t *len);
	static void HTTPpost(KURL &url, char *postBody, char *optHeaders, char *bf, size_t *len);

	// White list imp
	virtual bool hostPortOk(const char* host, int port)=0;

	virtual void getHostPort(const char *url, char* host, int *port);


	bool canReceiveAnotherDev() {return canReceiveAnotherDev_;}
	void resetCanReceiveAnotherDev() {canReceiveAnotherDev_ = false;}

	static void hexDump(const char *data, int len, int pos=0);

	UDPSocketHandle* socketHandle_;

	HNEventServerHandle* serverHandle_;


	bool m_stillRunning;
	bool m_droppedStillRunning;

	mutable RefPtr<UDPSocketHandle> m_udpSocket;

	bool stopDicovery_;
	bool threadDone_;

protected:

	virtual bool parseDev(const char* resp, size_t respLen, const char* hostPort)=0;

	bool canReceiveAnotherDev_;

	char url_[1024];

	std::string cur_type_;

	NavDsc *navDsc_;

	ThreadIdentifier m_tID;
	ThreadIdentifier m_tDroppedID;

};

} // namespace WebCore

#endif /* DISCOVERYBASE_H_ */
