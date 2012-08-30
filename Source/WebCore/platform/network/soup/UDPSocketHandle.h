/*
 * UDPSocketHandle.h
 *
 *  Created on: Apr 24, 2012
 *      Author: gar
 */

#ifndef UDPSOCKETHANDLE_H_
#define UDPSOCKETHANDLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>

#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <wtf/text/CString.h>

#include <wtf/RefCounted.h>
#include "UDPSocketHandleBase.h"

namespace WebCore {

class UDPSocketHandleClient;

class UDPSocketHandle : public RefCounted<UDPSocketHandle>, public UDPSocketHandleBase {
public:
	static PassRefPtr<UDPSocketHandle> create(const KURL& url, bool isMulticastGroup, UDPSocketHandleClient* client)
	{
		return adoptRef(new UDPSocketHandle(url, isMulticastGroup, client));
	}


	virtual ~UDPSocketHandle()
	{
		if (m_socket >= 0)
			::close(m_socket);

		m_socket = -1;
	}

protected:

	virtual int platformSend(const char* data, int length)
	{
		int len = sendto(m_socket, data, length, 0, (struct sockaddr *)&m_servaddr, sizeof(m_servaddr));

		if (m_client)
			m_client->UDPdidSendData(this, len);

		return len;
	}

	virtual void platformReceive()
	{

		char bf[8192];
		struct sockaddr_in si_rec;
		socklen_t siLen = sizeof(si_rec);
		fd_set read_fds;
		struct timeval timeout;
		FD_ZERO(&read_fds);
		FD_SET(m_socket, &read_fds);
		timeout.tv_sec = 0;
		timeout.tv_usec = 500*1000;
		int rc = select(m_socket+1, &read_fds, NULL, NULL, &timeout);
		if(rc < 0)
		{
			printf("-ERROR- UDP platformReceive() select() error %d!!!!", errno);
			return;
		}
		else if (rc == 0)
			return; // Timeout, we are done.

		int n=recvfrom(m_socket, bf, sizeof(bf), 0, (sockaddr*)&si_rec, &siLen);
		bf[n] = 0;

		if (n<=0)
			printf("platformReceive Didn't receive anything!!!!!\n");

		in_addr_t addr = si_rec.sin_addr.s_addr;
		std::stringstream ss;
		ss      <<  (addr & 0x0000ff)
				<< "." << ((addr & 0x00ff00) >> 8)
				<< "." << ((addr & 0xff0000) >> 16)
				<< "." <<  (addr >> 24)
				<< ":" << si_rec.sin_port;

		m_client->UDPdidReceiveData(this, bf, n, ss.str().c_str(), ss.str().length());
	}

	virtual void platformClose()
	{

	}

private:

	UDPSocketHandle(const KURL &url, bool isMulticastGroup, UDPSocketHandleClient *client) :
		UDPSocketHandleBase(url, isMulticastGroup, client)
	{

		m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_socket < 0)
		{
			printf("-ERROR- socket() failed err: %d\n", errno);
		}
		else
			printf("Socket(%d) set up.\n",m_socket);

		WTF::CString cHost = url.host().ascii();
		printf("HOST: %s, port: %d\n", cHost.data(), url.port());

		bzero(&m_servaddr,sizeof(m_servaddr));
		m_servaddr.sin_family = AF_INET;
		m_servaddr.sin_addr.s_addr=inet_addr(cHost.data());
		m_servaddr.sin_port=htons(url.port());

		m_url = url;
		m_client = client;
		m_isMulticast = isMulticastGroup;
	}


	// Private Data
private:
	KURL m_url;
	UDPSocketHandleClient *m_client;
	struct sockaddr_in m_servaddr;
	bool m_isMulticast;
	int m_socket;

};

}  // namespace WebCore



#endif /* UDPSOCKETHANDLE_H_ */
