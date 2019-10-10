/******************************************************************************
 * Copyright (C) 2009  Job Noorman <jobnoorman@gmail.com>                     *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexdata/calibration/calibrationdata.h"

#include "apexdata/connection/connectiondata.h"

#include "apexdata/datablock/datablocksdata.h"

#include "apexdata/device/devicesdata.h"

#include "apexdata/experimentdata.h"

#include "apexdata/filter/filtersdata.h"

#include "apexdata/interactive/parameterdialogresults.h"

#include "apexdata/mainconfigfiledata.h"

#include "apexdata/parameters/generalparameters.h"
#include "apexdata/parameters/parametermanagerdata.h"

#include "apexdata/procedure/adaptiveproceduredata.h"
#include "apexdata/procedure/constantproceduredata.h"
#include "apexdata/procedure/multiproceduredata.h"
#include "apexdata/procedure/proceduredata.h"
#include "apexdata/procedure/procedureinterface.h"
#include "apexdata/procedure/scriptproceduredata.h"
#include "apexdata/procedure/trainingproceduredata.h"

#include "apexdata/result/resultparameters.h"

#include "apexdata/screen/buttonelement.h"
#include "apexdata/screen/screen.h"
#include "apexdata/screen/screenelement.h"
#include "apexdata/screen/screensdata.h"

#include "apexmain/corrector/equalcorrector.h"

#include "apexmain/experimentparser.h"

#include "apexmain/procedure/procedureapiimplementation.h"

#include "apexmain/runner/experimentrundelegate.h"

#include "apextools/apexpaths.h"
#include "apextools/apexpluginloader.h"
#include "apextools/exceptions.h"
#include "apextools/version.h"

#include "apextools/xml/xmltools.h"

#include "common/debug.h"
#include "common/paths.h"
#include "common/testutils.h"

#include "procedurestest.h"

using namespace apex;
using namespace apex::data;
using namespace cmn;

void ProceduresTest::initTestCase()
{
    enableCoreDumps(QCoreApplication::applicationFilePath());
}

QString ProceduresTest::name() const
{
    return "procedures";
}

struct ProcedureAttributes {
    ProcedureAttributes(const QString &type = "", const QString &id = "")
        : id(id), type(type)
    {
    }
    const QString id;
    const QString type;
} constantProcedureAttributes("apex:constantProcedure", "1"),
    trainingProcedureAttributes("apex:trainingProcedure", "2"),
    adaptiveProcedureAttributes("apex:adaptiveProcedure");

void ProceduresTest::testProcedureApi()
{
    TEST_EXCEPTIONS_TRY

    MainConfigFileData mcd;

    QScopedPointer<ExperimentData> d(makeDummyExperimentData());
    QString defaultAnswerElement("dae");
    d->screensData()->getScreens()["screen"] =
        new data::Screen("screen", new ButtonElement("button", 0),
                         ScreenElementMap(), 0, defaultAnswerElement);

    QScopedPointer<ExperimentRunDelegate> rd(
        new ExperimentRunDelegate(*d, mcd, 0));
    ProcedureApiImplementation api(*rd);

    data::ConstantProcedureData *params = new ConstantProcedureData();
    params->setPresentations(2);
    params->setSkip(2);
    params->setNumberOfChoices(1);
    params->setOrder(ProcedureData::SequentialOrder);

    data::TrialData *trial = new TrialData();
    trial->SetID("trial1");
    trial->AddStimulus("stimulus1_1");
    trial->AddStimulus("stimulus1_2");
    trial->AddStandard("standard1_1");
    trial->SetScreen("screen");
    params->AddTrial(trial);

    trial = new TrialData();
    trial->SetID("trial2");
    trial->AddStimulus("stimulus2_1");
    trial->AddStimulus("stimulus2_2");
    trial->AddStandard("standard2_1");
    trial->SetScreen("screen");
    trial->SetAnswerElement("trial2_answerelement");
    params->AddTrial(trial);

    // Single interval
    {
        QStringList r1(api.makeTrialList(params, 1));
        QStringList r1_target;
        r1_target << "trial1"
                  << "trial2"
                  << "trial1"
                  << "trial2"
                  << "trial1"
                  << "trial2";
        QCOMPARE(r1, r1_target);

        QStringList standardList;
        standardList << "standard1"
                     << "standard2"
                     << "standard3";
        QStringList r2(api.makeStandardList(params, standardList));
        QStringList r2_target;
        QCOMPARE(r2.size(), 0);

        QStringList r3(api.makeOutputList(params, "stimulus", standardList, 0));
        QStringList r3_target;
        r3_target << "stimulus";
        QCOMPARE(r3, r3_target);
    }

    // 3 intervals
    params->setNumberOfChoices(3);
    params->setUniqueStandard(true);

    {
        QStringList standardList;
        standardList << "standard1"
                     << "standard2";
        QStringList r2(api.makeStandardList(params, standardList));
        // qCDebug(APEX_RS) <<r2;
        QStringList r2_target;
        r2.sort();
        QCOMPARE(r2, standardList);

        QStringList r3(api.makeOutputList(params, "stimulus", standardList, 0));
        QStringList r3_target;
        r3_target << "stimulus"
                  << "standard1"
                  << "standard2";
        r3.sort();
        r3_target.sort();
        QCOMPARE(r3, r3_target);
    }

    {
        // test default answer element
        QString answerElement(api.answerElement("trial1", params));
        QCOMPARE(answerElement, defaultAnswerElement);
    }

    {
        // test trial answer element
        QString answerElement(api.answerElement("trial2", params));
        QCOMPARE(answerElement, QString("trial2_answerelement"));
    }

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::testConstantProcedureParser()
{
    TEST_EXCEPTIONS_TRY

    MainConfigFileData mcd;

    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(
            QSL("apex:constantProcedure"));
    QScopedPointer<ProcedureParserInterface> parser(
        creator->createProcedureParser(QSL("apex:constantProcedure")));

    QVERIFY(parser.data());

    ExperimentParser expParser(Paths::searchFile(
        QL1S("tests/libwriters/idn1.apx"), Paths::dataDirectories()));
    QDomDocument doc = expParser.loadAndUpgradeDom();

    QScopedPointer<data::ProcedureData> data(parser->parse(
        doc.documentElement().firstChildElement(QSL("procedure"))));

    QCOMPARE(data->GetTrials().size(), 6);

    QCOMPARE(data->skip(), 0);

    QScopedPointer<ExperimentRunDelegate> rd(
        makeDummyExperimentRunDelegate(mcd));

    QScopedPointer<ProcedureApiImplementation> api(
        new ProcedureApiImplementation(*rd));

    QScopedPointer<ProcedureInterface> proc(creator->createProcedure(
        "apex:constantProcedure", api.data(), data.data()));

    TEST_EXCEPTIONS_CATCH
}

ListIt ProceduresTest::indexOf(const QString &text, QList<QStringList> &list)
{
    ListIt it = list.begin();
    while (it != list.end() && !it->contains(text.trimmed())) {
        ++it;
    }
    return it;
}

void ProceduresTest::checkResultXml(QString xml, QSet<QString> elements,
                                    QString type, QString id,
                                    QList<QStringList> alternatives)
{
    QStringList xmlLines = xml.split("\n");

    QString procedureTag = "procedure";
    if (!type.isEmpty())
        procedureTag += " type=\"" + type + "\"";
    if (!id.isEmpty())
        procedureTag += " id=\"" + id + "\"";

    QCOMPARE(xmlLines.front(), QString("<" + procedureTag + ">"));
    QCOMPARE(xmlLines.back(), QString("</procedure>"));

    xmlLines.erase(xmlLines.begin());
    xmlLines.erase(xmlLines.end() - 1);

    foreach (QString line, xmlLines) {
        if (!elements.remove(line.trimmed())) {
            ListIt it2 = indexOf(line, alternatives);

            if (it2 == alternatives.constEnd()) {
                QString message = "Unexpected line " + line;
                QFAIL(message.toStdString().c_str());
            } else {
                alternatives.removeAll(*it2);
            }
        }
    }

    QVERIFY(elements.isEmpty());
    QVERIFY(alternatives.isEmpty());
}

void ProceduresTest::testConstantProcedure()
{
    TEST_EXCEPTIONS_TRY

    MainConfigFileData mcd;
    QScopedPointer<ExperimentRunDelegate> rd(
        makeDummyExperimentRunDelegate(mcd));

    data::ConstantProcedureData *params = new ConstantProcedureData();
    params->setPresentations(2);
    params->setSkip(2);
    params->setNumberOfChoices(1);
    params->setOrder(ProcedureData::SequentialOrder);

    QString ae("button1"); // answerelement

    data::TrialData *trial = new TrialData();
    trial->SetID("trial1");
    trial->AddStimulus("stimulus1");
    //    trial->AddStimulus("stimulus1_2");
    trial->AddStandard("standard1_1");
    trial->SetAnswerElement(ae);
    trial->SetAnswer("answer1");
    params->AddTrial(trial);

    trial = new TrialData();
    trial->SetID("trial2");
    trial->AddStimulus("stimulus2");
    //    trial->AddStimulus("stimulus2_2");
    trial->AddStandard("standard2_1");
    trial->SetAnswerElement(ae);
    trial->SetAnswer("answer2");
    params->AddTrial(trial);

    QScopedPointer<ProcedureApiImplementation> api(
        new ProcedureApiImplementation(*rd));

    QStringList expectedStimuli;

    expectedStimuli << "stimulus1"
                    << "stimulus2"
                    << "stimulus1"
                    << "stimulus2"
                    << "stimulus1"
                    << "stimulus2";

    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(
            QSL("apex:constantProcedure"));

    // Test correct answers
    {
        QScopedPointer<ProcedureInterface> procedure(creator->createProcedure(
            QSL("apex:constantProcedure"), api.data(), params));

        QVERIFY(procedure.data() != 0);
        int count;

        for (count = 0;; ++count) {
            Trial newTrial = procedure->setupNextTrial();

            qCDebug(APEX_RS) << "New trial " << count << ": " << newTrial.id()
                             << " -> " << newTrial.isValid();

            if (newTrial.screenCount() == 0)
                break;

            QVERIFY(count < expectedStimuli.size());
            QCOMPARE(newTrial.stimulus(0, 0), expectedStimuli.at(count));
            QCOMPARE(procedure->progress(),
                     (double)100 * (count + 1) / expectedStimuli.size());

            ScreenResult sr;

            if (count % 2 == 0)
                sr[ae] = "answer1";
            else
                sr[ae] = "answer2";

            bool result = procedure->processResult(&sr).correct;

            QVERIFY(result);
            QString xml(procedure->resultXml());

            QSet<QString> elements;
            elements << "<correct>true</correct>";

            if (count % 2 == 0) {
                elements << "<correct_answer>answer1</correct_answer>";
                elements << "<answer>answer1</answer>";
                elements << "<stimulus>stimulus1</stimulus>";
            } else {
                elements << "<correct_answer>answer2</correct_answer>";
                elements << "<answer>answer2</answer>";
                elements << "<stimulus>stimulus2</stimulus>";
            }

            if (count < 2)
                elements << "<skip/>";

            checkResultXml(xml, elements, constantProcedureAttributes.type);
        }

        QCOMPARE(count, expectedStimuli.size());
    }

    // Test incorrect answers
    {
        QScopedPointer<ProcedureInterface> procedure(creator->createProcedure(
            QSL("apex:constantProcedure"), api.data(), params));
        QVERIFY(procedure.data());
        int count;

        for (count = 0;; ++count) {
            Trial newTrial = procedure->setupNextTrial();

            if (newTrial.screenCount() == 0)
                break;

            QVERIFY(count < expectedStimuli.size());
            QCOMPARE(newTrial.stimulus(0, 0), expectedStimuli.at(count));
            QCOMPARE(procedure->progress(),
                     (double)100 * (count + 1) / expectedStimuli.size());

            ScreenResult sr;

            if (count % 2 != 0)
                sr[ae] = "answer1";
            else
                sr[ae] = "answer2";

            bool result = procedure->processResult(&sr).correct;

            QVERIFY(!result);

            QString xml(procedure->resultXml());

            QSet<QString> elements;
            elements << "<correct>false</correct>";

            if (count % 2 == 0) {
                elements << "<correct_answer>answer1</correct_answer>";
                elements << "<answer>answer2</answer>";
                elements << "<stimulus>stimulus1</stimulus>";
            } else {
                elements << "<correct_answer>answer2</correct_answer>";
                elements << "<answer>answer1</answer>";
                elements << "<stimulus>stimulus2</stimulus>";
            }

            if (count < 2)
                elements << "<skip/>";

            checkResultXml(xml, elements, constantProcedureAttributes.type);
        }

        QCOMPARE(count, expectedStimuli.size());
    }

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::testScriptProcedureInvalidTrial()
{
    TEST_EXCEPTIONS_TRY

    MainConfigFileData mcd;
    QScopedPointer<ExperimentRunDelegate> rd(
        makeDummyExperimentRunDelegate(mcd));

    data::ScriptProcedureData *params = new ScriptProcedureData();
    params->setScript("testprocedureinvalidtrial.js");
    params->setDebugger(false);
    params->setPresentations(1);
    params->setSkip(0);
    params->setNumberOfChoices(1);
    params->setOrder(ProcedureData::SequentialOrder);

    for (int i = 1; i <= 6; ++i) {
        data::TrialData *trial = new TrialData();
        trial->SetID(QString("trial%1").arg(i));
        trial->AddStimulus(QString("stimulus%1").arg(i));
        trial->SetAnswer(QString("button%1").arg(i));
        params->AddTrial(trial);
    }

    QScopedPointer<ProcedureApiImplementation> api(
        new ProcedureApiImplementation(*rd));
    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(
            QSL("apex:pluginProcedure"));

    // Test if the procedure throws an error when setupNextTrial is executed
    {
        QScopedPointer<ProcedureInterface> procedure(creator->createProcedure(
            QSL("apex:pluginProcedure"), api.data(), params));
        QVERIFY(procedure.data());

        try {
            procedure->setupNextTrial();
            QFAIL("Expected an exception");
        } catch (const std::exception &) {
            // expected
        }
    }

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::testScriptProcedure()
{
    TEST_EXCEPTIONS_TRY

    qCDebug(APEX_RS) << "Testing plugin procedure";
    MainConfigFileData mcd;
    QScopedPointer<ExperimentRunDelegate> rd(
        makeDummyExperimentRunDelegate(mcd));

    data::ScriptProcedureData *params = new ScriptProcedureData();
    params->setScript("testprocedure.js");
    params->setDebugger(false);
    params->setPresentations(1);
    params->setSkip(0);
    params->setNumberOfChoices(1);
    params->setOrder(ProcedureData::SequentialOrder);

    QString ae("button1"); // answerelement

    for (int i = 1; i <= 6; ++i) {
        data::TrialData *trial = new TrialData();
        trial->SetID(QString("trial%1").arg(i));
        trial->AddStimulus(QString("stimulus%1").arg(i));
        trial->SetAnswer(QString("button%1").arg(i));
        params->AddTrial(trial);
    }

    QScopedPointer<ProcedureApiImplementation> api(
        new ProcedureApiImplementation(*rd));

    QStringList expectedStimuli;

    expectedStimuli << "stimulus1"
                    << "stimulus2"
                    << "stimulus3"
                    << "stimulus4"
                    << "stimulus5"
                    << "stimulus6";

    qCDebug(APEX_RS) << "Test correct anwers";
    // Test correct answers
    {
        ProcedureCreatorInterface *creator =
            createPluginCreator<ProcedureCreatorInterface>(
                QSL("apex:pluginProcedure"));
        QScopedPointer<ProcedureInterface> procedure(creator->createProcedure(
            QSL("apex:pluginProcedure"), api.data(), params));
        QVERIFY(procedure.data());
        int count;

        for (count = 0;; ++count) {
            Trial newTrial = procedure->setupNextTrial();
            if (newTrial.screenCount() == 0)
                break;

            QVERIFY(count < expectedStimuli.size());
            QCOMPARE(newTrial.stimulus(0, 0), expectedStimuli.at(count));
            QCOMPARE(procedure->progress(),
                     (double)100 * (count) / expectedStimuli.size());

            ScreenResult sr;
            sr["buttongroup1"] = "button" + QString::number(count + 1);
            bool result = procedure->processResult(&sr).correct;

            QVERIFY(result);
            QString xml(procedure->resultXml());

            QSet<QString> elements;

            elements << "<correct_answer>" + sr.value("buttongroup1") +
                            "</correct_answer>";
            elements << "<correct>true</correct>";
            elements << "<answer>" + sr.value("buttongroup1") + "</answer>";

            elements << "Result from testprocedure";
            checkResultXml(xml, elements);
        }

        QCOMPARE(count, expectedStimuli.size());
    }

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::testDummyProcedure()
{
    TEST_EXCEPTIONS_TRY

    MainConfigFileData mcd;
    QScopedPointer<ExperimentRunDelegate> rd(
        makeDummyExperimentRunDelegate(mcd));

    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(QSL("dummyprocedure"));
    QScopedPointer<ProcedureParserInterface> parser(
        creator->createProcedureParser(QSL("dummyprocedure")));

    QScopedPointer<data::ProcedureData> data(parser->parse(QDomElement()));

    QScopedPointer<ProcedureApiImplementation> api(
        new ProcedureApiImplementation(*rd));

    QScopedPointer<ProcedureInterface> procedure(creator->createProcedure(
        QSL("dummyprocedure"), api.data(), data.data()));

    Trial trial = procedure->setupNextTrial();

    QCOMPARE(trial.screenCount(), 2);

    QCOMPARE(trial.screen(0), QString("screen1"));

    QCOMPARE(trial.screen(1), QString("screen2"));

    QCOMPARE(trial.stimulus(0, 0), QString("stimulus"));

    QCOMPARE(trial.stimulus(1, 0), QString("stimulus1"));

    QCOMPARE(trial.stimulusCount(1), 2);

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::testTrainingProcedure()
{
    TEST_EXCEPTIONS_TRY

    MainConfigFileData mcd;
    QScopedPointer<ExperimentRunDelegate> rd(
        makeDummyExperimentRunDelegate(mcd));

    TrainingProcedureData *params = new TrainingProcedureData();
    params->setPresentations(3);
    params->setSkip(0);
    params->setNumberOfChoices(1);
    params->setOrder(ProcedureData::SequentialOrder);

    QString answerElement("button1");

    data::TrialData *trialData = new TrialData();
    trialData->SetID("trial1");

    QStringList trial1Stimuli;
    trial1Stimuli << "stimulus1_1"
                  << "stimulus1_2";
    Q_FOREACH (QString stimulus, trial1Stimuli)
        trialData->AddStimulus(stimulus);

    trialData->SetAnswerElement(answerElement);
    trialData->SetAnswer("answer1");
    params->AddTrial(trialData);

    trialData = new TrialData();
    trialData->SetID("trial2");
    trialData->AddStimulus("stimulus2");
    trialData->SetAnswerElement(answerElement);
    trialData->SetAnswer("answer2");
    params->AddTrial(trialData);

    trialData = new TrialData();
    trialData->SetID("trial3");
    trialData->AddStimulus("stimulus3");
    trialData->SetAnswerElement(answerElement);
    trialData->SetAnswer("answer3");
    params->AddTrial(trialData);

    QScopedPointer<ProcedureApiImplementation> api(
        new ProcedureApiImplementation(*rd));
    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(
            QSL("apex:trainingProcedure"));
    QScopedPointer<ProcedureInterface> procedure(creator->createProcedure(
        QSL("apex:trainingProcedure"), api.data(), params));

    // generate first trial. there should not be any stimuli
    data::Trial trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().first()->GetID());
    QCOMPARE(trial.stimulusCount(0), 0);

    // generate answers and check if the correct trial is played next
    ScreenResult result;
    result[answerElement] = "answer3";
    procedure->processResult(&result);

    QString xml = procedure->resultXml();
    QVERIFY(xml.isEmpty());

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), QString("trial3"));
    QCOMPARE(trial.stimulusCount(0), 1);
    QCOMPARE(trial.stimulus(0, 0), QString("stimulus3"));

    result[answerElement] = "answer1";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    QSet<QString> elements;
    elements << "<stimulus>stimulus3</stimulus>";
    elements << "<correct>true</correct>";
    elements << "<answer>answer3</answer>";
    elements << "<correct_answer>answer3</correct_answer>";
    checkResultXml(xml, elements, trainingProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), QString("trial1"));
    QCOMPARE(trial.stimulusCount(0), 1);
    QVERIFY(trial1Stimuli.contains(trial.stimulus(0, 0)));

    result[answerElement] = "answer2";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<answer>answer1</answer>";
    elements << "<correct_answer>answer1</correct_answer>";

    QList<QStringList> alternatives;
    QStringList stimulusAlternatives;
    foreach (QString s, trial1Stimuli) {
        stimulusAlternatives.append("<stimulus>" + s + "</stimulus>");
    }
    alternatives.append(stimulusAlternatives);
    checkResultXml(xml, elements, trainingProcedureAttributes.type, "",
                   alternatives);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), QString("trial2"));
    QCOMPARE(trial.stimulusCount(0), 1);
    QCOMPARE(trial.stimulus(0, 0), QString("stimulus2"));

    result[answerElement] = "answer2"; // last answer
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus2</stimulus>";
    elements << "<correct>true</correct>";
    elements << "<answer>answer2</answer>";
    elements << "<correct_answer>answer2</correct_answer>";

    checkResultXml(xml, elements, trainingProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(!trial.acceptResponse(0)); // no response on last trial
    QCOMPARE(trial.id(), QString("trial2"));
    QCOMPARE(trial.stimulusCount(0), 0);
    QVERIFY(procedure->resultXml().isEmpty());

    // procedure should be done now (presentations == 3)
    QCOMPARE(procedure->progress(), 100.0);
    QVERIFY(!procedure->setupNextTrial().isValid());

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::testKaernbachProcedure()
{
    TEST_EXCEPTIONS_TRY

    MainConfigFileData mcd;
    QScopedPointer<ExperimentRunDelegate> rd(
        makeStimulusExperimentRunDelegate(mcd));

    AdaptiveProcedureData *params = new AdaptiveProcedureData();
    params->setPresentations(6);
    params->setOrder(ProcedureData::SequentialOrder);
    params->setNUp(1);
    params->setNDown(1);
    params->addAdaptingParameter("snr");
    params->setStopAfter(6);
    params->setStartValue(4);
    params->setMinValue(-100);
    params->setMaxValue(100);
    params->addUpStepsize(0, 3);
    params->addDownStepsize(0, 3);
    params->addUpStepsize(2, 10);
    params->addDownStepsize(2, 4);
    params->setLargerIsEasier(true);

    QString answerElement("button1");

    data::TrialData *trialData = new TrialData();
    trialData->SetID("trial1");
    trialData->SetAnswer("answer1");
    trialData->SetAnswerElement(answerElement);
    trialData->AddStimulus("stimulus1");
    params->AddTrial(trialData);

    QScopedPointer<ProcedureApiImplementation> api(
        new ProcedureApiImplementation(*rd));
    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(
            QSL("apex:adaptiveProcedure"));
    QScopedPointer<ProcedureInterface> procedure(creator->createProcedure(
        QSL("apex:adaptiveProcedure"), api.data(), params));

    data::Trial trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().first()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);
    //--------------------------0-------------------------------
    ScreenResult result;
    result[answerElement] = "answer1";
    procedure->processResult(&result);

    QString xml = procedure->resultXml();
    QSet<QString> elements;
    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<answer>answer1</answer>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<correct>true</correct>";

    elements << "<parameter>4</parameter>";
    elements << "<stepsize>3</stepsize>";
    elements << "<reversals>0</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>0</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().last()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);

    //--------------------------1-------------------------------
    result[answerElement] = "answer1";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<answer>answer1</answer>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<correct>true</correct>";

    elements << "<parameter>1</parameter>";
    elements << "<stepsize>3</stepsize>";
    elements << "<reversals>0</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>1</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().first()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);

    //--------------------------2-------------------------------
    result[answerElement] = "answer1";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<answer>answer1</answer>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<correct>true</correct>";

    elements << "<parameter>-3</parameter>";
    elements << "<stepsize>4</stepsize>";
    elements << "<reversals>0</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>2</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().last()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);

    //--------------------------3-------------------------------
    result[answerElement] = "answer2";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<answer>answer2</answer>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<correct>false</correct>";

    elements << "<parameter>-7</parameter>";
    elements << "<stepsize>4</stepsize>";
    elements << "<reversal/>";
    elements << "<reversals>1</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>3</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().first()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);

    //--------------------------4-------------------------------
    result[answerElement] = "answer1";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<answer>answer1</answer>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<correct>true</correct>";

    elements << "<parameter>3</parameter>";
    elements << "<stepsize>10</stepsize>";
    elements << "<reversal/>";
    elements << "<reversals>2</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>4</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().last()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);

    //--------------------------5-------------------------------
    result[answerElement] = "answer2";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<answer>answer2</answer>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<correct>false</correct>";

    elements << "<parameter>-1</parameter>";
    elements << "<stepsize>4</stepsize>";
    elements << "<reversal/>";
    elements << "<reversals>3</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>5</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(!trial.isValid());

    //--------------------------6-------------------------------
    xml = procedure->finalResultXml();

    QStringList lines = xml.split("\n");

    QCOMPARE(lines.first().toStdString().c_str(),
             "<trial id=\"VIRTUAL_TRIAL\" type=\"virtual\">");
    lines.removeFirst();
    lines.removeFirst();

    QCOMPARE(lines.last().toStdString().c_str(), "</trial>");
    lines.removeLast();

    elements.clear();

    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<parameter>9</parameter>";
    elements << "<stepsize>10</stepsize>";
    elements << "<reversals>3</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>6</presentations>";

    checkResultXml(lines.join("\n"), elements,
                   adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(!trial.isValid());

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::testAdaptiveProcedure()
{
    TEST_EXCEPTIONS_TRY

    MainConfigFileData mcd;
    QScopedPointer<ExperimentRunDelegate> rd(
        makeStimulusExperimentRunDelegate(mcd));

    AdaptiveProcedureData *params = new AdaptiveProcedureData();
    params->setPresentations(3);
    params->setOrder(ProcedureData::SequentialOrder);
    params->setNUp(2);
    params->setNDown(2);
    params->addAdaptingParameter("snr");
    params->setStopAfter(3);
    params->setStartValue(4);
    params->setMinValue(0);
    params->setMaxValue(10);
    params->addUpStepsize(0, 3);
    params->addDownStepsize(0, 3);
    params->addUpStepsize(2, 5);
    params->addDownStepsize(2, 5);

    QString answerElement("button1");

    data::TrialData *trialData = new TrialData();
    trialData->SetID("trial1");
    trialData->SetAnswer("answer1");
    trialData->SetAnswerElement(answerElement);
    trialData->AddStimulus("stimulus1");
    params->AddTrial(trialData);

    trialData = new TrialData();
    trialData->SetID("trial2");
    trialData->SetAnswer("answer2");
    trialData->SetAnswerElement(answerElement);
    trialData->AddStimulus("stimulus2");
    params->AddTrial(trialData);

    QScopedPointer<ProcedureApiImplementation> api(
        new ProcedureApiImplementation(*rd));
    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(
            QSL("apex:adaptiveProcedure"));
    QScopedPointer<ProcedureInterface> procedure(creator->createProcedure(
        QSL("apex:adaptiveProcedure"), api.data(), params));

    data::Trial trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().first()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);
    //--------------------------0-------------------------------
    ScreenResult result;
    result[answerElement] = "answer1";
    procedure->processResult(&result);

    QString xml = procedure->resultXml();
    QSet<QString> elements;
    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<answer>answer1</answer>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<correct>true</correct>";

    elements << "<parameter>4</parameter>";
    elements << "<stepsize>3</stepsize>";
    elements << "<reversals>0</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>0</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().last()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);

    //--------------------------1-------------------------------
    result[answerElement] = "answer2";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus2</stimulus>";
    elements << "<answer>answer2</answer>";
    elements << "<correct_answer>answer2</correct_answer>";
    elements << "<correct>true</correct>";

    elements << "<parameter>4</parameter>";
    elements << "<stepsize>3</stepsize>";
    elements << "<reversals>0</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>1</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().first()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);

    //--------------------------2-------------------------------
    result[answerElement] = "answer2";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<answer>answer2</answer>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<correct>false</correct>";

    elements << "<parameter>0</parameter>";
    elements << "<stepsize>5</stepsize>";
    elements << "<reversals>0</reversals>";
    elements << "<saturation>true</saturation>";
    elements << "<presentations>2</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().last()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);

    //--------------------------3-------------------------------
    result[answerElement] = "answer1";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus2</stimulus>";
    elements << "<answer>answer1</answer>";
    elements << "<correct_answer>answer2</correct_answer>";
    elements << "<correct>false</correct>";

    elements << "<parameter>0</parameter>";
    elements << "<stepsize>5</stepsize>";
    elements << "<reversal/>";
    elements << "<reversals>1</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>3</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().first()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);

    //--------------------------4-------------------------------
    result[answerElement] = "answer1";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<answer>answer1</answer>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<correct>true</correct>";

    elements << "<parameter>5</parameter>";
    elements << "<stepsize>5</stepsize>";
    elements << "<reversals>1</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>4</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), params->GetTrials().last()->GetID());
    QCOMPARE(trial.stimulusCount(0), 1);

    //--------------------------5-------------------------------
    result[answerElement] = "answer1";
    procedure->processResult(&result);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus2</stimulus>";
    elements << "<answer>answer1</answer>";
    elements << "<correct_answer>answer2</correct_answer>";
    elements << "<correct>false</correct>";

    elements << "<parameter>5</parameter>";
    elements << "<stepsize>5</stepsize>";
    elements << "<reversals>1</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>5</presentations>";

    checkResultXml(xml, elements, adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(!trial.isValid());

    //--------------------------6-------------------------------
    xml = procedure->finalResultXml();

    QStringList lines = xml.split("\n");

    QCOMPARE(lines.first().toStdString().c_str(),
             "<trial id=\"VIRTUAL_TRIAL\" type=\"virtual\">");
    lines.removeFirst();
    lines.removeFirst();

    QCOMPARE(lines.last().toStdString().c_str(), "</trial>");
    lines.removeLast();

    elements.clear();

    elements << "<stimulus>stimulus2</stimulus>";
    elements << "<parameter>5</parameter>";
    elements << "<stepsize>5</stepsize>";
    elements << "<reversals>1</reversals>";
    elements << "<saturation>false</saturation>";
    elements << "<presentations>6</presentations>";

    checkResultXml(lines.join("\n"), elements,
                   adaptiveProcedureAttributes.type);

    trial = procedure->setupNextTrial();
    QVERIFY(!trial.isValid());

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::createConstantProcedure(ConstantProcedureData *&data,
                                             QStringList &expectedStimuli)
{
    data = new ConstantProcedureData();
    data->setPresentations(2);
    data->setSkip(2);
    data->setNumberOfChoices(1);
    data->setOrder(ProcedureData::SequentialOrder);

    QString ae1("button1"); // answerelement

    data::TrialData *trial = new TrialData();
    trial->SetID("trial1");
    trial->AddStimulus("stimulus1");
    trial->AddStandard("standard1_1");
    trial->SetAnswerElement(ae1);
    trial->SetAnswer("answer1");
    data->AddTrial(trial);

    trial = new TrialData();
    trial->SetID("trial2");
    trial->AddStimulus("stimulus2");
    trial->AddStandard("standard2_1");
    trial->SetAnswerElement(ae1);
    trial->SetAnswer("answer2");
    data->AddTrial(trial);

    data->SetID(constantProcedureAttributes.id);

    expectedStimuli << "stimulus1"
                    << "stimulus2"
                    << "stimulus1"
                    << "stimulus2"
                    << "stimulus1"
                    << "stimulus2";
}

void ProceduresTest::createTrainingProcedure(TrainingProcedureData *&data,
                                             QString &answerElement,
                                             QStringList &trial1Stimuli)
{
    data = new TrainingProcedureData();
    data->setPresentations(3);
    data->setSkip(0);
    data->setNumberOfChoices(1);
    data->setOrder(ProcedureData::SequentialOrder);

    answerElement = "button1";

    data::TrialData *trialData = new TrialData();
    trialData->SetID("trial1");

    trial1Stimuli << "stimulus1_1"
                  << "stimulus1_2";
    Q_FOREACH (QString stimulus, trial1Stimuli)
        trialData->AddStimulus(stimulus);

    trialData->SetAnswerElement(answerElement);
    trialData->SetAnswer("answer1");
    data->AddTrial(trialData);

    trialData = new TrialData();
    trialData->SetID("trial2");
    trialData->AddStimulus("stimulus2");
    trialData->SetAnswerElement(answerElement);
    trialData->SetAnswer("answer2");
    data->AddTrial(trialData);

    trialData = new TrialData();
    trialData->SetID("trial3");
    trialData->AddStimulus("stimulus3");
    trialData->SetAnswerElement(answerElement);
    trialData->SetAnswer("answer3");
    data->AddTrial(trialData);

    data->SetID(trainingProcedureAttributes.id);
}

void ProceduresTest::testTrial(const data::Trial &trial, const QString id,
                               const int count) const
{
    TEST_EXCEPTIONS_TRY

    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), id);
    QCOMPARE(trial.stimulusCount(0), count);

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::testMultiProcedureOneByOne()
{
    TEST_EXCEPTIONS_TRY

    MainConfigFileData mcd;
    QScopedPointer<ExperimentRunDelegate> rd(
        makeDummyExperimentRunDelegate(mcd));

    MultiProcedureData *params = new MultiProcedureData();
    params->setOrder(ProcedureData::OneByOneOrder);

    ConstantProcedureData *data1 = 0;
    QStringList expectedStimuli;
    createConstantProcedure(data1, expectedStimuli);

    TrainingProcedureData *data2 = 0;
    QString answerElement;
    QStringList trial1Stimuli;
    createTrainingProcedure(data2, answerElement, trial1Stimuli);

    params->addProcedure(data1);
    params->addProcedure(data2);

    QScopedPointer<ProcedureApiImplementation> api(
        new ProcedureApiImplementation(*rd));
    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(
            QSL("apex:multiProcedure"));
    QScopedPointer<ProcedureInterface> procedure(creator->createProcedure(
        QSL("apex:multiProcedure"), api.data(), params));
    QVERIFY(procedure.data());

    QString ae("button1"); // answerelement

    // Procedure 1
    {
        for (int count = 0; count < expectedStimuli.size(); ++count) {
            Trial newTrial = procedure->setupNextTrial();
            qCDebug(APEX_RS) << "New trial " << count << ": " << newTrial.id()
                             << " -> " << newTrial.isValid();

            QCOMPARE(newTrial.stimulus(0, 0), expectedStimuli.at(count));

            ScreenResult sr;

            if (count % 2 == 0) {
                testTrial(newTrial, "trial1", 1);
                sr[ae] = "answer1";
            } else {
                testTrial(newTrial, "trial2", 1);
                sr[ae] = "answer2";
            }

            qCDebug(APEX_RS) << "Before process result";

            bool result = procedure->processResult(&sr).correct;

            QVERIFY(result);

            QString xml(procedure->resultXml());

            QSet<QString> elements;
            elements << "<correct>true</correct>";

            if (count % 2 == 0) {
                elements << "<correct_answer>answer1</correct_answer>";
                elements << "<answer>answer1</answer>";
                elements << "<stimulus>stimulus1</stimulus>";
            } else {
                elements << "<correct_answer>answer2</correct_answer>";
                elements << "<answer>answer2</answer>";
                elements << "<stimulus>stimulus2</stimulus>";
            }

            if (count < 2)
                elements << "<skip/>";

            checkResultXml(xml, elements, constantProcedureAttributes.type,
                           constantProcedureAttributes.id);
        }
    }

    qCDebug(APEX_RS) << "Start procedure 2 ";

    // Procedure 2
    {
        data::Trial trial = procedure->setupNextTrial();
        testTrial(trial, "trial1", 0);

        // generate answers and check if the correct trial is played next
        ScreenResult result;
        result[answerElement] = "answer3";
        procedure->processResult(&result);

        QString xml = procedure->resultXml();
        QVERIFY(xml.isEmpty());

        trial = procedure->setupNextTrial();
        testTrial(trial, "trial3", 1);
        QCOMPARE(trial.stimulus(0, 0), QString("stimulus3"));

        result[answerElement] = "answer1";
        procedure->processResult(&result);

        xml = procedure->resultXml();
        QSet<QString> elements;
        elements << "<stimulus>stimulus3</stimulus>";
        elements << "<correct>true</correct>";
        elements << "<answer>answer3</answer>";
        elements << "<correct_answer>answer3</correct_answer>";
        checkResultXml(xml, elements, trainingProcedureAttributes.type,
                       trainingProcedureAttributes.id);

        trial = procedure->setupNextTrial();
        testTrial(trial, "trial1", 1);
        QVERIFY(trial1Stimuli.contains(trial.stimulus(0, 0)));

        result[answerElement] = "answer2";
        procedure->processResult(&result);

        xml = procedure->resultXml();
        elements.clear();
        elements << "<correct>true</correct>";
        elements << "<answer>answer1</answer>";
        elements << "<correct_answer>answer1</correct_answer>";

        QList<QStringList> alternatives;
        QStringList stimulusAlternatives;
        foreach (QString s, trial1Stimuli) {
            stimulusAlternatives.append("<stimulus>" + s + "</stimulus>");
        }
        alternatives.append(stimulusAlternatives);
        checkResultXml(xml, elements, trainingProcedureAttributes.type,
                       trainingProcedureAttributes.id, alternatives);

        trial = procedure->setupNextTrial();
        QVERIFY(trial.isValid());
        QVERIFY(trial.acceptResponse(0));
        QCOMPARE(trial.id(), QString("trial2"));
        QCOMPARE(trial.stimulusCount(0), 1);
        QCOMPARE(trial.stimulus(0, 0), QString("stimulus2"));

        result[answerElement] = "answer2";
        procedure->processResult(&result);

        xml = procedure->resultXml();
        elements.clear();
        elements << "<stimulus>stimulus2</stimulus>";
        elements << "<correct>true</correct>";
        elements << "<answer>answer2</answer>";
        elements << "<correct_answer>answer2</correct_answer>";
        checkResultXml(xml, elements, trainingProcedureAttributes.type,
                       trainingProcedureAttributes.id);

        trial = procedure->setupNextTrial();
        QVERIFY(trial.isValid());
        QVERIFY(!trial.acceptResponse(0)); // no response on last trial
        QCOMPARE(trial.id(), QString("trial2"));
        QCOMPARE(trial.stimulusCount(0), 0);
        QVERIFY(procedure->resultXml().isEmpty());

        // procedure should be done now (presentations == 3)
        QVERIFY(procedure->progress() >= 100.0);
        QVERIFY(!procedure->setupNextTrial().isValid());
    }

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::testMultiProcedureSequential()
{
    TEST_EXCEPTIONS_TRY

    MainConfigFileData mcd;
    QScopedPointer<ExperimentRunDelegate> rd(
        makeDummyExperimentRunDelegate(mcd));

    MultiProcedureData *params = new MultiProcedureData();
    params->setOrder(ProcedureData::SequentialOrder);

    ConstantProcedureData *data1 = 0;
    QStringList expectedStimuli;
    createConstantProcedure(data1, expectedStimuli);

    TrainingProcedureData *data2 = 0;
    QString answerElement;
    QStringList trial1Stimuli;
    createTrainingProcedure(data2, answerElement, trial1Stimuli);

    params->addProcedure(data1);
    params->addProcedure(data2);

    QScopedPointer<ProcedureApiImplementation> api(
        new ProcedureApiImplementation(*rd));
    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(
            QSL("apex:multiProcedure"));
    QScopedPointer<ProcedureInterface> procedure(creator->createProcedure(
        QSL("apex:multiProcedure"), api.data(), params));
    QVERIFY(procedure.data());

    QString ae("button1"); // answerelement

    // Test
    Trial trial = procedure->setupNextTrial();
    testTrial(trial, "trial1", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(0));
    ScreenResult sr;
    sr[ae] = "answer1";
    bool result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    QString xml(procedure->resultXml());
    QSet<QString> elements;
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<answer>answer1</answer>";
    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<skip/>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial1", 0);
    sr[answerElement] = "answer3";
    procedure->processResult(&sr);

    xml = procedure->resultXml();
    QVERIFY(xml.isEmpty());

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial2", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(1));
    sr[ae] = "answer2";
    result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    xml = (procedure->resultXml());
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer2</correct_answer>";
    elements << "<answer>answer2</answer>";
    elements << "<stimulus>stimulus2</stimulus>";
    elements << "<skip/>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial3", 1);
    sr[answerElement] = "answer1";
    procedure->processResult(&sr);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus3</stimulus>";
    elements << "<correct>true</correct>";
    elements << "<answer>answer3</answer>";
    elements << "<correct_answer>answer3</correct_answer>";
    checkResultXml(xml, elements, trainingProcedureAttributes.type,
                   trainingProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial1", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(2));
    sr[ae] = "answer1";
    result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    xml = (procedure->resultXml());
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<answer>answer1</answer>";
    elements << "<stimulus>stimulus1</stimulus>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial1", 1);
    sr[answerElement] = "answer2";
    procedure->processResult(&sr);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<answer>answer1</answer>";
    elements << "<correct_answer>answer1</correct_answer>";

    QList<QStringList> alternatives;
    QStringList stimulusAlternatives;
    foreach (QString s, trial1Stimuli) {
        stimulusAlternatives.append("<stimulus>" + s + "</stimulus>");
    }
    alternatives.append(stimulusAlternatives);
    checkResultXml(xml, elements, trainingProcedureAttributes.type,
                   trainingProcedureAttributes.id, alternatives);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial2", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(3));
    sr[ae] = "answer2";
    result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    xml = (procedure->resultXml());
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer2</correct_answer>";
    elements << "<answer>answer2</answer>";
    elements << "<stimulus>stimulus2</stimulus>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), QString("trial2"));
    QCOMPARE(trial.stimulusCount(0), 1);
    QCOMPARE(trial.stimulus(0, 0), QString("stimulus2"));

    sr[ae] = "answer2";
    procedure->processResult(&sr);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus2</stimulus>";
    elements << "<correct>true</correct>";
    elements << "<answer>answer2</answer>";
    elements << "<correct_answer>answer2</correct_answer>";
    checkResultXml(xml, elements, trainingProcedureAttributes.type,
                   trainingProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial1", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(4));
    sr[ae] = "answer1";
    result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    xml = (procedure->resultXml());
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<answer>answer1</answer>";
    elements << "<stimulus>stimulus1</stimulus>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(!trial.acceptResponse(0));
    QCOMPARE(trial.id(), QString("trial2"));
    QCOMPARE(trial.stimulusCount(0), 0);
    QVERIFY(procedure->resultXml().isEmpty());

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial2", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(5));
    sr[ae] = "answer2";
    result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    xml = (procedure->resultXml());
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer2</correct_answer>";
    elements << "<answer>answer2</answer>";
    elements << "<stimulus>stimulus2</stimulus>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    // procedure should be done now
    QVERIFY(procedure->progress() >= 100.0);
    QVERIFY(!procedure->setupNextTrial().isValid());

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::testMultiProcedureRandom()
{
    TEST_EXCEPTIONS_TRY

    cmn::Random::setDeterministic(true);

    MainConfigFileData mcd;
    QScopedPointer<ExperimentRunDelegate> rd(
        makeDummyExperimentRunDelegate(mcd));

    MultiProcedureData *params = new MultiProcedureData();
    params->setOrder(ProcedureData::RandomOrder);

    ConstantProcedureData *data1 = 0;
    QStringList expectedStimuli;
    createConstantProcedure(data1, expectedStimuli);

    TrainingProcedureData *data2 = 0;
    QString answerElement;
    QStringList trial1Stimuli;
    createTrainingProcedure(data2, answerElement, trial1Stimuli);

    params->addProcedure(data1);
    params->addProcedure(data2);

    QScopedPointer<ProcedureApiImplementation> api(
        new ProcedureApiImplementation(*rd));
    ProcedureCreatorInterface *creator =
        createPluginCreator<ProcedureCreatorInterface>(
            QSL("apex:multiProcedure"));
    QScopedPointer<ProcedureInterface> procedure(creator->createProcedure(
        QSL("apex:multiProcedure"), api.data(), params));
    QVERIFY(procedure.data());

    QString ae("button1"); // answerelement

    // Test
    Trial trial = procedure->setupNextTrial();
    testTrial(trial, "trial1", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(0));
    ScreenResult sr;
    sr[ae] = "answer1";
    bool result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    QString xml(procedure->resultXml());
    QSet<QString> elements;
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<answer>answer1</answer>";
    elements << "<stimulus>stimulus1</stimulus>";
    elements << "<skip/>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial2", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(1));
    sr[ae] = "answer2";
    result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    xml = (procedure->resultXml());
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer2</correct_answer>";
    elements << "<answer>answer2</answer>";
    elements << "<stimulus>stimulus2</stimulus>";
    elements << "<skip/>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial1", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(2));
    sr[ae] = "answer1";
    result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    xml = (procedure->resultXml());
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<answer>answer1</answer>";
    elements << "<stimulus>stimulus1</stimulus>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial2", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(3));
    sr[ae] = "answer2";
    result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    xml = (procedure->resultXml());
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer2</correct_answer>";
    elements << "<answer>answer2</answer>";
    elements << "<stimulus>stimulus2</stimulus>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial1", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(4));
    sr[ae] = "answer1";
    result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    xml = (procedure->resultXml());
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer1</correct_answer>";
    elements << "<answer>answer1</answer>";
    elements << "<stimulus>stimulus1</stimulus>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial2", 1);
    QCOMPARE(trial.stimulus(0, 0), expectedStimuli.at(5));
    sr[ae] = "answer2";
    result = procedure->processResult(&sr).correct;
    QVERIFY(result);

    xml = (procedure->resultXml());
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<correct_answer>answer2</correct_answer>";
    elements << "<answer>answer2</answer>";
    elements << "<stimulus>stimulus2</stimulus>";
    checkResultXml(xml, elements, constantProcedureAttributes.type,
                   constantProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial1", 0);
    sr[answerElement] = "answer3";
    procedure->processResult(&sr);

    xml = procedure->resultXml();
    QVERIFY(xml.isEmpty());

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial3", 1);
    sr[answerElement] = "answer1";
    procedure->processResult(&sr);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus3</stimulus>";
    elements << "<correct>true</correct>";
    elements << "<answer>answer3</answer>";
    elements << "<correct_answer>answer3</correct_answer>";
    checkResultXml(xml, elements, trainingProcedureAttributes.type,
                   trainingProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    testTrial(trial, "trial1", 1);
    sr[answerElement] = "answer2";
    procedure->processResult(&sr);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<correct>true</correct>";
    elements << "<answer>answer1</answer>";
    elements << "<correct_answer>answer1</correct_answer>";

    QList<QStringList> alternatives;
    QStringList stimulusAlternatives;
    foreach (QString s, trial1Stimuli) {
        stimulusAlternatives.append("<stimulus>" + s + "</stimulus>");
    }
    alternatives.append(stimulusAlternatives);
    checkResultXml(xml, elements, trainingProcedureAttributes.type,
                   trainingProcedureAttributes.id, alternatives);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(trial.acceptResponse(0));
    QCOMPARE(trial.id(), QString("trial2"));
    QCOMPARE(trial.stimulusCount(0), 1);
    QCOMPARE(trial.stimulus(0, 0), QString("stimulus2"));

    sr[answerElement] = "answer2";
    procedure->processResult(&sr);

    xml = procedure->resultXml();
    elements.clear();
    elements << "<stimulus>stimulus2</stimulus>";
    elements << "<correct>true</correct>";
    elements << "<answer>answer2</answer>";
    elements << "<correct_answer>answer2</correct_answer>";
    checkResultXml(xml, elements, trainingProcedureAttributes.type,
                   trainingProcedureAttributes.id);

    trial = procedure->setupNextTrial();
    QVERIFY(trial.isValid());
    QVERIFY(!trial.acceptResponse(0)); // no response on last trial
    QCOMPARE(trial.id(), QString("trial2"));
    QCOMPARE(trial.stimulusCount(0), 0);
    QVERIFY(procedure->resultXml().isEmpty());

    // procedure should be done now
    QVERIFY(procedure->progress() >= 100.0);
    QVERIFY(!procedure->setupNextTrial().isValid());

    TEST_EXCEPTIONS_CATCH
}

void ProceduresTest::testEqualCorrector()
{
    TEST_EXCEPTIONS_TRY

    EqualCorrector c;
    {
        QVariant result = c.compare("string1", "string2");
        QVERIFY(!result.toBool());
    }

    {
        QVariant result = c.compare("string1", "string1");
        QVERIFY(result.toBool());
    }

    TEST_EXCEPTIONS_CATCH
}

ExperimentData *ProceduresTest::makeDummyExperimentData()
{
    QString configFile("dummy.apx");
    QScopedPointer<data::ScreensData> screens(new ScreensData);
    QScopedPointer<data::ProcedureData> procedures(new ConstantProcedureData);
    procedures->setCorrectorData(new CorrectorData);
    procedures->correctorData()->setType(CorrectorData::EQUAL);
    QScopedPointer<data::ConnectionsData> connections(new ConnectionsData);
    QScopedPointer<data::CalibrationData> calibration;
    QScopedPointer<data::GeneralParameters> genParam;
    QScopedPointer<data::ResultParameters> resParam;
    QScopedPointer<data::ParameterDialogResults> paramDlg;
    QScopedPointer<QMap<QString, data::RandomGeneratorParameters *>> rndGen(
        new QMap<QString, data::RandomGeneratorParameters *>());
    QScopedPointer<data::DevicesData> devices(new DevicesData);
    QScopedPointer<data::FiltersData> filters(new FiltersData);
    QScopedPointer<data::DevicesData> controlDevices(new DevicesData);
    QScopedPointer<data::DatablocksData> datablocks(new DatablocksData);
    QScopedPointer<data::StimuliData> stimuli(new StimuliData);
    QString description;
    QScopedPointer<data::ParameterManagerData> paramMngr(
        new ParameterManagerData);

    return new data::ExperimentData(
        configFile, screens.take(), procedures.take(), connections.take(),
        calibration.take(), genParam.take(), resParam.take(), paramDlg.take(),
        rndGen.take(), devices.take(), filters.take(), controlDevices.take(),
        datablocks.take(), stimuli.take(), description, paramMngr.take());
}

ExperimentData *ProceduresTest::makeStimulusExperimentData()
{
    QString configFile("dummy.apx");
    QScopedPointer<data::ScreensData> screens(new ScreensData);
    QScopedPointer<data::ProcedureData> procedures(new ConstantProcedureData);
    procedures->setCorrectorData(new CorrectorData);
    procedures->correctorData()->setType(CorrectorData::EQUAL);
    QScopedPointer<data::ConnectionsData> connections(new ConnectionsData);
    QScopedPointer<data::CalibrationData> calibration(0);
    QScopedPointer<data::GeneralParameters> genParam(0);
    QScopedPointer<data::ResultParameters> resParam(0);
    QScopedPointer<data::ParameterDialogResults> paramDlg(0);
    QScopedPointer<QMap<QString, data::RandomGeneratorParameters *>> rndGen(
        new QMap<QString, data::RandomGeneratorParameters *>());
    QScopedPointer<data::DevicesData> devices(new DevicesData);
    QScopedPointer<data::FiltersData> filters(new FiltersData);
    QScopedPointer<data::DevicesData> controlDevices(new DevicesData);
    QScopedPointer<data::DatablocksData> datablocks(new DatablocksData);

    StimulusData stimulus1;
    stimulus1.setId("stimulus1");

    StimulusData stimulus2;
    stimulus2.setId("stimulus2");

    QScopedPointer<data::StimuliData> stimuli(new StimuliData);
    stimuli->insert("stimulus1", stimulus1);
    stimuli->insert("stimulus2", stimulus2);

    QString description;
    QScopedPointer<data::ParameterManagerData> paramMngr(
        new ParameterManagerData);

    return new data::ExperimentData(
        configFile, screens.take(), procedures.take(), connections.take(),
        calibration.take(), genParam.take(), resParam.take(), paramDlg.take(),
        rndGen.take(), devices.take(), filters.take(), controlDevices.take(),
        datablocks.take(), stimuli.take(), description, paramMngr.take());
}

ExperimentRunDelegate *
ProceduresTest::makeDummyExperimentRunDelegate(const MainConfigFileData &mcd)
{
    return new ExperimentRunDelegate(*makeDummyExperimentData(), mcd, 0);
}

ExperimentRunDelegate *
ProceduresTest::makeStimulusExperimentRunDelegate(const MainConfigFileData &mcd)
{
    return new ExperimentRunDelegate(*makeStimulusExperimentData(), mcd, 0);
}

QTEST_MAIN(ProceduresTest)
