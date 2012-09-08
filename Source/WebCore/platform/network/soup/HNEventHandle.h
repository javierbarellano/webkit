/*
 * HNEventHandle.h
 *
 *  Created on: Aug 24, 2012
 *      Author: gar
 */

#ifndef HNEVENTHANDLE_H_
#define HNEVENTHANDLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "HNEventServerHandleClient.h"
#include "HNEventServerHandle.h"

namespace WebCore {

class HNEventServerHandleClient;
void eventServerThread(void *context);

class HNEventHandle : public HNEventServerHandle
{
public:
	HNEventHandle(HNEventServerHandleClient *client) {
		m_client = client;
		m_socket = -1;
	}

    ~HNEventHandle() { }

	virtual void set_client(HNEventServerHandleClient *client) {
		m_client = client;
	}

    // Start Home Networking Event server.
    virtual void connect() {
    	if (m_socket < 0) {
    		(void)WTF::createThread(eventServerThread, this, "HN Event Server");
    	}
    }

    // Send Subscribe request to UPnP device
    virtual bool send(const char *data, int len) {
    	return false;
    }

    // Receive Events
    virtual void receive() {

    }

    // Close the server.
    virtual void close() {

    }

    int m_socket;
    int m_port;
    HNEventServerHandleClient *m_client;

private:


};

void eventServerThread(void *context)
{
	/*
	HTTP/1.1 200 OK
	Date: Mon, 27 Aug 2012 17:50:35 GMT
	Server: Apache/2.2.8 (Ubuntu) PHP/5.2.4-2ubuntu5.10 with Suhosin-Patch
	Last-Modified: Tue, 01 Sep 2009 16:34:42 GMT
	ETag: "2404b7-1e76-47286ba340c80"
	Accept-Ranges: bytes
	Content-Length: 7798
	Keep-Alive: timeout=15, max=100
	Connection: Keep-Alive
	Content-Type: text/css
	*/

	char http200Resp[] = "HTTP/1.1 200 OK/r/nKeep-Alive: timeout=15, max=100/r/nAccept-Ranges: bytes/r/nConnection: Keep-Alive/r/n/r/n";
	HNEventHandle* hnEventHandle = (HNEventHandle*)context;
	hnEventHandle->m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (hnEventHandle->m_socket == -1) {
		printf("HNEventServer Socket failed! errno: %d\n", errno);
		return;
	}

	int yes=1;
    if (setsockopt(hnEventHandle->m_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
            sizeof(int)) == -1) {
    	perror("server: setsockopt ");
        return;
    }

    sockaddr_in sa;
    bzero((char *) &sa, sizeof(sa));

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    hnEventHandle->m_port = 2777;
    sa.sin_port = htons(hnEventHandle->m_port);

    if (bind(hnEventHandle->m_socket, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
        ::close(hnEventHandle->m_socket);
        perror("server: bind");
        return;
    }

    listen(hnEventHandle->m_socket, 5);

    struct sockaddr_in their_addr; // connector's address information
    socklen_t sin_size = sizeof(their_addr);
    while(1) {
    	int new_fd = ::accept(hnEventHandle->m_socket, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("server: accept");
            return;
        }

        std::stringstream ss;
        char bf[8024];
        int len = 0;
        int total = 0;
        do {
        	len = ::recv(new_fd, &bf[total], sizeof(bf-1), 0);
        	total += len;
        } while (len>0);

		bf[total] = 0;

        ::send(new_fd, http200Resp, strlen(http200Resp), 0);

        hnEventHandle->m_client->HNdidReceiveData(hnEventHandle, bf, total, NULL, 0);
    }

	::close(hnEventHandle->m_socket);
}

}; // namespace WebCore


#endif /* HNEVENTHANDLE_H_ */
