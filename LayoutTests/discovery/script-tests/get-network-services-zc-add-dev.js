description('Tests navigator.getNetworkServices() for Zero Conf add device.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvs;
function okAddDev(services) {
	testPassed("navigator.getNetworkServices() called ok callback.");
	srvs = services;
	shouldBe("srvs.length==0", "true");
	srvs.ondevadded = addDevCB;
}

function errShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called ok Callback.');
}

function addDevCB() {
	if (srvs.ondevadded == null) {
		testFailed('addDevCB() called when srvs.ondevadded == null');
	}
	
	testPassed("HomeNetworking called Device Added callback.");
	srvs.ondevadded = null;
	finishJSTest();
}

getNetworkServices("zeroconf:_daap", okAddDev, errShouldNotbeCalled);
window.jsTestIsAsync = true;
