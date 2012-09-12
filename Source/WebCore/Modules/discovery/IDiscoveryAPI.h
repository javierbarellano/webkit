/*
 * IDiscoveryAPI.h
 *
 *  Created on: Aug 6, 2012
 *      Author: gar
 */

#ifndef IDISCOVERYAPI_H_
#define IDISCOVERYAPI_H_

#include <string>
#include <map>

#include "Modules/discovery/UPnPDevice.h"


namespace WebCore {

class IDiscoveryAPI
{
public:
	virtual ~IDiscoveryAPI(){}

	virtual void UPnPDevAdded(std::string type) = 0;
	virtual void UPnPDevDropped(std::string type) = 0;

    virtual void ZCDevAdded(std::string type) = 0;
    virtual void ZCDevDropped(std::string type) = 0;

	virtual void sendEvent(std::string uuid, std::string stype, std::string body) = 0;
	virtual void onError(int error) = 0;

	virtual void serverListUpdate(std::string type, std::map<std::string, UPnPDevice> *devs) = 0;

};

};  // NameSpace WebCore


#endif /* IDISCOVERYAPI_H_ */
