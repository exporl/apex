/* global params, api */

function adaptiveProcedure() {
    this.stairCase = [];
    this.nCorrect = 0;
    this.nWrong = 0;
    this.movingUp = true;
    this.nReversals = 0;
    this.nBack = 0;
    this.trialsDone = 0;
    this.saturated = false;

    this.rules = [{
        "stepsize": 1,
        "nup": 1,
        "ndown": 1
    }];
    this.largerIsEasier = true;
    this.answerHistory = [];
}

adaptiveProcedure.prototype = new ScriptProcedureInterface();

adaptiveProcedure.prototype.setupNextTrial = function() {
    var trial = new Trial(),
        value = this.trialsDone === 0 ? parseFloat(params.start_value)
                                      : this.adaptParameter(),
        parameters = {};
    parameters[params.adapt_parameter] = value;

    this.stairCase.push(value);

    if (this.nReversals === this.stopAfterReversals)
        return Trial();
    if (this.trialsDone === this.stopAfterTrials)
        return Trial();

    if (this.trialsDone < this.nBack) {
        trial.setId("trial-continue");
        trial.addScreen('screen-nback', true, 0);
    } else {
        trial.setId("trial-answer");
        trial.addScreen('screen1', true, 0);
    }
    if (Math.random() < 0.5) {
        trial.addStimulus('stimulus2', parameters, '', 0);
        trial.addStimulus('stimulus1', parameters, '', 0);
        trial.setAnswer('button-before');
    } else {
        trial.addStimulus('stimulus1', parameters, '', 0);
        trial.addStimulus('stimulus2', parameters, '', 0);
        trial.setAnswer('button-after');
    }

    this.trialsDone += 1;
    this.currentTrial = trial;
    return trial;

};

adaptiveProcedure.prototype.adaptParameter = function() {
    var parameterValue = this.stairCase[this.stairCase.length - 1];

    if (this.trialsDone <= this.nBack)
        return parameterValue;

    this.applyRules();
    this.calculateReversals();
    /* Reapply rules before setting adaptive parameter. */
    this.applyRules();

    if (this.movingUp) {
        if (this.largerIsEasier)
            parameterValue -= this.stepSize;
        else
            parameterValue += this.stepSize;
    } else {
        if (this.largerIsEasier)
            parameterValue += this.stepSize;
        else
            parameterValue -= this.stepSize;
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
};

adaptiveProcedure.prototype.applyRules = function() {
    if (this.nReversals < this.rules.length) {
        this.stepSize = this.rules[this.nReversals]['stepsize'];
        this.nUp = this.rules[this.nReversals]['nup'];
        this.nDown = this.rules[this.nReversals]['ndown'];
    }
};

adaptiveProcedure.prototype.calculateReversals = function() {
    if (this.lastAnswerWasCorrect()) {
        this.nCorrect += 1;
        this.nWrong = 0;
    } else {
        this.nWrong += 1;
        this.nCorrect = 0;
    }
    if (this.nCorrect === this.nDown) {
        if (!this.movingUp) {
            this.nReversals += 1;
        }
        this.nCorrect = 0;
        this.movingUp = true;
    } else if (this.nWrong === this.nUp) {
        if (this.movingUp) {
            this.nReversals += 1;
        }
        this.nWrong = 0;
        this.movingUp = false;
    }
};

adaptiveProcedure.prototype.lastAnswerWasCorrect = function() {
    if (this.trialsDone <= this.nBack)
        return true;
    var nBackResult =
        this.answerHistory[this.answerHistory.length - 1 - this.nBack],
        lastResult = this.answerHistory[this.answerHistory.length - 1];
    return lastResult['answer'] === nBackResult['correctanswer'];
};

adaptiveProcedure.prototype.progress = function() {
    if (typeof this.stopAfterReversals !== 'undefined')
        return 100 * this.nReversals / this.stopAfterReversals;
    if (typeof this.stopAfterTrials !== 'undefined')
        return 100 * this.trialsDone / this.stopAfterTrials;
    return 0;
};

adaptiveProcedure.prototype.firstScreen = function() {
    return 'screen-nback';
};

adaptiveProcedure.prototype.checkParameters = function() {
    if ('stop_after_reversals' in params)
        this.stopAfterReversals = parseInt(params['stop_after_reversals'], 10);
    if ('stop_after_trials' in params)
        this.stopAfterTrials = parseInt(params['stop_after_trials'], 10);
    if ('larger_is_easier' in params)
        this.largerIsEasier = (params['larger_is_easier'] === 'true');
    if ('nback' in params)
        this.nBack = parseInt(params['nback'], 10);
    if ('min_value' in params)
        this.minValue = parseFloat(params['min_value']);
    if ('max_value' in params)
        this.maxValue = parseFloat(params['max_value']);
    if ('rules' in params) {
        this.rules = JSON.parse(params['rules']);
        if (!(this.rules instanceof Array))
            return 'invalid JSON';
        for (var i = 0; i < this.rules.length; ++i) {
            if (!this.rules[i].hasOwnProperty('stepsize'))
                return 'please define a stepsize in rule ' + (i + 1);
            if (!this.rules[i].hasOwnProperty('nup'))
                return 'please define nup in rule ' + (i + 1);
            if (!this.rules[i].hasOwnProperty('ndown'))
                return 'please define ndown in rule ' + (i + 1);
        }
    }
    return '';
};

adaptiveProcedure.prototype.processResult = function(screenResult) {
    this.answerHistory.push({
        /* Whichever button was defined in the screenresult,
         * was pressed by the subject.
         */
        'answer' : typeof screenResult['button-before'] !== 'undefined' ?
            'button-before' : 'button-after',
        'correctanswer': this.currentTrial.answer()
    });
    var r = new ResultHighlight();
    r.overrideCorrectFalse = true;
    r.showCorrectFalse = (this.trialsDone > this.nBack);
    r.correct = this.lastAnswerWasCorrect();
    return r;
};

adaptiveProcedure.prototype.resultXml = function() {
    if (this.trialsDone <= this.nBack)
        return '<procedure>\n' +
               '    <presentation>' + this.trialsDone + '</presentation>\n' +
               '<nback>' + this.nBack + '</nback>\n' +
               '    <parameter>' + this.stairCase[this.trialsDone - 1] +
               '</parameter>\n' +
            '</procedure>';

    var nBackResult =
            this.answerHistory[this.answerHistory.length - 1 - this.nBack],
        lastResult = this.answerHistory[this.answerHistory.length - 1];
    return '<procedure>\n' +
           '    <presentation>' + this.trialsDone + '</presentation>\n' +
           '<nback>' + this.nBack + '</nback>\n' +
           '    <parameter>' + this.stairCase[this.trialsDone - 1] +
           '</parameter>\n' +
           '    <answer>' + lastResult['answer'] + '</answer>\n' +
           '    <correctanswer>' + nBackResult['correctanswer'] +
           '</correctanswer>\n' +
           '    <correct>' +
           String(nBackResult['correctanswer'] === lastResult['answer']) +
           '</correct>\n' +
           '</procedure>';
};

function getProcedure() {
    return new adaptiveProcedure();
}
