/*
 *  Copyright (C) 2013 Cable Labs
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef ZEROCONF_H_
#define ZEROCONF_H_

#if ENABLE(DISCOVERY)

#include <vector>
#include <map>

#ifdef LOG
#undef LOG
#endif

#include "../../../../Source/WebCore/platform/network/soup/UDPSocketHandle.h"
#include "Modules/discovery/ZCDevice.h"
#include "Modules/discovery/DiscoveryBase.h"

namespace WebCore {

class NavDsc;

typedef struct sZCDevMap
{
    std::map<std::string, ZCDevice> devMap;
}ZCDevMap;

class ZeroConf : public DiscoveryBase
//				 ,public base::SupportsWeakPtr<ZeroConf>
{
public:
    static std::map<std::string, ZCDevice> discoverDevs(const char *type, NavDsc *);

    static ZeroConf* getInstance();

    static ZCDevMap* getDevs(const char *type);

    // Called when Socket Stream is opened.
    virtual void UDPdidOpenStream(UDPSocketHandle*);

    // Called when |amountSent| bytes are sent.
    virtual void UDPdidSendData(UDPSocketHandle*, int /* amountSent */);

    // Called when data are received.
    virtual void UDPdidReceiveData(UDPSocketHandle*, const char* data, int dLen, const char *hostPort, int hpLen);

    // Called when Socket Stream is closed.
    virtual void UDPdidClose(UDPSocketHandle*);

    // Called when Socket Stream has an error.
    virtual void UDPdidFail(UDPSocketHandle*, UDPSocketError&);

    void initQuery(const char *type);

    // White list imp
    virtual bool hostPortOk(const char* host, int port);

    void checkForDroppedDevs();

    void reset() {m_devLock->lock(); m_devs.clear(); m_devLock->unlock();}

    char* m_query;
    int m_queryLen;

protected:

    virtual bool parseDev(const char* resp, size_t respLen, const char* hostPort);

private:
    static ZeroConf* m_instance;
    static char m_prefix[];
    static int m_prfixLen;

    static char m_postfix[];
    static int m_postfixLen;

    ZeroConf(const char *type);
    virtual ~ZeroConf();

    std::string m_daapType;

    // Return pos
    int parseRec(const char* resp, size_t respLen, int pos, int numRecs, std::string& name, std::string& portnm);

    // Return pos
    int cxSupport(const char* resp, int pos, std::string& subName, int curPos);

    // Return pos
    int setName(const char* resp, int pos, std::string& name);

    // key == service type
    // dev key == UUID
    std::map<std::string, ZCDevMap> m_devs;
    Mutex *m_devLock;

    long m_lastSend;

    bool m_sendPending;

};

}
#endif
#endif /* ZEROCONF_H_ */
