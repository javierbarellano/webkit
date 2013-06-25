/*
 * DiscoveryBase.h
 *
 *  Created on: Dec 27, 2011
 *      Author: gar
 */

#ifndef DISCOVERYBASE_H_
#define DISCOVERYBASE_H_

#include <sstream>
#include <string>
#include <vector>

#include "platform/network/UDPSocketHandleClient.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketHandle.h"
#include "../../../../Source/WebCore/platform/network/soup/UDPSocketError.h"

#include "platform/network/HNEventServerHandle.h"

namespace WebCore
{

class IDicoveryAPI;

class DiscoveryBase : public UDPSocketHandleClient
{
public:
    DiscoveryBase();
    virtual ~DiscoveryBase();

    static void socketSend(const char *url, int port, const char *toSend, size_t sLen, char *bf, size_t *len);
    static void HTTPget(const char *host, int port, const char *path, char *bf, size_t *len);

    // White list imp
    virtual bool hostPortOk(const char* host, int port)=0;

    virtual void getHostPort(const char *url, char* host, int *port);
    virtual void getPath(const char *url, char* path);


    bool canReceiveAnotherDev() {return m_canReceiveAnotherDev;}
    void resetCanReceiveAnotherDev() {m_canReceiveAnotherDev = false;}

    static void hexDump(const char *data, int len, int pos = 0);

    virtual bool networkIsUp() { return m_netIsUp;}

    UDPSocketHandle* m_socketHandle;

    HNEventServerHandle* m_serverHandle;


    bool m_stillRunning;
    bool m_droppedStillRunning;

    mutable RefPtr<UDPSocketHandle> m_udpSocket;
    mutable RefPtr<UDPSocketHandle> m_mcastSocket;

    bool m_stopDicovery;
    bool m_threadDone;

    std::string getElementValue(const char* buffer, const char* tag, char** ppos=0);
    std::vector<std::string> getElementArray(const char* buffer, const char* tag, char** ppos=0);

    char m_url[1024];

    IDiscoveryAPI *m_navDsc;
    bool m_netIsUp;

protected:

    virtual bool parseDev(const char* resp, size_t respLen, const char* hostPort)=0;

    // Added these to parse message headers (Sven 9/1/12)
    std::map<std::string,std::string> parseUDPMessage( const char *data, int dLen );
    std::string getTokenValue( std::map<std::string,std::string> map, std::string token );

    bool m_canReceiveAnotherDev;

    ThreadIdentifier m_tID;
    ThreadIdentifier m_tNotifyID;
    ThreadIdentifier m_tDroppedID;

};

} // namespace WebCore

#endif /* DISCOVERYBASE_H_ */
