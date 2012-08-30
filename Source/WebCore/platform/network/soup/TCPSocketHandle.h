/*
 * TCPSocketHandle.h
 *
 *  Created on: May 10, 2012
 *      Author: gar
 */

#ifndef TCPSOCKETHANDLE_H_
#define TCPSOCKETHANDLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "KURL.h"
#include <wtf/text/CString.h>

#include <wtf/RefCounted.h>

namespace WebCore {

class TCPSocketHandleClient;

class TCPSocketHandle : public RefCounted<TCPSocketHandle> {
public:

	static PassRefPtr<TCPSocketHandle> create(const KURL& url)
	{
		return adoptRef(new TCPSocketHandle(url));
	}


	virtual ~TCPSocketHandle()
	{
		if (m_socket >= 0)
			::close(m_socket);

		m_socket = -1;
	}

	virtual int send(const char* data, int length)
	{
		if (m_inError)
			return 0;

		int len = ::send(m_socket, data, length, 0);

		return len;
	}

	void receive(char *data, size_t *len)
	{
		int n = 1;
		int pos = 0;
		size_t lim = *len;
		fd_set read_fds;
		struct timeval timeout;

		if (m_inError)
		{
			*len = 0;
			return;
		}


		while (lim > 0 && (n>0))
		{
			FD_ZERO(&read_fds);
			FD_SET(m_socket, &read_fds);
			timeout.tv_sec = 0;
			timeout.tv_usec = 500*1000;
			int rc = select(m_socket+1, &read_fds, NULL, NULL, &timeout);
			if(rc < 0)
			{
				printf("-ERROR- receive() select() error %d!!!!", errno);
				*len = 0;
				return;
			}
			else if (rc == 0)
				break; // Timeout, we are done.

			n=recv(m_socket, data+pos, lim, 0);


			pos += n;
			lim -= n;
		}

		//			if (pos<=0)
		//			  printf("-ERROR- receive Didn't receive anything!!!!!\n");


		*len = pos;
	}

	KURL getUrl() { return m_url;}

protected:

	virtual void platformClose()
	{
		::close(m_socket);
		m_socket = -1;
	}

private:

	// Constructor
	TCPSocketHandle(const KURL &url)
	{
		int on = 1;
		int rc = 0;

		m_inError = 0; // No pending error

		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket < 0)
		{
			printf("-ERROR- socket() failed err: %d\n", errno);
			m_inError = errno;
			return;
		}

		if((rc = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
		{
			printf("-ERROR- setsockopt(SO_REUSEADDR) failed! errno: %d\n", errno);
			m_inError = errno;
			return;
		}

		WTF::CString cHost = url.host().ascii();

		bzero(&m_servaddr,sizeof(m_servaddr));
		m_servaddr.sin_family = AF_INET;
		m_servaddr.sin_addr.s_addr=inet_addr(cHost.data());
		m_servaddr.sin_port=htons(url.port() ? url.port():80);

		if (connect(m_socket, (struct sockaddr *)&m_servaddr, sizeof(m_servaddr)) < 0)
		{
			printf("-ERROR- connect failed! errno: %d\n", errno);
			m_inError = errno;
			return;
		}

		m_url = url;
	}


	// Private Data
private:
	KURL m_url;
	TCPSocketHandleClient *m_client;
	struct sockaddr_in m_servaddr;
	int m_socket;
	int m_inError;

};

}  // namespace WebCore





#endif /* TCPSOCKETHANDLE_H_ */
