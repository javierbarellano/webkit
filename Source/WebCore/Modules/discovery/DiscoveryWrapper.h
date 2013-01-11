#ifndef DISCOVERYWRAPPER_H
#define DISCOVERYWRAPPER_H

// This wrapper isolates the internal discovery client from all of the madness we need not know about

#include "UPnPDevice.h"
#include "IDiscoveryAPI.h"

namespace WebCore {

class DiscoveryWrapper
{
public:
    static void startUPnPInternalDiscovery(const char *type, IDiscoveryAPI *api);
};

} // namespace

#endif // DISCOVERYWRAPPER_H
