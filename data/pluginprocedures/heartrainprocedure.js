/* exported getProcedure */
/* global module, api, misc, params, ResultHighlight, Trial */

function createProcedure(api, misc, params, trials) {
    api.registerParameter("correct_answer");
    api.registerParameter("current_answer");

    var confirmationTrial = trials.filter(function(trial) {
        return trial.id === "confirmation_trial";
    })[0];

    var trialsToPresent = allTrialsToPresent(trials, confirmationTrial);

    var state = createTrialState(0);

    var nextResultXml = undefined;

    function createTrialState(trialIndex) {
        function setupNextTrial() {
            if (trialIndex >= trialsToPresent.length) return new Trial();

            var result = new Trial();
            result.setId(trial.id);
            result.addScreen(trial.screen, true, 0);
            result.addStimulus(trial.GetStimulus(), params["noisegain"] ? {noisegain: params["noisegain"]} : {}, "", 0);
            return result;
        }

        function processResult(screenresult) {
            var expectedAnswer  = trial.GetAnswer();
            var actualAnswer    = screenresult[api.answerElement(trial.GetID())];
            state.nextState     = confirmationTrialsEnabled() ? confirmationTrialStateFactory.initialConfirmationTrial(trialIndex, actualAnswer) : createTrialState(trialIndex + 1);
            state.resultXml     = createResultXml(expectedAnswer, actualAnswer, trial.GetStimulus());

            var result                  = new ResultHighlight();
            result.overrideCorrectFalse = true;
            result.showCorrectFalse     = true;
            result.correct              = actualAnswer === expectedAnswer;
            result.highlightElement     = expectedAnswer;
            return result;
        }

        function confirmationTrialsEnabled() {
            if (!confirmationTrial) return false;

            return params["heartrain_enabled"] === undefined ? true : params["heartrain_enabled"] === "true";
        }

        function createResultXml(expectedAnswer, actualAnswer, stimulusId) {
            var result = "<procedure type='heartrainProcedure'>\n";
            result += "    <answer>" + actualAnswer + "</answer>\n";
            result += "    <correct_answer>" + expectedAnswer + "</correct_answer>\n";
            result += "    <stimulus>" + stimulusId + "</stimulus>\n";
            result += "    <correct>" + JSON.stringify(actualAnswer === expectedAnswer) + "</correct>\n";
            result += "</procedure>\n";
            return result;
        }

        var trial = trialsToPresent[trialIndex];
        
        var state = {
            trialIndex: trialIndex,
            setupNextTrial: setupNextTrial,
            processResult: processResult,
            resultXml: undefined,
            nextState: undefined
        };

        return state;
    }

    var confirmationTrialStateFactory = function() {
        function initialConfirmationTrial(trialIndex, actualAnswer) {
            return createConfirmationTrialState(actualAnswer, trialIndex, {
                noisegain: params["noisegain"] ? -150 : undefined,
                stimulusId: "dummystimulus"
            });
        }

        function replayExpectedStimulus(trialIndex, actualAnswer) {
            var trial = trialsToPresent[trialIndex];
            return createConfirmationTrialState(actualAnswer, trialIndex, {
                noisegain: params["noisegain"] ? params["noisegain"] : undefined,
                stimulusId: trial.GetStimulus()
            });
        }

        function replayActualStimulus(trialIndex, actualAnswer) {
            var stimulusId = extractStimulusFrom(actualAnswer);
            return createConfirmationTrialState(actualAnswer, trialIndex, {
                noisegain: params["noisegain"] ? (stimulusExists(stimulusId) ? params["noisegain"] : -150) : undefined,
                stimulusId: stimulusExists(stimulusId) ? stimulusId : "dummystimulus"
            });
        }

        function extractStimulusFrom(id) {
            return "stimulus_" + id.split(":").shift();
        }

        function stimulusExists(stimulusId) {
            return api.stimuli().indexOf(stimulusId) !== -1;
        }

        function createConfirmationTrialState(actualAnswer, trialIndex, stimulusSpec) {
            function setupNextTrial() {
                var result = new Trial();
                result.setId(confirmationTrial.id);
                result.addScreen(confirmationTrial.screen, true, 0);

                var stimulusParams = {};
                stimulusParams.correct_answer = extractLabelFrom(getExpectedAnswer());
                stimulusParams.current_answer = extractLabelFrom(actualAnswer);
                if (stimulusSpec.noisegain) stimulusParams.noisegain = stimulusSpec.noisegain;

                result.addStimulus(stimulusSpec.stimulusId, stimulusParams, "", 0);
                return result;
            }

            function extractLabelFrom(id) {
                return removeLeadingUnderscore(id.split(":").pop());
            }

            function removeLeadingUnderscore(id) {
                return id[0] === "_" ? id.slice(1) : id;
            }

            function getExpectedAnswer() {
                return trialsToPresent[trialIndex].GetAnswer();
            }

            function processResult(screenresult) {
                var buttonId    = screenresult[api.answerElement(confirmationTrial.GetID())];
                state.nextState = getNextState(buttonId);
                state.resultXml = createResultXml(buttonId);

                var result                  = new ResultHighlight();
                result.overrideCorrectFalse = true;
                result.showCorrectFalse     = false;
                result.correct              = true;
                return result;
            }

            var confirmationTrialButtonActions = {
                "ownanswer_button": confirmationTrialStateFactory.replayActualStimulus,
                "again_button":     confirmationTrialStateFactory.replayExpectedStimulus,
                "next_button":      function(trialIndex) { return createTrialState(trialIndex + 1); }
            };

            function getNextState(buttonId) {
                return confirmationTrialButtonActions[buttonId](trialIndex, actualAnswer);
            }

            function createResultXml(buttonId) {
                var result = "<procedure type='heartrainProcedure'>\n";
                result += "    <answer>" + buttonId + "</answer>\n";
                result += "    <isConfirmationTrial />\n";
                result += "</procedure>\n";
                return result;
            }

            var state = {
                trialIndex:     trialIndex,
                setupNextTrial: setupNextTrial,
                processResult:  processResult,
                resultXml:      undefined,
                nextState:      undefined
            };

            return state;
        }

        return {
            initialConfirmationTrial:   initialConfirmationTrial,
            replayExpectedStimulus:     replayExpectedStimulus,
            replayActualStimulus:       replayActualStimulus
        };
    }();

    function allTrialsToPresent(trials, confirmationTrial) {
        var result = trials.filter(function(trial) {
            return [confirmationTrial ? confirmationTrial.id : undefined, params["firsttrial"]].indexOf(trial.id) === -1;
        });

        for (var i = 1; i < (params["presentations"] || 1); i++) {
            result = result.concat(result);
        }

        if (params["order"] === "random") {
            misc.shuffleArray(result);
        }

        var firstTrial = trials.filter(function(trial) {
            return trial.id === params["firsttrial"];
        })[0];

        if (firstTrial) {
            result = [firstTrial].concat(result);
        }

        return result;
    }

    function checkParameters() {
        return "";
    }

    function firstScreen() {
        return trialsToPresent[0].screen;
    }

    function setupNextTrial() {
        return state.setupNextTrial();
    }

    function processResult(screenresult) {
        var result = state.processResult(screenresult);
        nextResultXml = state.resultXml;
        state = state.nextState;
        return result;
    }

    function resultXml() {
        return nextResultXml;
    }

    function progress() {
        return state.trialIndex / trialsToPresent.length * 100;
    }

    return {
        checkParameters:    checkParameters,
        firstScreen:        firstScreen,
        setupNextTrial:     setupNextTrial,
        processResult:      processResult,
        resultXml:          resultXml,
        progress:           progress
    };
}

function getProcedure() {
    return createProcedure(api, misc, params, api.trials().slice());
}

if (typeof module !== "undefined") {
    module.exports.createProcedure = createProcedure;
}
