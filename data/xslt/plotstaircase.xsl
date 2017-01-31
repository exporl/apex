<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:apex="http://med.kuleuven.be/exporl/apex/result" xmlns:svg="http://www.w3.org/2000/svg"
    xmlns:xlink="http://www.w3.org/1999/xlink">

    <xsl:template name="svgstaircase">
        <xsl:param name="values"/>
        <xsl:variable name="width">500</xsl:variable>
        <xsl:variable name="height">300</xsl:variable>

        <xsl:variable name="axesoffset">30</xsl:variable>
        <!-- distance between axis and border -->
        <xsl:variable name="textoffset">5</xsl:variable>
        <!-- distance axis label and border -->
        <xsl:variable name="ticklabeloffset">15</xsl:variable>
        <!-- distance between tick label and axis -->

        <xsl:variable name="numticks">10</xsl:variable>

        <xsl:variable name="staircase" select="trial/procedure/parameter"/>
        <!-- cache -->
        <xsl:variable name="xmax" select="count($staircase)"/>
        <xsl:variable name="ymaxt"
            select="trial/procedure[not(../../trial/procedure/parameter &gt; parameter)]/parameter"/>
        <xsl:variable name="ymax" select="string($ymaxt[1])"/>
        <xsl:variable name="ymin"
            select="string((trial/procedure[not(../../trial/procedure/parameter &lt; parameter)]/parameter)[1])"/>
        <xsl:variable name="xtickinterval" select="floor($xmax div $numticks)"/>


        <svg xmlns="http://www.w3.org/2000/svg" width="{$width}" height="{$height}">

            <svg:g>
                <svg:defs>
                    <circle id="marker" r="5" fill="red"/>
                    <line id="xtick" x1="0" y1="0" x2="0" y2="5" stroke="black" stroke-width="2"/>
                    <line id="ytick" x1="0" y1="0" x2="5" y2="0" stroke="black" stroke-width="2"/>
                </svg:defs>


                <svg:g>
                    <!-- box -->
                    <svg:rect width="{$width}" height="{$height}" x="0" y="0" fill="aliceblue"/>
                    
                    <!-- Axes -->

                    <g font-size="10" text-anchor="middle">
                        <!-- X axis -->
                        <line x1="{$axesoffset}" y1="{$height - $axesoffset}" x2="{$width}"
                            y2="{$height - $axesoffset}" stroke="black" stroke-width="2"/>
                        <text x="{$axesoffset + (($width - $axesoffset) div 2)}"
                            y="{$height - $textoffset}">Trial number</text>
                        <!-- Y axis -->
                        <line x1="{$axesoffset}" y1="{$height - $axesoffset}" x2="{$axesoffset}"
                            y2="0" stroke="black" stroke-width="2"/>

                        <xsl:variable name="ylabelX" select="$textoffset + 2"/>
                        <xsl:variable name="ylabelY" select="($height - $axesoffset) div 2"/>
                        <g transform="rotate(-90,{$ylabelX},{$ylabelY})">
                            <text x="{$ylabelX}" y="{$ylabelY}">Parameter value</text>
                        </g>
                    </g>

                    <!--text x="100" y="100">
                    <xsl:value-of select="$axesoffset + (($width - $axesoffset) div 2)"/>
                    </text-->
                </svg:g>

                <g font-size="10" text-anchor="middle">
                    <!-- datapoints and x ticks-->
                    <xsl:apply-templates select="trial/procedure/parameter" mode="plotsvg">
                        <xsl:with-param name="xtickinterval" select="$xtickinterval"/>
                        <xsl:with-param name="axesoffset" select="$axesoffset"/>
                        <xsl:with-param name="textoffset" select="$textoffset"/>
                        <xsl:with-param name="ticklabeloffset" select="$ticklabeloffset"/>
                        <xsl:with-param name="width" select="$width"/>
                        <xsl:with-param name="height" select="$height"/>
                        <xsl:with-param name="xmax" select="$xmax"/>
                        <xsl:with-param name="ymax" select="$ymax"/>
                        <xsl:with-param name="ymin" select="$ymin"/>
                    </xsl:apply-templates>
                </g>
            </svg:g>
        </svg>

    </xsl:template>

    <xsl:template match="trial/procedure/parameter" mode="plotsvg">
        <xsl:param name="xtickinterval"/>
        <xsl:param name="axesoffset"/>
        <xsl:param name="textoffset"/>
        <xsl:param name="ticklabeloffset"/>
        <xsl:param name="width"/>
        <xsl:param name="height"/>
        <xsl:param name="xmax"/>
        <xsl:param name="ymax"/>
        <xsl:param name="ymin"/>
        <xsl:variable name="trialnr" select="position()"/>

        <xsl:variable name="xpos"
            select="$axesoffset + ((($width - $axesoffset) div ($xmax + 1)) * $trialnr)"/>
        <xsl:variable name="extraspace" select="($ymax - $ymin) div 10"/>
        <xsl:variable name="ypos"
            select="$height - $axesoffset - (($height - $axesoffset) div ($ymax - $ymin + $extraspace * 2) * (. - $ymin+$extraspace))"/>

        <!-- ticks -->
        <xsl:if test="not($trialnr mod $xtickinterval)">
            <svg:use xlink:href="#xtick" x="{$xpos}" y="{$height - $axesoffset}"/>
            <svg:text x="{$xpos}" y="{$height - $axesoffset + $ticklabeloffset}">
                <xsl:value-of select="$trialnr"/>
            </svg:text>
        </xsl:if>

        <svg:use xlink:href="#ytick" x="{$axesoffset}" y="{$ypos}"/>
        <svg:text x="{$axesoffset - $ticklabeloffset}" y="{$ypos}">
            <xsl:value-of select="."/>
        </svg:text>

        <!-- marker -->
        <svg:use xlink:href="#marker" x="{$xpos}" y="{$ypos}"/>

        <!-- lines between data points -->
        <xsl:if test="$trialnr &gt; 1">
            <xsl:variable name="prevxpos"
                select="$axesoffset + ((($width - $axesoffset) div ($xmax + 1)) * ($trialnr - 1))"/>
            <xsl:variable name="prevypos"
                select="$height - $axesoffset - (($height - $axesoffset) div ($ymax - $ymin + $extraspace * 2) * (string(../../preceding-sibling::trial[1]/procedure/parameter) - $ymin+$extraspace))"/>

            <svg:line x1="{$prevxpos}" y1="{$prevypos}" x2="{$xpos}" y2="{$ypos}" stroke="black"
                stroke-width="2"/>
        </xsl:if>
    </xsl:template>

</xsl:stylesheet>
