description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvsCDS = null;
var srvsCnn = null;
function okCDS(services) {
	srvsCDS = services;
	testPassed("navigator.getNetworkServices() first call to ok callback.");
	shouldBe("srvsCDS.servicesAvailable==0", "true");
	if (srvsCnn)
		finishJSTest();
}

function okCnn(services) {
	srvsCnn = services;
	testPassed("navigator.getNetworkServices() first call to ok callback.");
	shouldBe("srvsCnn.servicesAvailable==0", "true");
	if (srvsCDS)
		finishJSTest();
}

function errShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called ok Callback.');
}

getNetworkServices("upnp:reset:urn:schemas-upnp-org:service:ContentDirectory:1", okCDS, errShouldNotbeCalled);
getNetworkServices("upnp:urn:schemas-upnp-org:service:ConnectionManager:1", okCnn, errShouldNotbeCalled);
window.jsTestIsAsync = true;
