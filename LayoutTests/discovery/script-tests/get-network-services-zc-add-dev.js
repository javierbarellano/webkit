description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvs;
function okAddDev(services) {
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
	
	testPassed("navigator.getNetworkServices() called ok callback.");
	srvs.ondevadded = null;
	finishJSTest();
}

getNetworkServices("zeroconf:_daap", okAddDev, errShouldNotbeCalled);
window.jsTestIsAsync = true;
