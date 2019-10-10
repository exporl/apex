"use strict";

/* eslint-disable no-unused-vars, no-undef */

    var plots    = {};
    plots.types  = ["line", "matrix", "polar","text"];
    plots.line   = { data: [],
                     highChart: true,
                     prepare:   prepareLine
                   };
    plots.matrix = { data: [],
                     highChart: true,
                     prepare:   prepareMatrix
                   };
    plots.polar  = { data: [],
                     highChart: true,
                     prepare:   null
                   };
    plots.text   = { data: [],
                     highChart: false,
                     prepare:   prepareText,
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

    var results = { all: [],
                    groupNames: [],
                    group: {}
                   };


/*  Switch to see if Highcharts objects and config have been initialized before
 */
    var graphsReady = false, cfgReady = false;


var config =
{
    /* Global options
     *  stimuli:            <array of strings> full list of correct answers / presented stimuli
     *                                          - eg: ["button-90","button-45","button0","button45","button90"]
     *                                            or  ["-90","-75","-60","-45","-30","-15","0","15","30","45","60","75","90"]
     *  answers:            <array of strings> full list of possible answers / user responses
     *  removefromanswer:   <array of strings> list of parts that can be pruned from answerlabels, eg: "button_chair" becomes "chair"
     *                                          - if only one of these two is defined, they will be set to the same value
     *  multiprocedure:     <true/false> if true, will plot each different procedure separately (detected automatically if left undefined)
     *  groupproceduresby:  <string> decides which key to group the procedures by e.g.: "proceduretype"(default) or "procedureid"
     */
        global:
        {
            //answers:             [""],
            //stimuli:             [""],
            removefromanswer:    ["_button","button_","button","b_","answer_","_answer","answer"],
            //multiprocedure:      false,
            //groupproceduresby:   ""
        },


    /* Line graph options
     *  show:               <true/false> force line graph to show (true) or hide (false)
     *  parametername:      <string> shows parameter name on graph - e.g. "Gain"
     *  parameterunit:      <string> unit for parameter values - e.g. "dB SPL"
     *  parameterkey:       <string> the Trial object key to use for parameter plotting (default: parametervalue) - e.g. "responsetime"
     *  trialsformean:      <int> the amount of trials used for mean (default 6)
     *  reversalsformean:   <int> the amount of reversals used for mean (default 6)
     */
        line:
        {
            //show:             true,
            //parametername:    "",
            //parameterunit:    "",
            //parameterkey:     "",
            //trialsformean:    ,
            //reversalsformean:
        },


    /* Confusion matrix options
     *  show:               force matrix to show (true) or hide (false)
     *  removefrommatrix:   list of answers and stimuli that may be filtered out of matrix plotting
     *                      e.g.: ["up","down"]
     */
        matrix:
        {
            //show:             true,
            //removefrommatrix: ["up","down"]
        },


    /* Polar plot options
     *  show:               force polar plot to show (true) or hide (false)
     *  mindegree:          smallest degree of localization-arc (default -90)
     *  maxdegree:          largest degree of .. (default 90)
     */
        polar:
        {
            //show: true,
            //mindegree: ,
            //maxdegree:
        },

    /* Text printing options (applies to result data text)
     *  show:               force text to show (true) or hide (false)
     *  digitsafterdecimal: how many digits to show after decimal point (default 4)
     *  textstyle:          takes a css style object, applies it to ALL untabulated text
     *  tablestyle          same, for tabulated text
     *                      eg: {"font-size" : "120%", "font-weight" : "bold", "color" : "white", "background-color":"black"}
     *                      doc: http://api.jquery.com/css/#css-properties
     */
        text:
        {
            //show: true,
            //digitsafterdecimal: ,
            //textstyle: {},
            //tablestyle: {}
        }
};

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
 *  Data processing & preparation
 *  ============================================================================
 */

 /* Smart config prepare
  * Parses params[] into the config object
  * e.g. params["line_show"] -> config.line.show
  * Sets default values for various config keys
  */
 function prepareConfig()
 {
     if (cfgReady)
         return;

     // Parse resultparameters with proper notation
     if (typeof params !== "undefined") {
         $.each(params, function(key,value) {
             var configPath = key.split(/[._]/); //  split by dot or underscore

             if (configPath.length > 1) {   //  did it split?
                 // force type for specific params
                 if (configPath[1] === "show")   value = stringToBoolean(value);
                 if (configPath[1] === "mindegree" || configPath[1] === "maxdegree") {
                     value = parseFloat(value);}

                 config[configPath[0]][configPath[1]] = value;
             }
         });
     }

    //  equate to each other if only one is defined
    if (config.global.stimuli !== "undefined" && config.global.answers === "undefined") {
        config.global.answers = config.global.stimuli; }
    else if (config.global.stimuli === "undefined" && config.global.answers !== "undefined") {
        config.global.stimuli = config.global.answers; }

    //  split labels
    if (typeof config.global.answers === "string") {
        config.global.answers = config.global.answers.split(/,| /); }
    if (typeof config.global.stimuli === "string") {
        config.global.stimuli = config.global.stimuli.split(/,| /); }

    //  set to empty if undefined
    if (typeof config.global.removefromanswer === "undefined")   config.global.removefromanswer = [""];

    //  how many points after decimal
    if (typeof config.global.digitsafterdecimal === "undefined") config.global.digitsafterdecimal = 4;

    //  line chart yAxis units
    if (typeof config.line.parameterunit === "string") {
        plots.line.chartConfig.yAxis.labels = { format: '{value} '+ config.line.parameterunit };
    }

      //  line chart yAxis param name
    if (typeof config.line.parametername === "string") {
        plots.line.chartConfig.yAxis.title = { text: config.line.parametername };
    }

    cfgReady = true;
 }


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

// Convert results as xml string into array of trial objects by default means
function parseResults(xmlstring)
{
    var xml = new DOMParser().parseFromString(xmlstring, "text/xml");
    var trials = xml.getElementsByTagName("trial");

    var nbTrials = trials.length;
    var toReturn = [];

    var i = 0;
    for (; i < nbTrials; i++){
        var trialText = new XMLSerializer().serializeToString(trials[i]);
        toReturn[i] = defaultMapAnswer(trialText);
    }

    return toReturn;
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
    getstd = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : false;

    // Check if reversalsForMean is defined
    var rfm = 6;
    if (typeof config.line.reversalsformean !== "undefined")
        rfm = config.line.reversalsformean;

    var nrevs = Math.min(rfm, reversals.length);

    // Calculate average
    var sum = 0;
    var i = reversals.length - nrevs;
    for (; i < reversals.length; ++i) {
        sum += reversals[i];
    }
    var average = sum / nrevs;

    // Calculate std
    sum = 0;
    i = reversals.length - nrevs;
    for (; i < reversals.length; ++i) {
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
    getstd = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : false;

    // Check if trialsForMean is defined
    var tfm = 6;
    if (typeof config.line.trialsformean != "undefined")
        tfm = config.line.trialsformean;
    var nvalues = Math.min(tfm, parametervalues.length);

    // Calculate average
    var sum = 0;
    var i = parametervalues.length - nvalues;
    for (; i < parametervalues.length; ++i) {
        sum += parametervalues[i];
    }
    var average = sum / nvalues;

    // Calculate std
    sum = 0;
    i = parametervalues.length - nvalues;
    for (; i < parametervalues.length; ++i) {
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
    var i = 0;
    var j;
    for (; i < l_ca; i++) {
        confusionMatrix2D[i] = new Array(l_a);
        j = 0;
        for (; j < l_a; j++) {
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

    i = 0;
    for (; i < confusionMatrix2D.length; i++) {
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

    i = 0;
    for (; i < confusionMatrix2D.length; i++) {
        j = 0;
        for (; j < confusionMatrix2D[i].length; j++) {
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


/* Returns rounded percentage of given
 * results that were marked as correct
 */
function percentCorrect(trials)
{
    var trialsCorrect = trials.filter(function(trial) { return trial.correct; });
    return 100 * trialsCorrect.length / trials.length;
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




//============================================================================
//  RV internals (do not touch)
//============================================================================

//  For setting an empty object when the prepare functions have no results to work with
var dummy = {};
dummy.line =
{
    values: [null],
    reversals: [0],
    meanrevs: 0,
    meanrevstd: 0,
    meantrials: 0,
    meantrialsstd: 0
}
dummy.matrix =
{
    values:[null],
    percentages:[],
    xlabels: [],
    ylabels: [],
    raw: []
}
dummy.polar =
{
   percentages:[null]
}
dummy.text =
{
    line: {
        allvalues: "",
        lastvalue: "",
        reversals: "",
        meanrevs: "",
        meantrials: "",
        paramtable: ""
    },
    matrix: {
        correctpercentage: "",
        correctpercentages: "",
        matrixtable: "",
        answertable: ""
    }
}

function resetGroups()
{
    //  For turning off multiprocedure, resets all groups to 1 default groupproceduresby
    results.group = [];
    results.group["Default"] = results.all;
    results.groupNames = ["Default"];
    results.groupNames.length = 1;
}



/*  ============================================================================
 *  Standard transition condition functions for flowrunners
 *  ============================================================================
 */

function percentageCorrect(result, trialFilter) {
    return percentCorrect(parseResults(result).filter(trialFilter));
}

/**
 * At least the given number of trials were done in total
 * @param number target number of trials
 */
function trialsDone(number){
    var nTrials = getTrials().length;
    return nTrials >= number;
}

/**
 * At least the given number of trials were answerd correctly
 * @param number target number of trials
 */
function trialsCorrect(number){
    return getNCorrect() >= number;
}

/**
 * At least the given number of reversals have occured
 * @param number target number of reversals
 */
function reversalsDone(number){
    var trials = getTrials();
    var nTrials = trials.length;
    var nRevs = trials[nTrials - 1].reversals;
    return nRevs >= number;
}

/**
 * The average parameter value is at least the given value.
 * @param number target number for average parameter value
 * @param revs amount of reversals over which to take the average
 */
function meanOverRevs(number, revs){
    return getMeanOverRevs(revs) >= number;
}

/**
 * The average parameter value is at least the given value.
 * @param number target number for average parameter value
 * @param trials amount of trials over which to take the average
 */
function meanOverTrials(number, trials){
    return getMeanOverTrials(trials) >= number;
}

/**
 * The average parameter value has improved since the last
 * experiment by at least the given value.
 * @param number target amount by which average parameter should have improved
 * @param revs amount of reversals over which to take the average
 */
function meanOverRevsImprovedBy(number, revs){
    var curr = getMeanOverRevs(revs);
    var prev = getMeanOverRevs(revs, 1);
    return curr >= prev + number;
}

/**
 * The average parameter value has improved since the last
 * experiment by at least the given value.
 * @param number target amount by which average parameter should have improved
 * @param trials amount of trials over which to take the average
 */
function meanOverTrialsImprovedBy(number, trials){
    var curr = getMeanOverTrials(trials);
    var prev = getMeanOverTrials(trials, 1);
    return curr >= prev + number;
}

/**
 * Matrix value is at least the given number
 * @param number target value
 * @param x row index of target matrix value
 * @param y column index of target matrix value
 */
function matrixValue(number, x, y){
    return getMatrixValue(x, y) >= number;
}

/**
 * returns true
 */
function always(){ return true; }

/**
 * returns false
 */
function never(){ return false; }

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
}

/**
 * Rounds a number to the specified amount of digits behind the decimal.
 * Note that the function "round(num);" effectively replaces
 * "parseFloat(num.toFixed( config.global.digitsafterdecimal ));"
 * @param num number to be rounded
 * @param number of digits behind the decimal to be kept after rounding.
 *               If undefined, the configuration default value is used.
 */
function round(num, digits) {
    if(typeof(digits) == "undefined")
        digits = config.global.digitsafterdecimal;

    var fct = Math.pow(10, digits);
    return Math.round(num * fct) / fct;
}

/**
 * Returns amount of trials that were answered correctly
 * @param nBack amount of experiments to look back in history
 */
function getNCorrect(nBack){
    var trials = getTrials(nBack);
    var nCorrect = 0;
    for(var i = 0; i < trials.length; i++){
        if(trials[i].correct)
            nCorrect++;
    }
    return nCorrect;
}

/**
 * Returns list of trial (results) of a specific experiment in the history
 * @param nBack number of experiments to look back in the history.
 *        A value <= 0 corresponds to the last experiment, 1 is the last
 *        experiment behind that etc.
 */
function getTrials(nBack){
    var toReturn;
    if(isNaN(parseFloat(nBack)) || nBack <= 0)
        toReturn = parseResults(lastResult);
    else{
        var index = runner.previousResults.length - 1 - nBack;
        toReturn = parseResults(runner.previousResults[index]);
    }

    if (typeof resultsFilter === "function") {
        toReturn = toReturn.filter(resultsFilter);
    } else {
        toReturn = toReturn.filter(defaultResultsFilter);
    }

    return toReturn;
}

/**
 * Returns mean parameter value of a past experiment
 * @param revs number of reversals to take the average over
 * @param nBack amount of experiments to look back in history
 */
function getMeanOverRevs(revs, nBack){
    config.line.reversalsformean = revs;
    var extractedValues = getTrials(nBack).map(function(t) { return t.parameterValue(); });
    var allRevs = getReversals(extractedValues);
    return getMeanRevs(allRevs);
}

/**
 * Returns mean parameter value of a past experiment
 * @param trials number of trials to take the average over
 * @param nBack amount of experiments to look back in history
 */
function getMeanOverTrials(trials, nBack){
    config.line.trialsformean = trials;
    var extractedValues = getTrials(nBack).map(function(t) { return t.parameterValue(); });
    return getMeanTrials(extractedValues)
}

/**
 * Returns the data value of a matrix result from the previous experiment
 * @param x row
 * @param y column
 */
function getMatrixValue(x, y){
    plots.matrix.prepare(getTrials(), 0);
    var matrixData = plots.matrix.data[0];
    return matrixData.raw[x][y];
}