					<select id="{member_name}" name="{member_name}">
{foreach value}
					<xsl:choose>
						<xsl:when test='HTMLContainer/Data/{member_name} = "{value}"'>
							<option value="{value}" selected="selected">{value}</option>
						</xsl:when>
						<xsl:otherwise>
							<option value="{value}">{value}</option>
						</xsl:otherwise>
					</xsl:choose>
{endfor}
					</select>
