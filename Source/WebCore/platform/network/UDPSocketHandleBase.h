/*
 * UDPSocketHandleBase.h
 *
 *  Created on: Apr 24, 2012
 *      Author: gar
 */

#ifndef UDPSOCKETHANDLEBASE_H_
#define UDPSOCKETHANDLEBASE_H_

#include "KURL.h"
#include <wtf/Vector.h>

#include "UDPSocketHandleClient.h"

namespace WebCore {

    class UDPSocketHandleBase {
    public:
        virtual ~UDPSocketHandleBase() { }

        // Send UDP socket data on the socket.
        virtual bool send(const char *data, size_t len){int sent = (int)platformSend(data, (int)len); return sent==(int)len;}

        // unblocking receive on UDP socket
        virtual void receive(){platformReceive();}

        // Close the socket stream.
        virtual void close() {platformClose();}

    protected:
        UDPSocketHandleBase(const KURL& url, bool isMulticastGroup, UDPSocketHandleClient* client){}

        virtual int platformSend(const char* data, int length) = 0;
        virtual void platformReceive() = 0;
        virtual void platformClose() = 0;

        KURL m_url;
        UDPSocketHandleClient* m_client;
        Vector<char> m_buffer;
    };

}  // namespace WebCore

#endif /* UDPSOCKETHANDLEBASE_H_ */
