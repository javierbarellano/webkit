/*
 * UPnPDevice.h
 *
 *  Created on: Dec 26, 2011
 *      Author: gar
 */

#ifndef UPNPDEVICE_H_
#define UPNPDEVICE_H_

#include <string>

struct UPnPDevice
{
	std::string descURL;
	std::string friendlyName;
	std::string eventURL;
	std::string host;
	std::string port;
	std::string uuid;
    int  contactAttempts;
    bool online;

    UPnPDevice() :
    	  descURL("Bogus")
		, friendlyName("Bogus")
		, eventURL("Bogus")
		, host("Bogus")
		, port("Bogus")
		, uuid("Bogus")
		, contactAttempts(0)
		, online(true)
    {
    }

    bool changed(const UPnPDevice& other)
    {
        bool changed = false;
        changed = changed || descURL.compare(other.descURL) != 0;
        changed = changed || friendlyName.compare(other.friendlyName) != 0;
        changed = changed || eventURL.compare(other.eventURL) != 0;
        changed = changed || host.compare(other.host) != 0;
        changed = changed || uuid.compare(other.uuid) != 0;
        return changed;
    }

};

#endif /* UPNPDEVICE_H_ */
