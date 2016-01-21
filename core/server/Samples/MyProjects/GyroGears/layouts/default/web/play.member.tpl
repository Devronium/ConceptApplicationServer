<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:csp="http://www.devronium.com/csp" extension-element-prefixes="csp">
<xsl:output method="html" indent="yes"/>
<xsl:template match="/">
<xsl:variable name="redirect" select="HTMLContainer/Redirect"/>
<xsl:variable name="op" select="HTMLContainer/Op"/>
<xsl:variable name="message" select="HTMLContainer/Message"/>
<xsl:variable name="template" select="HTMLContainer/Template"/>

<html>
<body>
<div id="player"></div>

<script type="text/javascript" src="js/swfobject.js"></script>
<script type="text/javascript">
	var player = new SWFObject("jwplayer/player.swf","ply","630","400","9","#FFFFFF");
	player.addParam("allowfullscreen","true");
	player.addParam("allowscriptaccess","always");
	player.addParam("width","630");
	player.addParam("height","400");
	player.addParam("flashvars","icons=false&amp;type=video&amp;controlbar=bottom&amp;stretching=uniform&amp;duration=<xsl:value-of select="HTMLContainer/Data/{member_name}_totaltime"/>&amp;streamer=../video{public_name}{member_name}.csp&amp;file=<xsl:value-of select="HTMLContainer/Data/__DBID"/>.flv&amp;image=get{public_name}{member_name}.csp%3Fid%3D<xsl:value-of select="HTMLContainer/Data/__DBID"/>&amp;autostart=true");
	player.write("player");
</script>
</body>
</html>
</xsl:template>
</xsl:stylesheet>