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

    bool changed(const sUPnPDevice& other)
    {
        bool changed = false;
        changed = changed || descURL.compare(other.descURL) != 0;
        changed = changed || friendlyName.compare(other.friendlyName) != 0;
        changed = changed || eventURL.compare(other.eventURL) != 0;
        changed = changed || host.compare(other.host) != 0;
        changed = changed || uuid.compare(other.uuid) != 0;
        changed = changed || isOkToUse != other.isOkToUse;
        return changed;
    }

} UPnPDevice;

#endif /* UPNPDEVICE_H_ */
