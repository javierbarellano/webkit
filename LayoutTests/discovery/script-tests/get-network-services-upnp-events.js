description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvs;
var srv;
var event;

function eventCB(evnt) {
	testPassed("navigator.getNetworkServices() called eventCB callback.");
	event = evnt;
	shouldBe("event.propertyset.length>0", "true");
	shouldBe("event.friendlyName.length>0", "true");
	shouldBe("event.serviceType.length>0", "true");
	shouldBe("event.uuid.length>0", "true");
	srv.onupnp = null;
	finishJSTest();
}


function okSecondCallCB(services) {
	testPassed("navigator.getNetworkServices() called 2nd ok callback.");
	srvs = services;

	shouldBe("srvs.servicesAvailable>=1", "true");

	srv = srvs.item(0);
	srv.onupnp = eventCB;
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
