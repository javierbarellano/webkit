description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvsCDS = null;
var srvsCnn = null;
var CnnAdded = false;
var CDSAdded = false;

function okCDS(services) {
	srvsCDS = services;
	srvsCDS.onadddev = CDSAddDev;
	window.console.log("okCDS() called...");
}

function okCnn(services) {
	srvsCnn = services;
	srvsCnn.onadddev = CnnAddDev;
	window.console.log("okCnn() called...");
}

function CDSAddDev() {
	CDSAdded = true;
	srvsCDS = services;
	srvsCDS.onadddev = null;
	
	window.console.log("CDSAddDev() called...");
	if (CnnAdded)
		finishJSTest();
}

function CnnAddDev() {
	CnnAdded = true;
	srvsCnn = services;
	srvsCnn.onadddev = null;
	
	window.console.log("CnnAddDev() called...");
	if (CDSAdded)
		finishJSTest();
}

function errShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called ok Callback.');
}

getNetworkServices("upnp:urn:schemas-upnp-org:service:ContentDirectory:1", okCDS, errShouldNotbeCalled);
getNetworkServices("upnp:urn:schemas-upnp-org:service:ConnectionManager:1", okCnn, errShouldNotbeCalled);
window.jsTestIsAsync = true;
