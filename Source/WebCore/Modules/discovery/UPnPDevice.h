/* Copyright (C) 2011, 2012, 2013 Cable Television Laboratories, Inc.
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
