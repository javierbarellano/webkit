/*
 * IDiscoveryAPI.h
 *
 *  Created on: Aug 6, 2012
 *      Author: gar
 */

#ifndef IDISCOVERYAPI_H_
#define IDISCOVERYAPI_H_

#include <string>
#include <vector>

namespace WebCore {

class RUISever {
public:
	std::string name;
	std::string url;
	std::string iconUrl;
};

class RUIServerList {
public:

	void clear() {
		servers.clear();
	}

	size_t size() {
		return (size_t)servers.size();
	}

	RUISever operator[](unsigned int index) {
		RUISever rs;

		rs.name = "";
		rs.url = "";
		rs.iconUrl = "";

		if (index>=servers.size())
			return (rs);

		return (servers[index]);
	}

private:
		std::vector< RUISever> servers;
};


class RUICallback {
public:
	virtual ~RUICallback(){}

	virtual void serverAdded() = 0;
	virtual void serverDropped() = 0;
};


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

};

};  // NameSpace WebCore


#endif /* IDISCOVERYAPI_H_ */
