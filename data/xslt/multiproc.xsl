<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"  
    xmlns:apex="http://med.kuleuven.be/exporl/apex/result">

    <xsl:output method='text'/>
    
    <xsl:variable name="experiment_file" select="apex:results/@experiment_file"/>
    
    <xsl:template match="/">

        <!-- select all procedure id's from experiment file -->
        <xsl:variable name="procedures" select="document($experiment_file)/apex:apex/procedure/procedure/@id"/>
        <!--xsl:apply-templates select="document($experiment_file)/apex:apex/procedure/procedure/@id"></xsl:apply-templates-->
        <!--xsl:value-of select="$procedures[2]"/-->
        
        <!--xsl:for-each select="document($experiment_file)/apex:apex/procedure/procedure/@id">
            <xsl:variable name="procid" select="."/>
            <xsl:text>Results from procedure </xsl:text> <xsl:value-of select="$procid"/> <xsl:text>&#10;</xsl:text>
            <xsl:apply-templates select="apex:results/trials/procedure[@id='procedure1']"/>
            </xsl:for-each-->
        
        
   
        <xsl:apply-templates select="/apex:results">
            <xsl:with-param name="curproc">procedure1</xsl:with-param>
        </xsl:apply-templates>
        <xsl:apply-templates select="/apex:results">
            <xsl:with-param name="curproc">procedure2</xsl:with-param>
        </xsl:apply-templates>
        <xsl:apply-templates select="/apex:results">
            <xsl:with-param name="curproc">procedure3</xsl:with-param>
        </xsl:apply-templates>
        
        <!--xsl:apply-templates select="apex:results"/-->
    </xsl:template>
    

    <xsl:template match="/apex:results">
        <xsl:param name="curproc"></xsl:param>
        <xsl:text>&#10;&#10;</xsl:text>
        <xsl:text>{Results from procedure </xsl:text> <xsl:value-of select="$curproc"/> 
        <xsl:text>}</xsl:text>
        
        <xsl:text>&#10;&#10;[Correct answers]&#10;</xsl:text>
        <xsl:call-template name="getcorrectanswers">
            <xsl:with-param name="curproc" select="$curproc"/>
        </xsl:call-template>
        
        
        
        <xsl:text>&#10;&#10;[Reversals]&#10;</xsl:text>
        <xsl:call-template name="getreversals">
            <xsl:with-param name="curproc" select="$curproc"/>
        </xsl:call-template>
        <xsl:text>&#10;</xsl:text>
        <!--xsl:call-template name="meanreversals">
            <xsl:with-param name="curproc" select="$curproc"/>
        </xsl:call-template-->
                
        <xsl:text>&#10;&#10;[Sequence data]&#10;</xsl:text>
        <xsl:call-template name="getsequence">
            <xsl:with-param name="curproc" select="$curproc"/>
        </xsl:call-template>

    </xsl:template>        
    
    <xsl:template name="getresponsescorrect">
        <xsl:param name="curproc"/>
        <xsl:apply-templates select="trial[procedure/@id=$curproc]/corrector/result"/>
    </xsl:template>
    <xsl:template match="trial/corrector/result">
        <xsl:value-of select="."/>
        <xsl:text>&#x9; </xsl:text>
    </xsl:template>

    
    <xsl:template name="getanswers">
        <xsl:param name="curproc"/>
        <xsl:apply-templates select="trial[procedure/@id=$curproc]/screenresult/element[@id='text']"/>
    </xsl:template>
    <xsl:template match="trial/screenresult/element[@id='text']">
        <xsl:value-of select="../../@id"/>
        <xsl:text>&#x9;</xsl:text>
        <xsl:value-of select="."/>
        <xsl:text>&#xA;</xsl:text>
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
            
            <xsl:value-of select="document($experiment_file)//apex:apex/stimuli/stimulus[@id=string($trialname)]/fixedParameters/parameter[@id='sentence']"/>
             <xsl:text>&#xA;</xsl:text>
    </xsl:template>
    
    
    <xsl:template name="getreversals">
        <xsl:param name="curproc"/>
        <xsl:apply-templates select="trial[procedure/@id=$curproc]/procedure/reversal"/>
    </xsl:template>
    <xsl:template match="trial/procedure/reversal">
        <xsl:value-of select="../parameter"/>
        <xsl:text>&#x9; </xsl:text>
    </xsl:template>
    
    <xsl:template name="getsequence">
        <xsl:param name="curproc"/>
        <xsl:apply-templates select="trial[procedure/@id=$curproc]/procedure/parameter"/>
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

    
    <xsl:template name="meantrials">
        <xsl:apply-templates mode="mean" select="trial/procedure/parameter"/>
        
        <xsl:variable name="number" select="number(document($experiment_file)/apex:apex/procedure/parameters/rev_for_mean)"/>
        <xsl:variable name="revset" select="trial/procedure/parameter"/>
        <xsl:variable name="total" select="count($revset)"/>
        <xsl:variable name="offset" select="$total - $number"/>
        
        <xsl:text>Mean of last </xsl:text>
        <xsl:value-of select="$number"/> 
        <xsl:text> reversals = </xsl:text>
        <xsl:value-of select="sum($revset[position()>$offset]/.) div $number"/>
    </xsl:template>
    
    
    <!-- get all stimuli and show associated fixedparameter label -->
    <xsl:template name="getlabels">
        <xsl:apply-templates select="trial/procedure/stimulus"/>
    </xsl:template>

    
    
</xsl:stylesheet>
