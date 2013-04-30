description('Tests navigator.getNetworkServices() for Zero Conf add device.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvs;
function okAddDev(services) {
	testPassed("navigator.getNetworkServices() called ok callback.");
	srvs = services;
	shouldBe("srvs.length==0", "true");
	srvs.onserviceavailable = addDevCB;
}

function errShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called ok Callback.');
}

function addDevCB() {
	if (srvs.onserviceavailable == null) {
		testFailed('addDevCB() called when srvs.ondevadded == null');
	}
	
	testPassed("HomeNetworking called Device Added callback.");
	srvs.onserviceavailable = null;
	finishJSTest();
}

getNetworkServices("zeroconf:reset:_daap", okAddDev, errShouldNotbeCalled);
window.jsTestIsAsync = true;
