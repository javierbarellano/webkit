description('Tests navigator.getNetworkServices() works in bad type case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

function okShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called err Callback.');
}

var err;
function err(errtype) {
	err = errtype;
	testPassed("navigator.getNetworkServices() called err callback with bad type. Code: " + err.code);
	shouldBe("err.code", "2");
	finishJSTest();
}

getNetworkServices("bad:urn:schemas-upnp-org:service:ContentDirectory:1", okShouldNotbeCalled, err);
window.jsTestIsAsync = true;
