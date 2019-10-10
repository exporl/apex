/* eslint-env node */

import {afterEach, describe, it} from "mocha";
import {expect} from "chai";
import sinon from "sinon";
import _ from "lodash";

import * as proc from "../../pluginprocedures/heartrainprocedure";

describe("Heartrain procedure", function() {

    it("first screen", function() {
        const procedure = createProcedure({}, trials);

        expect(procedure.firstScreen()).to.equal("screen-1");
    });

    it("initial trial", function() {
        const procedure = createProcedure({noisegain: 10}, trials);

        const trial = procedure.setupNextTrial();

        expect(trial.id).to.equal("trial-1");
        expect(trial.screen).to.equal("screen-1");
        expect(trial.stimulus).to.equal("stimulus_expected-1");
        expect(trial.stimulusParams).to.deep.equal({noisegain: 10});
    });

    it("first regular trial correct", function() {
        const procedure = createProcedure({}, trials);
        const actualResult = procedure.processResult({answerElement: "expected-1"});

        expect(actualResult.overrideCorrectFalse).to.be.true;
        expect(actualResult.showCorrectFalse).to.be.true;
        expect(actualResult.correct).to.be.true;
        expect(actualResult.highlightElement).to.equal("expected-1");
    });

    it("first regular trial incorrect", function() {
        const procedure = createProcedure({}, trials);
        const actualResult = procedure.processResult({answerElement: "unexpected"});

        expect(actualResult.overrideCorrectFalse).to.be.true;
        expect(actualResult.showCorrectFalse).to.be.true;
        expect(actualResult.correct).to.be.false;
        expect(actualResult.highlightElement).to.equal("expected-1");
    });

    it("first confirmation trial after correct answer", function() {
        const procedure = createProcedure({noisegain: 10}, trials);
        procedure.processResult({answerElement: "expected-1"});
        const actualConfirmationTrial = procedure.setupNextTrial();

        expect(actualConfirmationTrial.id).to.equal("confirmation_trial");
        expect(actualConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(actualConfirmationTrial.stimulus).to.equal("dummystimulus");
        expect(actualConfirmationTrial.stimulusParams).to.deep.equal({
            correct_answer: "expected-1",
            current_answer: "expected-1",
            noisegain:      -150
        });
    });

    it("first confirmation trial after incorrect answer", function() {
        const procedure = createProcedure({noisegain: 10}, trials);
        procedure.processResult({answerElement: "unexpected"});
        const actualConfirmationTrial = procedure.setupNextTrial();

        expect(actualConfirmationTrial.id).to.equal("confirmation_trial");
        expect(actualConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(actualConfirmationTrial.stimulus).to.equal("dummystimulus");
        expect(actualConfirmationTrial.stimulusParams).to.deep.equal({
            correct_answer: "expected-1",
            current_answer: "unexpected",
            noisegain:      -150
        });
    });

    it("confirmation trial after choosing 'again' in previous confirmation trial", function() {
        const procedure = createProcedure({noisegain: 10}, trials);
        procedure.processResult({answerElement: "expected-1"});
        procedure.processResult({answerElement: "again_button"});
        const actualConfirmationTrial = procedure.setupNextTrial();

        expect(actualConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(actualConfirmationTrial.stimulus).to.equal("stimulus_expected-1");
        expect(actualConfirmationTrial.stimulusParams).to.deep.equal({
            correct_answer: "expected-1",
            current_answer: "expected-1",
            noisegain:      10
        });
    });

    it("confirmation trial after choosing 'ownanswer' in previous confirmation trial, answered correctly", function() {
        const procedure = createProcedure({noisegain: 10}, trials);
        procedure.processResult({answerElement: "expected-1"});
        procedure.processResult({answerElement: "ownanswer_button"});
        const actualConfirmationTrial = procedure.setupNextTrial();

        expect(actualConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(actualConfirmationTrial.stimulus).to.equal("stimulus_expected-1");
        expect(actualConfirmationTrial.stimulusParams).to.deep.equal({
            correct_answer: "expected-1",
            current_answer: "expected-1",
            noisegain:      10
        });
    });

    it("confirmation trial after choosing 'ownanswer' in previous confirmation trial, answered incorrectly", function() {
        const procedure = createProcedure({noisegain: 10}, trials);
        procedure.processResult({answerElement: "unexpected"});
        procedure.processResult({answerElement: "ownanswer_button"});
        const actualConfirmationTrial = procedure.setupNextTrial();

        expect(actualConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(actualConfirmationTrial.stimulus).to.equal("stimulus_unexpected");
        expect(actualConfirmationTrial.stimulusParams).to.deep.equal({
            correct_answer: "expected-1",
            current_answer: "unexpected",
            noisegain:      10
        });
    });

    it("confirmation trial after choosing 'ownanswer' in previous confirmation trial, answered incorrectly and stimulus from given answer doesn't exist", function() {
        const procedure = createProcedure({noisegain: 10}, trials);
        procedure.processResult({answerElement: "unexpected_and_unexisting"});
        procedure.processResult({answerElement: "ownanswer_button"});
        const actualConfirmationTrial = procedure.setupNextTrial();

        expect(actualConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(actualConfirmationTrial.stimulus).to.equal("dummystimulus");
        expect(actualConfirmationTrial.stimulusParams).to.deep.equal({
            correct_answer: "expected-1",
            current_answer: "unexpected_and_unexisting",
            noisegain:      -150
        });
    });

    it("should continue to next trial after choosing 'next' in confirmation trial", function() {
        const procedure = createProcedure({noisegain: 10}, trials);
        procedure.processResult({answerElement: "expected-1"});
        procedure.processResult({answerElement: "next_button"});
        const actualConfirmationTrial = procedure.setupNextTrial();

        expect(actualConfirmationTrial.screen).to.equal("screen-2");
        expect(actualConfirmationTrial.stimulus).to.equal("stimulus_expected-2");
        expect(actualConfirmationTrial.stimulusParams).to.deep.equal({
            noisegain: 10
        });
    });

    it("can disable heartraining with parameter", function() {
        const procedure = createProcedure({heartrain_enabled: "false", noisegain: 10}, trials);
        const firstTrial = procedure.setupNextTrial();
        expect(firstTrial.screen).to.equal("screen-1");
        procedure.processResult({answerElement: "expected-1"});
        const secondTrial = procedure.setupNextTrial();
        expect(secondTrial.screen).to.equal("screen-2");
    });

    it("heartraining is disabled when there is no confirmation_trial", function() {
        const trials = [
            new TrialData("trial-1", "screen-1", "stimulus_expected-1", "expected-1"),
            new TrialData("trial-2", "screen-2", "stimulus_expected-2", "expected-2")
        ];

        const procedure = createProcedure({noisegain: 10}, trials);
        procedure.processResult({answerElement: "expected-1"});
        const actualTrial = procedure.setupNextTrial();

        expect(actualTrial.screen).to.equal("screen-2");
    });

    it("should register parameters 'correct_answer' and 'current_answser'", function() {
        const api = createApi(trials);
        createProcedure({}, trials, api);

        expect(api.registerParameter.calledWith("correct_answer")).to.be.true;
        expect(api.registerParameter.calledWith("current_answer")).to.be.true;
    });

    it("should take 'presentations' parameter into account", function() {
        const procedure = createProcedure({presentations: 2}, trials);

        procedure.processResult({answerElement: "expected-1"});
        procedure.processResult({answerElement: "next_button"});

        expect(procedure.setupNextTrial().screen).to.equal("screen-2");

        procedure.processResult({answerElement: "expected-2"});
        procedure.processResult({answerElement: "next_button"});

        expect(procedure.setupNextTrial().screen).to.equal("screen-1");

        procedure.processResult({answerElement: "expected-1"});
        procedure.processResult({answerElement: "next_button"});

        expect(procedure.setupNextTrial().screen).to.equal("screen-2");

        procedure.processResult({answerElement: "expected-2"});
        procedure.processResult({answerElement: "next_button"});

        expect(procedure.setupNextTrial().id).to.be.undefined;
    });

    it("should end after confirmation trial of last trial", function() {
        const procedure = createProcedure({}, trials);

        procedure.processResult({answerElement: "expected-1"});
        procedure.processResult({answerElement: "next_button"});

        expect(procedure.setupNextTrial().screen).to.equal("screen-2");

        procedure.processResult({answerElement: "expected-2"});
        procedure.processResult({answerElement: "next_button"});

        expect(procedure.setupNextTrial().id).to.be.undefined;
    });

    it("trials can be ordered randomly", function() {
        const firstTrials = [];

        for (let i = 0; i < 20; i++) {
            const procedure = createProcedure({order: "random"}, trials);
            firstTrials.push(procedure.setupNextTrial().screen);
        }

        expect(firstTrials).to.include("screen-1");
        expect(firstTrials).to.include("screen-2");
    });

    it("should process results correctly for confirmation trials", function() {
        const procedure = createProcedure({}, trials);
        procedure.processResult({answerElement: "expected-1"});
        const actualResult = procedure.processResult({answerElement: "next_button"});

        expect(actualResult.overrideCorrectFalse).to.be.true;
        expect(actualResult.showCorrectFalse).to.be.false;
        expect(actualResult.correct).to.be.true;
        expect(actualResult.highlightElement).to.be.undefined;
    });

    it("should not set stimulus parameter 'noisegain' when no noisegain parameter is set", function() {
        const procedure = createProcedure({}, trials);

        const firstTrial = procedure.setupNextTrial();
        expect(firstTrial.screen).to.equal("screen-1");
        expect(firstTrial.stimulusParams).to.not.have.property("noisegain");

        procedure.processResult({answerElement: "expected-1"});
        const firstConfirmationTrial = procedure.setupNextTrial();
        expect(firstConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(firstConfirmationTrial.stimulusParams).to.not.have.property("noisegain");

        procedure.processResult({answerElement: "again_button"});
        const secondConfirmationTrial = procedure.setupNextTrial();

        expect(secondConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(secondConfirmationTrial.stimulusParams).to.not.have.property("noisegain");
    });

    it("should update progress after each confirmation trial", function() {
        const procedure = createProcedure({}, trials);

        expect(procedure.progress()).to.equal(0);

        procedure.processResult({answerElement: "expected-1"});
        procedure.processResult({answerElement: "next_button"});

        expect(procedure.progress()).to.equal(50);

        procedure.processResult({answerElement: "expected-2"});
        procedure.processResult({answerElement: "next_button"});

        expect(procedure.progress()).to.equal(100);
    });

    it("xml after trial, given answer is correct", function() {
        const procedure = createProcedure({}, trials);
        procedure.processResult({answerElement: "expected-1"});

        expect(procedure.resultXml().replace(/ +?/g, "")).to.equal(
            `<procedure type='heartrainProcedure'>
                    <answer>expected-1</answer>
                    <correct_answer>expected-1</correct_answer>
                    <stimulus>stimulus_expected-1</stimulus>
                    <correct>true</correct>
                </procedure>\n`.replace(/ +?/g, ""));
    });

    it("xml after trial, given answer is incorrect", function() {
        const procedure = createProcedure({}, trials);
        procedure.processResult({answerElement: "unexpected"});

        expect(procedure.resultXml().replace(/ +?/g, "")).to.equal(
            `<procedure type='heartrainProcedure'>
                    <answer>unexpected</answer>
                    <correct_answer>expected-1</correct_answer>
                    <stimulus>stimulus_expected-1</stimulus>
                    <correct>false</correct>
                </procedure>\n`.replace(/ +?/g, ""));
    });

    it("xml after confirmation trial", function() {
        const procedure = createProcedure({}, trials);
        procedure.processResult({answerElement: "expected-1"});
        procedure.processResult({answerElement: "again_button"});

        expect(procedure.resultXml().replace(/ +?/g, "")).to.equal(
            `<procedure type='heartrainProcedure'>
                    <answer>again_button</answer>
                    <isConfirmationTrial/>
                </procedure>\n`.replace(/ +?/g, ""));

        procedure.processResult({answerElement: "ownanswer_button"});

        expect(procedure.resultXml().replace(/ +?/g, "")).to.equal(
            `<procedure type='heartrainProcedure'>
                    <answer>ownanswer_button</answer>
                    <isConfirmationTrial/>
                </procedure>\n`.replace(/ +?/g, ""));

        procedure.processResult({answerElement: "next_button"});

        expect(procedure.resultXml().replace(/ +?/g, "")).to.equal(
            `<procedure type='heartrainProcedure'>
                    <answer>next_button</answer>
                    <isConfirmationTrial/>
                </procedure>\n`.replace(/ +?/g, ""));
    });

    it("can encode label en stimulus-id in answer/button-id as 'stimulus-suffix:label-value'", function() {
        const trials = [
            new TrialData("confirmation_trial", "confirmation_screen"),
            new TrialData("trial", "screen", "stimulus_expected", "expected:label")
        ];

        const procedure = createProcedure({}, trials, createApi(trials));
        procedure.processResult({answerElement: "expected:label"});
        const firstconfirmationTrial = procedure.setupNextTrial();

        expect(firstconfirmationTrial.screen).to.equal("confirmation_screen");
        expect(firstconfirmationTrial.stimulus).to.equal("dummystimulus");
        expect(firstconfirmationTrial.stimulusParams).to.deep.equal({
            correct_answer: "label",
            current_answer: "label"
        });

        procedure.processResult({answerElement: "again_button"});
        const secondConfirmationTrial = procedure.setupNextTrial();

        expect(secondConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(secondConfirmationTrial.stimulus).to.equal("stimulus_expected");
        expect(secondConfirmationTrial.stimulusParams).to.deep.equal({
            correct_answer: "label",
            current_answer: "label"
        });
    });

    it("can encode label en stimulus-id in answer/button-id as 'stimulus-suffix:label-value'", function() {
        const trials = [
            new TrialData("confirmation_trial", "confirmation_screen"),
            new TrialData("trial", "screen", "stimulus_expected", "expected:label")
        ];

        const procedure = createProcedure({}, trials, createApi(trials));
        procedure.processResult({answerElement: "unexpected:another-label"});
        const firstconfirmationTrial = procedure.setupNextTrial();

        expect(firstconfirmationTrial.screen).to.equal("confirmation_screen");
        expect(firstconfirmationTrial.stimulus).to.equal("dummystimulus");
        expect(firstconfirmationTrial.stimulusParams).to.deep.equal({
            correct_answer: "label",
            current_answer: "another-label"
        });

        procedure.processResult({answerElement: "again_button"});
        const secondConfirmationTrial = procedure.setupNextTrial();

        expect(secondConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(secondConfirmationTrial.stimulus).to.equal("stimulus_expected");

        procedure.processResult({answerElement: "ownanswer_button"});
        const thirdConfirmationTrial = procedure.setupNextTrial();

        expect(thirdConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(thirdConfirmationTrial.stimulus).to.equal("stimulus_unexpected");
    });

    it("should remove leading underscore in label", function() {
        const trials = [
            new TrialData("confirmation_trial", "confirmation_screen"),
            new TrialData("trial", "screen", "stimulus__expected", "_expected")
        ];

        const procedure = createProcedure({}, trials, createApi(trials));
        procedure.processResult({answerElement: "_unexpected"});
        const actualConfirmationTrial = procedure.setupNextTrial();

        expect(actualConfirmationTrial.screen).to.equal("confirmation_screen");
        expect(actualConfirmationTrial.stimulusParams).to.deep.equal({
            correct_answer: "expected",
            current_answer: "unexpected"
        });
    });

    it("should present first-trial first", function() {
        const firstTrials = [];

        for (let i = 0; i < 20; i++) {
            const procedure = createProcedure({order: "random", presentations: 2, firsttrial: "trial-2"}, trials);
            firstTrials.push(procedure.setupNextTrial().screen);
        }

        expect(firstTrials).to.include("screen-2");
        expect(firstTrials).to.not.include("screen-1");
    });

    it("should present first-trial only once", function() {
        const procedure = createProcedure({order: "random", presentations: 2, firsttrial: "trial-2"}, trials);

        expect(procedure.setupNextTrial().screen).to.equal("screen-2");

        procedure.processResult({answerElement: "unexpected"});
        procedure.processResult({answerElement: "next_button"});

        procedure.processResult({answerElement: "unexpected"});
        procedure.processResult({answerElement: "next_button"});

        procedure.processResult({answerElement: "unexpected"});
        procedure.processResult({answerElement: "next_button"});

        expect(procedure.setupNextTrial().id).to.be.undefined;
    });

    function createProcedure(params, trials, api) {
        return proc.createProcedure(api ? api : createApi(trials), misc, params, trials);
    }

    class TrialData {
        constructor(id, screen, stimulus, expectedAnswer) {
            this.id = id;
            this.screen = screen;
            this.stimulus = stimulus;
            this.expectedAswer = expectedAnswer;
        }

        GetID() {
            return this.id;
        }

        GetScreen() {
            return this.screen;
        }

        GetStimulus() {
            return this.stimulus;
        }

        GetAnswer() {
            return this.expectedAswer;
        }
    }

    const trials = [
        new TrialData("confirmation_trial", "confirmation_screen"),
        new TrialData("trial-1", "screen-1", "stimulus_expected-1", "expected-1"),
        new TrialData("trial-2", "screen-2", "stimulus_expected-2", "expected-2")
    ];

    const createApi = trials => ({
        answerElement: () => "answerElement",
        stimuli: () => _.chain(trials).map("stimulus").compact().value().concat(["stimulus_unexpected"]),
        registerParameter: sinon.spy()
    });

    afterEach(function() {
        sinon.restore();
    });

    const misc = {
        shuffleArray: (myArray) => {
            let i = myArray.length,
                j, tempi, tempj;

            if (!i) {
                return false;
            }

            while (--i) {
                j = Math.floor(Math.random() * (i + 1));
                tempi = myArray[i];
                tempj = myArray[j];
                myArray[i] = tempj;
                myArray[j] = tempi;
            }
        }
    };

    class Trial {
        setId(id) {
            this.id = id;
        }

        addScreen(screen) {
            this.screen = screen;
        }

        addStimulus(stimulus, params) {
            this.stimulus = stimulus;
            this.stimulusParams = params;
        }
    }

    global.Trial = Trial;

    class ResultHighlight {}

    global.ResultHighlight = ResultHighlight;

    class ScriptProcedureInterface {}

    global.ScriptProcedureInterface = ScriptProcedureInterface;
});
