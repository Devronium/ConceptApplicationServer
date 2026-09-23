<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Request verification</title>
<style>
*{box-sizing:border-box}
body{margin:0;min-height:100vh;padding:16px;display:flex;align-items:center;justify-content:center;background:radial-gradient(circle,#fff,#f4f5f7);font-family:"Century Gothic",Arial,sans-serif;color:#30343b}
#loading{width:min(560px,100%);padding:clamp(20px,6vw,32px);background:#fff;border:1px solid #e2e5e9;border-radius:10px;box-shadow:0 12px 35px #0001;animation:show .35s ease}
.head{display:flex;align-items:center;gap:14px}
.icon{width:42px;height:42px;display:grid;place-items:center;flex:none;color:#5874b8}
.icon svg{width:32px;height:32px;fill:none;stroke:currentColor;stroke-width:1.7;stroke-linecap:round;stroke-linejoin:round;filter:drop-shadow(0 2px 3px #5874b822)}
h1{margin:0;font-size:clamp(16px,4.5vw,20px);font-weight:500;line-height:1.35;letter-spacing:-.2px}
h3{margin:17px 0 0;font-size:clamp(12px,3.2vw,13px);font-weight:400;line-height:1.65;color:#686e78}
hr{margin:0 0 15px;border:0;border-top:1px solid #e9ebee}
.note{margin-top:18px;padding:8px 10px;border:1px solid #e6e8ec;border-radius:6px;background:#f8f9fa;color:#858a93;font-size:11px;line-height:1.5}
.powered{margin-top:12px;text-align:center;color:#b0b4bb;font-size:9px}
.spinner{display:inline-block;width:15px;height:15px;margin-left:8px;vertical-align:-2px;border:2px solid #e4e9f5;border-top-color:#5874b8;border-right-color:#7e9bd4;border-radius:50%;box-shadow:0 0 6px #5874b822;animation:spin .7s cubic-bezier(.5,0,.5,1) infinite}
@keyframes spin{to{transform:rotate(360deg)}}@keyframes show{from{opacity:0;transform:translateY(8px)}to{opacity:1;transform:none}}
</style>
</head>

<body>
<div id="loading">
	<div class="head">
		<div class="icon">
			<svg viewBox="0 0 24 24" aria-hidden="true">
				<path d="M12 2.5l7.5 3.2v5.1c0 4.7-3.1 8.2-7.5 10.7-4.4-2.5-7.5-6-7.5-10.7V5.7L12 2.5z"/>
				<path d="M8.7 12l2.1 2.1 4.5-4.5"/>
			</svg>
		</div>
		<h1>Please wait while we verify your request<span class="spinner"></span></h1>
	</div>

	<h3 id="message">
		<hr>
		Unusual traffic has been detected from your connection.
		We need to make sure that this request is valid and
		not coming from an automated source.
		<br><br>
		Please wait a moment while we complete the verification.
		Once verification is complete, you will be automatically
		redirected to the requested resource.
	</h3>

	<div class="note">
		If this is a POST request, it may need to be resent after verification.
	</div>

	<div class="powered">powered by Concept Application Server</div>
</div>

<script>
async function computeProofOfWork(input,complexity){const b=complexity>>3,r=complexity&7,e=new TextEncoder(),a=e.encode(input);for(let n=0;n<=Number.MAX_SAFE_INTEGER;n++){const s=e.encode(String(n)),d=new Uint8Array(a.length+s.length);d.set(a);d.set(s,a.length);const h=new Uint8Array(await crypto.subtle.digest("SHA-256",d));let ok=1;for(let i=0;i<b;i++)if(h[i]){ok=0;break}if(ok&&r&&(h[b]>>(8-r)))ok=0;if(ok)return{nonce:n,hash:[...h].map(x=>x.toString(16).padStart(2,"0")).join("")}}}
async function verifyRequest(){
	const m=document.getElementById("message"),challenge="{{{challenge}}}",complexity={{{complexity}}}
	try{
		const r=await computeProofOfWork(challenge,complexity);
		const proof={challenge,nonce:r.nonce};
		document.cookie="x-cas-verify="+encodeURIComponent(JSON.stringify(proof))+"; Max-Age=3; Path=/; SameSite=Lax";
		m.innerHTML="<hr>Verification complete. Reloading...";
		setTimeout(()=>location.reload(),1000);
	}catch(e){
		console.error(e);
		m.innerHTML="<hr>Unable to complete verification. Please try again.";
	}
}

setTimeout(verifyRequest,1000);
</script>
</body>
</html>