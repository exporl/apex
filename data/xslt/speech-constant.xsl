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
        <xsl:text>Results of speech test from file</xsl:text> <xsl:value-of select="$experiment_file"/> 
        <xsl:text> for subject </xsl:text>
        <xsl:value-of select="general[1]/interactive[1]/entry[2]/@new_value"/>
        <xsl:text>&#10;&#10;</xsl:text>
        <xsl:text>Presentation level: </xsl:text><xsl:value-of select="general[1]/interactive[1]/entry[1]/@new_value"/><xsl:text> dB (relative to calibration)</xsl:text>
        <xsl:text>&#10;Test duration: </xsl:text><xsl:value-of select="general[1]/duration"/><xsl:text> s</xsl:text>
        <xsl:text>&#10;&#10;[Stimulus/answerstimulus pairs]&#10;</xsl:text>
         <!--xsl:apply-templates select="trial"/-->
        <xsl:call-template name="getstimuli"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:call-template name="getresponses"/>
        
        <xsl:text>&#10;&#10;[Stimulus/correct answer/response/correctorvalue]&#10;</xsl:text>
        <xsl:call-template name="getstimuli"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:call-template name="getcorrectanswers"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:call-template name="getresponses"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:call-template name="getcorrectorresult"/>
        
        <xsl:text>&#10;&#10;[Score]&#10;</xsl:text>
        <xsl:call-template name="getscore"/>
    </xsl:template>        
    
    <xsl:template name="getstimuli">
        <xsl:apply-templates select="trial/procedure/stimulus"/>
     </xsl:template>
    
    <xsl:template name="getresponses">
        <xsl:apply-templates select="trial/corrector/currentanswer"/>
    </xsl:template>
    
    <xsl:template match="trial/corrector/currentanswer">
        <xsl:value-of select="."/>
        <xsl:text>&#x9;</xsl:text>
    </xsl:template>
    
    <xsl:template name="getanswers">
        <xsl:apply-templates select="trial/screenresult/element[@id=$answer_element]" mode="answers"/>
    </xsl:template>
    
    <xsl:template name="getcorrectanswers">
        <xsl:apply-templates select="trial/@id" mode="getcorrectanswers"/>
    </xsl:template>
    
    <xsl:template match="trial/@id" mode="getcorrectanswers">
        <xsl:variable name="trial" select="."></xsl:variable>
        <xsl:value-of select="document($experiment_file)/apex:apex/procedure/trials/trial[@id=string($trial)]/answer"/>
        <xsl:text>&#x9;</xsl:text>
    </xsl:template>
    
    <xsl:template match="trial/screenresult/element" mode="answers">
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
    
    
    <xsl:template name="getstimulus" match="trial/procedure/stimulus">
        <xsl:value-of select="."/>
        <xsl:text>&#x9;</xsl:text>
    </xsl:template>
    
    <xsl:template name="getresponse" match="trial/screenresult/element">
        <xsl:variable name="response" select="."></xsl:variable>
        <!-- get stimulus corresponding to response-->
        <xsl:value-of select="document($experiment_file)/apex:apex/procedure/trials/trial[answer=string($response)]/stimulus/@id"/>
        <xsl:text>&#x9;</xsl:text>
    </xsl:template>
    
    <xsl:template name="getscore">
        <xsl:variable name="total" select="trial/corrector/result"/>
        <xsl:variable name="ctotal" select="count($total)"/>
        
        <xsl:variable name="correct" select="trial/corrector[result='true']/result"/>
        <xsl:variable name="ccorrect" select="count($correct)"/>
        
        <xsl:value-of select="$ccorrect"/>
        <xsl:text>/</xsl:text>
        <xsl:value-of select="$ctotal"/>
        <xsl:text> = </xsl:text>
        <xsl:value-of select="100* $ccorrect div $ctotal"/>
        <xsl:text>%</xsl:text>
        
    </xsl:template>
    
</xsl:stylesheet>
