function get_appropriate_ws_url(extra_url)
{
	var pcol;
	var u = document.URL;

	/*
	 * We open the websocket encrypted if this page came on an
	 * https:// url itself, otherwise unencrypted
	 */

	if (u.substring(0, 5) === "https") {
		pcol = "wss://";
		u = u.substr(8);
	} else {
		pcol = "ws://";
		if (u.substring(0, 4) === "http")
			u = u.substr(7);
	}

	u = u.split("/");

	/* + "/xxx" bit is for IE10 workaround */

	return pcol + u[0] + "/" + extra_url;
}

function new_ws(urlpath, protocol)
{
	return new WebSocket(urlpath, protocol);
}

document.addEventListener("DOMContentLoaded", function() {

	var subscriber_ws = new_ws(get_appropriate_ws_url(""), "websocket");
	try {
		subscriber_ws.onopen = function() {
			document.getElementById("b").disabled = 0;
		};
	
		subscriber_ws.onmessage =function got_packet(msg) {
			document.getElementById("r").value =
				document.getElementById("r").value + msg.data + "\n";
			document.getElementById("r").scrollTop =
				document.getElementById("r").scrollHeight;
		};
	
		subscriber_ws.onclose = function(){
			document.getElementById("b").disabled = 1;
		};
	} catch(exception) {
		alert("<p>Error " + exception);  
	}
	
	var publisher_ws = new_ws(get_appropriate_ws_url("/publisher"), "websocket");
	try {
		publisher_ws.onopen = function() {
			document.getElementById("m").disabled = 0;
		};
	
		publisher_ws.onmessage =function got_packet(msg) {
		};
	
		publisher_ws.onclose = function(){
			document.getElementById("m").disabled = 1;
		};
	} catch(exception) {
		alert("<p>Error " + exception);  
	}

	function sendmsg()
	{
		publisher_ws.send(document.getElementById("m").value);
		document.getElementById("m").value = "";
	}

	document.getElementById("b").addEventListener("click", sendmsg);
	document.getElementById("dataButton").addEventListener("click", loadDoc);
	document.getElementById("dataButton2").addEventListener("click", updateDoc);

}, false);

function loadDoc() {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
		document.getElementById("getResponse").innerHTML = this.responseText;
		}
		if (this.readyState == 4 && this.status == 404) {
			document.getElementById("getResponse").innerHTML = "Not Found";
		}
	};
	xhttp.open("GET", "sampleResult", true);
	xhttp.send();
}

function updateDoc() {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
		document.getElementById("getResponse").innerHTML = this.responseText;
		}
	};
	xhttp.open("POST", "samplePost", true);
	xhttp.send();
}