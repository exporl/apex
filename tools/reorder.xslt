<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:apex="http://www.kuleuven.be/exporl/Lab/Apex">



  <xsl:template match="apex:apex">
    <xsl:copy select="apex">

                <xsl:copy-of select="procedure" />

                <xsl:copy-of select="corrector"/>

      <xsl:copy-of select="screens"/>
      <xsl:copy-of select="datablocks"/>
      <xsl:copy-of select="devices"/>
      <xsl:copy-of select="filters"/>
      <xsl:copy-of select="stimuli"/>
      <xsl:copy-of select="connections"/>

    </xsl:copy>
  </xsl:template>


</xsl:stylesheet>
