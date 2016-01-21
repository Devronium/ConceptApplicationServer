				<!-- radio group for "{member_name}" -->
{foreach value}
				<xsl:choose>
					<xsl:when test='HTMLContainer/Data/{member_name} = "{value}"'>
						<input name="{member_name}" type="radio" value="{value}" checked="checked"/>{value}
					</xsl:when>
					<xsl:otherwise>
						<input name="{member_name}" type="radio" value="{value}"/>{value}
					</xsl:otherwise>
				</xsl:choose>
				<br />
{endfor}
				<!-- end of radio group "{member_name}" -->
