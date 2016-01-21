importScripts("js/opus.js"); 

var decoder;

onmessage = function(e) {
	if (e.data.IsInit) {
		// to do
		decoder = new OpusDecoder(e.data.SampleRate, e.data.Channels);
	} else
	if (decoder) {
		var output = [ ];
		var d2 = new Uint8Array(e.data.Data);
		var offset = 0;
		var total_size = 0;
		while (offset < d2.length) {
			var size = d2[offset++];
			if (size > 0xC0) {
				size -= 0xC0;
				size *= 0x3F;
				size += d2[offset++];
			}
			var res = decoder.decode_float(d2.subarray(offset, offset + size));
			if ((res) && (res.length)) {
				output.push(res);
				total_size += res.length;
			}
			offset += size;
		}
		offset = 0;
		var output2 = new Float32Array(total_size);
		for (var i = 0; i < output.length; i++) {
			output2.set(output[i], offset)
			offset += output[i].length;
		}
		postMessage([output2]);
	}
}
