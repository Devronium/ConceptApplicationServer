<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:csp="http://www.devronium.com/csp" extension-element-prefixes="csp">
  <xsl:import href="tpls/_General.xslt"/>
  <xsl:output method="html" indent="yes"/>

  <xsl:template name="Login">
    <!-- Don't show login -->
  </xsl:template>

  <xsl:template name="Title">
    <xsl:text>Sign up ...</xsl:text>
  </xsl:template>

  <xsl:template name="Scripts">
    <xsl:variable name="scriptid" select="HTMLContainer/ScriptID"/>
    <xsl:variable name="__DBID" select="HTMLContainer/Data/__DBID"/>
    <script type="text/javascript">
	$(function() {ldelim}
{literal}
		$('textarea.richtext').tinymce({
			script_url : 'js/tiny_mce/tiny_mce.js',

			// General options
			theme : "advanced",
			plugins : "autolink,lists,pagebreak,style,layer,table,save,advhr,advimage,advlink,emotions,iespell,inlinepopups,insertdatetime,preview,media,searchreplace,print,contextmenu,paste,directionality,fullscreen,noneditable,visualchars,nonbreaking,xhtmlxtras,template,advlist",

			// Theme options
			theme_advanced_buttons1 : "save,newdocument,|,bold,italic,underline,strikethrough,|,justifyleft,justifycenter,justifyright,justifyfull,styleselect,formatselect,fontselect,fontsizeselect",
			theme_advanced_buttons2 : "cut,copy,paste,pastetext,pasteword,|,search,replace,|,bullist,numlist,|,outdent,indent,blockquote,|,undo,redo,|,link,unlink,anchor,image,cleanup,help,code,|,insertdate,inserttime,preview,|,forecolor,backcolor",
			theme_advanced_buttons3 : "tablecontrols,|,hr,removeformat,visualaid,|,sub,sup,|,charmap,emotions,iespell,media,advhr,|,print,|,ltr,rtl,|,fullscreen",
			theme_advanced_buttons4 : "insertlayer,moveforward,movebackward,absolute,|,styleprops,|,cite,abbr,acronym,del,ins,attribs,|,visualchars,nonbreaking,template,pagebreak",
			theme_advanced_toolbar_location : "top",
			theme_advanced_toolbar_align : "left",
			theme_advanced_statusbar_location : "bottom",
			theme_advanced_resizing : true
		});
{endliteral}
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
{endif}
{endfor}
{endfor}
{literal}
	});
{endliteral}
    </script>
    {extra_head}
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

    <form method="POST" action="" enctype="multipart/form-data">
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
              {if category.Elements.mandatory}
              <td align="right">
                <b>{category.Elements.column_name}</b>
              </td>
              {else}
              <td align="right">{category.Elements.column_name}</td>
              {endif}
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
                    <i>(none)</i>
                  </xsl:otherwise>
                </xsl:choose>
                <br/>
                <input type="file" id="{category.Elements.member_name}" name="{category.Elements.member_name}" />
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
                    <i>(none)</i>
                  </xsl:otherwise>
                </xsl:choose>
                <br/>
                <input type="file" id="{category.Elements.member_name}" name="{category.Elements.member_name}" />
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
                    (none)
                  </xsl:otherwise>
                </xsl:choose>
                <br/>
                <input type="file" id="{category.Elements.member_name}" name="{category.Elements.member_name}" />
                {else}
                {if category.Elements.is_relation}
                <div id="ax-{category.Elements.member_name}"></div>
                {else}
                {if category.Elements.no_format}
                <textarea rows="2" cols="20" name="{category.Elements.member_name}">
                  <xsl:value-of disable-output-escaping="yes" select="HTMLContainer/Data/{category.Elements.member_name}"/>
                </textarea>
                {else}
                {if category.Elements.is_text}
                <textarea rows="2" cols="20" name="{category.Elements.member_name}">
                  <xsl:value-of select="HTMLContainer/Data/{category.Elements.member_name}"/>
                </textarea>
                {else}
                {if category.Elements.is_bool}
                <xsl:choose>
                  <xsl:when test="HTMLContainer/Data/{category.Elements.member_name} = '1'">
                    <input name="{category.Elements.member_name}" type="checkbox" value="1" checked="checked"/>
                  </xsl:when>
                  <xsl:otherwise>
                    <input name="{category.Elements.member_name}" type="checkbox" value="1"/>
                  </xsl:otherwise>
                </xsl:choose>
                {else}
                {if category.Elements.is_combo}
                {category.Elements.values}
                {else}
                {if category.Elements.is_radio}
                {category.Elements.values}
                {else}
                {if category.Elements.is_secret}
                <input type="password" id="{category.Elements.member_name}" name="{category.Elements.member_name}" value=""/>
                <input type="password" id="{category.Elements.member_name}_repeated" name="{category.Elements.member_name}_repeated" value=""/>
                {else}
                <input type="text" id="{category.Elements.member_name}" name="{category.Elements.member_name}" value="{ldelim}${category.Elements.member_name}{rdelim}"/>
                {endif}
                {endif}
                {endif}
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
      <div class="signup">
        <img src="Captcha.csp" alt="Captcha" />
        <br />
        <input type="text" name="_c" value=""/>
        <br />
        <xsl:choose>
          <xsl:when test="HTMLContainer/Op = 'ok'">

            <xsl:choose>
              <xsl:when test="HTMLContainer/Redirect != ''">
                <script>
                  window.location.href="<xsl:value-of select="$redirect"/>"
                </script>
              </xsl:when>
              <xsl:otherwise>
                <script>
                  window.location.href="{public_name}.csp?id=<xsl:value-of select="$__DBID" />&amp;fullusers=1"
                </script>
              </xsl:otherwise>
            </xsl:choose>

          </xsl:when>
          <xsl:otherwise>
            <input type="hidden" name="op" value="add"/>
          </xsl:otherwise>
        </xsl:choose>
        <input type="hidden" name="__DBID" value="{ldelim}$__DBID{rdelim}"/>
        <input type="hidden" name="__REDIRECT" value="{ldelim}$redirect{rdelim}"/>
        <input type="hidden" name="xslt" value="{ldelim}$template{rdelim}"/>
        <br/>
      </div>
      <input type="submit" class="savebutton" value="Sign up"/>
    </form>
  </xsl:template>
</xsl:stylesheet>