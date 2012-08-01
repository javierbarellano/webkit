/*
 * ZCDevice.h
 *
 *  Created on: Dec 26, 2011
 *      Author: gar
 */

#ifndef ZCDEVICE_H_
#define ZCDEVICE_H_

#include <string>

typedef struct sZCDevice
{
	std::string url;
	std::string friendlyName;
	bool isOkToUse;
} ZCDevice;

#endif /* ZCDEVICE_H_ */
