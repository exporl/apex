<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:apex="http://med.kuleuven.be/exporl/apex/result" xmlns:svg="http://www.w3.org/2000/svg"
    xmlns:xlink="http://www.w3.org/1999/xlink">


    <xsl:import href="apexresult.xsl"/>


    <xsl:template name="extraprocessing">
        <xsl:choose>
            <xsl:when test="$target='parser'"> </xsl:when>
            <xsl:when test="$target='html'">
                <h2>Extra stuff</h2>
            </xsl:when>
            <xsl:when test="$target='text'"> </xsl:when>
        </xsl:choose>


    </xsl:template>

</xsl:stylesheet>
