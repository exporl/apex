<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:apex="http://med.kuleuven.be/exporl/apex/result"
    xmlns:apexp="http://med.kuleuven.be/exporl/apex/3.0.2/experiment"
    xmlns:svg="http://www.w3.org/2000/svg"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    >
    
    <xsl:output indent="yes"/>
    <xsl:include href="plotstaircase.xsl"/>

    <!-- Output target, can be
    parser: output to be parsed by another program
    html
    text: human readable text
    -->
    <xsl:param name="target">html</xsl:param>
    <xsl:output method="xml"  doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"  doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN" indent="yes"/>

    <xsl:variable name="experiment_file" select="apex:results/@experiment_file"/>
    <xsl:variable name="duration">
        <xsl:value-of select="apex:results/general/duration"/>
        <xsl:text>&#32;</xsl:text>
        <xsl:value-of select="apex:results/general/duration/@unit"/>
    </xsl:variable>

    <!--xsl:output method="text"/-->
    
    <!-- Get some variables -->
    <xsl:variable name="revformean">
        <xsl:value-of select="apex:results/parameters/parameter[@name= 'reversals for mean']"/>
    </xsl:variable>
    <xsl:variable name="showextra">
        <xsl:value-of select="apex:results/parameters/parameter[@name= 'showextra']"/>
    </xsl:variable>
    
    <!-- Determine procedure type -->
    <xsl:variable name="proceduretype">
        <xsl:choose>
            <xsl:when test="apex:results/trial[1]/procedure/@type = 'apex:adaptiveProcedure' or apex:results/trial[1]/procedure/@type = 'adaptiveProcedure'">
                <xsl:text>adaptive</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>constant</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
    
    <!-- Show procedure ID? -->
    <xsl:variable name="showProcedureID">
        <xsl:choose>
            <xsl:when test="apex:results/trial[1]/procedure/@id">
                <xsl:text>1</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>0</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
    
    <!-- Show click position? -->
    <xsl:variable name="showClickPosition">
        <xsl:choose>
            <xsl:when test="apex:results/trial[1]/screenresult/@xclickposition">
                <xsl:text>1</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>0</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:template match="/">
        <!-- Determine whether choices >1 -->
        <xsl:variable name="choices">
            <!-- todo -->
        </xsl:variable>

        <!-- Check input parameter and select mode -->
        <xsl:choose>
            <xsl:when test="$target='parser'">
                <xsl:apply-templates select="apex:results" mode="parser"/>
            </xsl:when>
            <xsl:when test="$target='html'">
                <xsl:apply-templates select="apex:results" mode="html"/>
            </xsl:when>
            <xsl:when test="$target='text'">
                <xsl:apply-templates select="apex:results" mode="text"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message>Error: invalid output target "<xsl:value-of select="$target"/>", choose one of parser, html, text</xsl:message>
            </xsl:otherwise>
        </xsl:choose>
        
        <!-- Go! -->
     
        
    </xsl:template>


    <xsl:template match="apex:results" mode="parser">
        <xsl:text>&#10;</xsl:text>
        <xsl:text>ExperimentFile=</xsl:text>
        <xsl:value-of select="$experiment_file"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:text>Duration=</xsl:text>
        <xsl:value-of select="$duration"/>
        <xsl:text>&#10;</xsl:text>
        
        <xsl:text>Header=</xsl:text><xsl:if test="$showProcedureID='1'">
            <xsl:text>procedure;</xsl:text>
        </xsl:if><xsl:text>trial;stimulus;correctanswer;corrector;useranswer</xsl:text><xsl:if test="$proceduretype='adaptive'">
            <xsl:text>;adaptiveparameter</xsl:text>
        </xsl:if><xsl:if test="$showClickPosition='1'">
            <xsl:text>;xclickposition;yclickposition</xsl:text>
        </xsl:if><xsl:for-each select="/apex:results/parameters/parameter[@name='showextra']"><xsl:text>;</xsl:text><xsl:value-of select="."/></xsl:for-each>
        <xsl:text>&#10;START&#10;</xsl:text>
        <xsl:apply-templates select="trial" mode="parser"/>
        <xsl:text>END&#10;</xsl:text>
    </xsl:template>
    
    <xsl:template match="apex:results" mode="html">
        <html xmlns="http://www.w3.org/1999/xhtml">
            <h1>APEX 4 results file</h1>
            <p>Jump to: <a href="#trials">Trials</a> 
             <xsl:if test="$proceduretype='adaptive'">
                - <a href="#staircase">Staircase</a> 
             </xsl:if>
            </p>
            <dl>
                <dt><xsl:text>ExperimentFile</xsl:text></dt><dd><xsl:value-of select="$experiment_file"/></dd>
                <dt><xsl:text>Duration</xsl:text></dt><dd><xsl:value-of select="$duration"/></dd>
            </dl>
            <h2>Trials</h2>
            <a name="trials"/>
            <table>
                <tr>
                    <xsl:if test="$showProcedureID='1'">
                        <th>Procedure ID</th>
                    </xsl:if>
                    <th>Trial ID</th>
                    <th>Stimulus</th>
                    <th>Correct answer</th>
                    <th>Answer</th>
                    <th>Result</th>
                    <xsl:if test="$showClickPosition='1'">
                        <th>xclickposition</th><th>yclickposition</th>
                    </xsl:if>
                    <xsl:if test="$proceduretype='adaptive'">
                        <th>Parameter</th>
                    </xsl:if>
                    <xsl:for-each select="/apex:results/parameters/parameter[@name= 'showextra']">
                        <th><xsl:value-of select="."/></th>
                    </xsl:for-each>
                </tr>
                <xsl:apply-templates select="trial" mode="html"/>
            </table>
            
            <p>
                <xsl:variable name="numcorrect" select="count(trial/procedure[correct='true'])"></xsl:variable>
                <xsl:variable name="numtotal">
                    <xsl:if test="$proceduretype='adaptive'">
                        <xsl:value-of select="count(trial)-1"/>
                    </xsl:if>
                    <xsl:if test="$proceduretype='constant'">
                        <xsl:value-of select="count(trial)"/>
                    </xsl:if>
                </xsl:variable>
                <xsl:value-of select="$numcorrect" />
                <xsl:text>/</xsl:text>
                <xsl:value-of select="$numtotal"/>
                <xsl:text> correct = </xsl:text>
                <xsl:value-of select="format-number($numcorrect div $numtotal * 100, '##.#')"/>
                <xsl:text> % correct</xsl:text>
            </p>
            
            <xsl:if test="$proceduretype='adaptive'">
                <h2>Adaptive staircase</h2>
                <a name="staircase"/>
                <p>Sequence: <xsl:call-template name="showstaircase"/> </p>
                <p>Reversals at: <xsl:call-template name="showreversals"/></p>
                <p><xsl:call-template name="meanreversals"/></p>
                <p><xsl:call-template name="meantrials"/></p>
                <xsl:call-template name="svgstaircase"/>
            </xsl:if>
            
            <xsl:if test="$proceduretype='constant'">
                <h2>Confusion matrix</h2>
                In previous versions of APEX, a confusion matrix was presented here. Alas, the implementation was very unstable. We suggest that if you want a confusionmatrix, you
                make one using JavaScript (e.g. using scripts/rtresults-confusionmatrix.html and rtconfusionmatrix.js). An example can be found in examples/results/realtimeresults-confusionmatrix.xml,
                normally only the results element should be changed to the one presented in examples/results/realtimeresults-confusionmatrix.xml.                
            </xsl:if>
            
            
            <xsl:call-template name="extraprocessing"/>        <!-- do extra processing, to be overwritten by another stylesheet -->
        </html>
    </xsl:template>

    <xsl:template match="trial" mode="parser">
        <xsl:if test="$showProcedureID='1'">
            <xsl:value-of select="procedure/@id"/>
            <xsl:text>;</xsl:text>
        </xsl:if>
        <xsl:value-of select="@id"/>
        <xsl:text>;</xsl:text>
        <xsl:value-of select="procedure/stimulus"/>
        <xsl:text>;</xsl:text>
        <xsl:value-of select="procedure/correctanswer"/>
        <xsl:text>;</xsl:text>
        <xsl:value-of select="procedure/correct"/>
        <xsl:text>;</xsl:text>
        <xsl:value-of select="procedure/answer"/>
        <xsl:if test="$showClickPosition='1'">
            <xsl:text>;</xsl:text>
            <xsl:value-of select="screenresult/@xclickposition"/>
            <xsl:text>;</xsl:text>
            <xsl:value-of select="screenresult/@yclickposition"/>
        </xsl:if>
        <xsl:if test="$proceduretype='adaptive'">
            <xsl:text>;</xsl:text>
            <xsl:value-of select="procedure/parameter"/>
        </xsl:if>
        <xsl:variable name="trialnode" select="."/>
        <xsl:for-each select="/apex:results/parameters/parameter[@name= 'showextra']">
            <xsl:variable name="ttt" select="."></xsl:variable>
            <xsl:text>;</xsl:text>
            <xsl:value-of select="$trialnode//@id[.=$ttt]/.."/>
        </xsl:for-each>
        <xsl:text>&#10;</xsl:text>
    </xsl:template>
    

    <xsl:template match="trial" mode="html">
        <tr xmlns="http://www.w3.org/1999/xhtml">
            <xsl:if test="$showProcedureID='1'">
                <td><xsl:value-of select="procedure/@id"/></td>
            </xsl:if>
            <td><xsl:value-of select="@id"/></td>
            <td><xsl:value-of select="procedure/stimulus"/></td>
            <td><xsl:value-of select="procedure/correctanswer"/></td>
            <td><xsl:value-of select="procedure/answer"/></td>
            <td><xsl:value-of select="procedure/correct"/></td>
            <xsl:if test="$proceduretype='adaptive'">
                <td><xsl:value-of select="procedure/parameter"/></td>
            </xsl:if>
            <xsl:variable name="trialnode" select="."/>
            <xsl:for-each select="/apex:results/parameters/parameter[@name= 'showextra']">
                <xsl:variable name="ttt" select="."></xsl:variable>
                <td>
                    <xsl:value-of select="$trialnode//@id[.=$ttt]/.."/>
                </td>
            </xsl:for-each>
        </tr>
    </xsl:template>
    

    
    
    <xsl:template name="showstaircase">
        <xsl:variable name="staircase" select="trial/procedure/parameter"/>
        
        <xsl:for-each select="$staircase">
            <xsl:value-of select="."/><xsl:text> </xsl:text>
        </xsl:for-each>
    </xsl:template>
    
    
    <xsl:template name="showreversals">
        <xsl:apply-templates select="trial/procedure/reversal" mode="showreversals"/>
    </xsl:template>
    <xsl:template match="trial/procedure/reversal" mode="showreversals">
        <!-- For APEX 4.0: get previous trial-->
        <!--xsl:variable name="stimname" select="../../preceding-sibling::trial[1]/procedure/parameter"/-->
        <!--xsl:value-of select="$stimname"/-->
        <xsl:value-of select="../parameter"/>
        <xsl:text>&#x9; </xsl:text>
    </xsl:template>
    
    <xsl:template name="meanreversals">
        <xsl:apply-templates mode="mean" select="trial/procedure/reversal"/>
        
        <!--xsl:choose>
            <xsl:when test="$revformean=''">
                <xsl:variable name="number" select="number(document($experiment_file)/apexp:apex/procedure/parameters/rev_for_mean)"/>     
            </xsl:when>
            <xsl:otherwise>
                <xsl:variable name="number" select="$revformean"/>
            </xsl:otherwise>
        </xsl:choose-->
        
        <xsl:variable name="number" select="$revformean"/>
        <xsl:if test="$number!=''">
        
            <xsl:variable name="revset" select="trial/procedure/reversal"/>
            <xsl:variable name="total" select="count($revset)"/>
            <xsl:variable name="offset" select="$total - $number"/>
            
            <xsl:text>Mean of last </xsl:text>
            <xsl:value-of select="$number"/> 
            <xsl:text> reversals: </xsl:text>
            <xsl:value-of select="format-number(sum($revset[position()>$offset]/../parameter) div $number, '##.#')"/>
        
        </xsl:if>
    </xsl:template>
    
    <xsl:template name="meantrials">
        <!--xsl:apply-templates mode="mean" select="trial/procedure/parameter"/-->
        
        <xsl:variable name="number" select="$revformean"/>
        <xsl:if test="$number!=''">
            
            <xsl:variable name="revset" select="trial/procedure/parameter"/>
            <xsl:variable name="total" select="count($revset)"/>
            <xsl:variable name="offset" select="$total - $number"/>
            
            <xsl:text>Mean of last </xsl:text>
            <xsl:value-of select="$number"/> 
            <xsl:text> trials: </xsl:text>
            <xsl:value-of select="format-number(sum($revset[position()>$offset]/.) div $number, '##.#')"/>
            
        </xsl:if>
    </xsl:template>
    

    <xsl:template name="extraprocessing">
        <!-- Dummy template, to be overwritten -->
    </xsl:template>
    
    
    
</xsl:stylesheet>
