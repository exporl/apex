"use strict";

/* Resultsprinter is a direct extension of resultsprocessor
 * It will not work unless that script is loaded earlier
 */
 
 /* eslint-disable no-unused-vars, no-undef */

/*  ============================================================================
 *  Plot types
 *  ==========
 *  - Line: adaptive procedure
 *  - Matrix: constant procedure
 *  - Polar: localization
 *  - Text: all textual data
 *
 *  Object specification:
 *      - data:         array of <object>s with prepared data for plot, see structure below
 *                      index corresponds to index of results.groupNames in case of multiprocedure
 *      - highChart:    <true/false> true if this type of plot draws a HC plot
 *      - chartConfig:  <object> to use the previous if true (see ...-config.js) (required)
 *      - seriesConfig: <object> to use for adding series to charts (optional, in case of multiprocedures)
 *      - prepare:      <function> for preparing plot data/....
 *      - setData:      <function> for setting data in plot/div/... or updating plot-config
 *
 *  Data objects
 *  ============
 *  plots.line.data   = [{ values: [],      - Array with parametervalues of every trial
 *                         reversals: [],   - Array with reversals
 *                         meanrevs: n,     - Mean of last 6 reversals (set revsForMean to other number to change)
 *                         meanrevstd: n,   - meanrevs std
 *                         meantrials: n    - Mean of last 6 trials (set trialsForMean to other number to change)
 *                         meantrialstd:    - meantrials std
 *                      }]
 *
 *  plots.matrix.data = [{ values: [ { x: n,       - (int) xlabel link
 *                                     y: n,       - (int) ylabel link
 *                                     z: n } ]    - (int) x & y pairings
 *                         percentages:     - % per stimulus correct (same as ..polar.data.values)
 *                         xlabels: [],     - correct answer / stimulus labels
 *                         ylabels: [],     - answer / response labels
 *                         raw: [][]        - 2D array used to parse matrix.values-objects
 *                      }]
 *
 *  plots.polar.data  = [{ values: []    - Array with % correct per correct answer/stimulus
 *                      }]
 *
 *  The following data is re-interpreted from other plotdata files,
 *  but tabulated with html tags, etc.
 *  plots.text.data   = [{ line:  { allvalues,
                                    lastvalue,
                                    reversals,
                                    meanrevs,
                                    meantrials,
                                    paramtable
                                  },
                           matrix:{ correctpercentage,
                                    correctpercentages,
                                    matrixtable,
                                    answertable,
                                  }
 *                      }]
 */



/*
 * Expand plots with set data functions
 */

 plots.line.setData = null;
 plots.matrix.setData = setDataMatrix;
 plots.polar.setData = setDataPolar;
 plots.text.setData = setDataText;


/*  ============================================================================
 *  Print functions
 *  ============================================================================
 */

function printAllValues(presults, index)
{
    if (typeof plots.line.data[index] === "undefined")
        return;

    var s = wrap("td","Parameter values") +  wrap("td", plots.line.data[index].values.join(", "));
    return wrap("tr",s);
}

function printLastValue(presults, index)
{
    if (typeof plots.line.data[index] === "undefined")
        return;

    var s = wrap("td","Last value") + wrap("td",plots.line.data[index].values.last());
    return wrap("tr",s);
}

function printReversals(presults, index)
{
    var s = wrap("td","Reversals") + wrap("td",plots.line.data[index].reversals.join(", "));
    return wrap("tr",s);
}

function printMeanRevs(presults, index)
{
    //var nrevs = plots.line.data[index].reversals.length;
    var nrevs = 6;
    if (typeof config.line.reversalsformean !== "undefined")
        nrevs = config.line.reversalsformean;

    var average = plots.line.data[index].meanrevs;
    var std = plots.line.data[index].meanrevstd;
    var digits = Math.min(4, config.global.digitsafterdecimal);

    var s1 = "Mean (std)<br>last " + nrevs + " reversals";
    var s2 = round(average, digits) + " ( &plusmn;" + round(std, digits) + " )";
    var s3 = wrap("td class=\"dataname\"",s1) + wrap("td",s2);

    return wrap("tr",s3);
}

function printMeanTrials(presults, index)
{
    //var nvalues = plots.line.data[index].values.length;
    var nvalues = 6;
    if (typeof config.line.trialsformean != "undefined")
        nvalues = config.line.trialsformean;

    var average = plots.line.data[index].meantrials;
    var std = plots.line.data[index].meantrialstd;
    var digits = Math.min(3, config.global.digitsafterdecimal);

    var s1 = "Mean (std)<br>last " + nvalues + " trials";
    var s2 = round(average, digits) + " ( &plusmn" + round(std, digits) + " )";
    var s3 = wrap("td class=\"dataname\"",s1) + wrap("td",s2);

    return wrap("tr",s3);
}

function printLineDataTable(presults, index)
{
    //var d = plots.text.data[index].line;
    var d =
    {
        allvalues:      printAllValues(presults, index),
        lastvalue:      printLastValue(presults, index),
        reversals:      printReversals(presults, index),
        meanrevs:       printMeanRevs(presults, index),
        meantrials:     printMeanTrials(presults, index)
    };

    //  Add all <tr> from textdata.line except paramtable to a table
    var t = "";
    if (typeof config.global.multiprocedure !== "undefined" && config.global.multiprocedure)
        t = tr( th( "Procedure")+ th( results.groupNames[index].capitalizeFirstLetter() , attr_class("seriesname")));
    else
        t = "";

    $.each (d,function(key,value) {
        if (key === "paramtable" ||  key === "linedatatable")
            return;
        t += value;
    });
    t = wrap("table", t, [ attr_id("linedatatable"),
                           attr_class("datatable")] );

    return t;
}

/* Prints table with adapted parameter results
 */
function printParamTable(presults, index)
{
    var resultsAfterFilter = presults.filter(function(t) {
        return t.hasValidParameter();
    });

    if (typeof lineDataFilter === "function")
        resultsAfterFilter = presults.filter(lineDataFilter);

    var t;
    if (typeof config.global.multiprocedure !== "undefined" && config.global.multiprocedure)
        t = tr( th( uline( "Procedure: "+results.groupNames[index].capitalizeFirstLetter()), attr("colspan", "4") ));
    else
        t = "";

    t += tr( th("Trial") + th("Answer") + th("Correct") + th("Parameter"+ printParameterName(true) + printParameterUnit()));

    $.each (resultsAfterFilter, function(i, result) {
        t += wrap("tr", wrap("td", i + 1)
                            + wrap("td", result.answer)
                            + printCorrectTd(result.correct)
                            + wrap("td", plots.line.data[index].values[i]) );
    });

    t = table( t, attr_class("stripedtable"));

    return t;
}

function printCorrectTd(c)
{
    if (c)
        return wrap("td", wrap("span", "Correct", attr("class","correct")) + "");
    else
        return wrap("td", wrap("span", "Incorrect", attr("class","incorrect")) + "");
}

//  table helper function
function printParameterName(parentheses)
{
    parentheses = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : false;

    if (typeof config.line.parametername !== "undefined") {
        if (parentheses)
            return " ("+config.line.parametername+")";
        else
            return config.line.parametername;
    }
    else
        return "";
}

//  table helper function
function printParameterUnit(parentheses)
{
    parentheses = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : false;

    if (typeof config.line.parameterunit !== "undefined") {
        if (parentheses)
            return " ("+config.line.parameterunit+")";
        else
            return config.line.parameterunit;
    }
    else
        return "";
}

/* Correct percentage per stimulus in table format
 */
function printCorrectPercentagePerStimulus(presults, index)
{
    var resultsAfterFilter = presults;
    if (typeof matrixDataFilter === "function" )
        resultsAfterFilter = presults.filter( matrixDataFilter );

    //  get array from polardata if exists
    var d = (typeof plots.matrix.data !== "undefined") ? plots.matrix.data[index] : null;
    if (!d)
        return "";

    var s = "", p = "";
    //if (config.global.multiprocedure)    s += tr( th( results.groupNames[index], attr( "colspan", d.xlabels.length ) ) );

    if (typeof config.global.multiprocedure !== "undefined" && config.global.multiprocedure)
    {   s = tr( th( "Procedure: "+results.groupNames[index].capitalizeFirstLetter(),
                            attr("colspan", d.xlabels.length) ),attr("height",25));
    }else {
        s = "";
    }

    s += tr( th( "Correct % per stimulus", attr( "colspan", d.xlabels.length ) ) );

    $.each ( d.xlabels, function(i,label) {
        p += th( label.capitalizeFirstLetter() );
    });
    s += tr( p );
    p = "";

    //  loop over percentages, add td
    $.each (d.xlabels, function(i,lbl) {
        if (i < d.percentages.length)
            p += printPercentCell(i);
    });
    p = tr( p );    s += p;

    return s;

    function printPercentCell(i)
    {
        if (d.percentages[i] === 0)
            return td( (d.percentages[i]*100).toFixed( 2 )+"%" , attr_class("lightcell"));
        else
            return td( (d.percentages[i]*100).toFixed( 2 )+"%" );
    }
}

/* Correct percentage of all answers
 */
function printCorrectPercentage(presults, index)
{
    var resultsAfterFilter = presults;
    if (typeof matrixDataFilter === "function")
        resultsAfterFilter = presults.filter( matrixDataFilter );

    //  get array from polardata if exists
    var d = (typeof plots.matrix.data !== "undefined") ? plots.matrix.data[index] : null;
    if (!d)
        return "";

    // calculate percentage correct
    var percentage = percentCorrect(resultsAfterFilter);

    var perStr = "Total percentage correct:  " + percentage + "%";
    var result = tr( td( perStr, attr("colspan", d.xlabels.length)));

    return result;
}

function printMatrixTable(presults, index)
{
    var cmData = plots.matrix.data[0],
        yLabels = cmData.ylabels,
        xLabels = cmData.xlabels,
        raw = cmData.raw;

    var t = "";
    if (typeof config.global.multiprocedure !== "undefined" && config.global.multiprocedure) {
        var procStr = "Procedure: "+ results.groupNames[index].capitalizeFirstLetter();
        t = tr( th( procStr, attr("colspan", xLabels.length+1) ));
    }

    for (var i = -1; i < yLabels.length; ++i) {
        for (var j = -1; j < xLabels.length; ++j) {
            if (i === -1 && j === -1)   t += th( "" );
            else if (i === -1)          t += th( xLabels[j] );
            else if (j == -1)           t += th( yLabels[i] );
            else                        t += printMatrixCell(j,i);
        }
        t = wrap("tr", t);
    }
    t = table( wrap("tbody", t), attr_class("stripedtable"));

    return t;

    function printMatrixCell(j,i)
    {
        if (raw[j][i] === 0)
            return td( raw[j][i], attr_class("lightcell"));
        else
            return td( raw[j][i] );
    }
}

/* Prints table with matrix results in list-form
 */
function printAnswerTable(presults, index)
{
    var resultsAfterFilter = presults;

    if (typeof matrixDataFilter === "function")
        resultsAfterFilter = resultsAfterFilter.filter(matrixDataFilter);

    //  header
    var isMultiProc = typeof config.global.multiprocedure !== "undefined" && config.global.multiprocedure;
    var procStr = "Procedure: " + results.groupNames[index].capitalizeFirstLetter();
    var t = isMultiProc ? tr( th( procStr, attr("colspan", "4") )) : "";
    t    += tr( th("Trial") + th("Stimulus") + th("Answer") + th("Correct") );

    //  data
    for (var i = 0; i < resultsAfterFilter.length; ++i) {
        t += wrap("tr", wrap("td", i + 1)
               + wrap("td", resultsAfterFilter[i].correct_answer)
               + wrap("td", resultsAfterFilter[i].answer)
               + printCorrectTd(resultsAfterFilter[i].correct));
    }
    t = table( t, attr("class","stripedtable"));

    return t;
}






/*  ============================================================================
 *  Data plotting (This is where the magic happens)
 *  - Main function called by Apex
 *  - Always has to be called after newAnswer()
 *  ============================================================================
 */
function plot()
{
    //  Clear textcontainer
    $("#textcontainer").empty();

    //  Prepare data & global stuff
    if (!preparePlotData())
        console.log("Prepare didn't find any data yet");

    //  Initialize chart objects
    //  (no need to re-initialize every time newAnswer is called)
    if (!graphsReady) {
        //  Initialize highchart objects
        $.each (plots.types, function(index, type) {
            if (config[type].show)
                drawGraph(type);
        });

        graphsReady = true;
    }

    // Set data
    $.each (plots.types, function(i, type) {
        if (config[type].show) {
            $.each (results.groupNames, function(index, groupname) {
                setData(type, index);
            });
        }
    });

    //  Do some afterwork
    postPlot();
}


/* Set plot data
 * - setData() is the general function, accesses the div containing the chart (if highCharts == true)
 *   and handles the dataset. Creates extra data-series in the chart in case of multiprocedure.
 *   (if highCharts == false) setData() won't be called, only specialized functions will be
 * - further specialized functions can be defined and stored in plots.<type>.setData (top of doc)
 */
 function setData(type, index)
 {
    //  index is # of procedure id in results.groupNames
    //  var index = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 0;
    if (typeof index === "undefined")
        console.log("ERROR Setdata");

    // Check if dataset is valid
    var dataSet = plots[type].data[index];
    if (typeof dataSet === "undefined")
        return;

    var target;

    //  Highchart plots
    if (plots[type].highChart) {
        if (typeof dataSet.values === "undefined")
            return;
        if (dataSet.values.length == 0)
            return;

        // Target div w chart object
        target = getChart(type);
        if (typeof target === "undefined") {
            drawGraph(type);
            target = getChart(type);
        }

        //  Check if series exists, if not add
        while(index >= target.series.length) {
            var tname = "Procedure "+(index+1);
            if (config.global.multiprocedure)
                tname = results.groupNames[index].capitalizeFirstLetter();

            //  Series object to add to highcharts
            var seriesObj = { name: tname,
                              color:  config.global.chartColors[index%config.global.chartColors.length],
                              data: 0 };

            //  Extend series obj based on plottype's seriesConfig (squash together w $.extend())
            if (typeof plots[type].seriesConfig !== "undefined")
                $.extend(seriesObj, plots[type].seriesConfig);

            //  Add series to chart, but do not redraw (will be done after setting data)
            target.addSeries( seriesObj, false );
        }

        //  Set name of first series if multiproc
        if (config.global.multiprocedure && index === 0)
            target.series[0].update({name: results.groupNames[0].capitalizeFirstLetter() }, false);

        // Load data into correct series
        target.series[index].setData(dataSet.values, false, { duration: 800, easing: 'easeOutSine' });

        // Call chart-specific setData-functions
        if (typeof plots[type].setData === "function")
            plots[type].setData(target, dataSet, index);

        //  Draw graph
        target.redraw();
    }
    else {   //  text and stuff
        target = $("#"+type+"container");
        if (!plots[type].setData(target, dataSet, index))
            return;
    }
 }

/*  Matrix specific setData function
 *  updates x and y axes with the correct amount of labels
 *  Prints out matrix data in table
 */
function setDataMatrix(target, data, index)
{
    //  x & y axis labels update
    target.xAxis[0].update({
        categories: data.xlabels,
        ceiling: data.xlabels.length - 1
    });
    target.yAxis[0].update({
        categories: data.ylabels,
        max: data.ylabels.length - 1,
        ceiling: data.ylabels.length - 1
    });

    return true;
}

/*  Polar specific setData function
 *  updates angles, intervals, size and center-point
 */
function setDataPolar(target, data, index)
{
    if (typeof data === "undefined")
        return;
    //  Find angles and intervals for correct chart drawing
    var minAngle, maxAngle, polarIntervals, polarSize, polarCenterY;

    minAngle = typeof config.polar.mindegree !== "undefined" ? config.polar.mindegree : -90;
    maxAngle = typeof config.polar.maxdegree !== "undefined" ? config.polar.maxdegree : 90;

    polarIntervals = maxAngle * 2 / (data.values.length - 1);

    if (maxAngle * 2 > 160) {
        polarSize = '120%';
        polarCenterY = '70%';
    } else {
        polarSize = '160%';
        polarCenterY = '100%';
    }

    target.panes.startAngle = minAngle;
    target.panes.endAngle = maxAngle;
    target.panes.size = polarSize;
    target.center = ['50%',polarCenterY];

    target.xAxis[0].update({
        min: minAngle,
        max: maxAngle,
        tickInterval: polarIntervals
    });

    target.series[index].update({
        data: data.values,
        pointStart: minAngle,
        pointInterval: polarIntervals
    });

    return true;
}

function setDataText(target, data, index)
{
    if (!config.text.show)
        return false;

    //  Line Text
    if (config.line.show) {
        if (typeof data.line === "undefined")
            return;

        $("#linedatatable"+index).html(data.line.linedatatable);
        $("#procedureparametertable"+index).html(data.line.paramtable);
    }

    //  Matrix Text
    if (config.matrix.show || config.polar.show) {
        if (typeof data.matrix === "undefined")
            return;

        var t = table(data.matrix.correctpercentages + data.matrix.correctpercentage, attr_class("stripedtable"));
        $("#percentagescorrect"+index).html( t );
        t = data.matrix.matrixtable;
        $("#confusionmatrixtable"+index).html( t );
        t = data.matrix.answertable;
        $("#answertable"+index).html( t );
    }

    return true;
}






/*  ============================================================================
 *  Chart functions
 *  ============================================================================
 */
 function getChart(type)
{
    if (plots[type].highChart) {
        return $("#"+type+"container").highcharts();
    }
    else {
        return null;
    }
}

 function drawGraph(type)
 {
     // Parses the chartConfig objects &
     // draws graph in the container divs
     if (plots[type].highChart) {
         $("#"+type+"container").highcharts(_defineProperty( plots[type].chartConfig ));
     }
 }

// Tooltip formatters
function printLineTooltip(index,x,y)
{
    var s = "Adapted parameter";
    if (typeof config.line.parametername === "string") {
        s = config.line.parametername;
    }
    s = wrap("b", s);
    var pUn = "";
    if (typeof config.line.parameterunit === "string") {
        pUn = config.line.parameterunit;
    }
    s = wrap("div",index,attr_class("seriesname"))
            + wrap("div", "<br>Trial " + bold(x) + " :<br> "+printParameterName() +"value " + bold(y) + " "
            + printParameterUnit() );
    return s;
}

function printMatrixTooltip(index, x, y, z) {
    var cmData = plots.matrix.data[index];
    var s = "<b>Stimulus: " + cmData.xlabels[x] + "</b><br>  <b>Response: "
            + cmData.ylabels[y] + "</b><br>  <b>Pairings: " + cmData.raw[x][y] + "</b>";
    return s;
}

//  Remove series above index 0
function removeSeries()
{
    $.each(plots.types, function(i,type) {
        if (plots[type].highChart) {
            var c = getChart(type);
            if (!c)
                return;

            while( c.series.length > 1) {
                c.series[c.series.length - 1].remove();
            }
        }
    });
}

//  Turns off legends
function legendsOff()
{
    $.each(plots.types, function(i,type) {
        if (plots[type].highChart) {
            var c = getChart(type);
            if (c) c.legend.update({ enabled: false},false);
            else plots[type].chartConfig.legend.enabled = false;
        }
    });
}






/* =====================================
 *  HTML config tools
 * =====================================

 /* Prepares config.<type>.show based on results heuristics
  */
 function prepareConfigShow()
 {
    // Adaptive - Line graph
    if (results.all.some(function(t)
            { return t.isAdaptiveProcedure(); })) config.line.show = true;
    else if (results.all.some(function(t)
            { return t.hasValidParameter(); })) config.line.show = true;
    else config.line.show = false;

    // Constant - Confusion matrix (bubbleplot)
    if (results.all.some(function(t)
            { return t.isConstantProcedure(); })) config.matrix.show = true;
    else config.matrix.show = false;

    // Localization - Polar plot
    config.polar.show = false;

    //  Text
    config.text.show = true;
 }


 /* Smart prepare multiprocedure config
  * Checks for multiple procedures in the results, by proceduretype or procedureid
  * Sets default values for various config keys
  */
  var forceMultiProcOff = false;
 function prepareMultiProcedureConfig()
 {
     // Don't do this if they turned the checkbox off before
     if (forceMultiProcOff)
         return;

    //  Check for multiple procedures
    //  Either sort by proceduretype (has priority) or procedureid
    var proctypes = results.all.map(function(t) { return t.proceduretype; }).unique();
    var procids   = results.all.map(function(t) { return t.procedureid; }).unique();

    if (typeof config.global.multiprocedure === "undefined") {
        config.global.multiprocedure = prepareGroupProceduresBy(proctypes,procids);
    }
    else {
        if (typeof config.global.groupproceduresby === "undefined") {
            prepareGroupProceduresBy(proctypes,procids);
        }
    }

    //  Turn on legend in every type of highchart
    if (config.global.multiprocedure) {
        $.each(plots.types, function(index,type) {
            if (plots[type].highChart)
                plots[type].chartConfig.legend.enabled = true;
        });
    }

    //  Decide config.global.groupproceduresby key
    function prepareGroupProceduresBy(tproctypes,tprocids)
    {
        if (proctypes.length > 1) {
            config.global.groupproceduresby = "proceduretype";
            return true;
        }
        else if (procids.length > 1) {
            config.global.groupproceduresby = "procedureid";
            return true;
        }
    }

    //  Group results by key (config.global.groupproceduresby) if multiprocedure is enabled
    var key = "proceduretype";
    if (config.global.multiprocedure) {
        if (typeof config.global.groupproceduresby !== "undefined")
            key = config.global.groupproceduresby;
        results.group = groupBy(results.all,key);
    }
    else {
        results.group["default"] = results.all;
    }

    //  loop over groupnames, see if they're already part of results.groupNames, if not then push
    $.each(Object.keys(results.group), function(index, key) {
        if (!results.groupNames.contains(key))
            results.groupNames.push(key);
    });
 }

function postPlot()
{
    //  Text re-style (must be at end)
    customStyleText();

    //  Set multiproc checkbox state
    updateMultiprocCheckbox();

    //  No data placeholder
    if (typeof($("#linegraphcontainer").highcharts()) === "undefined" && typeof($("#confusionmatrixcontainer").highcharts()) === "undefined" && typeof($("#polarplotcontainer").highcharts()) === "undefined") {
        $("#linegraphcontainer").html("<center><p>No data yet</p></center>");
    }
}

function customStyleText()
{
    if (typeof config.text.textstyle !== "undefined")
       $('body').css(config.text.textstyle);

    if (typeof config.text.tablestyle !== "undefined")
       $('#textcontainer table tr td').css(config.text.tablestyle);
}

/*  ============================================================================
 *  Interface
 *  ============================================================================
 */

 function interfaceButtons()
 {
     //  Custom buttons
    $.each(customButtons, function(index, button) {
        //  Draw html
        //$("#custombuttonscontainer").append( "<button id=\""+button.name+"\" name=\""+button.name+"\">"+button.label+"</button>" );
        $("#custombuttonscontainer").append( wrap("button id=\""+button.name+"\" name=\""+button.name+"\"",button.label) );

        //  Behavior on click
        $("#"+button.name).click(function ()
        {
            button.behavior();
            if (button.replot)
                plot();
        });
    });

    //  Enable/disable buttons
    $.each(plots.types, function(index, type) {
        //  Draw html
        var bt = "<input type=\"checkbox\" class=\"enablebutton\" id=\"enable"+type+"\">"
        bt    += wrap("label", type.toString().capitalizeFirstLetter(), attr("for","enable"+type));
        bt     = wrap("div", bt, attr_class("spacer"));
        $("#chartbuttonscontainer").append( bt );

        //  Behavior on click
        $("#enable"+type).change(function ()
        {
            config[type].show = this.checked;

            if (this.checked) {
                plot();
                $("#"+type+"container").css("display","block");
            }
            else {
                $("#"+type+"container").css("display","none");
                plot();
            }
        });
    });

    drawMultiprocCheckbox();
    multiprocCheckbox();

    //  Set checkboxes to correct state (checks config object)
    updateCheckboxStates();
 }

 function updateCheckboxStates()
 {
    $.each(plots.types, function( index, type) {
        if (typeof config[type].show !== "undefined") {
            $("#enable"+type).prop("checked", config[type].show);
            if (config[type].show)
                $("#"+type+"container").css("display","block");
        }
    });
 }


/* Checks whether config.<type>.show is undefined for all plottypes
 */
function allUndefined()
{
    var b = true;
    $.each(plots.types, function(index, type) {
        b = (typeof config[type].show === "undefined" && b);
        if (!b)
            return false;
    });
    return b;
}

/* Checks whether config.<type>.show is false for all plottypes
 */
function allOff()
{
    var b = true;
    $.each(plots.types, function(index, type) {
        b = (!config[type].show && b);
        if (!b)
            return false;
    });
    return b;
}

/* Only text enabled?
 */
function onlyText()
{
    return !config.line.show && !config.matrix.show && !config.polar.show && config.text.show;
}

var mpcReady = false;
function drawMultiprocCheckbox()
{
    if (!mpcReady && config.global.multiprocedure) {
    var bt = "<input type=\"checkbox\" class=\"enablebutton\" id=\"cbenablemultiproc\" checked=\"checked\" />"
        bt    += wrap("label", "Multiprocedure", attr("for","cbenablemultiproc"));
        bt     = wrap("div", bt, attr_id("enablemultiproc"));
        $("#chartbuttonscontainer").append( bt );
        mpcReady = true;
    }
}

function multiprocCheckbox()
{
    $("#cbenablemultiproc").change(function ()
    {
        config.global.multiprocedure = this.checked;
        if (!this.checked) {
            $("#enablemultiproc").css("display","none");
            forceMultiProcOff = true;
            resetGroups();
            removeSeries();
            legendsOff();
            plot();
        }
        else {
            plot();
        }
    });

}

function updateMultiprocCheckbox()
{

    if (typeof config.global.multiprocedure !== "undefined") {
        if (config.global.multiprocedure) {
            $("#cbenablemultiproc").prop("checked",true);
            $("#enablemultiproc").css("display","block");
        }
        else {
            $("#enablemultiproc").css("display","none");
        }
    }
}

//============================================================================
//  Javascript internals (do not touch)
//============================================================================

function _defineProperty(obj, key, value)
{
    if (key in obj) {
        Object.defineProperty(obj, key,
        { value: value, enumerable: true, configurable: true, writable: true });
    }
    else {
        obj[key] = value;
    }
    return obj;
}









/*  ============================================================================
 *  ADVANCED CUSTOMIZATION
 *  Highcharts configuration objects
 *  Documentation can be found on http://www.highcharts.com/docs
 *                            and http://api.highcharts.com/highcharts
 *  -
 *  Line:   http://www.highcharts.com/docs/chart-and-series-types/line-chart
 *  Matrix: http://www.highcharts.com/docs/chart-and-series-types/scatter-chart
            http://www.highcharts.com/demo/bubble
 *  Polar:  http://www.highcharts.com/docs/chart-and-series-types/polar-chart
 *  ============================================================================
 */



 var customButtons = [];

 // Use this block as template
 /*
 customButtons.push({ name: "",
                      label: "",
                      replot: <true/false>,
                      behavior: function()
                               {
                                    //code
                               }
                     });
 */

 // Examples (for line graph)
 /*
 customButtons.push({ name: "btn1",
                     label: "Add series",
                     replot: true,
                     behavior: function()
                               {
                                    var target = getChart("line");
                                    if (typeof target === "null")   alert("Enable line first");

                                    target.addSeries( { data: [1,2,3,4,5,6] } );
                                    target.redraw();
                               }
                    });
*/
/*
 customButtons.push({ name: "btn2",
                     label: "Toggle columns",
                     replot: true,
                     behavior: function()
                               {
                                    var target = getChart("line");
                                    if (typeof target === "null")   alert("Enable line first");

                                    target.series[0].update( { type: "column" }, false );
                               }
                    });
*/
/*
 customButtons.push({ name: "btn3",
                     label: "Add axis",
                     replot: true,
                     behavior: function()
                               {
                                    var target = getChart("line");
                                    if (typeof target === "null")   alert("Enable line first");

                                    target.addAxis({ id: 'ReactionAxis', title: { text: 'Reaction' }, labels: { format: '{value} ms' }, opposite: true });
                                    target.series[1].update({yAxis: 1});
                               }
                    });
*/



/*  Line chart config
 */
 plots.line.chartConfig =
 {
    title: {
        text: 'Adaptive procedure',
        style: {"fontSize" : "1.6em", "fontFamily" : "Open Sans", "color" : "#666666", "fontWeight" : "bold"}
    },
    subtitle: {
        text: 'Experiment Results'
    },
    xAxis: {
        allowDecimals: false,
        floor: 1,
        tickInterval: 1,
        title: {
            text: 'Trial n°'
        }
    },
    yAxis: {
        allowDecimals: true,
        tickInterval: 1,
        title: {
            text: 'Parameter values'
        },
        labels: {
            format: '{value}'
        },
        plotLines: [{
            value: 0,
            width: 1,
            color: '#808080'
        }]
    },
    tooltip: {
        crosshairs: [true, true],
        formatter: function(){
            return printLineTooltip(this.series.name,this.x,this.y);
        }
    },
    legend: {
        backgroundColor: '#eee',
        enabled: false
    },
    series: [],
    plotOptions: {
        series: {
            pointStart: 1,
            allowPointSelect: false,
            color: 'rgba(41, 180, 180, 0.80)'
        }
    },
    lang: {
        noData: "no data"
    },
    noData: {
        style: {
            fontWeight: 'bold',
            fontSize: '13px',
            color: '#303030'
        }
    },
    credits: {
        enabled: false
    }
};

/*  Matrix chart config
 */
  plots.matrix.chartConfig =
 {
    title: {
        text: 'Constant Procedure',
        style: {"fontSize" : "1.6em", "fontFamily" : "Open Sans", "color" : "#666666", "fontWeight" : "bold"}
    },
    subtitle: {
        text: 'Experiment Results'
    },
    chart: {
        type: 'bubble'
    },
    xAxis: {
        allowDecimals: false,
        gridLineWidth: 1,
        labels: {
            format: '{value}'
        },
        tickInterval: 1,
        title: {
            text: 'Stimulus'
        },
        categories: [] //   x-labels / stimuli / stimuli

    },
    yAxis: {
        reversed: true,
        allowDecimals: false,
        tickInterval: 1,
        floor: 0,
        ceiling: 1,
        title: {
            text: 'Response'
        },
        labels: {
            format: '{value}'
        },
        categories: [] //   y-labels / answers / responses
    },
    tooltip: {
        crosshairs: [true, true],
        formatter: function() {
            return printMatrixTooltip(this.series.index, this.point.x,this.point.y,this.point.z);
        }
    },
    legend: {
        backgroundColor: '#eee',
        enabled: false
    },
    series: [{
        data: {}
    }],
    plotOptions: {
        series: {
            pointStart: 1,
            allowPointSelect: false,
            color: 'rgba(41, 180, 180, 0.80)',
            dataLabels: {
                enabled: true,
                crop: false,
                //overflow: "none",
                style: {
                    fontSize: "1.2em",
                    color: "black"
                },
                formatter: function formatter() {
                    if (this.point.z == 1)
                        return;
                    return this.point.z;
                }
            }
        }
    },
    credits: {
        enabled: false
    },
    lang: {
        noData: "no data"
    },
    noData: {
        style: {
            fontWeight: 'bold',
            fontSize: '13px',
            color: '#303030'
        }
    }
};

/*  Polar chart config
 */
  plots.polar.chartConfig =
 {
    title: {
        text: 'Localization',
        style: {"fontSize" : "1.6em", "fontFamily" : "Open Sans", "color" : "#666666", "fontWeight" : "bold"}
    },

    chart: {
        polar: true
    },
    subtitle: {
        text: '% correct per direction'
    },
    pane: {
        startAngle: -90,
        endAngle: 90,
        center: ["50%", "85%"],
        size: "140%"
    },

    xAxis: {
        tickInterval: 45,
        min: -90,
        max: 90,
        showLastLabel: true,
        labels: {
            formatter: function formatter() {
                return this.value + '°';
            }
        }
    },
    yAxis: {
        min: 0,
        max: 1,
        labels: {
            enabled: false
        }
    },
    tooltip: {
        formatter: function formatter() {
            return 'The score for <b>' + this.x + ' °</b> is <b>' + (this.y * 100).toFixed(3) + ' %</b>';
        }
    },
    plotOptions: {
        series: {
            pointStart: -90,
            pointInterval: 45,
            pointPlacement: 'on',
            borderWidth: 2,
            borderColor: 'rgba(255, 255, 255, 0.50)'
        },
        column: {
            pointPadding: 0,
            groupPadding: 0
        }
    },
    legend: {
        enabled: false
    },
    series: [{
        type: 'column',
        name: 'Column',
        data: [],
        color: 'rgba(41, 180, 180, 0.80)'
    }],
    credits: {
        enabled: false
    },
    lang: {
        noData: "no data"
    },
    noData: {
        style: {
            fontWeight: 'bold',
            fontSize: '13px',
            color: '#303030'
        }
    }
};

plots.polar.seriesConfig =
{
    type: 'column'
}

/* Chart colors
 * picked to be compound colors of first color
 */

config.global.chartColors = [ "rgba(41,  180, 180, 0.80)",
                              "rgba(231, 70,  56,  0.80)",
                              "rgba(58,  68,  78,  0.80)",
                              "rgba(56,  139, 231, 0.80)",
                              "rgba(189, 130, 113, 0.80)" ];
