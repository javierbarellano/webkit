description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvs;
function okAddDev(services) {
	testPassed("navigator.getNetworkServices() called ok callback.");
	srvs = services;
	srvs.ondevadded = addDevCB;
}

function errShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called ok Callback.');
}

function addDevCB() {
	if (srvs.ondevadded == null) {
		testFailed('GetNetworkServices() should have called dummyCB.');
	}
	
	testPassed("Add Device callback called.");
	srvs.ondevadded = null;
	finishJSTest();
}

getNetworkServices("upnp:urn:schemas-upnp-org:service:ContentDirectory:1", okAddDev, errShouldNotbeCalled);
window.jsTestIsAsync = true;
