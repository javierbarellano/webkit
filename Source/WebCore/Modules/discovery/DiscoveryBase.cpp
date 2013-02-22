/*
 * DiscoveryBase.cpp
 *
 *  Created on: Dec 27, 2011
 *      Author: gar
 */

#include "config.h"

#if ENABLE(DISCOVERY)

#define WEBKIT_IMPLEMENTATION 1

#include "../../../../Source/WebCore/platform/network/soup/TCPSocketHandle.h"

#include "../../../../Source/WebCore/platform/network/UDPSocketHandleClient.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketHandle.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketError.h"

#include "NavDsc.h"
#include "DiscoveryBase.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

#include <sstream>
#include <string>
#include <iomanip>
#include <map>
#include <stdlib.h>
#include <stdio.h>

#define MAX_RCVBUF 8192
#define MAX_TAG 256

// Utility methods. Should we find a better location for these?
// trim from start
static std::string& ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static std::string& rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static std::string& trim(std::string &s)
{
    return ltrim(rtrim(s));
}

namespace WebCore
{

DiscoveryBase::DiscoveryBase()
{
	//cur_type_ = "Bad Type, Don't use!";
    m_udpSocket = NULL;
    m_mcastSocket = NULL;
    canReceiveAnotherDev_ = false;
	threadDone_ = true;
	socketHandle_ = NULL;
	serverHandle_ = NULL;
	m_netIsUp = false;
	m_stillRunning = false;
	m_droppedStillRunning = false;
	m_tID = 0;
	navDsc_ = NULL;
	m_tNotifyID = 0;
	m_tDroppedID = 0;
	stopDicovery_ = false;
}

DiscoveryBase::~DiscoveryBase()
{
	stopDicovery_ = true;
	while (!threadDone_) usleep(50000); // 50 ms

}

// extremely primitive xml element parser. Returns value for FIRST occurance of tag or empty string if not found.
std::string DiscoveryBase::getElementValue(const char* buffer, const char* tag, char** ppos)
{
    std::string result;

    if ( strlen(tag) < (MAX_TAG-4)) {

        char beginTag[MAX_TAG];
        char endTag[MAX_TAG];

        sprintf(beginTag, "<%s>", tag);
        sprintf(endTag, "</%s>", tag);

        char parsedValue[8192];
        char* start = const_cast<char*>(strstr(buffer, beginTag));
        char* end = const_cast<char*>(strstr(buffer,endTag));

        if (start && end) {
            start += strlen(beginTag);
            if (ppos) {
                *ppos = end + strlen(endTag);
            }
            strncpy(parsedValue, start,  end - start);
            parsedValue[end - start] = 0;
            result = std::string(parsedValue);
            trim(result);
        }
    }

    return result;
}

// extremely primitive xml element parser. Returns array of matching elements
std::vector<std::string> DiscoveryBase::getElementArray(const char* buffer, const char* tag, char** ppos)
{
    std::vector<std::string> elements;

    char* bufptr = const_cast<char*>(buffer);

    while(true) {

        std::string elem = getElementValue(bufptr, tag, &bufptr);
        if (elem.length()) {
            elements.push_back(elem);
            if (ppos) {
                *ppos = bufptr;
            }
        } else {
            break;
        }
    }

    return elements;
}

void DiscoveryBase::getHostPort(const char *url, char* host, int *port)
{

	int start = 0;
	if (strstr(url, "://"))
	{
		start = strstr(url, "://") - url + 3;
	}

	strcpy(host, &url[start]);
	int end = strstr(host, ":")-host;
	host[end] = 0;

	char sPort[100];
	strncpy(sPort, &host[end+1],20);
	sPort[strlen(&host[end+1])] = 0;

	if (strchr(sPort,'/')) {
		end = strchr(sPort,'/')-sPort;
		sPort[end] = 0;
	}

	*port = atoi(sPort);
}

void DiscoveryBase::getPath(const char *url,char* path)
{
	char bURL[4096];

	strcpy(bURL, url);

	int start = 0;
	if (strstr(bURL, "://"))
	{
		start = strstr(bURL, "://") - bURL + 3;
	}
	char *host = &bURL[start];

	strcpy(path, &host[strchr(host,'/')-host]);

	//printf("getPath: host=%s, path=%s\n",host, path);
}

void DiscoveryBase::socketSend(const char *host, int port, const char *toSend, size_t sLen, char *bf, size_t *len)
{
	RefPtr<TCPSocketHandle> socket = TCPSocketHandle::create(host, port);

	socket->send(toSend, sLen);
	socket->receive(bf, len);
}

void DiscoveryBase::HTTPget(const char *host, int port, const char *path, char *bf, size_t *len)
{
    /* TODO: Review requirements for User-Agent string, i.e. DLNA-HTML5/1.0, (CertID 1122334455667788)
     */

	/*
	GET /upnphost/udhisapi.dll?content=uuid:6a66eb21-7c9c-4699-a49d-f47752c5afd5 HTTP/1.1
	User-Agent: Platinum/0.5.3.0, DLNADOC/1.50
	Host: 10.36.0.237:2869
	Connection: keep-alive
	*/

	std::stringstream toSend;
	toSend << "GET "<< path << " HTTP/1.1\r\n";
	toSend << "User-Agent: Platinum/0.5.3.0, DLNADOC/1.50\r\n";
	toSend << "Host: " << host << ":" << port << "\r\n";
	toSend << "Connection: keep-alive\r\n\r\n";

	//printf("HTTPget: host=%s, port=%d, path=%s\n",host, port, path);

	socketSend(host, port, toSend.str().c_str(), toSend.str().length(), bf, len);
}

void DiscoveryBase::HTTPget(KURL &url, char *bf, size_t *len)
{
	//printf("HTTPget(KURL)\n");
	String path = url.path();
	String query = url.query();
	std::string pq(path.ascii().data());
	if (query.length())
	{
		pq += "?";
		pq += std::string(query.ascii().data());
	}
	String host = url.host();
	int port = url.port();

	HTTPget(host.ascii().data(), port, pq.c_str(), bf, len);

}

// optHeaders can be NULL
// optHeaders should include /r/n at the end of lines
void DiscoveryBase::HTTPpost(KURL &url, char *postBody, char *optHeaders, char *bf, size_t *len)
{
	//printf("HTTPget2(KURL)\n");
	/*
	POST /upnphost/udhisapi.dll?content=uuid:6a66eb21-7c9c-4699-a49d-f47752c5afd5 HTTP/1.1
	User-Agent: Platinum/0.5.3.0, DLNADOC/1.50
	Content-Length: ???
	Host: 10.36.0.237:2869
	*/

	String path = url.path();
	String query = url.query();
	std::string pq(path.ascii().data());
	if (query.length())
	{
		pq += "?";
		pq += std::string(query.ascii().data());
	}
	std::string host(url.host().ascii().data());
	int port = url.port();

	std::stringstream toSend;
	toSend << "POST "<< pq << " http/1.1\r\n";
	toSend << "User-Agent: Platinum/0.5.3.0, DLNADOC/1.50\r\n";
	toSend << "Host: " << host << ":" << port << "\r\n\r\n";
	toSend << "Content-Length: " << strlen(postBody) << "\r\n";
	toSend << "Content-Type: application/soap+xml; charset=utf-8" << "\r\n";
	if (optHeaders)
		toSend << optHeaders;
	toSend << "\r\n";
	toSend << postBody << "\r\n\r\n";

	socketSend(host.c_str(), port, toSend.str().c_str(), toSend.str().length(), bf, len);
}

// static
void DiscoveryBase::hexDump(const char *data, int len, int pos)
{
	std::stringstream ss;
	int start = 0; //pos;
	int ln = len;
	const char *bf = data;
	for (int row=pos/16; row<ln/16; row++)
	{
		int off = row*16;

		for (int x = 0; x < start; x++) ss << "   ";

		for (int x = start; x<start+16 && (x+off)<ln; x++)
		{
			if (x == 8) ss << "  ";
			ss << std::setfill ('0') << std::setw(sizeof(char)*2) << std::hex << ((int)bf[x+off]&0xff) << " ";
		}

		ss << "    ";

		for (int x = start; x<start+16 && (x+off)<ln; x++)
		{
			if (x == 8) ss << "  ";
			ss << (((bf[x+off]>=' ' && bf[x+off]<='~') ? bf[x+off]:'.'));
		}

		ss << "\n";
		start = 0;
	}

	printf("%s",ss.str().c_str());
}

// Here to lookup a token in a map returned from parseUDPMessage
std::string DiscoveryBase::getTokenValue( std::map<std::string,std::string> map, std::string token )
{
    std::string value;
    std::map<std::string,std::string>::iterator iter = map.find(token);

    if (iter != map.end()) {
        value = (*iter).second;
    }
    return value;
}

// Here to parse a received message into a dictionary of token/value pairs
std::map<std::string,std::string> DiscoveryBase::parseUDPMessage( const char *data, int dLen )
{
    //char* buffer = new char(dLen+1);
    char buffer[MAX_RCVBUF];
    memcpy(buffer, data, dLen);
    buffer[dLen] = 0;

    std::map<std::string,std::string> map;

    int lineCount = 0;
    char* p = strtok(buffer,"\r\n");
    while (p != 0) {

        std::string tokenValue = p;
        if ( ++lineCount == 1 ) {
            map["LINE1"] = p;
        }

        size_t pos = tokenValue.find_first_of(':');
        if (pos != std::string::npos) {

            std::string token = tokenValue.substr(0,pos);
            trim(token);
            std::transform(token.begin(), token.end(), token.begin(), toupper);

            std::string value = tokenValue.substr(pos+1);
            trim(value);

            map[token] = value;
        }

        p = strtok(NULL,"\r\n");
    }

    //delete buffer;
    return map;
}





}; // WebCore Namespace

#endif // ENABLE_DISCOVERY

