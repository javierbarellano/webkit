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

#ifndef TCPSocketHandle_h
#define TCPSocketHandle_h

#include "URL.h"
#include "Logging.h"
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
#include <wtf/RefCounted.h>
#include <wtf/text/CString.h>

namespace WebCore {

class TCPSocketHandleClient;

class TCPSocketHandle : public RefCounted<TCPSocketHandle> {
public:
    static PassRefPtr<TCPSocketHandle> create(const char *url, int port)
    {
        return adoptRef(new TCPSocketHandle(url, port));
    }

    virtual ~TCPSocketHandle()
    {
        if (m_socket >= 0)
            ::close(m_socket);

        m_socket = -1;
    }

    virtual int getError()
    {
        return m_inError;
    }

    virtual int send(const char* data, int length)
    {
        if (m_socket < 0) {
            init(m_host, m_port);
            if (m_socket < 0)
                return 0;
        }

        if (m_inError)
            return 0;

        int len = ::send(m_socket, data, length, 0);

        return len;
    }

    void receive(char *data, size_t *len)
    {
        if (m_socket < 0) {
            init(m_host, m_port);
            if (m_socket < 0)
                return;
        }

        int n = 1;
        int pos = 0;
        size_t lim = *len;
        fd_set readFds;
        struct timeval timeout;
        const int mSec = 1000;

        if (m_inError) {
            *len = 0;
            return;
        }

        while (lim > 0 && (n > 0)) {
            FD_ZERO(&readFds);
            FD_SET(m_socket, &readFds);
            timeout.tv_sec = 0;
            timeout.tv_usec = 1500 * mSec;
            int rc = select(m_socket+1, &readFds, 0, 0, &timeout);
            if (rc < 0) {
                LOG_ERROR("-ERROR- receive() select() error %d!!!!", errno);
                *len = 0;
                return;
            }
            if (!rc)
                break; // Timeout, we are done.

            n = recv(m_socket, data + pos, lim, 0);

            pos += n;
            lim -= n;
        }

        if (pos <= 0)
            LOG_ERROR("-ERROR- receive Didn't receive anything!!!!!\n");

        *len = pos;
    }

    URL getUrl() { return m_url; }

protected:

    virtual void platformClose()
    {
        if (m_socket >= 0) {
            ::close(m_socket);
            m_socket = -1;
        }
    }

private:

    TCPSocketHandle(const char *host, int port)
    {
        init(host, port);
    }

    void init(const char *host, int port)
    {
        int on = 1;
        int rc = 0;

        strcpy(m_host, host);
        m_port = port;

        m_inError = 0; // No pending error

        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_socket < 0) {
            LOG_ERROR("-ERROR- socket() failed err: %d\n", errno);
            m_inError = errno;
            return;
        }

        if ((rc = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on))) < 0) {
            LOG_ERROR("-ERROR- setsockopt(SO_REUSEADDR) failed! errno: %d\n", errno);
            m_inError = errno;
            ::close(m_socket);
            m_socket = -1;
            return;
        }

        on = 1;
        if ((rc = setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&on, sizeof(on))) < 0) {
            LOG_ERROR("-ERROR- setsockopt(SO_KEEPALIVE) failed! errno: %d\n", errno);
            m_inError = errno;
            ::close(m_socket);
            m_socket = -1;
            return;
        }

        bzero(&m_servaddr, sizeof(m_servaddr));
        m_servaddr.sin_family = AF_INET;
        m_servaddr.sin_addr.s_addr = inet_addr(host);
        m_servaddr.sin_port = htons(port ? port:80);

        if (connect(m_socket, (struct sockaddr*)&m_servaddr, sizeof(m_servaddr)) < 0) {
            LOG_ERROR("-ERROR- connect to: %s:%d failed! errno: %d\n", host, port, errno);
            m_inError = errno;
            ::close(m_socket);
            m_socket = -1;
            return;
        }

        std::stringstream ss;
        ss << "http://" << host << ":" << port;
        // URL url(ParsedURLString, String(ss.str().c_str(), ss.str().length()));
        // m_url = url;
    }

private:
    URL m_url;
    TCPSocketHandleClient *m_client;
    struct sockaddr_in m_servaddr;
    int m_socket;
    int m_inError;
    char m_host[1024];
    int m_port;
};

} // namespace WebCore

#endif /* TCPSocketHandle_h */
