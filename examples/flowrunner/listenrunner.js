/*
 * Listen runner: internal workings.
 * See listenrunner.apf for further details
 */

/* eslint-disable no-unused-vars, no-undef */

var lastExperiment;
var lastResult;
var savePath;
var savedRunner;
var autoRunNext;
var nextExperiment;

function setStart(){
    api.on("savedFile", experimentCompleted);
    api.on("experimentClosed", autoContinue);
    //TODO: implement this 'on' function in flowrunner.cpp
    api.on("experimentFailed", experimentFailed);

    var container = document.getElementsByClassName('container')[0];
    container.innerHTML = makeInstructionMessage(runner.introMessage);
    var onClick = 'setNextExperiments(runner.start); autoContinue();'
    var startButton = '<button id="startButton" onclick="' + onClick +
    '" class=" ui-btn ui-shadow ui-corner-all">Start</button>';
    container.innerHTML += '\n' + startButton;

    experimentIndex = 0;

    loadState();
}

function loadState(){
    if(typeof(runner) == "undefined" || typeof(savePath) == "undefined")
        return;

    var saveFile = savePath + runner.saveFile;
    api.readFile(saveFile).then( function(jsonString){
        savedRunner = $.parseJSON(jsonString);
        $("#loadButton").show();
    });
}

function runExperiment(experiment) {
    autoRunNext = false;

    //add error message in case we return to the same screen
    var container = document.getElementsByClassName('container')[0];
    var errorMessage = "<br>" + makeInstructionMessage("Experiment was stopped early or encountered an error.");
    if(!endsWith(container.innerHTML, errorMessage))
        container.innerHTML += errorMessage;

    //find next experiment file
    var relExpPath = runner.experimentPrefix + experiment + runner.experimentExtension;
    lastExperiment = findElement(runner.experiments, "name", experiment);
    if(typeof(lastExperiment) == "undefined")
        lastExperiment = {name: experiment};

    //include results path if we want to save automatically
    var resultsPath = "";
    if(runner.autoSaveExperiments)
        resultsPath = runner.resultsFolder + experiment + "-results.apr";

    var absExpPath = savePath + relExpPath;
    console.log("api: " + absExpPath);
    api.runExperiment(absExpPath, {}, resultsPath);
}

function endsWith(input, match){
    var n = input.length;
    var x = match.length;
    if(n < x)
        return false;
    return input.substring(n - x, n) == match;
}

function findElement(arr, propName, propValue) {
    for (var i=0; i < arr.length; i++){
        if (arr[i][propName] == propValue)
            return arr[i];
    }
}

function experimentCompleted(resultPath){
    api.readFile(resultPath).then( function(results) {
        $("#loadButton").hide();
        runner.results.push(results);
        runner.transitions = getValidTransitions(lastExperiment.transitions, results);
        saveState();
        runner.previousResults.push(results);
        setNextExperiments(runner.transitions);
    });

    runner.previousExperiments.push(lastExperiment.name);
}

function autoContinue(){
    if(autoRunNext)
        runExperiment(nextExperiment);
}

function experimentFailed(errorMessage){
    var container = document.getElementsByClassName('container')[0];
    var lastExperiment = runner
    container.innerHTML += "\n<p> Experiment " + lastExperiment +
        " failed due to an error: " + errorMessage + "</p>"
}

function getValidTransitions(transitions, results){
    lastResult = results;
    var toReturn = [];

    if(typeof transitions === "undefined")
        return toReturn;

    for(var i = 0; i < transitions.length; i++){
        if(checkCondition(transitions[i].condition))
            toReturn.push(transitions[i].name);
    }
    return toReturn;
}

function checkCondition(condition){
    //no conditions; always true
    if(typeof condition === "undefined")
        return true;

    //function condition; evaluate and check if result is a boolean
    if(typeof condition === "function"){

        var eval;
        try{
            eval = condition();
        }
        catch(e){
            eval = false;
            console.error("failed to evaluate transition condition: " + condition);
            console.error(e);
        }
        if(typeof eval === "boolean")
            return eval;

        console.error("condition evaluation gave unusable result: " + JSON.stringify(condition));
        return false;
    }

    //string condition; check for special predefined values
    if(typeof condition === "string"){
        var cl = condition.toLowerCase();

        if(cl === "always" || cl === "true")
            return true;
        if(cl === "never" || cl === "false")
            return false;

        console.error("Could not parse condition string: " + condition);
        return false;
    }

    //anything else is a problem!
    else{
        console.error("Unkown condition type");
        console.log(condition);
        return false;
    }
}

/**
 * options should be a list of strings representing experiments.
 * These are set as options to the user.
 */
function setNextExperiments(options){

    var container = document.getElementsByClassName('container')[0];
    container.innerHTML = makeInstructionMessage(runner.instructionMessage);

    if(options.length === 0){
        container.innerHTML = makeInstructionMessage(runner.outroMessage);
        return;
    }
    var nbOptions = options.length
    for (var i = 0; i < nbOptions; i++){
        appendButton(container, options[i]);
    }

    if(options.length === 1 && runner.runImmediatelyWhenSingleTransition){
        autoRunNext = true;
        nextExperiment = options[0];
    }
}

function appendButton(div, experiment) {
    var onClick = 'runExperiment(\'' + experiment + '\');'

    var button = '<button id="exp1" onclick="' + onClick +
    '" class=" ui-btn ui-shadow ui-corner-all">' + experiment + '</button>';

    div.innerHTML += '\n' + button;
}

function saveState(){
    var jsonString = JSON.stringify(runner, null, '  ');
    var saveFile = savePath + runner.saveFile;
    api.writeFile(saveFile, jsonString);
}

function setLoadedState(){
    $("#loadButton").hide();
    runner = savedRunner;
    setNextExperiments(runner.transitions);
    autoContinue();
}

/**
 * Data filter is used by resultsprocessor for calculating correct percentages
 * @param t input trial object for this data filter
 * @return true if the given trial should be kept for further processing
 */
function dataFilter(t) {
    var value;
    var isConfirm = $(t.DOM).find("isConfirmationTrial").text();
    if (!isConfirm.length)
        value = true;
    else
        value = !stringToBoolean(isConfirm);
    return value;
}

function makeInstructionMessage(message){
    return '<center> <p id="instruction">' + message + '</p> </center>';
}

function experimentDone(experiment){
    return runner.previousExperiments.includes(experiment);
}


var conditionsFactory = function (results) {

    var totalTimeExceeds = function(seconds) {
        return function() { return getTotalTime() > seconds; };
    };

    var timeTodayExceeds = function(seconds) {
        return function() { return getTimeToday() > seconds; };
    };

    var timeLastExperimentExceeds = function(seconds) {
        return function() { return getTimeLastExperiment() > seconds; };
    };

    var lastExperimentStoppedManually = function() {
        return function() { return getStopCondition(results) == "user"};
    }

    function getTimeToday() {
        return getAccumulatedDuration(getResultsStartedToday());
    }

    function getResultsStartedToday() {
        return results.filter(function(result) { return isToday(result); });
    }

    function getTotalTime() {
        return getAccumulatedDuration(results);
    }

    function getAccumulatedDuration(results) {
        return results.reduce(
            function(accumulator, currentValue) { return accumulator + getTimeExperiment(currentValue); },
            0);
    }

    function getTimeLastExperiment() {
        return results.length > 0 ? getTimeExperiment(results[results.length-1]) : 0;
    }

    function getTimeExperiment(result) {
        return parseInt(createParser(result)
            .getElementsByTagName('general')[0]
            .getElementsByTagName('duration')[0]
            .childNodes[0]
            .nodeValue, 10);
    }

    function isToday(result) {
        return new Date().toDateString() == getEndDateExperiment(result).toDateString();
    }

    function getEndDateExperiment(result) {
        return new Date(createParser(result)
            .getElementsByTagName('general')[0]
            .getElementsByTagName('enddate')[0] // TODO: can startdate be used?
            .childNodes[0]
            .nodeValue);
    }

    function getStopCondition(result) {
        return createParser(result)
            .getElementsByTagName('general')[0]
            .getElementsByTagName('stopped_by')[0]
            .childNodes[0]
            .nodeValue;
    }

    function createParser(result) {
        return new DOMParser().parseFromString(result,"text/xml");
    }

    return {
        totalTimeExceeds: totalTimeExceeds,
        timeTodayExceeds: timeTodayExceeds,
        timeLastExperimentExceeds: timeLastExperimentExceeds
    }
};
