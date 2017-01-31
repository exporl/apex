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

#include "apextools/xml/apexxmltools.h"

#include "trainingprocedure.h"

using namespace apex;
using ApexXMLTools::XMLutils;

TrainingProcedure::TrainingProcedure(ProcedureApi* a,
                                     const data::ProcedureData* d,
                                     bool deterministic) :
                        ProcedureInterface(a, d),
                        trialsDone(0),
                        //+2 because the first and last trial are not real ones
                        totalTrials(data->presentations() + 2)
{
    if (deterministic)
        randomGenerator.setSeed(0);
}

data::Trial TrainingProcedure::setupNextTrial()
{
    data::Trial trial;

    if (++trialsDone <= totalTrials)
    {
        if (lastAnswer.isEmpty())
        {
            //first trial just shows the screen without stimuli
            data::TrialData* trialData = data->GetTrials().first();
            trial.setId(trialData->GetID());
            trial.setAnswer(trialData->GetAnswer());
            trial.addScreen(trialData->GetScreen(), true, 0);
        }
        else
        {
            //get all trials that have the last given answer as answer
            QList<data::TrialData*> trials;

            Q_FOREACH (data::TrialData* d, data->GetTrials())
            {
                if (d->GetAnswer() == lastAnswer)
                    trials << d;
            }

            if (trials.isEmpty()) {
                api->stopWithError("TrainingProcedure", QObject::tr("Didn't find a trial that corresponds with button \"%1\".").arg(lastAnswer));
                return trial;
            } else {
                //int index = ApexTools::RandomRange(0, trials.size() - 1);
                int index = randomGenerator.nextUInt(trials.size());
                data::TrialData* trialData = trials[index];

                trial.setId(trialData->GetID());
                trial.setAnswer(trialData->GetAnswer());
                bool lastTrial = trialsDone == totalTrials;

                trial.addScreen(trialData->GetScreen(), !lastTrial, 0);

                double waitAfter = lastTrial ?
                        0.0 : data->pauseBetweenStimuli() / 1000.0;

                if(trialsDone != totalTrials)
                    Q_FOREACH (QString stimulus, api->makeOutputList(data, trialData))
                        trial.addStimulus(stimulus, QMap<QString, QVariant>(), "", waitAfter);
            }
        }
    }

    currentTrial = trial.id();
    lastTrial = trial;
    return trial;
}

QString TrainingProcedure::firstScreen()
{
    return data->GetTrials().first()->GetScreen();
}

double TrainingProcedure::progress() const
{
    return (trialsDone >= totalTrials-2) ? 100.0 : 100.0 * trialsDone / (totalTrials-2);
}

ResultHighlight TrainingProcedure::processResult(const ScreenResult* screenResult)
{
    Q_ASSERT(!currentTrial.isEmpty());
    previousAnswer = lastAnswer;
    QString key = api->answerElement(currentTrial, data);
    lastAnswer = screenResult->value(key);
    Q_ASSERT(!lastAnswer.isEmpty());

    //HACK
    ScreenResult* lastScreen = const_cast<ScreenResult*>(screenResult);
    lastScreen->operator[](key) = previousAnswer;

    ResultHighlight ret(true, "");
    return ret;
}

QString TrainingProcedure::resultXml() const
{
    // we can't report results if there's not been any trial yet
    Q_ASSERT(currentTrial >= 0);
    qCDebug(APEX_RS, "***resultXML");

    if(previousAnswer.isEmpty() || (trialsDone == totalTrials))
        return "";

    QStringList result;
    if (!previousAnswer.isEmpty()) { //answer and corresponding stimulus should be happily together
        if(data->GetID().isEmpty()) {
            result.append(QLatin1String("<procedure type=\"apex:trainingProcedure\">"));
        } else {
            result.append(QString("<procedure type=\"apex:trainingProcedure\" id=\"%1\">").arg(data->GetID()));
        }

        for (int i = 0; i < lastTrial.stimulusCount(0); ++i)
            result.append(XMLutils::wrapTag("stimulus", lastTrial.stimulus(0, i)));

        result.append(XMLutils::wrapTag("correct", lastTrial.answer() == previousAnswer));
        result.append(XMLutils::wrapTag("answer", previousAnswer));
        result.append(XMLutils::wrapTag("correct_answer", lastTrial.answer()));

        result.append(QLatin1String("</procedure>"));
    }

    return result.join("\n");
}

QString TrainingProcedure::finalResultXml() const
{
    return QString();
}
