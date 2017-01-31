// Simple constantProcedure implemented as a plugin procedure

function scriptProcedure() {        // Constructor
    currentTrial = 0;
    this.correct_answer = "";
    this.answer = "";
    this.correct = "";
}

scriptProcedure.prototype=new ScriptProcedureInterface();

scriptProcedure.prototype.setupNextTrial = function()
{
    //console.log("TestProcedure setupNextTrial called.");

    var targettrial;
    var targetstimulus;
    var targetscreen;
    var targetanswer;

    trials = api.trials(); //Implement this function in API by using old API (PluginProcedureAPI)

    if (currentTrial>=trials.length) {
        //console.log("No more trials, stopping procedure");
        return Trial();
    }

    targettrial = trials[currentTrial].id;
    targetscreen = trials[currentTrial].screen;
    targetstimulus = trials[currentTrial].GetStimulus();
    targetanswer = trials[currentTrial].GetAnswer();
//debugger;
    var t = new Trial();
    t.addScreen(targetscreen, true, 0);
    t.addStimulus(targetstimulus, {"gain": -9+currentTrial }, "", 0);
    t.setAnswer(targetanswer);
    t.setId(targettrial);

    return t;
}

scriptProcedure.prototype.firstScreen = function() {
    //console.log("TestProcedure firstScreen called");

    trials = api.trials();
    return trials[0].screen;
}


scriptProcedure.prototype.resultXml = function() {
    //console.log("TestProcedure getResultXML called.");

    var result = "<procedure>\n";
    result += ("<correct_answer>" + this.correct_answer + "</correct_answer>\n");
    result += ("<correct>" + this.correct + "</correct>\n");
    result += ("<answer>" + this.answer + "</answer>\n");
    result += "Result from testprocedure\n"
    result += "</procedure>";

    return result;
}


scriptProcedure.prototype.progress = function()
{
    //console.log("TestProcedure progress called.");

    nTrials = api.trials().length;

    return 100*currentTrial/nTrials;

}

scriptProcedure.prototype.processResult = function(screenResult)
{
    //console.log("TestProcedure processResult called.");
    var trials = api.trials(); //Implemelnt this function in API by using old API (PluginProcedureAPI)
    this.correct_answer = trials[currentTrial].GetAnswer();
    this.answer = screenResult['buttongroup1'];
    this.correct = (this.answer == this.correct_answer);

    var rh = new ResultHighlight();
    rh.correct = this.correct;
    rh.highlightElement = this.correct_answer;

    ++currentTrial;

    return rh;
}

scriptProcedure.prototype.checkParameters = function()
{
    //console.log("TestProcedure CheckParameters called");

    for (prop in params)
    {
        //console.log("Property " + prop + "=" + params[prop] );

        if ( prop == "invalidparameter")
            return "CheckParameters: Invalid parameter " + prop + "=" + params[prop];
    }

    return "";
}


function getProcedure()
{
    //console.log("Returning procedure");
    return new scriptProcedure();
}
