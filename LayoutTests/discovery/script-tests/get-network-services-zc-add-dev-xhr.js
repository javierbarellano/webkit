description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvs;
var resp;
var req;

function okAddDev(services) {
	testPassed("navigator.getNetworkServices() first call to ok callback.");
	srvs = services;
	shouldBe("srvs.servicesAvailable==0", "true");
	srvs.onserviceavailable = addDevCB;
}

function errShouldNotbeCalled(err) {
	testFailed('GetNetworkServices() should have called ok Callback.');
}

function okSecondCallCB(services) {
	srvs = services;
	shouldBe("srvs.servicesAvailable>=1", "true");
	if (srvs.servicesAvailable) {
		var srv = srvs[0];
		//if (srv && srv.url)
		//	window.console.log("okSecondCallCB():  srv.url: " + srv.url + "/databases1/containers");

		req = new XMLHttpRequest();

		req.open("GET", srv.url + "/databases/1/containers", false);
		req.send();
		resp = req.responseText;

		shouldBe("req.status==200", "true");
		
	}
	testPassed("navigator.getNetworkServices() second call to ok callback.");
	finishJSTest();
}

function addDevCB() {
	if (srvs.onserviceavailable == null) {
		testFailed('GetNetworkServices() should have called dummyCB.');
	}
	srvs.onserviceavailable = null;
	
	getNetworkServices("zeroconf:_daap", okSecondCallCB, errShouldNotbeCalled)
}

getNetworkServices("zeroconf:reset:_daap", okAddDev, errShouldNotbeCalled);
window.jsTestIsAsync = true;
