description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}
var srvs;

function okNop(services) {
	testPassed("navigator.getNetworkServices() called ok callback.");
	srvs = services;
	shouldBe("srvs.servicesAvailable==0", "true");
	finishJSTest();
}

function errShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called ok Callback.');
}

getNetworkServices("zeroconf:reset:_daap", okNop, errShouldNotbeCalled);
window.jsTestIsAsync = true;
