<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="fractal">
    <xsl:param name="depth" select="@depth"/>
    <table border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td>
          <xsl:call-template name="iterate">
            <xsl:with-param name="depth" select="$depth"/>
          </xsl:call-template>
        </td>
      </tr>
      <tr>
        <td>
          <xsl:call-template name="iterate">
            <xsl:with-param name="depth" select="$depth"/>
          </xsl:call-template>
        </td>
        <td>
          <xsl:call-template name="iterate">
            <xsl:with-param name="depth" select="$depth"/>
          </xsl:call-template>
        </td>
      </tr>
    </table>
  </xsl:template>

  <xsl:template name="iterate">
    <xsl:param name="depth"/>
    <xsl:choose>
      <xsl:when test="$depth &gt; 0">
        <xsl:apply-templates select=".">
          <xsl:with-param name="depth" select="$depth+(-1)"/>
        </xsl:apply-templates>  
      </xsl:when>
      <xsl:otherwise>
        <table cellpadding="0"  cellspacing="0" border="0" height="{@size}" width="{@size}">
          <tr><td bgcolor="{@color}"></td></tr>
        </table>
      </xsl:otherwise>
    </xsl:choose> 
  </xsl:template>

</xsl:stylesheet>