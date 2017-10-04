<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
    xmlns:apex="http://med.kuleuven.be/exporl/apex/3.0.2/experiment"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="1.0">
    <xsl:template match="@*|node()">
        <xsl:copy>
            <xsl:apply-templates select="@*|node()"/>
        </xsl:copy>
    </xsl:template>
    <xsl:template match="/apex:apex/stimuli/pluginstimuli/script">
        <xsl:copy>
            <xsl:apply-templates select="@*"/>
            <xsl:text>replaces all child nodes</xsl:text>
        </xsl:copy>
    </xsl:template>
    <xsl:template match="/apex:apex/stimuli/pluginstimuli/script/@source">
        <xsl:copy/>
        <xsl:attribute name="source2">replacement or added text</xsl:attribute>
    </xsl:template>
</xsl:stylesheet>
