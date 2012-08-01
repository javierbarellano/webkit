/*
 * UDPSocketHandleClient.h
 *
 *  Created on: Dec 7, 2011
 *      Author: gar
 */

#ifndef UDPSOCKETHANDLECLIENT_H_
#define UDPSOCKETHANDLECLIENT_H_

namespace WebCore {

class UDPSocketError;
class UDPSocketHandle;

class UDPSocketHandleClient {
public:

    virtual ~UDPSocketHandleClient(){}

    // Called when Socket Stream is opened.
    virtual void UDPdidOpenStream(UDPSocketHandle*) = 0;

    // Called when |amountSent| bytes are sent.
    virtual void UDPdidSendData(UDPSocketHandle*, int /* amountSent */) = 0;

    // Called when data are received.
    virtual void UDPdidReceiveData(UDPSocketHandle*, const char* data, int dLen, const char *hostPort, int hpLen) = 0;

    // Called when Socket Stream is closed.
    virtual void UDPdidClose(UDPSocketHandle*) = 0;

    // Called when Socket Stream has an error.
    virtual void UDPdidFail(UDPSocketHandle*, UDPSocketError&) = 0;

};

} // namespace WebKit

#endif /* UDPSOCKETHANDLECLIENT_H_ */
