<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"  
    xmlns:apex="http://med.kuleuven.be/exporl/apex/result">

    <xsl:variable name="experiment_file" select="apex:results/@experiment_file"/>
    
    <xsl:template match="/">
        <xsl:apply-templates select="apex:results"/>
    </xsl:template>

    <xsl:template match="apex:results">
        <xsl:text>Results from </xsl:text> <xsl:value-of select="$experiment_file"/> 
        
        <xsl:text>&#10;&#10;[Correct answers]&#10;</xsl:text>
        <xsl:call-template name="getcorrectanswers"/>
        
        
        <xsl:text>&#10;&#10;[Reversals]&#10;</xsl:text>
        <xsl:call-template name="getreversals"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:call-template name="meanreversals"/>
                
        <xsl:text>&#10;&#10;[Sequence data]&#10;</xsl:text>
        <xsl:call-template name="getsequence"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:call-template name="meantrials"/>
        
        
    </xsl:template>        
    
    <xsl:template name="getresponsescorrect">
        <xsl:apply-templates select="trial/corrector/result"/>
    </xsl:template>
    <xsl:template match="trial/corrector/result">
        <xsl:value-of select="."/>
        <xsl:text>&#x9; </xsl:text>
    </xsl:template>

    
    <xsl:template name="getanswers">
        <xsl:apply-templates select="trial/screenresult/element[@id='text']"/>
    </xsl:template>
    <xsl:template match="trial/screenresult/element[@id='text']">
        <xsl:value-of select="../../@id"/>
        <xsl:text>&#x9;</xsl:text>
        <xsl:value-of select="."/>
        <xsl:text>&#xA;</xsl:text>
      </xsl:template>
    
    
    <!-- get all stimuli and show associated fixedparameter label -->
    <xsl:template name="getcorrectanswers">
        <xsl:apply-templates select="trial/procedure/stimulus"/>
    </xsl:template>
    <xsl:template name="getcorrectanswer" match="trial/procedure/stimulus">
            <xsl:variable name="trialname" select="."/>
            <xsl:value-of select="$trialname"/>
            <xsl:text>&#x9;</xsl:text>
            
            <xsl:value-of select="document($experiment_file)//apex:apex/stimuli/stimulus[@id=string($trialname)]/fixedParameters/parameter[@id='sentence']"/>
             <xsl:text>&#xA;</xsl:text>
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
    <xsl:template match="trial/procedure/parameter" mode="mean">
        <!--xsl:value-of select="."/>
        <xsl:text>&#x9; </xsl:text-->
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

    <xsl:template name="meantrials">
        <xsl:apply-templates mode="mean" select="trial/procedure/parameter"/>
        
        <xsl:variable name="number" select="number(document($experiment_file)/apex:apex/procedure/parameters/rev_for_mean)"/>
        <xsl:variable name="revset" select="trial/procedure/parameter"/>
        <xsl:variable name="total" select="count($revset)"/>
        <xsl:variable name="offset" select="$total - $number"/>
        
        <xsl:text>Mean of last </xsl:text>
        <xsl:value-of select="$number"/> 
        <xsl:text> trials = </xsl:text>
        <xsl:value-of select="sum($revset[position()>$offset]/.) div $number"/>
    </xsl:template>
    
    
    <!-- get all stimuli and show associated fixedparameter label -->
    <xsl:template name="getlabels">
        <xsl:apply-templates select="trial/procedure/stimulus"/>
    </xsl:template>
    
    
</xsl:stylesheet>
