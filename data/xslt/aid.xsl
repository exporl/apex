<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"  
    xmlns:apex="http://med.kuleuven.be/exporl/apex/result">

    <xsl:output method='text'/>
    
    <xsl:variable name="experiment_file" select="apex:results/@experiment_file"/>
    
    <xsl:template match="/">
        <xsl:apply-templates select="apex:results"/>
    </xsl:template>

    <xsl:template match="apex:results">
        <xsl:text>Results from </xsl:text> <xsl:value-of select="$experiment_file"/> 
        
        <xsl:text>&#10;&#10;[Stimulus/respons pairs]&#10;</xsl:text>
         <!--xsl:apply-templates select="trial"/-->
        <xsl:call-template name="getstimuli"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:call-template name="getresponsescorrect"/>
        
        <xsl:text>&#10;&#10;[Reversals]&#10;</xsl:text>
        <xsl:call-template name="getreversals"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:call-template name="meanreversals"/>
                
        <xsl:text>&#10;&#10;[Sequence data]&#10;</xsl:text>
        <xsl:call-template name="getsequence"/>
        
        <xsl:text>&#10;&#10;[Label value]&#10;</xsl:text>
        <xsl:call-template name="getlabels"/>
    </xsl:template>        
    
    <xsl:template name="getstimuli">
        <xsl:apply-templates select="trial/procedure/stimulus"/>
    </xsl:template>
    <xsl:template name="getstimulus" match="trial/procedure/stimulus">
        <xsl:if test="not(../../@id='VIRTUAL_TRIAL')">
            <xsl:value-of select="."/>
            <xsl:text>&#x9;</xsl:text>
        </xsl:if>
    </xsl:template>
    
    <xsl:template name="getresponsescorrect">
        <xsl:apply-templates select="trial/corrector/result"/>
    </xsl:template>
    <xsl:template match="trial/corrector/result">
        <xsl:value-of select="."/>
        <xsl:text>&#x9; </xsl:text>
    </xsl:template>
    
    
    <xsl:template name="getreversals">
        <xsl:apply-templates select="trial/procedure/reversal"/>
    </xsl:template>
    <xsl:template match="trial/procedure/reversal">
        <!-- get previous trial-->
        <xsl:variable name="stimname" select="../../preceding-sibling::trial[1]/procedure/parameter"/>
        <!--xsl:value-of select="../parameter"/-->
        <xsl:value-of select="$stimname"/>
        <xsl:text>&#x9; </xsl:text>
    </xsl:template>
    
    <xsl:template name="getsequence">
        <xsl:apply-templates select="trial/procedure/parameter"/>
    </xsl:template>
    <xsl:template match="trial/procedure/parameter">
        <xsl:value-of select="."/>
        <xsl:text>&#x9; </xsl:text>
    </xsl:template>
    
    <xsl:template name="meanreversals">
        <xsl:apply-templates mode="mean" select="trial/procedure/reversal"/>
        
        <xsl:variable name="number" select="number(document($experiment_file)/apex:apex/procedure/parameters/rev_for_mean)"/>
        <xsl:variable name="revset" select="trial/procedure/reversal"/>
        <xsl:variable name="total" select="count($revset)"/>
        <xsl:variable name="offset" select="$total - $number"/>
        
        <xsl:text>Mean of last </xsl:text>
        <xsl:value-of select="$number"/> 
        <xsl:text> reversals = </xsl:text>
        <xsl:value-of select="sum($revset[position()>$offset]/../parameter) div $number"/>
    </xsl:template>
    <!--xsl:template match="trial/procedure/reversal" mode="mean">
        <xsl:value-of select="sum(../parameter)"/>
    </xsl:template-->

    
    <!-- get all stimuli and show associated fixedparameter label -->
    <xsl:template name="getlabels">
        <xsl:apply-templates select="trial/procedure/stimulus" mode="label"/>
    </xsl:template>
    <xsl:template name="getlabel" match="trial/procedure/stimulus" mode="label">
        <xsl:if test="not(../../@id='VIRTUAL_TRIAL')">
            <xsl:variable name="trialname" select="."/>
            <xsl:value-of select="document($experiment_file)//apex:apex/stimuli/stimulus[@id=string($trialname)]/fixedParameters/parameter[@id='label']"/>
            <xsl:text>&#x9;</xsl:text>
        </xsl:if>
    </xsl:template>
    
    
</xsl:stylesheet>
