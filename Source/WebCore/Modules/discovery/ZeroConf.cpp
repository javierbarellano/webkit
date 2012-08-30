/*
 * ZeroConf.cpp
 *
 *  Created on: Dec 27, 2011
 *      Author: gar
 */

#include "config.h"
#include "config.h"

#if ENABLE(DISCOVERY)

#define WEBKIT_IMPLEMENTATION 1

#include "../../../../Source/WebCore/platform/network/soup/TCPSocketHandle.h"

#include "../../../../Source/WebCore/platform/network/UDPSocketHandleClient.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketHandle.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketError.h"

//#include "base/bind.h"

#include "NavDsc.h"
#include "ZeroConf.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/time.h>

#include <sstream>
#include <string>
#include <map>

#include <stdlib.h>
#include <stdio.h>

#define SKIP true
#define GET_NAME false

namespace WebCore
{

ZeroConf* ZeroConf::instance_ = NULL;

char ZeroConf::prefix_[] = {
      0x00,0x00,           // Transaction ID
      0x00,0x00,           // Flags (Standard query)
      0x00,0x01,           // Questions
      0x00,0x00,           // Answers RRs
      0x00,0x00,           // Authority RRs
      0x00,0x00            // Additional RRs
};
int ZeroConf::prfixLen_ = 12;

char ZeroConf::postfix_[] = {
      0x04,0x5f,0x74,0x63,0x70,                       // _tcp
      0x05,0x6c,0x6f,0x63,0x61,0x6c,                  // local
      0x00,

      0x00,0x0c,           // Type: PTR (Domain name pointer)
      0x00,0x01      	  // Class: IN, "QU" question: false (QM)
      };
int ZeroConf::postfixLen_ = 16;


void zcDroppedDevsThread(void *context)
{
	long lastSend = -1L;
	ZeroConf* zc = (ZeroConf*)context;
	zc->m_droppedStillRunning = true;

	while (zc->m_droppedStillRunning)
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		long now_ms = (now.tv_sec*1000L + now.tv_usec/1000.0) + 0.5;

		if (lastSend < 0L || (now_ms - lastSend) > 25000)
		{
			lastSend = now_ms;
			zc->checkForDroppedDevs();
		}

		usleep(1000L*1000L); // 1000 ms
	}
}

void zcDiscoveryThread(void *context)
{
	long lastSend = -1L;
	ZeroConf* zc = (ZeroConf*)context;

	zc->m_stillRunning = true;

	while (zc->m_stillRunning) {
		struct timeval now;
		gettimeofday(&now, NULL);
		long now_ms = (now.tv_sec*1000L + now.tv_usec/1000.0) + 0.5;

		if (lastSend < 0L || (now_ms - lastSend) > 25000)
		{
			lastSend = now_ms;
			zc->m_udpSocket->send(zc->query_, zc->queryLen_);
		}

		zc->m_udpSocket->receive();
		usleep(100L*1000L); // 100 ms
	}
}

// static
std::map<std::string, ZCDevice> ZeroConf::discoverDevs(const char *type, NavDsc *navDsc)
{
 	if (!instance_)
 	{
 		instance_ = new ZeroConf(type);
		KURL url(ParsedURLString, String(instance_->url_));

		instance_->m_udpSocket = UDPSocketHandle::create(url, true, instance_) ;
		instance_->m_tID = WTF::createThread(zcDiscoveryThread, instance_, "ZC_discovery");
		instance_->m_tDroppedID = WTF::createThread(zcDroppedDevsThread, instance_, "DroppedZC");
 	}
 	instance_->cur_type_ = std::string(type);
 	instance_->navDsc_ = navDsc;

 	if (instance_->devs_.find(std::string(type)) != instance_->devs_.end())
 	{
 		ZCDevMap dm = instance_->devs_.find(std::string(type))->second;
 		return (dm.devMap);
 	}

	return std::map<std::string, ZCDevice>();
}

// static
ZeroConf* ZeroConf::getInstance()
{
	if (!instance_)
		instance_ = new ZeroConf(NULL);

	return instance_;
}

ZeroConf::ZeroConf(const char *type) :
				DiscoveryBase(type)
{
 	devs_.clear();

 	strcpy(url_, "udp://224.0.0.251:5353");

 	initQuery();
}

ZeroConf::~ZeroConf()
{
	devs_.clear();
	delete[] query_;
}


// Called when Socket Stream is opened.
void ZeroConf::UDPdidOpenStream(UDPSocketHandle* handle)
{
}


// Called when |amountSent| bytes are sent.
void ZeroConf::UDPdidSendData(UDPSocketHandle* handle, int amountSent)
{
}

// Called when data are received.
void ZeroConf::UDPdidReceiveData(UDPSocketHandle*, const char* data, int dLen, const char *hostPort, int hpLen)
{
	if (dLen > 0)
	{
		//hexDump(data, dLen);
		parseDev(data, dLen, hostPort);
	}
}

void ZeroConf::UDPdidClose(UDPSocketHandle* udpHandle)
{
}

void ZeroConf::UDPdidFail(UDPSocketHandle* handle, UDPSocketError& error)
{
// 	printf("ZeroConf::didFail() UDPSocketHandle error: %d\n", error.getErr());
	navDsc_->onZCError(error.getErr());
}

void ZeroConf::checkForDroppedDevs()
{
	//printf("checkForDroppedDevs() start\n");
	for (std::map<std::string, ZCDevMap>::iterator i = devs_.begin(); i != devs_.end(); i++)
	{
		std::vector<std::string> dropMe;
		ZCDevMap dm = (*i).second;
		std::string type = (*i).first;
		for (std::map<std::string, ZCDevice>::iterator k = dm.devMap.begin(); k != dm.devMap.end(); k++)
		{
			ZCDevice dv = (*k).second;
			if (dv.isOkToUse)
			{
				std::string path = dv.url+"/databases/1/containers";
				KURL url(ParsedURLString, String(path.c_str()));
				char bf[8000];
				size_t len = sizeof(bf)-1;
				//printf("Try: %s\n", path.c_str());
				HTTPget(url, bf, &len);

				// Get Description of servcie to insure the device is still working
 				if (len > 0)
					continue;

				// Found one that dropped
				dropMe.push_back((*k).first);
			}
		}

		if (dropMe.size())
		{
			for (int d=0; d<(int)dropMe.size(); d++)
				dm.devMap.erase(dm.devMap.find(dropMe.at(d)));

			devs_[type] = dm;
			navDsc_->lostZCDev(type);
		}
	}

}

bool ZeroConf::parseDev(const char* resp, size_t respLen, const char* hostPort)
{
	ZCDevice zcd;

	//printf("parseDev() start\n");

	zcd.url = "http://";
	std::string hp(hostPort);
	if (hp.find(":") != std::string::npos)
		hp = hp.substr(0,hp.find(":"));
	zcd.url += hp;

	// the following is not used
	//int transID = ((int)(resp[0]&0xff)<<8) + (int)(resp[1]&0xff);

	int flags = ((int)(resp[2]&0xff)<<8) + (int)(resp[3]&0xff);

	int questions = ((int)(resp[4]&0xff)<<8) + (int)(resp[5]&0xff);
	int answers = ((int)(resp[6]&0xff)<<8) + (int)(resp[7]&0xff);
	int authorities = ((int)(resp[8]&0xff)<<8) + (int)(resp[9]&0xff);
	int additionals = ((int)(resp[10]&0xff)<<8) + (int)(resp[11]&0xff);

	if (!flags || !answers)
		return false;

	//printf("parseDev() questions: %d, len: %d\n", questions, (int)respLen);
	int pos = 12;
	std::string dummy;
	pos = parseRec(resp, respLen, pos, questions, dummy);

	//printf("parseDev() answers: %d, pos: %d, len: %d\n", answers, pos, (int)respLen);
	// Get friendly name
	pos = parseRec(resp, respLen, pos, answers, zcd.friendlyName);
	//printf("parseDev() name: %s\n", zcd.friendlyName.c_str());

	//printf("parseDev() authorities: %d, pos: %d, len: %d\n", authorities, pos, (int)respLen);
	// Skip authorities
	pos = parseRec(resp, respLen, pos, authorities, dummy);

	//printf("parseDev() additionals: %d, pos: %d, len: %d\n", additionals, pos, (int)respLen);
	// Get port
	std::string port;
	pos = parseRec(resp, respLen, pos, additionals, port);
	zcd.url += ":"+port;

	//printf("ZC parseRec(): url: %s\n", zcd.url.c_str());

	ZCDevMap dm;
	if (devs_.find(cur_type_) != devs_.end())
	{
		dm = devs_.find(cur_type_)->second;
	}

	if (dm.devMap.find(zcd.friendlyName) == dm.devMap.end())
	{
		zcd.isOkToUse = true;

		dm.devMap[zcd.friendlyName] = zcd;
		devs_[cur_type_] = dm;

		printf("Adding dev: %s\n", zcd.friendlyName.c_str());
		navDsc_->foundZCDev(cur_type_);
	}

	return true;
}

int ZeroConf::parseRec(const char* resp, size_t respLen, int pos, int numRecs, std::string& name)
{
	//printf("parseRec() start - ");

	if (!respLen || !numRecs)
	{
		//printf("end len==0\n");
		return pos;
	}

	for (int p=0; p<numRecs; p++)
	{
		//printf("Record[%d] [0]: 0x%x, [1]: 0x%x [2]: 0x%x\n",p, resp[pos], resp[pos+1], resp[pos+2]);
		// Rec name
		std::string recName;
		pos = setName(resp, pos, recName);

		int type = ((resp[pos]&0xff) << 8) + (resp[pos+1]&0xff);
		//printf(" Type: %d\n", type);
		// Type
		pos += 2;

		// Class
		pos += 2;

		if (resp[pos] == 0 && resp[pos+1] == 0)
		{

			int dataLen = 0;

			// time to live
			pos += 4;

			// Data
			dataLen = ((int)(resp[pos]&0xff)<<8);
			pos++;
			dataLen += (int)(resp[pos]&0xff);
			pos++;

			if (type == 12) // PTR (Domain Name)
			{
				setName(resp, pos, name);

			}
			else if (type == 33)  // SRV (get port)
			{
				//int priority = ((resp[pos]&0xff) << 8) + (resp[pos+1]&0xff);
				//int weight = ((resp[pos+2]&0xff) << 8) + (resp[pos+3]&0xff);
				int port = ((resp[pos+4]&0xff) << 8) + (resp[pos+5]&0xff);
				std::stringstream ss;
				ss << port;
				name.append(ss.str());
			}

			pos += dataLen;
		}

	}

	//printf("end\n");
	return pos;
}

int ZeroConf::setName(const char* resp, int pos, std::string& name)
{
	bool c0Char = false;
	do
	{
		if (name.length()>0)
			name += ".";

		int curPos = pos;

		pos = cxSupport(resp, pos, name, curPos);
		c0Char = (curPos != pos);

        if (!c0Char)
        {
           int nameLen = (int)resp[pos++] & 0xff;
           std::string subName;
           for (int i=0; i<nameLen; i++)
           {
              curPos = pos;
              pos = cxSupport(resp, pos, subName, curPos);

              if (curPos == pos)
            	  subName += resp[pos++];
              else
                 i = i + pos - curPos - 1;
           }
           name += subName;
        }
	} while (resp[pos]!=0 && !c0Char);

	if (!c0Char) pos++;

	return pos;
}

int ZeroConf::cxSupport(const char* resp, int pos, std::string& subName, int curPos)
{
    if (resp[pos]==(char)0xc0 || resp[pos]==(char)0xc1)
    {
       if (curPos != pos)
    	   subName += ".";

       std::string indirectName;
       int inPos = (int)resp[pos+1]&0xff;
       setName(resp, inPos, indirectName);
       subName += indirectName;
       pos += 2;
    }
    return pos;

}

void ZeroConf::initQuery()
{
   queryLen_ = prfixLen_ + postfixLen_ + cur_type_.length()+1;
   query_ = new char[queryLen_];

   memcpy(query_, prefix_, prfixLen_);
   query_[prfixLen_]=(char)cur_type_.length();
   memcpy(&query_[prfixLen_+1],                    cur_type_.c_str(), cur_type_.length());
   memcpy(&query_[prfixLen_+1+cur_type_.length()], postfix_, postfixLen_);
}

bool ZeroConf::hostPortOk(const char* host, int port)
{
	return false;
}

}  // Namespace

#endif // ENABLE_DISCOVERY

