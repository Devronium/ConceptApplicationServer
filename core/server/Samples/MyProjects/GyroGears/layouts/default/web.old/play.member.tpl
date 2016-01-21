<html>
<body>
<div id="player"></div>

<script type="text/javascript" src="js/swfobject.js"></script>
<script type="text/javascript">
    // this function is caught by the JavascriptView object of the player.
    // This is a javascript handler for the player and is always needed.
	var player = new SWFObject("jwplayer/player.swf","ply","440","360","9","#FFFFFF");
	player.addParam("allowfullscreen","true");
	player.addParam("allowscriptaccess","always");
	player.addParam("wmode","opaque");
	player.addParam("width","440");
	player.addParam("height","360");
	player.addParam("flashvars","icons=false&type=video&controlbar=bottom&stretching=uniform&duration={ldelim}duration{rdelim}&streamer=../video{public_name}{member_name}.csp&file={ldelim}ID{rdelim}.flv&image=get{public_name}{member_name}.csp%3Fid%3D{ldelim}ID{rdelim}&autostart=true");
	//player.addVariable("enablejs","true");
	//player.addVariable("javascriptid","ply");
	player.write("player");
</script>
</body>
</html>
