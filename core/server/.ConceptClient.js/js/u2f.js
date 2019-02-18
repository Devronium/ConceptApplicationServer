function u2f_buffer2string(buf) {
	var str = "";
	if (!(buf.constructor === Uint8Array))
		buf = new Uint8Array(buf);

	buf.map(function(x) { return str += String.fromCharCode(x); });
	return str;
}

function u2f_base642buffer(base64) {
        // check if already padded
        if (base64.indexOf('=') == -1) {
                base64 = base64.split("-").join("+").split("_").join("/");
                var pad = (4 - base64.length % 4) % 4;
                while (pad > 0) {
                        base64 += "=";
                        pad --;
                }
        }
        var raw = window.atob(base64);
        var rawLength = raw.length;
        var array = new Uint8Array(new ArrayBuffer(rawLength));

        for (var i = 0; i < rawLength; i++)
                array[i] = raw.charCodeAt(i);
        return array;
}

function u2f_make_data(id, response) {
	var data = u2f_buffer2string(response.clientDataJSON);
	var obj = {
		"id": id,
        	"clientDataJSON": JSON.parse(data),
		"rawClientDataJSON": btoa(data),
		"attestationObject": btoa(String.fromCharCode.apply(null, new Uint8Array(response.attestationObject)))
	};
	return JSON.stringify(obj);
}

function u2f_make_data_verify(id, response) {
	var data = u2f_buffer2string(response.clientDataJSON);
	var obj = {
		"id": id,
        	"clientDataJSON": JSON.parse(data),
		"rawClientDataJSON": btoa(data),
		"authenticatorData": btoa(String.fromCharCode.apply(null, new Uint8Array(response.authenticatorData))),
		"signature": btoa(String.fromCharCode.apply(null, new Uint8Array(response.signature)))
	};
	return JSON.stringify(obj);
}

function u2f_register(challenge_b64, appname, username, fullname, timeout, callback) {
	if (!navigator.credentials) {
		callback("U2F not supported", true)
		return;
	}
	var challenge = u2f_base642buffer(challenge_b64);
	var pubKeyCredParams = [{
		type: "public-key",
		alg: -7 // ecdsa + sha256
	}];
	var rp = { name: appname };
	var user = {
		name: username,
		displayName: fullname,
		id: new TextEncoder("utf-8").encode(name)
	};

	let publicKey = {"challenge": challenge, "timeout": timeout, "pubKeyCredParams": pubKeyCredParams, "rp": rp, "user": user};
	navigator.credentials.create({publicKey}).then((credential) => {
		callback(u2f_make_data(credential.id, credential.response), false);
	}, (reason) => { 
		console.warn(reason);
		callback(reason.message, true);
	});
}

function u2f_verify(challenge_b64, key_b64, callback) {
	if (!navigator.credentials) {
		callback("U2F not supported", true)
		return;
	}
	var challenge = u2f_base642buffer(challenge_b64);
	var key = u2f_base642buffer(key_b64);
	var allowCredentials = [{
		type: "public-key",
		id: key,
		transports: ["usb"]
	}];

	var publicKey = {"rpId": document.domain, "challenge": challenge, "allowCredentials": allowCredentials};

	navigator.credentials.get({publicKey}).then((assertion) => {
		callback(u2f_make_data_verify(assertion.id, assertion.response), false);
	}, (reason) => {
		console.warn(reason);
		callback(reason.message, true);
	});
}
