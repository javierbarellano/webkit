#include "DiscoveryWrapper.h"
#include "NavDsc.h"

namespace WebCore {

// static method
void DiscoveryWrapper::startUPnPInternalDiscovery(const char *type, IDiscoveryAPI *api)
{
    //fprintf(stderr,"DiscoveryWrapper::startUPnPInternalDiscovery\n");
    NavDsc::getInstance()->startUPnPDiscovery(type, 0, 0);
    NavDsc::getInstance()->onUPnPDiscovery(type, api);
    return;
}

} // namespace

