#include "DiscoveryWrapper.h"
#include "NavDsc.h"

namespace WebCore {

// static method
void DiscoveryWrapper::startUPnPInternalDiscovery(const char *type, IDiscoveryAPI *api)
{
    //fprintf(stderr,"DiscoveryWrapper::startUPnPInternalDiscovery\n");
    NavDsc::getInstance()->startUPnPInternalDiscovery(type, api);
    return;
}

} // namespace

