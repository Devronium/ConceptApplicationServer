<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"> 
<xsl:template match="/">
<xsl:variable name="n">
<xsl:value-of select="//arg/@n"/>
</xsl:variable>
<xsl:element name="hanoi-solve">
<xsl:call-template name="dohanoi">
<xsl:with-param name="n" select="number($n)"/>
<xsl:with-param name="to" select="3"/>
<xsl:with-param name="from" select="1"/>
<xsl:with-param name="using" select="2"/>
</xsl:call-template>
</xsl:element>
</xsl:template>
<xsl:template name="dohanoi">
<xsl:param name="n"/>
<xsl:param name="to"/>
<xsl:param name="from"/>
<xsl:param name="using"/>
<xsl:if test="number($n) &gt; 0">
<xsl:call-template name="dohanoi">
<xsl:with-param name="n" select="number($n) - 1"/>
<xsl:with-param name="to" select="$using"/>
<xsl:with-param name="from" select="$from"/>
<xsl:with-param name="using" select="$to"/>
</xsl:call-template>
<xsl:element name="move">
<xsl:attribute name="from">
<xsl:value-of select="$from"/>
</xsl:attribute>
<xsl:attribute name="to">
<xsl:value-of select="$to"/>
</xsl:attribute>
</xsl:element>
<xsl:call-template name="dohanoi">
<xsl:with-param name="n" select="number($n) - 1"/>
<xsl:with-param name="to" select="$to"/>
<xsl:with-param name="from" select="$using"/>
<xsl:with-param name="using" select="$from"/>
</xsl:call-template>
</xsl:if>
</xsl:template>
</xsl:stylesheet>