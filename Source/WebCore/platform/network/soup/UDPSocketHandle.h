/* Copyright (C) 2012, 2013 Cable Television Laboratories, Inc.
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

#ifndef UDPSocketHandle_h
#define UDPSocketHandle_h

#include "UDPSocketHandleBase.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <wtf/Assertions.h>
#include <wtf/Forward.h>
#include <wtf/RefCounted.h>
#include <wtf/text/CString.h>

namespace WebCore {

class UDPSocketHandleClient;

class UDPSocketHandle : public RefCounted<UDPSocketHandle>, public UDPSocketHandleBase {
public:
    static PassRefPtr<UDPSocketHandle> create(const URL& url, bool isMulticastGroup, UDPSocketHandleClient* client)
    {
        return adoptRef(new UDPSocketHandle(url, isMulticastGroup, client));
    }

    virtual ~UDPSocketHandle()
    {
        if (m_socket >= 0)
            ::close(m_socket);

        m_socket = -1;
    }

    virtual bool connected()
    {
        return !m_connectionFailed;
    }

protected:

    virtual int platformSend(const char* data, int length)
    {
        if (m_connectionFailed)
            return -1;

        int len = sendto(m_socket, data, length, 0, (struct sockaddr*)&m_servaddr, sizeof(m_servaddr));

        if (m_client)
            m_client->UDPdidSendData(this, len);

        return len;
    }

    virtual void platformReceive()
    {
        if (m_connectionFailed)
            return;


        char bf[8192];
        struct sockaddr_in sirec;
        socklen_t siLen = sizeof(sirec);
        fd_set readFds;
        struct timeval timeout;
        FD_ZERO(&readFds);
        FD_SET(m_socket, &readFds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 500 * 1000;
        int rc = select(m_socket + 1, &readFds, 0, 0, &timeout);
        if (rc < 0) {
            // printf( "-ERROR- UDP platformReceive() select() error %d!!!!", errno);
            return;
        }
        if (!rc)
            return; // Timeout, we are done.

        int n = recvfrom(m_socket, bf, sizeof(bf), 0, (sockaddr*)&sirec, &siLen);
        bf[n] = 0;

        // if (n<=0)
            // printf("platformReceive Didn't receive anything!!!!!\n");

        in_addr_t addr = sirec.sin_addr.s_addr;
        std::stringstream ss;
        ss << (addr & 0x0000ff)
            << "." << ((addr & 0x00ff00) >> 8)
            << "." << ((addr & 0xff0000) >> 16)
            << "." <<  (addr >> 24)
            << ":" << sirec.sin_port;

        m_client->UDPdidReceiveData(this, bf, n, ss.str().c_str(), ss.str().length());
    }

    virtual void platformClose()
    {
    }

private:

    UDPSocketHandle(const URL &url, bool isMulticastGroup, UDPSocketHandleClient *client)
        : UDPSocketHandleBase(url, isMulticastGroup, client)
    {
        m_connectionFailed = true;

        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket < 0) {
            // printf("-ERROR- socket() failed err: %d\n", errno);
            return;
        }
//        else
//            printf("Socket(%d) set up.\n",m_socket);

        WTF::CString cHost = url.host().ascii();
        // printf("HOST: %s, port: %d\n", cHost.data(), url.port());

        bzero(&m_servaddr, sizeof(m_servaddr));
        m_servaddr.sin_family = AF_INET;
        m_servaddr.sin_addr.s_addr = inet_addr(cHost.data());
        m_servaddr.sin_port = htons(url.port());

        m_url = url;
        m_client = client;
        m_isMulticast = isMulticastGroup;

        // Setup multicast group
        if (m_isMulticast) {
            struct ip_mreq imr;

            imr.imr_multiaddr.s_addr = inet_addr(cHost.data());
            imr.imr_interface.s_addr = htonl(INADDR_ANY);

            if (setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(struct ip_mreq)) < 0)
                fprintf(stderr, "UDPSocketHandle::setsockopt - IP_ADD_MEMBERSHIP failed\n");
            else {

                int reuse = 1;
                setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (int*)&reuse, sizeof(reuse));

                char loop = 0;
                setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(char));

                const struct sockaddr* addr = (sockaddr*)&m_servaddr;
                if (bind(m_socket, addr, sizeof(m_servaddr)) < 0) {
                    int lastError = errno;
                    fprintf(stderr, "UDPSocketHandle::bind failed: %d - %s\n", lastError, strerror(lastError));
                    return;
                }
            }
        }

        m_connectionFailed = false;
    }

private:
    URL m_url;
    UDPSocketHandleClient* m_client;
    struct sockaddr_in m_servaddr;
    bool m_isMulticast;
    int m_socket;
    bool m_connectionFailed;
};

} // namespace WebCore

#endif /* UDPSocketHandle_h */
