<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" indent="yes"/>

  <xsl:template name="Save">
    <input type="submit" class="savebutton" value="Save"/>
  </xsl:template>

  <xsl:template name="None">
    <i>None</i>
  </xsl:template>

  <xsl:template name="NoRecords">
    <xsl:if test="HTMLContainer/NoHeader='0'">
      <xsl:text>No records</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template name="Title">
    <xsl:if test="HTMLContainer/Title!=''">
      <xsl:value-of select="HTMLContainer/Title"/>
    </xsl:if>
  </xsl:template>

  <xsl:template name="Header">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <link type="text/css" href="css/flick/jquery-ui-1.8.14.custom.css" rel="stylesheet" />
    <link type="text/css" href="css/tables.css" rel="stylesheet" />
    <link rel="stylesheet" type="text/css" href="js/fancybox/jquery.fancybox-1.3.4.css" media="screen" />
    <link rel="shortcut icon" href="res/icon.png" />
    <script type="text/javascript" src="js/jquery.min.js"></script>
    <script type="text/javascript" src="js/jquery-ui.min.js"></script>
    <script type="text/javascript" src="js/fancybox/jquery.mousewheel-3.0.4.pack.js"></script>
    <script type="text/javascript" src="js/fancybox/jquery.fancybox-1.3.4.pack.js"></script>
    <script type="text/javascript" src="js/tiny_mce/jquery.tinymce.js"></script>
    <script type="text/javascript" src="js/md5.js" />
    <script type="text/javascript" src="js/expander/expand.js"></script>
  </xsl:template>

  <xsl:template name="Scripts">
  </xsl:template>

  <xsl:template name="Message">
    <xsl:if test="HTMLContainer/Message!=''">
      <div class="ui-widget">
        <div class="ui-state-error ui-corner-all" style="padding: 0 .7em;">
          <p>
            <span class="ui-icon ui-icon-alert" style="float: left; margin-right: .3em;"></span>
            <xsl:value-of disable-output-escaping="yes" select="HTMLContainer/Message"/>
          </p>
        </div>
      </div>
    </xsl:if>
  </xsl:template>

  <xsl:template name="Response">
    <xsl:if test="HTMLContainer/Response!=''">
      <div class="ui-widget">
        <div class="ui-state-highlight ui-corner-all" style="margin-top: 20px; padding: 0 .7em;">
          <p>
            <span class="ui-icon ui-icon-info" style="float: left; margin-right: .3em;"></span>
            <xsl:value-of disable-output-escaping="yes" select="HTMLContainer/Response"/>
          </p>
        </div>
      </div>
    </xsl:if>
  </xsl:template>

  <xsl:template name="Login">
    <xsl:if test="HTMLContainer/IsChildOf = ''">
      <div class="ui-widget">
        <div class="ui-state-highlight ui-corner-all" style="margin-top: 0px; padding: 0 .7em;">
          <p>
            <span class="ui-icon ui-icon-info" style="float: left; margin-right: .3em;"></span>
            <xsl:choose>
              <xsl:when test="HTMLContainer/Username!=''">
                You are logged in as <xsl:value-of disable-output-escaping="yes" select="HTMLContainer/Username"/>
                | <a href="LogIn.csp?logout=1">Sign out</a>
              </xsl:when>
              <xsl:otherwise>
                Please <a href="LogIn.csp?__REDIRECT=index.html">Sign in</a> or <a href="SignUp.csp?__REDIRECT=index.html">sign up</a>
              </xsl:otherwise>
            </xsl:choose>
          </p>
        </div>
      </div>
    </xsl:if>
  </xsl:template>

  <xsl:template name="LoginSimple">
    <xsl:if test="HTMLContainer/IsChildOf = ''">
      <div class="LoginInfo">
            <xsl:choose>
              <xsl:when test="HTMLContainer/Username!=''">
                <xsl:variable name="UID" select="HTMLContainer/UID"/>
{if userentity}
                You are logged in as <a href="{userentity}.csp?id={ldelim}$UID{rdelim}">
                  <xsl:value-of disable-output-escaping="yes" select="HTMLContainer/Username"/>
                </a>
{else}
                You are logged in as <xsl:value-of disable-output-escaping="yes" select="HTMLContainer/Username"/>
{endif}
                | <a href="LogIn.csp?logout=1">Sign out</a>
              </xsl:when>
              <xsl:otherwise>
                Please <a href="LogIn.csp?__REDIRECT=index.html">sign in</a> or <a href="SignUp.csp?__REDIRECT=index.html">sign up</a>
              </xsl:otherwise>
            </xsl:choose>
      </div>
    </xsl:if>
  </xsl:template>

  <xsl:template name="Content">
    No content has been set. Please add a template named <b>"Content"</b> in your XSLT file.".
  </xsl:template>

  <xsl:template name="HtmlHeader">
    <div id="header">
      <a href="index.html">
        <img src="res/header.png" alt="" border="0" align="left"/>
      </a>
      <div id="headermenu">
        <xsl:call-template name="LoginSimple"/>
{foreach cat}
	  {if cat.Name}
  		  <a href="{cat.Parsed}.html">{cat.Name}</a> |
	  {else}
  		  <a href="{cat.Parsed}.html">Home</a> |
	  {endif}
{endfor}
      </div>
      <div class="clear"/>
    </div>
  </xsl:template>

  <xsl:template name="HtmlFooter">
    <div id="footer">
      Do you like this application?<br/>
      It was created in only a few minutes using <a href="http://devronium.com/GyroGears.html">GyroGears</a> &#xA9; 2008-2014 <a href="http://devronium.com">Devronium Applications</a>.
    </div>
  </xsl:template>

  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
      <head>
        <title>
          <xsl:call-template name="Title"/>
        </title>
        <xsl:if test="HTMLContainer/NoHeader='0'">
          <xsl:call-template name="Header"/>
        </xsl:if>
        <xsl:call-template name="Scripts"/>
      </head>
      <body>
        <xsl:choose>
          <xsl:when test="HTMLContainer/NoHeader='0'">
            <div id="MasterContainer">
            <xsl:call-template name="HtmlHeader"/>
            <div id="MasterContent">
              <xsl:call-template name="Message"/>
              <xsl:call-template name="Response"/>
              <xsl:call-template name="Content"/>
            </div>
            <xsl:call-template name="HtmlFooter"/>
            </div>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="Message"/>
            <xsl:call-template name="Response"/>
            <xsl:call-template name="Content"/>
          </xsl:otherwise>
        </xsl:choose>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>
