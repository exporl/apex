<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"  
    xmlns:apex="http://med.kuleuven.be/exporl/apex/result">

    <xsl:output method='text'/>
    
    <xsl:variable name="experiment_file" select="apex:results/@experiment_file"/>
    
    <xsl:template match="/">

        <!-- select all procedure id's from experiment file -->
        <xsl:variable name="procedures" select="document($experiment_file)/apex:apex/procedure/procedure/@id"/>
   
        <xsl:apply-templates select="/apex:results">
            <xsl:with-param name="curproc">proc_mcl</xsl:with-param>
        </xsl:apply-templates>
        <xsl:apply-templates select="/apex:results">
            <xsl:with-param name="curproc">proc_thresh</xsl:with-param>
        </xsl:apply-templates>
        
        <!--xsl:apply-templates select="apex:results"/-->
    </xsl:template>
    

    <xsl:template match="/apex:results">
        <xsl:param name="curproc"></xsl:param>
        <xsl:text>&#10;&#10;</xsl:text>
        <xsl:text>{Results from procedure </xsl:text> <xsl:value-of select="$curproc"/> 
        <xsl:text>}</xsl:text>
        
        <xsl:text>&#10;&#10;[Stimulus/parameter]&#10;</xsl:text>
        <xsl:call-template name="getcorrectanswers">
            <xsl:with-param name="curproc" select="$curproc"/>
        </xsl:call-template>
        
        <xsl:text>&#10;</xsl:text>
        <xsl:call-template name="getparameters">
            <xsl:with-param name="curproc" select="$curproc"/>
        </xsl:call-template>
        
        <xsl:text>&#10;&#10;[Response]&#10;</xsl:text>
        <xsl:call-template name="getsequence">
            <xsl:with-param name="curproc" select="$curproc"/>
        </xsl:call-template>

    </xsl:template>        
    
    
    <!-- get all stimuli and show associated fixedparameter label -->
    <xsl:template name="getcorrectanswers">
        <xsl:param name="curproc"/>
        <xsl:apply-templates select="trial[procedure/@id=$curproc]/procedure/stimulus"/>
    </xsl:template>
    <xsl:template name="getcorrectanswer" match="trial/procedure/stimulus">
            <xsl:variable name="trialname" select="."/>
            <xsl:value-of select="$trialname"/>
            <xsl:text>&#x9;</xsl:text>
    </xsl:template>
    
    
    
    <xsl:template name="getsequence">
        <xsl:param name="curproc"/>
        <xsl:apply-templates select="trial[procedure/@id=$curproc]/corrector/currentanswer"/>
    </xsl:template>
    <xsl:template match="trial/corrector/currentanswer">
        <xsl:value-of select="."/>
        <xsl:text>&#x9; </xsl:text>
    </xsl:template>
    
    <xsl:template name="getparameters">
        <xsl:param name="curproc"/>
        <xsl:apply-templates select="trial[procedure/@id=$curproc]/procedure/parameter"/>
    </xsl:template>
    <xsl:template match="trial/procedure/parameter">
        <xsl:value-of select="."/>
        <xsl:text>&#x9; </xsl:text>
    </xsl:template>
    
    
    <!-- get all stimuli and show associated fixedparameter label -->
    <xsl:template name="getlabels">
        <xsl:apply-templates select="trial/procedure/stimulus"/>
    </xsl:template>

    
    
</xsl:stylesheet>
