description('Tests navigator.getNetworkServices() works in happy case as expected.');

function getNetworkServices(type, okCallback, errCallback) {

	navigator.getNetworkServices(type, okCallback, errCallback);
}

var srvs;
var resp;
var req;
function okAddDev(services) {
	srvs = services;
	srvs.ondevadded = addDevCB;
}

function errShouldNotbeCalled(err) {
	testFailed('GetNetworkServices() should have called ok Callback.');
}

function okSecondCallCB(services) {
	srvs = services;
	shouldBe("srvs.servicesAvailable>=1", "true");
	if (srvs.servicesAvailable) {
		var srv = srvs.item(0);
		if (srv && srv.url)
			window.console.log("okSecondCallCB():  srv.url: " + srv.url + "/databases");

		req = new XMLHttpRequest();

		req.open("GET", srv.url + "/databases", false);
		req.send();
		resp = req.responseText;

		shouldBe("req.status==200", "true");
		
	}
	testPassed("navigator.getNetworkServices() second call to ok callback.");
	finishJSTest();
}

function addDevCB() {
	if (srvs.ondevadded == null) {
		testFailed('GetNetworkServices() should have called dummyCB.');
	}
	srvs.ondevadded = null;
	
	getNetworkServices("zeroconf:_daap", okSecondCallCB, errShouldNotbeCalled)
}

getNetworkServices("zeroconf:_daap", okAddDev, errShouldNotbeCalled);
window.jsTestIsAsync = true;
