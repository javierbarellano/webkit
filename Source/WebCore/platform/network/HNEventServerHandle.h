/*
 * HNEventServerHandle.h
 *
 *  Created on: Feb 22, 2012
 *      Author: gar
 */

#ifndef HNEVENTSERVERHANDLE_H_
#define HNEVENTSERVERHANDLE_H_



namespace WebCore {

class HNEventServerHandleClient;



// Base class for UDPSocketHandleImpl
// see webkit/glue/UDPsockethandle_impl.h

class HNEventServerHandle {
public:
    virtual ~HNEventServerHandle() { }

	virtual void set_client(HNEventServerHandleClient *client)=0;

    // Start Home Networking server for eventing support.
    virtual void connect(const char *url, bool isMulticastGroup) = 0;

    // Send Subscribe request to UPnP device
    virtual bool send(const char *data, int len) = 0;

    // Receive Events
    virtual void receive() = 0;

    // Close the server.
    virtual void close() = 0;

};

} // namespace WebKit


#endif /* HNEVENTSERVERHANDLE_H_ */
