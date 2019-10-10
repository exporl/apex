function adaptiveProcedure() {        // Constructor
    this.stairCase = Array();        // List of parameter values
    this.nCorrect = 0;
    this.nWrong = 0;
    this.movingUp = 0;
    this.nReversals = 0;
    this.saturated = false;

    this.nUp;
    this.nDown;
    this.largerIsEasier;
    this.stepSize;

    this.lastStimulus = "";
}

adaptiveProcedure.prototype=new pluginProcedureInterface();

adaptiveProcedure.prototype.adaptParameter = function(lastAnswer)
{
    var parameterValue = this.stairCase[this.stairCase.length-1] ;

    if (lastAnswer) {
    ++this.nCorrect;
    this.nWrong = 0;
    } else {
    ++this.nWrong;
    this.nCorrect = 0;
    }

    if (this.nCorrect == this.nDown) {
    if (!this.movingUp) {
        ++this.nReversals;
    }

    if (this.largerIsEasier) {
        parameterValue -= this.stepSize;
    } else {
        parameterValue += this.stepSize;
    }
    this.nCorrect = 0;
    this.movingUp = true;

    } else if (this.nWrong == this.nUp) {
    if (this.movingUp) {
        ++this.nReversals;
    }

    if (this.largerIsEasier) {
        parameterValue += this.stepSize;
    } else {
        parameterValue -= this.stepSize;
    }
    this.nWrong = 0;
    this.movingUp = false;

    }


    if (parameterValue > this.maxValue) {
    parameterValue = this.maxValue;
    this.saturated = true;
    }
    if (parameterValue < this.minValue) {
    parameterValue = this.minValue;
    this.saturated = true;
    }

    return parameterValue;


}

adaptiveProcedure.prototype.NextTrial = function(answer, screenresult) {
    //console.log("TestProcedure NextTrial called");

    api.SetProgress(this.nReversals+1);

    trials = api.GetTrials();

    if (params.order == "random" ) {
       nTrial = Math.floor(Math.random()*trials.length);
    } else {
    api.Abort( "Order sequential not implemented");
    }

    trial=trials[nTrial];

    var value;    // value of parameter to be adapted
    if (screenresult == null) {        // First trial
        value = parseFloat(params.start_value);
    } else {
    value = this.adaptParameter(answer);
    }
    this.stairCase.push(value);

    // find stimulus with corresponding fixed parameter
    var stimuli = api.GetStimuli();
    var stimulus;
    for (var i=0; i<stimuli.length; ++i) {
    if (api.GetFixedParameterValue(stimuli[i], params.adapt_parameter) == value) {
        stimulus = stimuli[i];
        break
    }
    }
    if (!stimulus) {
    api.Abort("Could not find stimulus with " + params.adapt_parameter + " ==  " + value);
    }
    this.lastStimulus = stimulus;

    if (this.nReversals == this.stopAfterReversals) {
//        api.Finished();
    return "";
    }

    api.CreateOutputList(trial.id, stimulus);
    api.SetScreen(trial.screen);
    return trial.id;

}


adaptiveProcedure.prototype.FirstTrial = function() {
    //console.log("TestProcedure FirstTrial called");
    api.SetNumTrials(params.stop_after_reversals);

    //api.MessageBox("adaptiveProcedure::FirstTrial");

    return this.NextTrial(0,null);
}

adaptiveProcedure.prototype.CheckParameters = function() {
    //console.log("TestProcedure CheckParameters called");

    // nUp, nDown, adapt_parameter, start_value, stop_after_reversals, stepsize, largerIsEasier, min_value, max_value

    for (prop in params) {
        //console.log("Property " + prop + "=" + params[prop] );

        switch (prop) {
        case "nUp":
        this.nUp = parseInt(params[prop]);
        break;
        case "nDown":
        this.nDown = parseInt(params[prop]);
        break;
        case "stop_after_reversals":
        this.stopAfterReversals = parseInt(params[prop]);
        break;
        case "larger_is_easier":
        this.largerIsEasier = ( params[prop] == "true" );
        break;
        case "stepsize":
        this.stepSize = parseFloat( params[prop] );
        case "min_value":
        this.minValue = parseFloat( params[prop] );
        case "max_value":
        this.maxValue = parseFloat( params[prop] );
        break;
    }

    }

    return "";
}


adaptiveProcedure.prototype.GetResultXML = function() {
    return "<procedure>\n" +
    "<stimulus>" + this.lastStimulus + "</stimulus>\n" +
    "<parameter>" + this.stairCase[this.stairCase.length-1] + "</parameter>\n" +
    "</procedure>";
}



new adaptiveProcedure();
