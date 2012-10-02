description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvsCDS;
var srvsCnn;
var CnnAdded = false;
var CDSAdded = false;

function CDSAddDev() {
	window.console.log("CDSAddDev() called...");
	testPassed("navigator.getNetworkServices() called ondevadded CDS callback.");
	CDSAdded = true;
	srvsCDS.ondevadded = null;
	
	if (CnnAdded)
		finishJSTest();
}

function CnnAddDev() {
	window.console.log("CnnAddDev() called...");
	testPassed("navigator.getNetworkServices() called ondevadded Cnn callback.");
	CnnAdded = true;
	srvsCnn.ondevadded = null;
	
	if (CDSAdded)
		finishJSTest();
}

function okCDS(services) {
	testPassed("navigator.getNetworkServices() called ok CDS callback.");
	srvsCDS = services;
	srvsCDS.ondevadded = CDSAddDev;
	window.console.log("okCDS() called...");
}

function okCnn(services) {
	testPassed("navigator.getNetworkServices() called ok Cnn callback.");
	srvsCnn = services;
	srvsCnn.ondevadded = CnnAddDev;
	window.console.log("okCnn() called...");
}

function errShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called ok Callback!');
}

getNetworkServices("upnp:urn:schemas-upnp-org:service:ContentDirectory:1", okCDS, errShouldNotbeCalled);
getNetworkServices("upnp:urn:schemas-upnp-org:service:ConnectionManager:1", okCnn, errShouldNotbeCalled);
window.jsTestIsAsync = true;
