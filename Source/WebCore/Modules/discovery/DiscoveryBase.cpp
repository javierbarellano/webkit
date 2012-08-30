/*
 * DiscoveryBase.cpp
 *
 *  Created on: Dec 27, 2011
 *      Author: gar
 */

#include "config.h"

#if ENABLE(DISCOVERY)

#define WEBKIT_IMPLEMENTATION 1

#include "../../../../Source/WebCore/platform/network/soup/TCPSocketHandle.h"

#include "../../../../Source/WebCore/platform/network/UDPSocketHandleClient.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketHandle.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketError.h"

#include "NavDsc.h"
#include "DiscoveryBase.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

#include <sstream>
#include <string>
#include <iomanip>
#include <map>
#include <stdlib.h>
#include <stdio.h>

namespace WebCore
{

DiscoveryBase::DiscoveryBase(const char *type) :
		cur_type_(type)
{
	canReceiveAnotherDev_ = false;
	threadDone_ = true;
	socketHandle_ = NULL;
	serverHandle_ = NULL;
}

DiscoveryBase::~DiscoveryBase()
{
	stopDicovery_ = true;
	while (!threadDone_) usleep(50000); // 50 ms

}


void DiscoveryBase::getHostPort(const char *url, char* host, int *port)
{

	int start = 0;
	if (strstr(url, "://"))
	{
		start = strstr(url, "://") - url + 3;
	}

	strcpy(host, &url[start]);
	int end = strstr(host, ":")-host;
	host[end] = 0;

	char sPort[100];
	strncpy(sPort, &host[end+1],20);
	end = strchr(sPort,'/')-sPort;
	sPort[end] = 0;
	*port = atoi(sPort);
}

void DiscoveryBase::HTTPget(KURL &url, char *bf, size_t *len)
{
	/*
	GET /upnphost/udhisapi.dll?content=uuid:6a66eb21-7c9c-4699-a49d-f47752c5afd5 HTTP/1.1
	User-Agent: Platinum/0.5.3.0, DLNADOC/1.50
	Host: 10.36.0.237:2869
	*/

	RefPtr<TCPSocketHandle> socket = TCPSocketHandle::create(url);

	String path = url.path();
	String query = url.query();
	std::string req = "GET ";
	req.append(path.ascii().data());
	if (query.length())
	{
		req += "?";
		req.append(query.ascii().data());
	}

	req += " HTTP/1.1\r\nUser-Agent: Platinum/0.5.3.0, DLNADOC/1.50\r\nHost: ";
	req.append(url.host().ascii().data());
	req += ":";

	std::stringstream ss;
	int port = url.port() ? url.port() : 80;
	ss << req << port << "\r\n\r\n";

	socket->send(ss.str().c_str(), ss.str().length());
	socket->receive(bf, len);

}

// static
void DiscoveryBase::hexDump(const char *data, int len, int pos)
{
	std::stringstream ss;
	int start = 0; //pos;
	int ln = len;
	const char *bf = data;
	for (int row=pos/16; row<ln/16; row++)
	{
		int off = row*16;

		for (int x = 0; x < start; x++) ss << "   ";

		for (int x = start; x<start+16 && (x+off)<ln; x++)
		{
			if (x == 8) ss << "  ";
			ss << std::setfill ('0') << std::setw(sizeof(char)*2) << std::hex << ((int)bf[x+off]&0xff) << " ";
		}

		ss << "    ";

		for (int x = start; x<start+16 && (x+off)<ln; x++)
		{
			if (x == 8) ss << "  ";
			ss << (((bf[x+off]>=' ' && bf[x+off]<='~') ? bf[x+off]:'.'));
		}

		ss << "\n";
		start = 0;
	}

	printf("%s",ss.str().c_str());
}




}; // WebCore Namespace

#endif // ENABLE_DISCOVERY

