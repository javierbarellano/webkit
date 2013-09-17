description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

    navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvs;
var resp;
var req;
function okAddDev(services) {
    testPassed("navigator.getNetworkServices() first call to ok callback.");
    srvs = services;
    shouldBe("srvs.servicesAvailable==0", "true");
    srvs.onserviceavailable = addDevCB;
}

function errShouldNotbeCalled(err) {
    testFailed('GetNetworkServices() should have called ok Callback.');
}

function okSecondCallCB(services) {
    srvs = services;
    shouldBe("srvs.servicesAvailable>=1", "true");
    if (srvs.servicesAvailable) {
        var srv = srvs[0];

        req = new XMLHttpRequest();

        try {
            req.open("GET", srv.url, false);
            req.send();
            shouldBe("req.status==200", "true");
        } catch (e) {
            testFailed("XMLHttpRequest threw exception: " + e);
        }
    }
    testPassed("navigator.getNetworkServices() second call to ok callback.");
    finishJSTest();
}

function addDevCB() {
    if (srvs.onserviceavailable == null) {
        testFailed('GetNetworkServices() should have called dummyCB.');
    }
    srvs.onserviceavailable = null;

    getNetworkServices("upnp:urn:schemas-upnp-org:service:ContentDirectory:1", okSecondCallCB, errShouldNotbeCalled)
}

getNetworkServices("upnp:reset:urn:schemas-upnp-org:service:ContentDirectory:1", okAddDev, errShouldNotbeCalled);
window.jsTestIsAsync = true;
