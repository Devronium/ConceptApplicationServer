<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:import href="tpls/_General.xslt"/>
  <xsl:output method="html" indent="yes"/>

  <xsl:template name="Content">
    <xsl:variable name="redirect" select="HTMLContainer/Redirect"/>
    <xsl:variable name="op" select="HTMLContainer/Op"/>
    <xsl:variable name="message" select="HTMLContainer/Message"/>
    <xsl:variable name="template" select="HTMLContainer/Template"/>

    <xsl:variable name="sort" select="HTMLContainer/Sort"/>
    <xsl:variable name="desc" select="HTMLContainer/Desc"/>
    <xsl:variable name="criteria" select="HTMLContainer/Criteria"/>
    <xsl:variable name="format" select="HTMLContainer/Format"/>
    <xsl:variable name="username" select="HTMLContainer/Username"/>
    <xsl:variable name="uid" select="HTMLContainer/UID"/>
    <xsl:variable name="scriptid" select="HTMLContainer/ScriptID"/>
    <xsl:variable name="prev_link" select="HTMLContainer/PrevLink"/>
    <xsl:variable name="next_link" select="HTMLContainer/NextLink"/>
    <xsl:variable name="current_page" select="HTMLContainer/Page"/>
    <xsl:variable name="total_pages" select="HTMLContainer/TotalPages"/>
    <xsl:variable name="parent" select="HTMLContainer/IsChildOf"/>

    <xsl:if test="$parent = ''">
      <xsl:if test="HTMLContainer/CanAdd='1'">
        <a href="{public_name}.csp?xslt&#x3D;tpls/{public_name}.edit.xslt&amp;op&#x3D;new&amp;__REDIRECT&#x3D;{ldelim}$redirect{rdelim}&amp;fullusers&#x3D;1">New</a>| <br/>
      </xsl:if>
    </xsl:if>

    <xsl:choose>
      <xsl:when test="HTMLContainer/Count &gt; 0">
        <div class="ui-widget">
          <xsl:for-each select="HTMLContainer/Data/element">
            <xsl:variable name="__DBID" select="__DBID"/>
            <xsl:variable name="type" select="__VIEWINDEX"/>
            <xsl:variable name="iconmember" select="__EVENINDEX"/>
            <xsl:variable name="description" select="__USERDATA"/>
            <div class="ui-widget ui-widget-content">
              <a href="{$type}.csp?id&#x3D;{$__DBID}&amp;fullusers&#x3D;1">
                <xsl:if test="$iconmember!='0'">
                  <img src="get{$type}{$iconmember}.csp?id&#x3D;{$__DBID}&amp;thumb&#x3D;1" alt=""/>
                  <br/>
                </xsl:if>
                <xsl:value-of select="$description"/>
              </a>
            </div>
          </xsl:for-each>
        </div>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="NoRecords"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>
