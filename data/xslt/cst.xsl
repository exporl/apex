<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"  
    xmlns:apex="http://med.kuleuven.be/exporl/apex/result">

    <xsl:output method='text'/>
    
    <xsl:variable name="experiment_file" select="apex:results/@experiment_file"/>
    <xsl:variable name="answer_element" select="document($experiment_file)/apex:apex/screens/screen/default_answer_element" />
    
    <xsl:template match="/">
        <xsl:apply-templates select="apex:results"/>
    </xsl:template>

    <xsl:template match="apex:results">
        <xsl:text>Results from </xsl:text> <xsl:value-of select="$experiment_file"/> 
        
        <xsl:text>&#10;&#10;[Stimulus/answer/correct pairs]&#10;</xsl:text>
         <!--xsl:apply-templates select="trial"/-->
        <xsl:call-template name="getstimuli"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:call-template name="getanswers"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:call-template name="getcorrectorresult"/>
    </xsl:template>        
    
    <xsl:template name="getstimuli">
        <xsl:apply-templates select="trial/procedure/stimulus"/>
    </xsl:template>
    
    <xsl:template name="getstimulus" match="trial/procedure/stimulus">
        <xsl:value-of select="."/>
        <xsl:text>&#x9;</xsl:text>
    </xsl:template>
    
    <xsl:template name="getanswers">
        <xsl:apply-templates select="trial/corrector/currentanswer"/>
    </xsl:template>
    
    <xsl:template name="getanswer" match="trial/corrector/currentanswer">
        <xsl:value-of select="."/>
        <xsl:text>&#x9;</xsl:text>
    </xsl:template>
    
    
    <xsl:template name="getcorrectorresult">
        <xsl:apply-templates select="trial/corrector/result"/>
    </xsl:template>
    
    <xsl:template match="trial/corrector/result">
        <xsl:value-of select="."/>
        <xsl:text>&#x9;</xsl:text>
    </xsl:template>
    
</xsl:stylesheet>
