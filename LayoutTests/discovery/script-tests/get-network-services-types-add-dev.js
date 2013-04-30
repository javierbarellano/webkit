description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvsCDS;
var srvsCnn;
var CnnAdded = false;
var CDSAdded = false;

function CDSAddDev() {
	//window.console.log("CDSAddDev() called...");
	//testPassed("navigator.getNetworkServices() called ondevadded CDS callback.");
	CDSAdded = true;
	srvsCDS.onserviceavailable = null;
	
	if (CnnAdded)
		finishJSTest();
}

function CnnAddDev() {
	//window.console.log("CnnAddDev() called...");
	//testPassed("navigator.getNetworkServices() called ondevadded Cnn callback.");
	CnnAdded = true;
	srvsCnn.onserviceavailable = null;
	
	if (CDSAdded)
		finishJSTest();
}

function okCDS(services) {
	testPassed("navigator.getNetworkServices() called ok CDS callback.");
	srvsCDS = services;
	shouldBe("srvsCDS.length==0", "true");

	srvsCDS.onserviceavailable = CDSAddDev;
	//window.console.log("okCDS() called...");
}

function okCnn(services) {
	testPassed("navigator.getNetworkServices() called ok Cnn callback.");
	srvsCnn = services;
	shouldBe("srvsCnn.length==0", "true");
	srvsCnn.onserviceavailable = CnnAddDev;
	//window.console.log("okCnn() called...");
}

function errShouldNotbeCalled(services) {
	testFailed('GetNetworkServices() should have called ok Callback!');
}

getNetworkServices("upnp:reset:urn:schemas-upnp-org:service:ContentDirectory:1", okCDS, errShouldNotbeCalled);
getNetworkServices("upnp:urn:schemas-upnp-org:service:ConnectionManager:1", okCnn, errShouldNotbeCalled);
window.jsTestIsAsync = true;
