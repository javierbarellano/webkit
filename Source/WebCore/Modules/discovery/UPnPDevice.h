/*
 * UPnPDevice.h
 *
 *  Created on: Dec 26, 2011
 *      Author: gar
 */

#ifndef UPNPDEVICE_H_
#define UPNPDEVICE_H_

#include <string>

typedef struct sUPnPDevice
{
	std::string descURL;
	std::string friendlyName;
	std::string eventURL;
	std::string host;
	std::string port;
	std::string uuid;
	bool isOkToUse;
} UPnPDevice;

#endif /* UPNPDEVICE_H_ */
