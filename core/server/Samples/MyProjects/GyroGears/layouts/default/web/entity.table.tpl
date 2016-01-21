<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:csp="http://www.devronium.com/csp" extension-element-prefixes="csp">
  <xsl:import href="tpls/_General.xslt"/>
  <xsl:output method="html" indent="yes"/>

  <xsl:template name="Scripts">
    {extra_head}
  </xsl:template>

  <xsl:template name="Title">
    <xsl:text>{title}</xsl:text>
  </xsl:template>

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
    <xsl:variable name="noheader" select="HTMLContainer/NoHeader"/>
    <!-- <xsl:variable name="title" select="'{title}'"/> -->

    <xsl:if test="HTMLContainer/CanAdd='1'">
      <a href="{public_name}.csp?xslt&#x3D;tpls/{public_name}.edit.xslt&amp;op&#x3D;new&amp;__REDIRECT&#x3D;{ldelim}$redirect{rdelim}&amp;fullusers&#x3D;1">New</a>| <br/>
    </xsl:if>
    <xsl:if test="$parent = ''">
      <div class="search">
        <form id="search_{public_name}_form" action="{ldelim}$scriptid{rdelim}.csp">
          <div class="simplesearchbox">
            <input type="text" name="q" id="search_{public_name}" class="searchbox" value="{ldelim}$criteria{rdelim}" />
            <input type="submit" class="searchbutton" value="Search" />
          </div>
          <input type="hidden" name="noheader" value="{ldelim}$noheader{rdelim}" />
          <input type="hidden" name="fullusers" value="1" />
          <div class="clear"/>
          {if advanced_member}
          <script type="text/javascript">
            $(function() {ldelim}
                $("h2#advanced_search_{public_name}").toggler({ldelim}method: "toggle", speed: 0{rdelim});
            {rdelim});
          </script>
          <div class="adv_wrapper">
            <div class="adv_content">
              <h2 class="expand" id="advanced_search_{public_name}">Advanced search</h2>
              <div class="collapse">
                <table>
                  {foreach advanced_member}
                  <tr>
                    <td>
                      <p class="labeltext">{advanced_member.column_name}</p>
                    </td>
                    <td>
                      <input class="checkboxclass" type="checkbox" name="filter{advanced_member.index}" id="filter{advanced_member.index}" value="{advanced_member.member_name}" />
                    </td>
                    <td>
                      <select name="op{advanced_member.index}" id="op{advanced_member.index}">
                        <option value="&#x3D;">&#x3D;</option>
                        <option value="&lt;&gt;">&lt;&gt;</option>
                        <option value="&lt;&#x3D;">&lt;&#x3D;</option>
                        <option value="&gt;&#x3D;" selected="selected">&gt;&#x3D;</option>
                        <option value="&lt;">&lt;</option>
                        <option value="&gt;">&gt;</option>
                      </select>
                    </td>
                    <td>
                      <input type="text" name="val{advanced_member.index}" id="val{advanced_member.index}" value="" />
                    </td>
                    <td>
                      <input class="checkboxclass" type="checkbox" name="filter{advanced_member.index2}" id="filter{advanced_member.index2}" value="{advanced_member.member_name}" />
                    </td>
                    <td>
                      <select name="op{advanced_member.index2}" id="op{advanced_member.index2}">
                        <option value="&#x3D;">&#x3D;</option>
                        <option value="&lt;&gt;">&lt;&gt;</option>
                        <option value="&lt;&#x3D;" selected="selected">&lt;&#x3D;</option>
                        <option value="&gt;&#x3D;">&gt;&#x3D;</option>
                        <option value="&lt;">&lt;</option>
                        <option value="&gt;">&gt;</option>
                      </select>
                    </td>
                    <td>
                      <input type="text" name="val{advanced_member.index2}" id="val{advanced_member.index2}" value="" />
                    </td>
                  </tr>
                  {endfor}
                </table>
              </div>
            </div>
          </div>
          <div class="clear"/>
          {endif}
        </form>
      </div>
    </xsl:if>

    <xsl:if test="HTMLContainer/LoggedIn='1'">
      <xsl:if test="$parent != ''">
        <a href="{public_name}.csp?p&#x3D;{ldelim}$parent{rdelim}&amp;w&#x3D;{ldelim}$scriptid{rdelim}&amp;xslt&#x3D;tpls/{public_name}.edit.xslt&amp;op&#x3D;new&amp;__REDIRECT&#x3D;{ldelim}$redirect{rdelim}&amp;fullusers&#x3D;1">New</a>
        <br/>
      </xsl:if>
    </xsl:if>

    <xsl:choose>
      <xsl:when test="HTMLContainer/Count &gt; 0">
        <div class="tablesort">
          <b>Sort by: </b>
          {foreach column}
          {if column.can_sort}
          &#160;&#160;&#160;&#160;
          {column.column_name}
          <a rel="pagination" href="{ldelim}$scriptid{rdelim}.csp?q&#x3D;{ldelim}$criteria{rdelim}&amp;sort&#x3D;{column.member_name}&amp;desc&#x3D;1&amp;p&#x3D;{ldelim}$parent{rdelim}&amp;noheader&#x3D;{ldelim}$noheader{rdelim}&amp;fullusers&#x3D;1">
            <img src="img/arrow-up.gif" alt="Descending" border="0"/>
          </a>&#160;
          <a rel="pagination" href="{ldelim}$scriptid{rdelim}.csp?q&#x3D;{ldelim}$criteria{rdelim}&amp;sort&#x3D;{column.member_name}&amp;desc&#x3D;0&amp;p&#x3D;{ldelim}$parent{rdelim}&amp;xslt&#x3D;{ldelim}$template{rdelim}&amp;noheader&#x3D;{ldelim}$noheader{rdelim}">
            <img src="img/arrow-down.gif" alt="Ascending" border="0"/>
          </a>&#160;
          {endif}
          {endfor}
        </div>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="NoRecords"/>
      </xsl:otherwise>
    </xsl:choose>

    <xsl:if test="$total_pages &gt; 1">
      <div class="tablesort">
        <b>
          <xsl:value-of select="HTMLContainer/Count" />
        </b> records&#160;&#160;&#160;&#160;
        <b>Jump to page</b>&#160;&#160;
        <xsl:if test="$prev_link != ''">
          <a rel="pagination" href="{ldelim}$scriptid{rdelim}.csp?q&#x3D;{ldelim}$criteria{rdelim}&amp;page&#x3D;{ldelim}$prev_link{rdelim}&amp;sort&#x3D;{ldelim}$sort{rdelim}&amp;desc&#x3D;{ldelim}$desc{rdelim}&amp;p&#x3D;{ldelim}$parent{rdelim}&amp;xslt&#x3D;{ldelim}$template{rdelim}&amp;noheader&#x3D;{ldelim}$noheader{rdelim}&amp;fullusers&#x3D;1">&lt;&lt;</a>&#160;
        </xsl:if>
        <xsl:for-each select="HTMLContainer/PageLink/*">
          <xsl:variable name="Page" select="text()"/>
          <xsl:choose>
            <xsl:when test="text() != '0'">
              <a rel="pagination" href="{ldelim}$scriptid{rdelim}.csp?q&#x3D;{ldelim}$criteria{rdelim}&amp;page&#x3D;{ldelim}$Page{rdelim}&amp;sort&#x3D;{ldelim}$sort{rdelim}&amp;desc&#x3D;{ldelim}$desc{rdelim}&amp;p&#x3D;{ldelim}$parent{rdelim}&amp;xslt&#x3D;{ldelim}$template{rdelim}&amp;noheader&#x3D;{ldelim}$noheader{rdelim}&amp;fullusers&#x3D;1">
                <xsl:value-of select="$Page"/>
              </a>&#160;
            </xsl:when>
            <xsl:otherwise>
              <b>
                <xsl:value-of select="$current_page"/>
              </b>&#160;
            </xsl:otherwise>
          </xsl:choose>
        </xsl:for-each>
        <xsl:if test="$next_link != ''">
          <a rel="pagination" href="{ldelim}$scriptid{rdelim}.csp?q&#x3D;{ldelim}$criteria{rdelim}&amp;page&#x3D;{ldelim}$next_link{rdelim}&amp;sort&#x3D;{ldelim}$sort{rdelim}&amp;desc&#x3D;{ldelim}$desc{rdelim}&amp;p&#x3D;{ldelim}$parent{rdelim}&amp;xslt&#x3D;{ldelim}$template{rdelim}&amp;noheader&#x3D;{ldelim}$noheader{rdelim}&amp;fullusers&#x3D;1">&gt;&gt;</a>&#160;
        </xsl:if>
      </div>
    </xsl:if>

    <xsl:for-each select="HTMLContainer/Data/*">
      <xsl:variable name="__DBID" select="__DBID"/>
      <div class="tableitem">
        <div class="tablecellnumber" onclick="location.href='{public_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;fullusers&#x3D;1';" style="cursor: pointer;">
          <xsl:value-of select="__VIEWINDEX"/>
        </div>
        <div class="tablecellnumber" onclick="location.href='{public_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;fullusers&#x3D;1';" style="cursor: pointer;">
        </div>

        <xsl:if test="__CHANGED_FLAG/__DBID &gt; 0 or _CTIME != ''">
          <div class="tableelement">
            <div class="tableinfocolumn" onclick="location.href='{public_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;fullusers&#x3D;1';" style="cursor: pointer;">&#160;</div>
            <div class="tableinfocell">
              <xsl:if test="__CHANGED_FLAG/__DBID &gt; 0">
                <xsl:variable name="_CUID" select="__CHANGED_FLAG/__DBID"/>
                {if userentity}
                by <a href="{userentity}.csp?id={ldelim}$_CUID{rdelim}&amp;fullusers=1">
                  <b>
                    <xsl:value-of select="__CHANGED_FLAG/{userrepresentative}"/>
                  </b>
                </a> |
                {else}
                by <b>
                  <xsl:value-of select="__CHANGED_FLAG/{userrepresentative}"/>
                </b> |
                {endif}
              </xsl:if>
              <xsl:if test="_CTIME != ''">
                on <xsl:value-of select="_CTIME"/>
                <xsl:if test="_CTIME != _MODTIME">
                  | modified on <xsl:value-of select="_MODTIME"/>
                </xsl:if>
              </xsl:if>
            </div>
          </div>
        </xsl:if>

        {foreach column}
        {if column.is_picture}
        <xsl:if test="{column.member_name}_filename != ''">
          {else}
          {if column.is_video}
          <xsl:if test="{column.member_name}_filename != ''">
          {else}
          <xsl:if test="{column.member_name} != ''">
          {endif}
          {endif}
          <div class="tableelement">
            <div class="tablecolumn" onclick="location.href='{public_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;fullusers&#x3D;1';" style="cursor: pointer;">{column.column_name}</div>
            <div class="tablecell">
              {if column.is_picture}
              <xsl:if test="{column.member_name}_filename != ''">
                <a rel="img_group_{public_name}" title="{ldelim}row.{master_field}{rdelim}" href="get{public_name}{column.member_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;e&#x3D;.jpg">
                  <img border="0" src="get{public_name}{column.member_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;thumb&#x3D;1" alt=""/>
                </a>
              </xsl:if>
              {else}
              {if column.is_video}
              <xsl:if test="{column.member_name}_filename != ''">
                <a rel="movie_group_{public_name}" href="Show{public_name}{column.member_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}">
                  <img border="0" src="get{public_name}{column.member_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;thumb&#x3D;1" alt=""/>
                </a>
              </xsl:if>
              {else}
              {if column.is_file}
              <a href="get{public_name}{column.member_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}">
                <xsl:value-of disable-output-escaping="yes" select="{column.member_name}"/>
              </a>
              {else}
              {if column.no_format}
                <xsl:value-of disable-output-escaping="yes" select="{column.member_name}"/>
              {else}
              <a href="{public_name}.csp?id&#x3D;{ldelim}$__DBID{rdelim}&amp;fullusers&#x3D;1">
                <xsl:value-of disable-output-escaping="yes" select="{column.member_name}"/>
              </a>
              {endif}
              {endif}
              {endif}
              {endif}
            </div>
            <div class="clear"></div>
          </div>
          </xsl:if>
        {endfor}
      </div>
      <div class="clear"></div>
    </xsl:for-each>

    <xsl:if test="$total_pages &gt; 1">
      <div class="tablesort">
        <b>
          <xsl:value-of select="HTMLContainer/Count" />
        </b> records&#160;&#160;&#160;&#160;
        <b>Jump to page</b>&#160;&#160;
        <xsl:if test="$prev_link != ''">
          <a rel="pagination" href="{ldelim}$scriptid{rdelim}.csp?q&#x3D;{ldelim}$criteria{rdelim}&amp;page&#x3D;{ldelim}$prev_link{rdelim}&amp;sort&#x3D;{ldelim}$sort{rdelim}&amp;desc&#x3D;{ldelim}$desc{rdelim}&amp;p&#x3D;{ldelim}$parent{rdelim}&amp;xslt&#x3D;{ldelim}$template{rdelim}&amp;noheader&#x3D;{ldelim}$noheader{rdelim}&amp;fullusers&#x3D;1">&lt;&lt;</a>&#160;
        </xsl:if>
        <xsl:for-each select="HTMLContainer/PageLink/*">
          <xsl:variable name="Page" select="text()"/>
          <xsl:choose>
            <xsl:when test="text() != '0'">
              <a rel="pagination" href="{ldelim}$scriptid{rdelim}.csp?q&#x3D;{ldelim}$criteria{rdelim}&amp;page&#x3D;{ldelim}$Page{rdelim}&amp;sort&#x3D;{ldelim}$sort{rdelim}&amp;desc&#x3D;{ldelim}$desc{rdelim}&amp;p&#x3D;{ldelim}$parent{rdelim}&amp;xslt&#x3D;{ldelim}$template{rdelim}&amp;noheader&#x3D;{ldelim}$noheader{rdelim}&amp;fullusers&#x3D;1">
                <xsl:value-of select="$Page"/>
              </a>&#160;
            </xsl:when>
            <xsl:otherwise>
              <b>
                <xsl:value-of select="$current_page"/>
              </b>&#160;
            </xsl:otherwise>
          </xsl:choose>
        </xsl:for-each>
        <xsl:if test="$next_link != ''">
          <a rel="pagination" href="{ldelim}$scriptid{rdelim}.csp?q&#x3D;{ldelim}$criteria{rdelim}&amp;page&#x3D;{ldelim}$next_link{rdelim}&amp;sort&#x3D;{ldelim}$sort{rdelim}&amp;desc&#x3D;{ldelim}$desc{rdelim}&amp;p&#x3D;{ldelim}$parent{rdelim}&amp;xslt&#x3D;{ldelim}$template{rdelim}&amp;noheader&#x3D;{ldelim}$noheader{rdelim}&amp;fullusers&#x3D;1">&gt;&gt;</a>&#160;
        </xsl:if>
      </div>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>