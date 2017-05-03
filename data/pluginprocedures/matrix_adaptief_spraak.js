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
    this.SNR = Number(params.startSNR);

    this.trialdatas = api.trials();
    this.trialcount = this.trialdatas.length;
    this.currenttrial = 0;
    
    this.wordtypes = ["name", "verb", "number", "color", "object"];
    
    this.previousscore = null;
    this.reversals = 0;

    this.itemcount = 5;
    this.target = 0.5;
    this.adaptation = 1.41;
    this.ROCbase = 1.5;

    this.trialindices = new Array(this.trialcount);
    for (i=0; i<this.trialcount; i++)
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
    var score = this.parseMatrixResult(screenresult) / this.itemcount;
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
    trial.addStimulus(trialdata.GetStimulus(), {
        SNR: Math.round(this.SNR * 10) / 10,
        gain: this.SNR
    }, '', 0);
    trial.setId(trialdata.id);
    return trial;
}

SPIN.prototype.parseMatrixResult = function(screenresult)  {
    var re = /<answer>(\w*)</g,
        count = 0,
        wordID = 0,
        input,
        trial,
        correct;
     if (screenresult.matrix_subject) {
        while (input = re.exec(screenresult.matrix_subject)) {
            trial = this.trialdatas[this.trialindices[this.currenttrial]];
            correct = api.fixedParameterValue(trial.GetStimulus(),this.wordtypes[wordID]);
            count += Number(input[1] == correct);
            wordID += 1;        
        }
     } else if (screenresult.matrix_experimenter) {
        while (input = re.exec(screenresult.matrix_experimenter))
            count += Number(input[1]);
     } else {
        while (input = re.exec(screenresult.matrix))
            count += Number(input[1]);
     }

    return count.toString();
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

