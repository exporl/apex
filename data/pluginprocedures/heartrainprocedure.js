"use strict";

 function shuffle(myArray)
 {
     var i = myArray.length;

     if (i === 0)
         return false;

     while (--i)
     {
         var j = Math.floor( Math.random() * ( i + 1 ) );
         var tempi = myArray[i];
         var tempj = myArray[j];
         myArray[i] = tempj;
         myArray[j] = tempi;
     }
 }


function heartrainProcedure() // Constructor
{
    //trying to mimic an enum :-)
    this.FIRST_TRIAL = 0;
    this.AFTER_TRIAL = 1;
    this.AFTER_CONFIRMATION = 2;
    this.FINISHED = 3;

    this.currentState = this.FIRST_TRIAL;
    this.currentTrial = -1;
    var singleTrials = api.trials();		// all trials without confirmation trials, without repetition

    this.confirmationTrial = singleTrials[singleTrials.length - 1];
    singleTrials.splice(singleTrials.length - 1, 1);

    this.trials = new Array();

    for (i = 0; i < params.presentations; i++)
        this.trials = this.trials.concat(singleTrials);

    if (params.order == "random")
    {
        shuffle(this.trials);
    }

    this.nbOfTrials = this.trials.length;
    api.registerParameter("correct_answer");

    this.currentStimulus = "";

}

heartrainProcedure.prototype = new ScriptProcedureInterface();

heartrainProcedure.prototype.processResult = function(screenresult)
{
	var r = new ResultHighlight();
	r.overrideCorrectFalse = true;

	if (this.currentState == this.AFTER_TRIAL) {
        this.current_answer = screenresult[api.answerElement(this.trials[this.currentTrial].GetID() )];
        this.current_correct_answer = this.trials[this.currentTrial].GetAnswer()
		r.correct = this.current_answer == this.current_correct_answer;
		r.showCorrectFalse = true;
		r.highlightElement = this.trials[this.currentTrial].GetAnswer();
	} else  {
        this.current_answer = screenresult[api.answerElement(this.confirmationTrial.GetID() )];
		r.correct = true;
		r.showCorrectFalse = false;
	}

    this.current_correctness = r.correct;

	return r;
}


heartrainProcedure.prototype.setupNextTrial = function()
{
    //determine which trial to run next
    var trial;
    var stimulusParameters = {};
    var confirmationStimulus = 0;

    if (this.currentState == this.AFTER_TRIAL)
    {
        trial = this.confirmationTrial;
        confirmationStimulus = "dummystimulus";
        /*answer = api.screenElementText(trials[currentTrial].screen,
                                       trials[currentTrial].answerString);*/
		//var thisAnswer = screenresult[]
        //api.SetParameter("correct_answer", answer);
        stimulusParameters["correct_answer"] = this.trials[this.currentTrial].GetAnswer();
        this.currentState = this.AFTER_CONFIRMATION;
    }
    else
    {
        if (this.current_answer == "again_button")
        {
            trial = this.confirmationTrial;
            confirmationStimulus =
		      this.trials[this.currentTrial].GetStimulus();
            this.currentState = this.AFTER_CONFIRMATION;
        }
        else
        {
            if (++this.currentTrial >= this.nbOfTrials)
                return new Trial();

            trial = this.trials[this.currentTrial];
            this.currentState = this.AFTER_TRIAL;
        }
    }

    var targetScreen = trial.GetScreen();
    var targetStimulus;

    if (confirmationStimulus === 0)
        targetStimulus = trial.GetStimulus();
    else
        targetStimulus = confirmationStimulus;

    var newTrial = new Trial();

    newTrial.addScreen(targetScreen, true, 0);
    newTrial.addStimulus(targetStimulus, stimulusParameters, "", 0);
    newTrial.setId("trial");

    this.currentStimulus = targetStimulus;

    return newTrial;
}


heartrainProcedure.prototype.progress = function()
{
    return (this.currentTrial + 1)/this.nbOfTrials;
}

heartrainProcedure.prototype.firstScreen = function()
{
    return this.trials[0].GetScreen();
}

heartrainProcedure.prototype.checkParameters = function()
{
    for (var prop in params)
    {
        //console.log("Property " + prop + "=" + params[prop] );

        if ( prop == "invalidparameter")
            return "CheckParameters: Invalid parameter " + prop + "=" + params[prop];
    }

    return "";
}


heartrainProcedure.prototype.resultXml = function()
{
    var result = "<procedure type=\"heartrainProcedure\">\n";
    result += "    <answer>" + this.current_answer + "</answer>\n";
    if ( !(this.currentState == this.AFTER_CONFIRMATION) ) {
        result += "    <correct_answer>" + this.current_correct_answer + "</correct_answer>\n";
        result += "    <stimulus>" + this.currentStimulus + "</stimulus>\n";
        result += "    <correct>" + JSON.stringify(this.current_correctness) + "</correct>\n";
    }
    result += "    <isConfirmationTrial>" + JSON.stringify(this.currentState == this.AFTER_CONFIRMATION) + "</isConfirmationTrial>\n";

    return result + "</procedure>\n";
}


function getProcedure()
{
    return new heartrainProcedure();
}
