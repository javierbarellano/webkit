description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvs;
var srv;

function eventCB(event) {
	testPassed("navigator.getNetworkServices() called eventCB callback.");
	srv.onupnpevent = null;
	finishJSTest();
}


function okSecondCallCB(services) {
	testPassed("navigator.getNetworkServices() called 2nd ok callback.");
	srvs = services;

	shouldBe("srvs.servicesAvailable>=1", "true");

	srv = srvs.item(0);
	srv.onupnpevent = eventCB;
}

function okAddDev(services) {
	testPassed("navigator.getNetworkServices() called ok callback.");
	srvs = services;
	srvs.ondevadded = addDevCB;
}

function errShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called ok Callback!');
}

function addDevCB() {
	if (srvs.ondevadded == null) {
		testFailed('GetNetworkServices() srvs.ondevadded = null did not work!');
	}
	
	testPassed("Add Device callback called.");
	srvs.ondevadded = null;
	getNetworkServices("upnp:urn:schemas-upnp-org:service:ContentDirectory:1", okSecondCallCB, errShouldNotbeCalled)
}

getNetworkServices("upnp:urn:schemas-upnp-org:service:ContentDirectory:1", okAddDev, errShouldNotbeCalled);
window.jsTestIsAsync = true;
