<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:apex="http://med.kuleuven.be/exporl/apex/result" xmlns:svg="http://www.w3.org/2000/svg"
    xmlns:xlink="http://www.w3.org/1999/xlink">



    <xsl:template name="confusionmatrix">
        <!-- Select unique stimuli -->
        <xsl:variable name="uniquestimuli" select="//stimulus[not(.=preceding::stimulus)]"/>

        <!--xsl:for-each select="$uniquestimuli">
        <xsl:value-of select="."/>
        </xsl:for-each-->

        <!-- Select unique answers -->
        <xsl:variable name="uniqueanswers" select="//answer[not(.=preceding::answer)]"/>

        <!-- Generate confusion matrix -->
        <table border="1">
            <tr>
                <td/>
                <xsl:for-each select="$uniquestimuli">
                    <xsl:sort select="."/>
                    <td>
                        <b>
                            <xsl:value-of select="."/>
                        </b>
                    </td>
                </xsl:for-each>
            </tr>
            <xsl:for-each select="$uniqueanswers">
                <xsl:sort select="."/>
                <tr>
                    <td>
                        <b>
                            <xsl:value-of select="."/>
                        </b>
                    </td>
                    <xsl:variable name="currentanswer" select="."/>
                    <xsl:for-each select="$uniquestimuli">
                        <xsl:sort select="."/>
                        <xsl:variable name="currentstimulus" select="."/>
                        <td>
                            <xsl:value-of
                                select="count(/apex:results/trial[procedure/stimulus = $currentstimulus and corrector/answer = $currentanswer])"
                            />
                        </td>
                    </xsl:for-each>
                </tr>
            </xsl:for-each>
        </table>

    </xsl:template>

</xsl:stylesheet>
