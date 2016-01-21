<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:csp="http://www.devronium.com/csp" extension-element-prefixes="csp">
  <xsl:import href="tpls/_General.xslt"/>
  <xsl:output method="html" indent="yes"/>

  <xsl:template name="Scripts">
    <xsl:variable name="scriptid" select="HTMLContainer/ScriptID"/>
    <xsl:variable name="__DBID" select="HTMLContainer/Data/__DBID"/>
    <script type="text/javascript">
      $(function() {ldelim}
        $("#master_{public_name}").tabs(
      {literal}
          {
            history: true,
            create: function(event, ui) {
	            $("a[rel=pagination]", ui.panel).live("click", function () {
		            $(ui.panel).load(this.href);
		            return false;
	            });
            }
          }
        );
      {endliteral}
        $("a[rel=img_group_{public_name}]").fancybox({literal}
        {
          'transitionIn'		: 'elastic',
          'transitionOut'		: 'elastic',
          'titlePosition' 	: 'over',
          'titleFormat'		: function(title, currentArray, currentIndex, currentOpts) {
            return '<span id="fancybox-title-over">Image ' + (currentIndex + 1) + ' / ' + currentArray.length + (title.length ? ' &amp;nbsp; ' + title : '') + '</span>';
          }
        });
      {endliteral}
        $("a[rel=movie_group_{public_name}]").fancybox({literal}
        {
          'transitionIn'		: 'elastic',
          'transitionOut'		: 'elastic'
        });
      {endliteral}
      {foreach category}
      {foreach category.Elements}
      {if category.Elements.is_relation}
      $("#ax-{category.Elements.member_name}").load('_REL_{public_name}{category.Elements.member_name}.csp?p=<xsl:value-of select="$__DBID" />&amp;__REDIRECT=<xsl:value-of select="$scriptid" />.csp%253Fid%253D<xsl:value-of select="$__DBID" />&amp;noheader=1&amp;fullusers=1', function() {ldelim}
        $("a[rel=img_group_{category.Elements.member_name2}]").fancybox({literal} {
          'transitionIn'		: 'elastic',
          'transitionOut'		: 'elastic',
          'titlePosition' 	: 'over',
          'titleFormat'		: function(title, currentArray, currentIndex, currentOpts) {
            return '<span id="fancybox-title-over">Image ' + (currentIndex + 1) + ' / ' + currentArray.length + (title.length ? ' &#160; ' + title : '') + '</span>';
          }
        });
      });
      {endliteral}
      {else}
      {if category.Elements.is_mixed}
      $("#ax-{category.Elements.member_name}").load('_REL_{public_name}{category.Elements.member_name}.csp?p=<xsl:value-of select="$__DBID" />&amp;__REDIRECT=<xsl:value-of select="$scriptid" />.csp%253Fid%253D<xsl:value-of select="$__DBID" />&amp;noheader=1&amp;fullusers=1');
      {endif}
      {endif}
      {endfor}
      {endfor}
      {literal}
      });
      {endliteral}
    </script>
    {extra_head}
  </xsl:template>

  <xsl:template name="Title">
    <xsl:if test="HTMLContainer/Title!=''">
      <xsl:value-of select="HTMLContainer/Title"/>
      <xsl:text> | </xsl:text>
    </xsl:if>
    <xsl:text>{title}</xsl:text>
  </xsl:template>

  <xsl:template name="Content">
    <xsl:variable name="format" select="HTMLContainer/Format"/>
    <xsl:variable name="username" select="HTMLContainer/Username"/>
    <xsl:variable name="uid" select="HTMLContainer/UID"/>
    <xsl:variable name="scriptid" select="HTMLContainer/ScriptID"/>
    <xsl:variable name="__DBID" select="HTMLContainer/Data/__DBID"/>
    <xsl:variable name="redirect" select="HTMLContainer/Redirect"/>
    <xsl:variable name="op" select="HTMLContainer/Op"/>
    <xsl:variable name="message" select="HTMLContainer/Message"/>
    <xsl:variable name="response" select="HTMLContainer/Response"/>
    <xsl:variable name="template" select="HTMLContainer/Template"/>
    <!-- <xsl:variable name="title" select="'{title}'"/> -->

    <xsl:if test="HTMLContainer/CanAdd='1'">
      <a href="{public_name}.csp?xslt&#x3D;tpls/{public_name}.edit.xslt&amp;op&#x3D;new&amp;fullusers&#x3D;1">New</a>|
    </xsl:if>
    <xsl:if test="HTMLContainer/CanModify='1'">
      <a href="{public_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;xslt&#x3D;tpls/{public_name}.edit.xslt&amp;fullusers&#x3D;1">Modify</a>|
    </xsl:if>
    <xsl:if test="HTMLContainer/CanArchive='1'">
      <a href="{public_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;xslt&#x3D;&amp;op&#x3D;arc&amp;fullusers&#x3D;1">Archive</a>|
    </xsl:if>
    <xsl:if test="HTMLContainer/CanDelete='1'">
      <a href="{public_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;xslt&#x3D;&amp;op&#x3D;del&amp;fullusers&#x3D;1">Delete</a>|
    </xsl:if>

<!--    <xsl:if test="HTMLContainer/Data/__CHANGED_FLAG/__DBID &gt; 0 or HTMLContainer/Data/_CTIME != ''"> -->
      <div class="tablecell">
        <xsl:if test="HTMLContainer/Parent/*[1]/__DBID &gt; 0">
          <xsl:variable name="ParentID" select="HTMLContainer/Parent/*[1]/__DBID" />
          <xsl:variable name="ParentType" select="name(HTMLContainer/Parent/*[1])" />
          <a href="{ldelim}$ParentType{rdelim}.csp?id={ldelim}$ParentID{rdelim}">Open parent</a> |
        </xsl:if>
        <xsl:if test="HTMLContainer/Data/__CHANGED_FLAG/__DBID &gt; 0">
          <xsl:variable name="_CUID" select="HTMLContainer/Data/__CHANGED_FLAG/__DBID"/>
          {if userentity}
          created by <a href="{userentity}.csp?id={ldelim}$_CUID{rdelim}&amp;fullusers=1">
            <b>
              <xsl:value-of select="HTMLContainer/Data/__CHANGED_FLAG/{userrepresentative}"/>
            </b>
          </a> |
          {else}
          by <b>
            <xsl:value-of select="HTMLContainer/Data/__CHANGED_FLAG/{userrepresentative}"/>
          </b> |
          {endif}
        </xsl:if>
        <xsl:if test="HTMLContainer/Data/_CTIME != ''">
          created on <xsl:value-of select="HTMLContainer/Data/_CTIME"/>
          <xsl:if test="HTMLContainer/Data/_CTIME != HTMLContainer/Data/_MODTIME">
            | modified on <xsl:value-of select="HTMLContainer/Data/_MODTIME"/>
          </xsl:if>
        </xsl:if>
      </div>
<!--        </xsl:if> -->
    <xsl:if test="HTMLContainer/Data!='0'">

      <div id="master_{public_name}">
        <ul>
          {foreach category}
          <li>
            <a href="#tab-{category.Reserved}">{category.Name}</a>
          </li>
          {endfor}
        </ul>
        {foreach category}
        <div id="tab-{category.Reserved}">
          <table>
            {foreach category.Elements}
            <tr>
              {if category.Elements.is_subcat}
              <td colspan="2">
                <u>
                  <b>{category.Elements.column_name}</b>
                </u>
              </td>
              {else}
              <xsl:variable name="{category.Elements.member_name}" select="HTMLContainer/Data/{category.Elements.member_name}"/>
              <td align="right">
                <b>{category.Elements.column_name}</b>
              </td>
              <td>
                {if category.Elements.is_picture}
                <xsl:variable name="{category.Elements.member_name}_filename" select="HTMLContainer/Data/{category.Elements.member_name}_filename"/>
                <xsl:choose>
                  <xsl:when test="HTMLContainer/Data/{category.Elements.member_name}_filename != ''">
                    <a rel="img_group_{public_name}" href="get{public_name}{category.Elements.member_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;e&#x3D;.jpg">
                      <img src="get{public_name}{category.Elements.member_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;preview&#x3D;1" alt="{ldelim}${category.Elements.member_name}_filename{rdelim}" />
                    </a>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:call-template name="None"/>
                  </xsl:otherwise>
                </xsl:choose>
                {else}
                {if category.Elements.is_file}
                <xsl:variable name="{category.Elements.member_name}_filename" select="HTMLContainer/Data/{category.Elements.member_name}_filename"/>
                <xsl:choose>
                  <xsl:when test="HTMLContainer/Data/{category.Elements.member_name}_filename != ''">
                    <a href="get{public_name}{category.Elements.member_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}">
                      <xsl:value-of select="HTMLContainer/Data/{category.Elements.member_name}_filename"/>
                    </a>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:call-template name="None"/>
                  </xsl:otherwise>
                </xsl:choose>
                {else}
                {if category.Elements.is_video}
                <xsl:variable name="{category.Elements.member_name}_filename" select="HTMLContainer/Data/{category.Elements.member_name}_filename"/>
                <xsl:choose>
                  <xsl:when test="HTMLContainer/Data/{category.Elements.member_name}_filename != ''">
                    <a rel="movie_group_{public_name}" href="Show{public_name}{category.Elements.member_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}">
                      <img src="get{public_name}{category.Elements.member_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;preview&#x3D;1" alt="{ldelim}${category.Elements.member_name}_filename{rdelim}" />
                    </a>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:call-template name="None"/>
                  </xsl:otherwise>
                </xsl:choose>
                {else}
                {if category.Elements.is_mixed}
                <div id="ax-{category.Elements.member_name}"></div>
                {else}
                {if category.Elements.is_relation}
                <div id="ax-{category.Elements.member_name}"></div>
                {else}
                {if category.Elements.no_format}
                <xsl:value-of disable-output-escaping="yes" select="HTMLContainer/Data/{category.Elements.member_name}"/>
                {else}
                {if category.Elements.is_secret}
                <b>********</b>
                {else}
                <xsl:value-of select="HTMLContainer/Data/{category.Elements.member_name}"/>
                {endif}
                {endif}
                {endif}
                {endif}
                {endif}
                {endif}
                {endif}
              </td>
              {endif}
            </tr>
            {endfor}
          </table>
        </div>
        {endfor}
      </div>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>
