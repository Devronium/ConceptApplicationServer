<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:import href="tpls/_General.xslt"/>
  <xsl:output method="html" indent="yes"/>

  <xsl:template name="Scripts">
    <script type="text/javascript">
      //<![CDATA[
	  $(function() {
		  $( "#dialog" ).dialog();
	  });

	  function encrypt() {
		  var s = document.loginform.pwd.value;
		  var s = hex_md5(s);
		  document.loginform.password.value = s;
                  document.loginform.pwd.value = ""
	  }
  //]]>
    </script>
  </xsl:template>

  <xsl:template name="Title">
    <xsl:text>Log in</xsl:text>
  </xsl:template>

  <xsl:template name="Content">
    <xsl:variable name="format" select="HTMLContainer/Format"/>
    <xsl:variable name="username" select="HTMLContainer/Username"/>
    <xsl:variable name="uid" select="HTMLContainer/UID"/>
    <xsl:variable name="scriptid" select="HTMLContainer/ScriptID"/>
    <xsl:variable name="redirect" select="HTMLContainer/Redirect"/>
    <xsl:variable name="op" select="HTMLContainer/Op"/>
    <xsl:variable name="message" select="HTMLContainer/Message"/>
    <xsl:variable name="response" select="HTMLContainer/Response"/>
    <xsl:variable name="template" select="HTMLContainer/Template"/>

    <div class="ui-overlay">
      <div id="dialog" title="Log in" class="ui-widget ui-widget-content ui-corner-all">
        <form id="loginform" name="loginform" method="post" action="">

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

          <xsl:if test="HTMLContainer/Op = 'ok'">
            <xsl:choose>
              <xsl:when test="HTMLContainer/Redirect != ''">
                <script>
                  window.location.href="<xsl:value-of select="$redirect"/>";
                </script>
              </xsl:when>
              <xsl:otherwise>
                <script>window.location.href="/" />"</script>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:if>

          <label for="username">Username:</label>
          <br/>
          <input type="text" name="username" tabindex="1" id="username" />
          <br/>
          <label for="password">Password:</label>
          <br/>
          <input type="password" name="pwd" tabindex="2" id="pwd" />
          <input name="password" type="hidden" id="password" value="" />
          <br/>
          <label for="remember_me">Remember me ?</label>
          <input type="checkbox" name="remember" class="checkboxclass" value="1" tabindex="3" id="remember" />
          <br/>
          <input name="__REDIRECT" type="hidden" value="{$redirect}" />
          <input name="method" type="hidden" value="md5" />
          <br/>
          <a href="SignUp.csp">New user? Click here to signup.</a>
          <br/>
          <input name="Submit" class="submitbutton" type="submit" id="submit" tabindex="4" value="Log in" onclick="encrypt();"/>
        </form>
      </div>

    </div>
  </xsl:template>
</xsl:stylesheet>
