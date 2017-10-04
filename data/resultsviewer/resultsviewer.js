"use strict";

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
    var plots    = {};
    plots.types  = ["line", "matrix", "polar","text"];
    plots.line   = { data: [],
                     highChart: true,
                     prepare:   prepareLine,
                     setData:   null
                   };
    plots.matrix = { data: [],
                     highChart: true,
                     prepare:   prepareMatrix,
                     setData:   setDataMatrix
                   };
    plots.polar  = { data: [],
                     highChart: true,
                     prepare:   null,
                     setData:   setDataPolar
                   };
    plots.text   = { data: [],
                     highChart: false,
                     prepare:   prepareText,
                     setData:   setDataText,
                     textdivs:  [ "linedatatable",
                                  "procedureparametertable",
                                  "percentagescorrect",
                                  "confusionmatrixtable",
                                  "answertable" ]
                   };

/*  ============================================================================
 *  Results
 *  =======
 *                  - all holds every Trial objects parsed by mapAnswer, before sorting
 *                  - default container: group[0]
 *                  - can be grouped by using config.global.multiprocedure
 *                  - names are the groupnames, aka values of keys by which they're grouped
 *                      (this gives us a pseudo-map implementation)
 */
    /*var results = [];
    var resultsGrouped = { names: [],
                           group: null
                         };*/

    var results = { all: [],
                    groupNames: [],
                    group: {}
                   };


/*  Switch to see if Highcharts objects and config have been initialized before
 */
    var graphsReady = false, cfgReady = false;

/*  ============================================================================
 *  Trial Class
 *  - provides an easy-to-handle js object for processing data
 *  - defaultMapAnswer() maps XML data to a Trial object
 *  - Custom implementations of mapAnswer() have to define all (required) fields
 *  ============================================================================
 */
function Trial( xml,                                                    //  Original data in DOM format (required)
                answer, correct_answer, correct, proctype,              //  Default information (required)
                parametervalue,                                         //  Default adaptive procedure information (required for linegraph)
                stepsize, reversals, saturated,                         //  Extra information for adaptive procedure (optional)
                stimulusid, standardid, trialid, responsetime, procid)  //  Extra information (optional)
{
    //  default answer elements
    this.answer = answer;
    this.correct_answer = correct_answer;
    this.correct = correct;
    this.proceduretype = proctype;

    //  adaptive procedure elements
    this.parametervalue = parametervalue;
    this.stepsize = stepsize;
    this.reversals = reversals;
    this.saturated = saturated;

    //  extra data (optional)
    this.stimulusid = stimulusid;
    this.standardid = standardid;
    this.trialid = trialid;
    this.responsetime = responsetime;
    this.procedureid = procid;

    //  source data in DOM format
    this.DOM = xml;
}

 Trial.prototype.isConstantProcedure = function()
 {
    if (typeof this.proceduretype === "undefined")
        return false;
    if (this.proceduretype.indexOf("constantProcedure") !== -1 )
        return true;
    else if (this.proceduretype.indexOf("trainingProcedure") !== -1)
        return true;
    else
        return false;
 }

 Trial.prototype.isAdaptiveProcedure = function()
 {
    if (typeof this.proceduretype === "undefined")
        return false;
    if (this.proceduretype.indexOf("adaptiveProcedure") !== -1 )
        return true;
    else if (this.proceduretype.indexOf("adjustmentProcedure") !== -1)
        return true;
    else
        return false;
}

 Trial.prototype.isCorrect = function()
 {
    if (typeof this.correct === "undefined")
        return false;
    return this.correct;
}

 Trial.prototype.groupKey = function()
 {
     if (typeof config.global.groupproceduresby !== "undefined") {
        return this[config.global.groupproceduresby];
     }else {
        if (typeof this.proceduretype !== "undefined")
            return this.proceduretype;
        else
            return "";
     }
 }

/* Default filter function for linegraph
 * Checks trial's parameter value to see if it is in a proper format / has a numerical value
 */
 Trial.prototype.hasValidParameter = function()
 {
    if (typeof config.line.parameterkey === "undefined")
    {
        //  Default plotted parameter
        if (typeof(this.parametervalue) === "undefined")
            return false;
        return !isNaN(this.parametervalue);
    }
    else
    {
        if (typeof(this[config.line.parameterkey]) === "undefined")
            return false;
        return !isNaN(this[config.line.parameterkey]);
    }
}

/* Extracts adapted parameter value
 * Applies convertParameterValue if it is defined
 */
 Trial.prototype.parameterValue = function()
 {
    if (typeof config.line.parameterkey === "undefined") {
        if (!this.hasValidParameter())
            return null;

        return this.parametervalue;
    }
    else {
        return this[config.line.parameterkey];
    }
}

 Trial.prototype.getAnswer = function()
 {
    if (typeof this.answer === "undefined")
        return "";
    return this.answer;
}

 Trial.prototype.getCorrectAnswer = function()
 {
    if (typeof this.correct_answer === "undefined")
        return "";
    return this.correct_answer;
}

/*  ============================================================================
 *  Utilities
 *  ============================================================================
 */
function stringToBoolean(string)
{
    if (typeof string === "boolean")
        return string;
    if (typeof string !== "string")
        string = string.toString();

    switch (string.toLowerCase().trim()) {
        case "true":case "yes":case "1":
            return true;
        case "false":case "no":case "0":case null:
            return false;
        default:
            return Boolean(string);
    }
}

Array.prototype.contains = function (v)
{
    for (var i = 0; i < this.length; i++) {
        if (this[i] === v)
            return true;
    }
    return false;
};

Array.prototype.unique = function ()
{
    var arr = [];
    for (var i = 0; i < this.length; i++) {
        if (!arr.contains(this[i])) {
            arr.push(this[i]);
        }
    }
    return arr;
};

Array.prototype.sum = function ()
{
    return this.reduce(function (a, b) {
        return a + b;
    }, 0);
};

Array.prototype.last = function ()
{
    return this[this.length - 1];
};

/*  Special sorting function for either alphabetical or numerical arrays
 *  Sorts textual values alphabetically
 *  And numerical values from -inf to +inf
 */
Array.prototype.sortAlphaNumerical = function()
{
    if (!this.some($.isNumeric)) {
        return this.sort();
    } else {
        return this.sort(function (a, b) {
            return parseFloat(a) - parseFloat(b);
        });
    }
}

/*  wraps content around element tags
 *  adds attribute if defined
 *  extra = [{ name: "", value: ""}] (optional)
 */
function wrap(element, content, extra)
{
    if (typeof extra !== "undefined") {
        var s = "";
        if (extra.length > 0) {
            $.each (extra, function(index, xtr) {
                s = " " + xtr.attr + "=\"" + xtr.val + "\"";
            });
        }
        else {
            s = " " + extra.attr + "=\"" + extra.val + "\"";
        }
        return "<" + element + s + ">" + content + "</" + element + ">";
    }
    else
        return "<" + element + ">" + content + "</" + element + ">";
}

//  wrap helper functions

function bold(content) { return wrap("b", content); }

function uline(content) { return wrap("u", content); }

function italic(content) { return wrap("i", content); }

function table(content,extra) { return wrap("table",content, extra); }

function tr(content) { return wrap("tr", content); }

function th(content,extra) { return wrap("th", content,extra); }

function td(content,extra) { return wrap("td", content,extra); }

function attr(a,v) { return { attr: a,val: v }; }

function attr_id(v) { return attr("id",v); }

function attr_class(v) { return attr("class",v); }

/* Replaces common parts from answers and stimuli
 * @param cae Array with strings to be removed
 * @return Processed string
 */
String.prototype.replaceCommonAnswerElements = function(cae)
{
    var str = this;
    for (var i = 0; i < cae.length; i++) {
        var b = false;
        if (this.indexOf(cae[i]) !== -1) {
            str = this.replace(cae[i], "");
            break;
        }
    }
    return str;
}

String.prototype.capitalizeFirstLetter = function() {
    return this.charAt(0).toUpperCase() + this.slice(1);
}

/* Groups arrays by object's key/property
 * e.g.: console.log(groupBy(['one', 'two', 'three'], 'length'));
 *           => {3: ["one", "two"], 5: ["three"]}
 * @param xs Array of objects
 * @param key Key by which to group them
 * @return New object with the properties as keys holding arrays of objects
 */
function groupBy(xs, key) {
  return xs.reduce(function(rv, x) {
    (rv[x[key]] = rv[x[key]] || []).push(x);
    return rv;
  }, {});
};


/*  ============================================================================
 *  Data processing & preparation
 *  ============================================================================
 */
function newAnswer(xmlstring)
{
    //  Add Trial object to results array by parsing answer xmlstring
    //  mapAnswer can be custom implemented
    if (typeof mapAnswer === "function") {
        console.log("Custom answer mapping function found");
        results.all.push(mapAnswer(xmlstring));
    } else {
        //Using default resultsviewer answer mapper
        results.all.push(defaultMapAnswer(xmlstring));
    }
}

//  Default Answer to Trial object mapper
function defaultMapAnswer(xmlstring)
{
    var xml = new DOMParser().parseFromString(xmlstring, "application/xml");

    // default values
    var answer = $(xml).find("answer").text();                              // answer
    answer = answer.replaceCommonAnswerElements(config.global.removefromanswer);
    if (answer === "") answer = "n/a"

    var corransw = $(xml).find("correct_answer").text();                    // correct answer
    if (corransw === "") corransw = $(xml).find("correctanswer").text();
    corransw = corransw.replaceCommonAnswerElements(config.global.removefromanswer);

    var correct = $(xml).find("correct").text();                            // correctness
    if (correct === "") {
        correct = answer === corransw;
    }
    if (correct === "") {
        correct = $(xml).find("corrector > result").text();
    }
    correct = stringToBoolean(correct);

    var stim = $(xml).find("stimulus").text();                              // stimulus id
    var stand = $(xml).find("standard").text();                             // standard id
    var trial = $(xml).find("trial").attr("id");                            // trial id
    var resptime = $(xml).find("responsetime").text();                      // response time
    var proctyp = $(xml).find("procedure").attr("type");                    // procedure type
    var procid = $(xml).find("procedure").attr("id");                       // procedure id

    // adaptive only
    var param = $(xml).find("parameter").text();                            // adaptive parameter value
    if (!param.length) {
        param = $(xml).find("procedure > parameter").text();
    }
    var stepsize = $(xml).find("stepsize").text();                  // stepsize
    var reversals = $(xml).find("reversals").text();                // reversals
    var saturated = $(xml).find("saturated").text();                // saturated

    //  Parse types
    param =     parseFloat(param);
    resptime =  parseFloat(resptime);
    stepsize =  parseFloat(stepsize);
    reversals = parseFloat(reversals);
    saturated = stringToBoolean(saturated);

    return new Trial(xml,
                     answer, corransw, correct, proctyp,
                     param,
                     stepsize, reversals, saturated,
                     stim, stand, trial, resptime, procid);
}

function preparePlotData()
{
    //  Some param & config prep
    prepareConfig();

    /*  FILTER
     *   Use filter function to remove unwanted trials from results (such as confirmation etc)
     *   Will use defaultResultsFilter if (custom) resultsFilter is not defined
     *   Array.filter(function) returns an array of all elements inside the Array for which function returned true
     */

    if (results.all.length > 0) {
        if (typeof resultsFilter === "function") {
            results.all = results.all.filter(resultsFilter);
        } else {
            results.all = results.all.filter(defaultResultsFilter);
        }
    } else
    {
        console.log("No results in resultsarray");
        return false;
    }


    /*  FIND PLOT TYPES
     *   Decide which plot types will be used to visualize data
     *   First checks if config.line.show & config.matrix.show & config.polar.show are undefined
     *   If they are, the results & DOM will be searched for keywords - if that fails only the confusionmatrix will be shown
     */

    if (results.all.length > 0) {
        if (allUndefined()) {
            prepareConfigShow();
        }

        //  If all fail - force matrix
        if (allUndefined() || allOff()) {
            console.log("No plot configuration detected, turning on confusion matrix & text");
            config.matrix.show = true;
            config.text.show = true;
        }

        updateCheckboxStates();

        //  Hide all charts except text, but still need to switch on line or matrix for data
        if (onlyText()) {
            prepareConfigShow();
            config.text.show = true;
            $.each ( plots.types, function(i,type) {
                if (type !== "text") $("#"+type+"container").css("display","none");
            });
        }
    }
    else {
        console.log("No results in resultsarray after filter");
        return false;
    }

    /* PREPARE GROUPS
    *   - For multiprocedures or procedures with several id's
    *   - Values of key used for sorting are used as groupnames
    *   - If multiprocedure is false, all results will be grouped into results.group.default = []
    */

    prepareMultiProcedureConfig();

    /* PARSE RESULTS FOR PLOTTING
    *   Calls parsing/mapping functions depending on the graphs that will be shown
    *   (See data object specification at top of document)
    *   Highcharts specifically needs:
    *   Linegraph       -   Array of paramvalues
    *   Confusionmatrix -   {x,y,z} (all numerical) values where
                            x = correct answer, y = answer, z = amount of pairings between x & y
                            x and y axes can be given labels with categories option
    *   Polar plot      -   Array of % correct per direction - re-uses confusionmatrix' 2D array
    */

    $.each (plots.types, function(index, type)
    {
        //  only call prepare for plots who need it
        if (typeof plots[type].prepare === "function") {
            $.each (results.groupNames, function(index, groupname)
            {
                if (!plots[type].prepare(results.group[groupname], index))
                    return;
            });
        }
    });

    return true;
}


/* =====================================
 *  Line prepare functions
 * =====================================
 */
function prepareLine(presults, index)
{
    var lineData, resultsAfterFilter;

    //  Filter out the trials that are unwanted in the linegraph
    if (typeof lineDataFilter === "function")
        resultsAfterFilter = presults.filter(lineDataFilter);
    else
        resultsAfterFilter = presults.filter(function(t) { return t.hasValidParameter(); });

    if (resultsAfterFilter.length < 1) {
        plots.line.data[index] = dummy.line;
        return "No results after linefilter";
    }

    var extractedValues = resultsAfterFilter.map(function(t) { return t.parameterValue(); });
    var revs = getReversals(extractedValues);

    //  line data object
    lineData =   { values: extractedValues,
                   reversals: revs,
                   meanrevs: getMeanRevs(revs),
                   meanrevstd: getMeanRevs(revs, true),
                   meantrials: getMeanTrials(extractedValues),
                   meantrialstd: getMeanTrials(extractedValues, true)
                  };
    
    // convert line data
    if (typeof convertParameterValue === "function") {
        for ( var key in lineData ) {
            if ( lineData[key].constructor === Array ) {
                lineData[key] = lineData[key].map(convertParameterValue);
            } else {
                lineData[key] = convertParameterValue(lineData[key]);                
            }
        }
    }

    plots.line.data[index] = lineData;
    return true;
}

function getReversals(parametervalues)
{
    var reversals = [];
    var up = 0;
    var down = 0;
    for (var i = 1; i < parametervalues.length; ++i) {
        if (parametervalues[i] > parametervalues[i - 1]) {
            if (down) {
                reversals.push(parametervalues[i - 1]);
                down = 0;
            }
            up = 1;
        }

        if (parametervalues[i] < parametervalues[i - 1]) {
            if (up) {
                reversals.push(parametervalues[i - 1]);
                up = 0;
            }
            down = 1;
        }
    }

    // Last (virtual) trial is not a reversal
    // reversals.push(parametervalues[parametervalues.length-1]);

    return reversals;
}

function getMeanRevs(reversals, getstd)
{
    //  Default argument
    var getstd = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : false;

    // Check if reversalsForMean is defined
    var rfm = 6;
    if (typeof config.line.reversalsformean !== "undefined")
        rfm = config.line.reversalsformean;

    var nrevs = Math.min(rfm, reversals.length);

    // Calculate average
    var sum = 0;
    for (var i = reversals.length - nrevs; i < reversals.length; ++i) {
        sum += reversals[i];
    }
    var average = sum / nrevs;

    // Calculate std
    sum = 0;
    for (var i = reversals.length - nrevs; i < reversals.length; ++i) {
        sum += Math.pow(reversals[i] - average, 2);
    }
    var std = Math.sqrt(sum / nrevs);

    if (!getstd)
        return average;
    else
        return std;
}

function getMeanTrials(parametervalues, getstd)
{
    //  Default argument
    var getstd = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : false;

    // Check if trialsForMean is defined
    var tfm = 6;
    if (typeof config.line.trialsformean != "undefined")
        tfm = config.line.trialsformean;
    var nvalues = Math.min(tfm, parametervalues.length);

    // Calculate average
    var sum = 0;
    for (var i = parametervalues.length - nvalues; i < parametervalues.length; ++i) {
        sum += parametervalues[i];
    }
    var average = sum / nvalues;

    // Calculate std
    sum = 0;
    for (var i = parametervalues.length - nvalues; i < parametervalues.length; ++i) {
        sum += Math.pow(parametervalues[i] - average, 2);
    }
    var std = Math.sqrt(sum / nvalues);

    if (!getstd)
        return average;
    else
        return std;
}


/* =====================================
 *  Matrix prepare functions
 * =====================================
 */

/* Prepare Matrix
  * - makes the plots.matrix.data and plots.polar.data object (see top)
  * @param presults results array (split into groups if multiprocedure)
  * @param index corresponds to results.group[] and plots.<type>.data[] index
  */
function prepareMatrix(presults, index)
{
    /*  Get list of answers and correct_answers & get unique values
     *  Check if they are pre-defined first
     *  If not, extract them from results
     */
    var resultsAfterFilter;
    var correctAnswerLabels = getLabels("stimuli", index);
    var answerLabels = getLabels("answers", index);

    //  Replace common elements
    correctAnswerLabels = correctAnswerLabels.map(function(t)
            { return t.replaceCommonAnswerElements(config.global.removefromanswer); });
    answerLabels = answerLabels.map(function(t)
            { return t.replaceCommonAnswerElements(config.global.removefromanswer); });

    //  Sort them correctly
    correctAnswerLabels = correctAnswerLabels.sortAlphaNumerical();
    answerLabels = answerLabels.sortAlphaNumerical();

    //  Filter out results if filterfunction is defined
    if (typeof matrixDataFilter === "function")
    {
        resultsAfterFilter = presults.filter(matrixDataFilter);
    }
    else {
        resultsAfterFilter = presults;
    }

    //  Check for empty results
    if (resultsAfterFilter.length < 1) {
        plots.matrix.data[index] = dummy.matrix;
        plots.polar.data[index] = dummy.polar;
        return "No results after matrixfilter";
    }

    //  Make 2D array (or check if it already exists in plotData)
    var confusionMatrix2D,
        l_ca = correctAnswerLabels.length,
        l_a = answerLabels.length;

    confusionMatrix2D = new Array(l_ca);
    for (var i = 0; i < l_ca; i++) {
        confusionMatrix2D[i] = new Array(l_a);
        for (var j = 0; j < l_a; j++) {
            confusionMatrix2D[i][j] = 0;
        }
    }

    //  Iterate through every trial obj, map it to x & y
    $.each (resultsAfterFilter, function(index,trial) {
        mapCMdata(trial, confusionMatrix2D, correctAnswerLabels, answerLabels);
    });

    //  polar plot data
    //  get percentage correct per column
    var sumCols = [];
    var polarData = { values: [] };

    for (var i = 0; i < confusionMatrix2D.length; i++) {
        polarData.values[i] = 0.0;
        sumCols[i] = confusionMatrix2D[i].sum();
        if (sumCols[i] === 0) continue;
        var indexAnswer = answerLabels.indexOf(correctAnswerLabels[i]);
        var val = (indexAnswer !== -1) ? confusionMatrix2D[i][indexAnswer] / sumCols[i] : 0;
        polarData.values[i] = val;
    }

    plots.polar.data[index] = polarData;

    //  confusion matrix data
    //  turn 2D matrix into datapoint objects for chart
    var matrixData = { values: [],
                       percentages: polarData.values,
                       xlabels: correctAnswerLabels,
                       ylabels: answerLabels,
                       raw: confusionMatrix2D };

    for (var i = 0; i < confusionMatrix2D.length; i++) {
        for (var j = 0; j < confusionMatrix2D[i].length; j++) {
            if (confusionMatrix2D[i][j] == 0)    continue; //  Do not create datapoints for empty pairings
            matrixData.values.push({ x: i, y: j, z: confusionMatrix2D[i][j] });
        }
    }

    plots.matrix.data[index] = matrixData;

    return true;
}

/* Get stimulus/(correct) answer labels
  * - List of labels that can be used on the axes as categories
  * - can be set in config.global.answers and config.global.stimuli
  * @param type "answers" or "stimuli"
  */
function getLabels(type)
{
    if (typeof config.global[type] === "string")
        return config.global[type].split(/,| /);
    else if (typeof config.global[type] === "object")
        return config.global[type];
    else {
                var r = results.all;

                if (typeof matrixDataFilter === "function")
                    r = r.filter(matrixDataFilter);

                return r.map(function(t) {
                    if (type === "stimuli")
                        return t.getCorrectAnswer();
                    if (type === "answers")
                        return t.getAnswer();
                }).unique();
    }
}

/* Maps Trial objects to a 2D array for confusion matrix calculation
 * @param t Trial object
 * @param resultArr 2D array that will hold calculated data
 * @param xLabels labels with correct answers
 * @param yLabels labels with answers
 */
function mapCMdata(t, resultArr, xLabels, yLabels)
{
    //  Takes trial objects and counts them in a 2D array structure
    var x = xLabels.indexOf(t.correct_answer);
    var y = yLabels.indexOf(t.answer);
    if (x === -1 || y === -1)
        return;
    resultArr[x][y] += 1;
}


/* =====================================
 *  Text prepare functions
 * =====================================
 */

 /* Prepare Text
  * - makes the plots.text.data object (see top)
  * @param presults results array (split into groups if multiprocedure)
  * @param index corresponds to results.group[] and plots.text.data[] index
  */
function prepareText(presults, index)
{
    //  Creates all necessary divs in div #textcontainer
    //  Duplicates for multiple procedures
    var s = "";
    var textDivs = plots.text.textdivs;

    $.each (textDivs, function(i, div) {
        if (config.global.multiprocedure) {
            s += wrap("div","", {attr: "id", val: div+index});
        }
        else {
            s += wrap("div","", {attr: "id", val: div+"0"});
        }
    });

    s = wrap("div", s , {attr: "id", val:"textcontainer"+index});
    $("#textcontainer").append(s);

    //  Prepare tabulated text strings and store them in plots.text.data
    plots.text.data[index] = {};

    if (config.line.show) {
        if ( plots.line.data[index].values[0] !== null) {
            plots.text.data[index].line =
            {
                allvalues:      printAllValues(presults, index),
                lastvalue:      printLastValue(presults, index),
                reversals:      printReversals(presults, index),
                meanrevs:       printMeanRevs(presults, index),
                meantrials:     printMeanTrials(presults, index),
                linedatatable:  printLineDataTable(presults, index),
                paramtable:     printParamTable(presults, index)
            };
        }
    }

    if (config.matrix.show || config.polar.show) {
        if ( plots.matrix.data[index].values[0] !== null && plots.matrix.data[index].values[0].z !== null) {
            plots.text.data[index].matrix =
            {
                correctpercentage:  printCorrectPercentage(presults, index),
                correctpercentages: printCorrectPercentagePerStimulus(presults, index),
                matrixtable: printMatrixTable(presults, index),
                answertable: printAnswerTable(presults, index)
            };
        }
    }
}

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

    var s1 = "Mean (std)<br>last " + nrevs + " reversals";
    var s2 = parseFloat(average.toFixed( config.global.digitsafterdecimal ))
           + " ( &plusmn;" + parseFloat((Math.round(std * 1000) / 1000).toFixed( config.global.digitsafterdecimal )) + " )";
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

    var s1 = "Mean (std)<br>last " + nvalues + " trials";
    var s2 = parseFloat(average.toFixed( config.global.digitsafterdecimal ))
           + " ( &plusmn" + parseFloat((Math.round(std * 1000) / 1000).toFixed( config.global.digitsafterdecimal )) + " )";
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
    var parentheses = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : false;

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
    var parentheses = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : false;

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
    var totalcorrect = 0;
    for (var i = 0; i < resultsAfterFilter.length; i++) {
        if (resultsAfterFilter[i].correct)
            totalcorrect++;
    }

    var percentage = parseFloat((100 * totalcorrect / resultsAfterFilter.length)).toFixed( config.global.digitsafterdecimal );
    var result = tr( td( "Total percentage correct:  " + percentage + "%", attr("colspan", d.xlabels.length)));

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
        t = tr( th( "Procedure: "+results.groupNames[index].capitalizeFirstLetter(), attr("colspan", xLabels.length+1) ));
    }
    else {
        t = "";
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
    //var table = tr( th( results.groupNames[index].capitalizeFirstLetter(), attr("colspan",4) ) )
    var t = (typeof config.global.multiprocedure !== "undefined" && config.global.multiprocedure) ? tr( th( "Procedure: "+results.groupNames[index].capitalizeFirstLetter(), attr("colspan", "4") )) : "";
    t    += tr( th("Trial") + th("Stimulus") + th("Answer") + th("Correct") );
                //var table = wrap("tr", th("Trial") + th("Answer") + th("Correct") + th("Parameter"+ printParameterName(true) + printParameterUnit()));

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
 *  Default Filters
 *  ============================================================================
 */

 /* Filter for all results (removes permanently from results array)
  * @param t Trial object
  * returns true if Trial may stay, false if it must be removed
  */
function defaultResultsFilter(t)
{
    //  Returns true for trials that have to be saved for data plotting (contain actual data)
    //  False for confirmation trials, etc.

    var isConfirm = $(t.DOM).find("isConfirmation").text();
    if (!isConfirm.length)
        return true;
    
    isConfirm = $(t.DOM).find("isConfirmationTrial").text();
    if (!isConfirm.length)
        return true;
    
    return !stringToBoolean(isConfirm);
}

 /* Filter for matrix results plotting (does not remove from results array)
  * @param t Trial object
  * returns true if Trial may stay, false if it must be removed
  * Can be configured with config.matrix.removefrommatrix
  */
function matrixDataFilter(t)
{
    if (typeof config.matrix.removefrommatrix !== "undefined" && config.matrix.removefrommatrix.length > 0) {
        var rem = config.matrix.removefrommatrix;
        for (var i = 0; i < rem.length; i++) {
            if (rem[i] === t.answer || rem[i] === t.correct_answer)
                return false;
        }
    }
    if (typeof t.proceduretype !== "undefined")
        return !t.isAdaptiveProcedure();
    else
        return true;
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

    //  Highchart plots
    if (plots[type].highChart) {
        if (typeof dataSet.values === "undefined")
            return;
        if (dataSet.values.length == 0)
            return;

        // Target div w chart object
        var target = getChart(type);
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
        var target = $("#"+type+"container");
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
