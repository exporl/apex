/* exported getProcedure */
/* global api params ScriptProcedureInterface ResultHighlight Trial */

function shuffle(myArray) {
     var i = myArray.length;

     if (i == 0)
         return false;

     while (--i) {
         var j = Math.floor(Math.random() * (i + 1));
         var tempi = myArray[i];
         var tempj = myArray[j];
         myArray[i] = tempj;
         myArray[j] = tempi;
     }
 }

function getProcedure() {
    return new SPIN();
}

function SPIN() {
    api.registerParameter('SNR');
    this.SNR = Number(params.start_value);

    this.trialdatas = api.trials();
    this.trialcount = this.trialdatas.length;
    this.currenttrial = 0;

    this.wordtypes = ["speechtokenPart1", "speechtokenPart2", "speechtokenPart3", "speechtokenPart4", "speechtokenPart5", "speechtokenPart6", "speechtokenPart7", "speechtokenPart8", "speechtokenPart9"];

    this.previousscore = null;
    this.reversals = 0;

    this.target = 0.5;
    this.adaptation = 1.41;
    this.ROCbase = 1.5;

    this.trialindices = new Array(this.trialcount);
    for (var i=0; i<this.trialcount; i++)
        this.trialindices[i] = i;
    if (params.order == 'random')
        shuffle(this.trialindices);
}

SPIN.prototype = new ScriptProcedureInterface();

SPIN.prototype.firstScreen = function() {
    print('Returning first screen of first trial as first screen');

    return this.trialdatas[0].screen;
}

SPIN.prototype.progress = function() {
    print('Returning progress');

    return String(100 * (this.currenttrial / this.trialcount));
}

SPIN.prototype.processResult = function(screenresult) {
    print('Processing result');

    // save information about what was presented
    this.trialresult.SNR = this.SNR;

    // calculated scores and adaptation
    var score = this.parseMatrixResult(screenresult);
    if (this.currenttrial > 0) {
        if ((score > this.target && this.previousscore < this.target) ||
            (score < this.target && this.previousscore > this.target)) {
            this.reversals += 1;
        }
    }
    var ROC = Math.max(0.1, this.ROCbase * Math.pow(this.adaptation, -this.reversals));
    var delta = -ROC / 0.15 * (score - this.target);

    // save information about subject performance
    this.trialresult.score = score;
    this.trialresult.reversals = this.reversals;
    this.trialresult.ROC = ROC;
    this.trialresult.delta = delta;

    // adapt parameters
    this.SNR = this.SNR + delta;
    this.previousscore = score;
    if (this.currenttrial > 0 ||
        this.currenttrial == 0 && score > this.target)
        this.currenttrial += 1;

    return new ResultHighlight();
}

SPIN.prototype.setupNextTrial = function() {
    print('Returning next trial');

    if (this.currenttrial == this.trialcount)
        return new Trial();

    var trialdata = this.trialdatas[this.trialindices[this.currenttrial]],
        trial = new Trial();

    this.trialresult = {};

    trial.addScreen(trialdata.screen, true, 0);

    var next_stimulus_parameters = {};
    var SNR = Math.round(this.SNR * 10) / 10;
    next_stimulus_parameters.SNR = SNR;

    if ( params.adaptive_gain.indexOf(';') !== -1 ) {
        // more than one gain to adapt

        var adaptive_gains = params.adaptive_gain.split(';');
        var nAdaptive_gains = adaptive_gains.length;

        for (var iAdaptive_gain = 0; iAdaptive_gain < nAdaptive_gains; iAdaptive_gain++)
            next_stimulus_parameters[adaptive_gains[iAdaptive_gain]] = SNR;
            // e.g. next_stimulus_parameters['speechgain'] or next_stimulus_parameters['gain_noisechannel0']

    } else {
        // only one gain to adapt
        next_stimulus_parameters[params.adaptive_gain] = SNR;
        // e.g. next_stimulus_parameters['speechgain'] or next_stimulus_parameters['gain_noisechannel0']
    }
    trial.addStimulus(trialdata.GetStimulus(), next_stimulus_parameters, '', 0);
    trial.setId(trialdata.id);
    return trial;
}

SPIN.prototype.parseMatrixResult = function(screenresult)  {

    var re = /<answer>(\w*)</g,
        score = 0,
        wordID = 0,
        input,
        trial = this.trialdatas[this.trialindices[this.currenttrial]],
        correct,
        nSpeechtokenParts = Number(api.fixedParameterValue(trial.GetStimulus(),"nSpeechtokenParts"));

     if (screenresult.matrix_subject) {
        while ((input = re.exec(screenresult.matrix_subject))) {
            correct = api.fixedParameterValue(trial.GetStimulus(),this.wordtypes[wordID]);
            score += Number(input[1] == correct);
            wordID += 1;
        }
        score = score / nSpeechtokenParts;
     } else if (screenresult.matrix_experimenter) {
        while ((input = re.exec(screenresult.matrix_experimenter)))
            score += Number(input[1]);
        score = score / nSpeechtokenParts;
     } else {
         score = Number(screenresult.buttongroup == "correct");
     }
     score = score.toString();

     return score;
}

SPIN.prototype.resultXml = function() {
    var xml = '';
    xml += '<myScore>' + this.trialresult.score + '</myScore>\n';
    xml += '<myReversals>' + this.trialresult.reversals + '</myReversals>\n';
    xml += '<myROC>' + this.trialresult.ROC + '</myROC>\n';
    xml += '<myDelta>' + this.trialresult.delta + '</myDelta>\n';
    xml += '<procedure><parameter>' + this.trialresult.SNR + '</parameter></procedure>\n';
    return xml;
}

