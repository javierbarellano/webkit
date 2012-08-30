/*
 * HNEventHandleClient.h
 *
 *  Created on: Feb 22, 2012
 *      Author: gar
 */

#ifndef HNEVENTSERVERHANDLECLIENT_H_
#define HNEVENTSERVERHANDLECLIENT_H_

#include "HNEventError.h"
#include "HNEventServerHandle.h"

namespace WebCore {

class HNEventServerHandleClient {
public:

    // Called when Socket Stream is opened.
    virtual void HNdidOpenStream(HNEventServerHandle*) = 0;

    // Called when |amountSent| bytes are sent.
    virtual void HNdidSendData(HNEventServerHandle*, int /* amountSent */) = 0;

    // Called when data are received.
    virtual void HNdidReceiveData(HNEventServerHandle*, const char *data, int dLen, const char *hostPort, int hpLen) = 0;

    // Called when Socket Stream is closed.
    virtual void HNdidClose(HNEventServerHandle*) = 0;

    // Called when Socket Stream has an error.
    virtual void HNdidFail(HNEventServerHandle*, HNEventError&) = 0;

};

}  // namespace WebKit


#endif /* HNEVENTSERVERHANDLECLIENT_H_ */
