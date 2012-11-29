description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

function okNop(services) {
	testPassed("navigator.getNetworkServices() called ok callback.");
	finishJSTest();
}

function errShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called ok Callback.');
}

getNetworkServices("zeroconf:reset", okNop, errShouldNotbeCalled);
getNetworkServices("zeroconf:_daap", okNop, errShouldNotbeCalled);
window.jsTestIsAsync = true;
