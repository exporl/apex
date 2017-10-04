/******************************************************************************
 * Copyright (C) 2010  Job Noorman <jobnoorman@gmail.com>                     *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apextools/apextools.h"
#include "apextools/exceptions.h"
#include "apextools/random.h"

#include "apextools/xml/xmltools.h"

#include "common/xmlutils.h"

#include "adaptiveprocedure.h"
#include "parameteradapter.h"

#include <limits>

using namespace cmn;

namespace apex
{

struct AdaptiveProcedurePrivate {
    AdaptiveProcedurePrivate(const data::ProcedureData *d);

    const data::AdaptiveProcedureData *data;
    ParameterAdapter adapter;

    bool lastAnswerCorrect;
    QString lastAnswer;
    QString lastCorrectAnswer;
    data::Trial lastTrial;
    bool trialSetup;
    bool receivedResult;

    data::adapting_parameter currentParameter;
    float previousStepsize;
    int nTrialsDone;
    int nPresentations;
    QStringList trialList;
    int currentTrial;     // index in trialList
    int stimulusPosition; // position of the current stimulus, only valid when
                          // data->choices().nChoices() > 1
    QString firstScreen;

    bool hasFixedParameter;
    bool hasMinValue, hasMaxValue;
    data::adapting_parameter minValue, maxValue;
    bool saturated;

    Random random;

    QString XMLId() const;
};

AdaptiveProcedurePrivate::AdaptiveProcedurePrivate(const data::ProcedureData *d)
    : data(static_cast<const data::AdaptiveProcedureData *>(d)),
      adapter(data),
      nTrialsDone(0),
      nPresentations(0)
{
}

AdaptiveProcedure::AdaptiveProcedure(ProcedureApi *api,
                                     const data::ProcedureData *data)
    : ProcedureInterface(api, data),
      d(new AdaptiveProcedurePrivate(data)),
      started(false)
{
    // FIXME is this correct?
    d->hasFixedParameter =
        api->stimulus(data->GetTrials().first()->GetRandomStimulus())
            ->GetFixedParameters()
            .contains(d->data->adaptingParameters().first());

    d->trialList = api->makeTrialList(data, true);
    d->currentTrial = -1;
    d->lastAnswerCorrect = true;

    // firstScreen is kept in a variable because trialList can change
    // during the procedure.
    d->firstScreen = data->GetTrial(d->trialList.first())->GetScreen();
}

AdaptiveProcedure::~AdaptiveProcedure()
{
    delete d;
}

data::Trial AdaptiveProcedure::setupNextTrial()
{
    data::Trial ret;

    if (doNextTrial() && ++d->currentTrial == d->trialList.length()) {
        // done all trials so recreate the trial list. don't do skip because
        // that has already been done in the first trial list.
        d->trialList = api->makeTrialList(data, false);
        d->currentTrial = 0;
    }

    data::TrialData *trial = data->GetTrial(d->trialList[d->currentTrial]);

    // Initialise min and max because these values are used in setNewParameter.
    // This cannot be always done before setNewParameter because when we
    // generate
    // a virtual trial, the min and max shouldn't be updated.
    if (!started) {
        updateMinMaxValues(trial);
    }

    setNewParameter();

    if (!started) {
        started = true;
        d->saturated = false;
    }

    // Note that we need the above to generate the virtual trial
    if (progress() >= 100.0)
        return ret;

    updateMinMaxValues(trial);

    ret.setId(trial->GetID());
    ret.addScreen(trial->GetScreen(), true, 0);

    d->stimulusPosition = d->data->choices().randomPosition();
    qCDebug(APEX_RS) << "Randomly selected position" << d->stimulusPosition;

    QString answer;
    //     if (d->parameters->choices().choices() > 1)
    //         answer =
    //         d->parameters->choices().intervals()[d->stimulusPosition];
    //     else
    //         answer = trial->GetAnswer();

    answer = d->data->choices().element(d->stimulusPosition);

    if (answer.isEmpty())
        answer = trial->GetAnswer();

    ret.setAnswer(answer);

    QStringList standards(findStimuli(trial->GetStandards()));
    if (standards.isEmpty())
        standards = trial->GetStandards();
    if (standards.isEmpty())
        standards << d->data->defaultStandard();
    if (standards.isEmpty())
        throw ApexStringException(tr("Could not find any standard"));

    standards = api->makeStandardList(data, standards);

    QString stimulusID = findStimulus(trial);
    QStringList outputList =
        api->makeOutputList(data, stimulusID, standards, d->stimulusPosition);

    { // Set the value of the parameter to the value of the used stimulus
        const data::StimulusData *stimulus = api->stimulus(stimulusID);
        QString fixedParameter = d->data->adaptingParameters().first();
        QVariant variant = stimulus->GetFixedParameters().value(fixedParameter);

        if (variant.isValid()) {
            Q_ASSERT(variant.canConvert<data::adapting_parameter>());

            d->currentParameter = variant.value<data::adapting_parameter>();
        }
    }

    for (int i = 0; i < outputList.size(); i++) {
        double waitAfter = 0.0;

        if (i < outputList.count() - 1)
            waitAfter = d->data->pauseBetweenStimuli() / 1000.0;

        QString stimulus = outputList[i];
        ret.addStimulus(stimulus, createParameters(stimulus),
                        api->highlightedElement(data, trial->GetAnswer(), i),
                        waitAfter);
    }

    qCDebug(APEX_RS) << "*** Starting next trial with parameter"
                     << d->currentParameter << ", progress is" << progress()
                     << "% ***";

    d->lastTrial = ret;
    d->trialSetup = true;
    d->receivedResult = false;
    d->lastAnswer.clear();
    d->lastCorrectAnswer.clear();

    return ret;
}

void AdaptiveProcedure::setNewParameter()
{
    // check for saturation
    d->currentParameter = d->adapter.currentParameter();
    d->saturated = false;

    if (d->hasMinValue && d->currentParameter < d->minValue) {
        d->saturated = true;
        d->currentParameter = d->minValue;
    } else if (d->hasMaxValue && d->currentParameter > d->maxValue) {
        d->saturated = true;
        d->currentParameter = d->maxValue;
    }

    if (started && d->saturated)
        api->showMessage(QObject::tr("Saturated"));
}

QString AdaptiveProcedure::firstScreen()
{
    return d->firstScreen;
}

double AdaptiveProcedure::progress() const
{
    int progression;
    int maxProgression = d->data->stopAfter();

    switch (d->data->stopAfterType()) {
    case data::AdaptiveProcedureData::StopAfterReversals:

        progression = d->adapter.numberOfReversals();
        break;

    case data::AdaptiveProcedureData::StopAfterPresentations:

        maxProgression = d->trialList.size();
        progression = d->nTrialsDone;
        break;

    case data::AdaptiveProcedureData::StopAfterTrials:

        progression = d->nTrialsDone;
        break;

    default:

        qFatal("Unknown stop-after type");
        break;
    }

    return 100.0 * progression / maxProgression;
}

ResultHighlight
AdaptiveProcedure::processResult(const ScreenResult *screenResult)
{
    Q_ASSERT(d->trialSetup);
    d->trialSetup = false;
    d->receivedResult = true;

    AnswerInfo info = api->processAnswer(
        data, screenResult, d->trialList[d->currentTrial], d->stimulusPosition);

    Q_ASSERT(info.correctness.canConvert(QVariant::Bool));
    d->lastAnswerCorrect = info.correctness.toBool();
    d->lastAnswer = info.userAnswer;
    d->lastCorrectAnswer = info.correctAnswer;

    bool doNextTrialVal = doNextTrial();

    if (doNextTrialVal) {
        ++d->nTrialsDone;
        ++d->nPresentations;
    }

    d->previousStepsize = d->adapter.currentStepsize();
    d->adapter.updateParameter(info.correctness, d->nTrialsDone,
                               d->currentParameter, !doNextTrialVal);

    ResultHighlight ret(d->lastAnswerCorrect, info.highlightElement);
    return ret;
}

QString AdaptiveProcedure::resultXml() const
{
    return resultXml(false);
}

QString AdaptiveProcedure::resultXml(bool isVirtual) const
{
    Q_ASSERT(d->currentTrial >= 0);
    // the result should have been processed before asking for a result
    Q_ASSERT(!d->trialSetup);

    QStringList result;

    if (data->GetID().isEmpty()) {
        result.append(
            QLatin1String("<procedure type=\"apex:adaptiveProcedure\">"));
    } else {
        result.append(
            QString("<procedure type=\"apex:adaptiveProcedure\" id=\"%1\">")
                .arg(data->GetID()));
    }

    if (!isVirtual) {
        result.append(QSL("<%1>%2</%1>")
                          .arg(QSL("answer"), xmlEscapedText(d->lastAnswer)));
        result.append(QSL("<%1>%2</%1>")
                          .arg(QSL("correct_answer"),
                               xmlEscapedText(d->lastCorrectAnswer)));

        if (d->data->choices().hasMultipleIntervals()) {
            result.append(QSL("<%1>%2</%1>")
                              .arg(QSL("correct_interval"))
                              .arg(d->stimulusPosition + 1));
            result.append(
                QSL("<%1>%2</%1>")
                    .arg(QSL("answer_interval"))
                    .arg(d->data->choices().interval(d->lastAnswer) + 1));
        }
    }

    if (d->nTrialsDone < d->data->skip())
        result.append("\t<skip/>");

    if (!d->data->choices().hasMultipleIntervals()) {
        result.append(QSL("<%1>%2</%1>")
                          .arg(QSL("stimulus"),
                               xmlEscapedText(d->lastTrial.stimulus(0, 0))));
    } else {
        for (int i = 0; i < d->lastTrial.stimulusCount(0); ++i) {
            result.append(
                QSL("<%1>%2</%1>")
                    .arg(i == d->stimulusPosition ? QSL("stimulus")
                                                  : QSL("standard"),
                         xmlEscapedText(d->lastTrial.stimulus(0, i))));
        }
    }

    if (!isVirtual) {
        result.append(QSL("<%1>%2</%1>")
                          .arg(QSL("correct"))
                          .arg(ApexTools::boolToString(d->lastAnswerCorrect)));
        if (d->adapter.lastReversal())
            result += "\t<reversal/>";
    }

    result.append("\t<parameter>" + QString::number(d->currentParameter) +
                  "</parameter>");
    if (isVirtual) {
        result.append("\t<stepsize>" +
                      QString::number(d->adapter.currentStepsize()) +
                      "</stepsize>");
    } else {
        result.append("\t<stepsize>" + QString::number(d->previousStepsize) +
                      "</stepsize>");
    }
    result.append("\t<reversals>" + QString::number(d->adapter.nReversals()) +
                  "</reversals>");
    result.append("\t<saturation>" +
                  (d->saturated ? QString("true") : QString("false")) +
                  "</saturation>");
    int visualNPresentations =
        (d->nPresentations - 1 < 0) ? 0 : d->nPresentations - 1;
    result.append("\t<presentations>" +
                  QString::number(visualNPresentations + isVirtual) +
                  "</presentations>");

    result.append(QLatin1String("</procedure>"));

    return result.join("\n");
}

QString AdaptiveProcedure::finalResultXml() const
{
    if (!d->receivedResult)
        return QString();

    QStringList result;
    result.append("<trial id=\"VIRTUAL_TRIAL\" type=\"virtual\">");
    result.append("<!-- This is not a real trial, but a virtual trial based on "
                  "the previous one -->");
    result.append(resultXml(true));
    result.append("</trial>");

    return result.join("\n");
}

QString AdaptiveProcedurePrivate::XMLId() const
{
    QString temp;
    if (!data->GetID().isEmpty()) {
        temp = "id=\"" + data->GetID() + "\"";
    }
    return temp;
}

void AdaptiveProcedure::updateMinMaxValues(const data::TrialData *trial)
{
    // if we have a fixed parameter and either the min or the max value has
    // not been specified, we need to iterate over all stimuli to get those
    // values from the fixed parameters
    if (d->hasFixedParameter &&
        (!d->data->hasMinValue() || !d->data->hasMaxValue())) {
        d->hasMaxValue = d->hasMinValue = true;

        d->minValue = std::numeric_limits<data::adapting_parameter>::max();
        d->maxValue = std::numeric_limits<data::adapting_parameter>::min();

        QString fixedParameter = d->data->adaptingParameters().first();

        Q_FOREACH (QString stimulusId, trial->GetStimuli()) {
            const data::StimulusData *stimulus = api->stimulus(stimulusId);
            QVariant variant =
                stimulus->GetFixedParameters().value(fixedParameter);

            Q_ASSERT(variant.canConvert<data::adapting_parameter>());

            data::adapting_parameter value =
                variant.value<data::adapting_parameter>();

            if (value < d->minValue)
                d->minValue = value;
            if (value > d->maxValue)
                d->maxValue = value;
        }

        if (d->data->hasMinValue())
            d->minValue = d->data->minValue();
        else if (d->data->hasMaxValue())
            d->maxValue = d->data->maxValue();
    } else // we don't have a fixed parameter or both min and max have been
           // specified
    {
        if (d->data->hasMinValue()) {
            d->hasMinValue = true;
            d->minValue = d->data->minValue();
        } else
            d->hasMinValue = false;

        if (d->data->hasMaxValue()) {
            d->hasMaxValue = true;
            d->maxValue = d->data->maxValue();
        } else
            d->hasMaxValue = false;
    }

    qCDebug(APEX_RS) << "Updated min / max to"
                     << (d->hasMinValue ? QString::number(d->minValue) : "none")
                     << "/" << (d->hasMaxValue ? QString::number(d->maxValue)
                                               : "none");

    if (d->hasMinValue && d->hasMaxValue)
        Q_ASSERT(d->minValue <= d->maxValue);
}

bool AdaptiveProcedure::doNextTrial() const
{
    // if this is the first trial AND we're repeating the first trial until the
    // answer is correct AND the given answer is not correct, we do NOT go to
    // the next trial. this condition negated indicates when we DO want to want
    // to go to the next trial and becomes the following by De Morgan's law:
    return d->nTrialsDone != 0 || !d->data->repeatFirstUntilCorrect() ||
           d->lastAnswerCorrect;
}

QMap<QString, QVariant>
AdaptiveProcedure::createParameters(const QString &stimulus) const
{
    QMap<QString, QVariant> ret;
    QStringList parameters = d->data->adaptingParameters();

    if (d->hasFixedParameter)
        ret[parameters.takeFirst()] = fixedParameterValue(stimulus);

    Q_FOREACH (QString parameter, parameters)
        ret[parameter] = d->currentParameter;

    return ret;
}

data::adapting_parameter
AdaptiveProcedure::fixedParameterValue(const QString &stimulusId) const
{
    const data::StimulusData *stimulus = api->stimulus(stimulusId);
    QString fixedParameter = d->data->adaptingParameters().first();
    QVariant variant = stimulus->GetFixedParameters().value(fixedParameter);

    Q_ASSERT(variant.canConvert<data::adapting_parameter>());

    return variant.value<data::adapting_parameter>();
}

QString AdaptiveProcedure::findStimulus(const data::TrialData *trial) const
{
    if (!d->hasFixedParameter)
        return trial->GetRandomStimulus();
    else {
        QStringList bestStimuli(findStimuli(trial->GetStimuli()));
        /*data::adapting_parameter minDelta =
                        std::numeric_limits<data::adapting_parameter>::max();



        Q_FOREACH (QString stimulusId, trial->GetStimuli())
        {
            data::adapting_parameter value = fixedParameterValue(stimulusId);
            data::adapting_parameter delta = qAbs(value - d->currentParameter);

            // [Tom] Added, cf commit 923a5c1a1d8c624bfb8da800197a41402673bea0
            if (d->hasMinValue && value<d->minValue)
                continue;
            if (d->hasMaxValue && value>d->maxValue)
                continue;

            if (delta <= minDelta)
            {
                if (delta < minDelta)
                {
                    //we found a smaller delta so all current best stimuli
                    //are not best anymore
                    bestStimuli.clear();
                    minDelta = delta;
                }

                bestStimuli << stimulusId;
            }
        }
*/
        if (bestStimuli.isEmpty()) {
            throw(ApexStringException(
                tr("Cannot find stimulus with requested fixed parameter (%1)")
                    .arg(d->currentParameter)));
        }

        QString resultStimulus(
            bestStimuli[d->random.nextUInt(bestStimuli.size())]);

        // minValue and maxValue are always defined for fixed parameter
        if (fixedParameterValue(resultStimulus) < d->minValue ||
            fixedParameterValue(resultStimulus) > d->maxValue)
            d->saturated = true;

        return resultStimulus;
    }
}

QStringList AdaptiveProcedure::findStimuli(const QStringList &list) const
{
    if (!d->hasFixedParameter)
        return list;
    else {
        QStringList bestStimuli;
        data::adapting_parameter minDelta =
            std::numeric_limits<data::adapting_parameter>::max();

        Q_FOREACH (QString stimulusId, list) {
            data::adapting_parameter value = fixedParameterValue(stimulusId);
            data::adapting_parameter delta = qAbs(value - d->currentParameter);

            // [Tom] Added, cf commit 923a5c1a1d8c624bfb8da800197a41402673bea0
            if (d->hasMinValue && value < d->minValue)
                continue;
            if (d->hasMaxValue && value > d->maxValue)
                continue;

            if (delta <= minDelta) {
                if (delta < minDelta) {
                    // we found a smaller delta so all current best stimuli
                    // are not best anymore
                    bestStimuli.clear();
                    minDelta = delta;
                }

                bestStimuli << stimulusId;
            }
        }

        /*if (bestStimuli.isEmpty()) {
            throw( ApexStringException(
                        tr("Cannot find standard with requested fixed parameter
        (%1)")
                        .arg(d->currentParameter)));
        }*/

        return bestStimuli;
    }
}

} // ns apex
