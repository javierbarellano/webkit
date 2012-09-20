description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvsCDS = null;
var srvsCnn = null;
function okCDS(services) {
	srvsCDS = services;
	window.console.log("okCDS() called...");
	if (srvsCnn)
		finishJSTest();
}

function okCnn(services) {
	srvsCnn = services;
	window.console.log("okCnn() called...");
	if (srvsCDS)
		finishJSTest();
}

function errShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called ok Callback.');
}

getNetworkServices("upnp:urn:schemas-upnp-org:service:ContentDirectory:1", okCDS, errShouldNotbeCalled);
getNetworkServices("upnp:urn:schemas-upnp-org:service:ConnectionManager:1", okCnn, errShouldNotbeCalled);
window.jsTestIsAsync = true;
