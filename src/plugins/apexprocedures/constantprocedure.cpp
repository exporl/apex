/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#include "apexdata/procedure/constantproceduredata.h"

#include "apexdata/screen/screen.h"

#include "apextools/apextools.h"

#include "common/xmlutils.h"

#include "constantprocedure.h"

#include <QMap>
#include <QVariant>

using namespace cmn;

namespace apex
{

ConstantProcedure::ConstantProcedure(ProcedureApi *api,
                                     const data::ProcedureData *data)
    : ProcedureInterface(api, data),
      data(dynamic_cast<const data::ConstantProcedureData *>(data)),
      currentTrial(-1),
      trialSetup(false),
      lastResult(false)
{
    Q_ASSERT(data);

    trialList = api->makeTrialList(data, true);
}

double ConstantProcedure::progress() const
{
    if (currentTrial < 0)
        return 0;

    return 100 * (double)(currentTrial + 1) / trialList.size();
}

ResultHighlight
ConstantProcedure::processResult(const ScreenResult *screenResult)
{
    Q_ASSERT(trialSetup);
    trialSetup = false;

    AnswerInfo info = api->processAnswer(
        data, screenResult, trialList[currentTrial], stimulusPosition);

    Q_ASSERT(info.correctness.canConvert(QVariant::Bool));
    lastResult = info.correctness.toBool();
    lastAnswer = info.userAnswer;
    lastCorrectAnswer = info.correctAnswer;

    ResultHighlight ret(lastResult, info.highlightElement);
    return ret;
}

QString ConstantProcedure::resultXml() const
{
    // we can't report results if there's not been any trial yet
    Q_ASSERT(currentTrial >= 0);
    // the result should have been processed before asking for a result
    Q_ASSERT(!trialSetup);
    qCDebug(APEX_RS, "***resultXML");

    QStringList result;

    if (data->GetID().isEmpty()) {
        result.append(
            QLatin1String("<procedure type=\"apex:constantProcedure\">"));
    } else {
        result.append(
            QString("<procedure type=\"apex:constantProcedure\" id=\"%1\">")
                .arg(data->GetID()));
    }

    result.append(
        QSL("<%1>%2</%1>").arg(QSL("answer"), xmlEscapedText(lastAnswer)));
    result.append(
        QSL("<%1>%2</%1>")
            .arg(QSL("correct_answer"), xmlEscapedText(lastCorrectAnswer)));

    if (data->choices().hasMultipleIntervals()) {
        result.append(QSL("<%1>%2</%1>")
                          .arg(QSL("correct_interval"))
                          .arg(stimulusPosition + 1));
        result.append(QSL("<%1>%2</%1>")
                          .arg(QSL("answer_interval"))
                          .arg(data->choices().interval(lastAnswer) + 1));
    }

    if (currentTrial < data->skip())
        result.append("<skip/>");

    if (!data->choices().hasMultipleIntervals()) {
        result.append(QSL("<%1>%2</%1>")
                          .arg(QSL("stimulus"),
                               xmlEscapedText(lastTrial.stimulus(0, 0))));
    } else {
        for (int i = 0; i < lastTrial.stimulusCount(0); ++i) {
            result.append(QSL("<%1>%2</%1>")
                              .arg(i == stimulusPosition ? QSL("stimulus")
                                                         : QSL("standard"),
                                   xmlEscapedText(lastTrial.stimulus(0, i))));
        }
    }

    result.append(QSL("<%1>%2</%1>")
                      .arg(QSL("correct"))
                      .arg(ApexTools::boolToString(lastResult)));

    result.append(QLatin1String("</procedure>"));

    return result.join("\n");
}

QString ConstantProcedure::finalResultXml() const
{
    return QString();
}

QString ConstantProcedure::firstScreen()
{
    return data->GetTrial(trialList.first())->GetScreen();
}

data::Trial ConstantProcedure::setupNextTrial()
{
    data::Trial trial;

    if (++currentTrial >= trialList.size()) // finished
        return trial;

    data::TrialData *trialData = data->GetTrial(trialList.at(currentTrial));
    trial.setId(trialData->GetID());
    trial.setAnswer(trialData->GetAnswer());
    trial.addScreen(trialData->GetScreen(), true, 0);

    stimulusPosition = data->choices().randomPosition();

    QStringList standards;
    if (trialData->GetStandards().isEmpty())
        standards = api->makeStandardList(data, QStringList()
                                                    << data->defaultStandard());
    else
        standards = api->makeStandardList(data, trialData->GetStandards());

    QStringList outputlist = api->makeOutputList(
        data, trialData->GetRandomStimulus(), standards, stimulusPosition);

    for (int i = 0; i < outputlist.size(); ++i) {
        double waitAfter; // time to wait after stimulus

        if (i < outputlist.size() - 1)
            waitAfter = data->pauseBetweenStimuli() / 1000.0; // ms to s
        else
            waitAfter = 0;

        trial.addStimulus(
            outputlist[i], QMap<QString, QVariant>(),
            api->highlightedElement(data, trialData->GetAnswer(), i),
            waitAfter);
    }

    lastTrial = trial;
    trialSetup = true;

    qCDebug(APEX_RS) << "Setting up trial in constant procedure "
                     << currentTrial;

    return trial;
}
}
