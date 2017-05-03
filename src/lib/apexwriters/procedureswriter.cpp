/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "apexdata/corrector/correctordata.h"

#include "apexdata/procedure/adaptiveproceduredata.h"
#include "apexdata/procedure/multiproceduredata.h"
#include "apexdata/procedure/proceduredata.h"
#include "apexdata/procedure/proceduredata.h"
#include "apexdata/procedure/scriptproceduredata.h"
#include "apexdata/procedure/trialdata.h"

#include "apextools/apextools.h"

#include "apextools/xml/xmltools.h"

#include "correctorwriter.h"
#include "procedureswriter.h"

using namespace apex;

using apex::data::TrialData;
using apex::data::ProcedureData;
using apex::data::MultiProcedureData;
using apex::data::AdaptiveProcedureData;
using apex::data::ScriptProcedureData;
using apex::writer::ProceduresWriter;

QDomElement ProceduresWriter::addElement(QDomDocument *doc,
        const ProcedureData& data)
{
    QDomElement root = doc->documentElement();
    QDomElement procedure = doc->createElement(QSL("procedure"));
    root.appendChild(procedure);

    if (!isMultiProcedure(data)) {
        fillProcedure(&procedure, data);
    } else {
        fillMultiProcedure(&procedure, dynamic_cast<const MultiProcedureData&>(data));
    }

    return procedure;
}

void ProceduresWriter::fillProcedure(QDomElement *theElement,
                                     const ProcedureData& data)
{
    QDomDocument doc = theElement->ownerDocument();

    QDomElement parameters = doc.createElement(QSL("parameters"));

    //get the type of the parameters
    QString type = getTypeString(data.type());
    theElement->setAttribute(QSL("xsi:type"), type);

    //check if the procedure has an id attribute
    if (!data.GetID().isEmpty())
        theElement->setAttribute(QSL("id"), data.GetID());

    //fill them and append to procedure
    fillParametersElement(data, &parameters);
    theElement->appendChild(parameters);

    QDomElement trials = doc.createElement(QSL("trials"));
    //get the trials
    const data::tTrialList& procTrials = data.GetTrials();
    //fill them and append to procedure
    fillTrialsElement(procTrials, &trials);
    theElement->appendChild(trials);
}

void ProceduresWriter::fillMultiProcedure(QDomElement *theElement,
        const MultiProcedureData& data)
{
    QDomDocument doc = theElement->ownerDocument();

    theElement->setAttribute(QSL("xsi:type"), QSL("apex:multiProcedureType"));

    //<parameters>
    QDomElement params = doc.createElement(QSL("parameters"));
    theElement->appendChild(params);
    //  <order>
    QString order = getOrderString(data.order());
    params.appendChild(XmlUtils::createTextElement(&doc, "order", order));

    //get all procedures and append one by one
    //NOTE typedef std::vector<ProcedureData*>
    //tProcConfigList in apexmultiprocedureconfig.h
    data::tProcConfigList procs = data.procedures();
    data::tProcConfigList::const_iterator it;

    for (it = procs.begin(); it != procs.end(); it++)
    {
        QDomElement procedure = doc.createElement(QSL("procedure"));

        const ProcedureData& d = **it;

        if (!isMultiProcedure(d))
            fillProcedure(&procedure, d);
        else
            fillMultiProcedure(&procedure,
                               dynamic_cast<const MultiProcedureData&>(d));

        theElement->appendChild(procedure);
    }
}

void ProceduresWriter::fillParametersElement
(const ProcedureData& data, QDomElement *theElement)
{
    int n; //used for temp integers

    //get the document this element will live in
    QDomDocument doc = theElement->ownerDocument();

    //<presentations>
    n = data.presentations();
    if (n > 0)
    {
        theElement->appendChild(XmlUtils::createTextElement
                (&doc, "presentations", n));
    }

    //<skip>
    theElement->appendChild(XmlUtils::createTextElement
            (&doc, "skip", data.skip()));

    //<order>
    QString order = getOrderString(data.order());
    theElement->appendChild(XmlUtils::createTextElement
            (&doc, "order", order));

    //<defaultstandard>
    QString ds = data.defaultStandard();
    if (!ds.isEmpty())
    {
        theElement->appendChild(XmlUtils::createTextElement
                (&doc, "defaultstandard", ds));
    }

    //<uniquestandard>
    theElement->appendChild(XmlUtils::createTextElement
            (&doc, "uniquestandard", data.uniqueStandard()));

    if (data.type() == ProcedureData::MultiType)
        return; //only order parameter for multiprocedure

    //<choices>
    /*n = params.GetChoices().choices();
    if (n >= 0)
    {
        theElement->appendChild(XmlUtils::createTextElement
                                (&doc, "choices", n));
    }*/

    apex::data::Choices cs = data.choices();

    if (cs.hasMultipleIntervals()) {
        QDomElement intervals = doc.createElement(QSL("intervals"));
        intervals.setAttribute(QSL("count"), QString::number(cs.nChoices()));

        if (!cs.selectedIntervals().isEmpty()) {
            QStringList selectedIntervals;
            Q_FOREACH (const int i, cs.selectedIntervals()) {
                selectedIntervals.push_back(QString::number(i+1));
            }
            intervals.setAttribute(QSL("select"), selectedIntervals.join(","));
        }

        for (int i = 0; i < cs.nChoices(); ++i) {
            QDomElement interval = doc.createElement(QSL("interval"));
            interval.setAttribute(QSL("number"), QString::number(i+1));
            interval.setAttribute(QSL("element"), cs.element(i));
            intervals.appendChild(interval);
        }
        theElement->appendChild(intervals);
    } else {
        CorrectorWriter::addElement(theElement, *(data.correctorData()));
    }

    //<pause_between_stimuli>
    n = data.pauseBetweenStimuli();
    if (n > 0)
        theElement->appendChild(XmlUtils::createTextElement
                                (&doc, "pause_between_stimuli", n));

    //<time_before_first_trial>
    theElement->appendChild(XmlUtils::createTextElement
                            (&doc, "time_before_first_trial",
                             data.timeBeforeFirstStimulus()));

    //<input_during_stimulus>
    theElement->appendChild(XmlUtils::createTextElement
                            (&doc, "input_during_stimulus",
                             data.inputDuringStimulus()));

    //add type specific parameters
    switch (data.type())
    {
        case ProcedureData::AdaptiveType:

            finishAsAdaptive(*dynamic_cast<
                    const AdaptiveProcedureData*>(&data), theElement);
            break;

        case ProcedureData::ConstantType:
        case ProcedureData::TrainingType:
            break;//nothing special here
        case ProcedureData::PluginType:

            finishAsPlugin(*dynamic_cast<
                    const ScriptProcedureData*>(&data), theElement);
            break;

        default:
            qFatal("illegal procedure parameters type");
    }
}

void ProceduresWriter::fillCorrector(const data::CorrectorData& correctorData, QDomElement *corrector)
{
    QString type;
    switch (correctorData.type())
    {
        case data::CorrectorData::EQUAL:
        {
            type = "apex:isequal";
            break;
        }
        default:
        {
             qFatal("Invalid corrector type");
        }
    }

    corrector->setAttribute(QSL("xsi:type"), type);
}

void ProceduresWriter::finishAsAdaptive(const AdaptiveProcedureData&data,
                                        QDomElement *toFinish)
{
    QDomDocument doc = toFinish->ownerDocument();

    //nUp
    toFinish->appendChild(XmlUtils::createTextElement(&doc, "nUp", data.nUp()));
    //nDown
    toFinish->appendChild(XmlUtils::createTextElement(&doc, "nDown",
                          data.nDown()));
    //adapt_parameter
    Q_FOREACH(QString adap, data.adaptingParameters())
    {
        toFinish->appendChild(XmlUtils::createTextElement(&doc,
                              "adapt_parameter", adap));
    }

    //start_value
    toFinish->appendChild(XmlUtils::createTextElement(&doc, "start_value",
                          data.startValue()));
    //stop_after_type
    toFinish->appendChild(XmlUtils::createTextElement(&doc, "stop_after_type",
                          data.stopAfterTypeString()));
    //stop_after
    toFinish->appendChild(XmlUtils::createTextElement(&doc, "stop_after",
                          data.stopAfter()));
    //min_value
    if (data.hasMinValue())
    {
        toFinish->appendChild(XmlUtils::createTextElement(&doc, "min_value",
                              data.minValue()));
    }
    //max_value
    if (data.hasMaxValue())
    {
        toFinish->appendChild(XmlUtils::createTextElement(&doc, "max_value",
                              data.maxValue()));
    }

    //TODO rev_for_mean
    //larger_is_easier
    toFinish->appendChild(XmlUtils::createTextElement(&doc, "larger_is_easier",
                          ApexTools::boolToString(data.largerIsEasier())));
    //repeat_first_until_correct
    if (data.repeatFirstUntilCorrect())
    {
        toFinish->appendChild(XmlUtils::createTextElement(&doc,
                              "repeat_first_until_correct", "true"));
    }

    //stepsizes
    QDomElement stepsizes = doc.createElement(QSL("stepsizes"));
    stepsizes.appendChild(XmlUtils::createTextElement(&doc, "change_after",
                           data.changeStepsizeAfterString()));

    QMap<int,float> upStepsizesMap = data.upStepsizes();
    QMap<int,float> downStepsizesMap = data.downStepsizes();

    QMap<int,float>::const_iterator it;
    for (it = upStepsizesMap.begin(); it != upStepsizesMap.end(); it++) {
        if (downStepsizesMap.contains(it.key())) {
            auto stepsize = createStepsizeElement(&doc, it.key(), it.value());
            stepsizes.appendChild(stepsize);
        } else {
            auto stepsize = createStepsizeElement(&doc, it.key(), it.value(), "up");
            stepsizes.appendChild(stepsize);
        }
    }

    for (it = downStepsizesMap.begin(); it != downStepsizesMap.end(); it++)
    {
        if (!upStepsizesMap.contains(it.key())) {
            auto stepsize = createStepsizeElement(&doc, it.key(), it.value(), "down");
            stepsizes.appendChild(stepsize);
        }
    }

    toFinish->appendChild(stepsizes);
}

QDomElement ProceduresWriter::createStepsizeElement(QDomDocument *doc, int begin, float size, QString direction)
{
    QDomElement stepsize = doc->createElement(QSL("stepsize"));
    stepsize.setAttribute(QSL("begin"), QString::number(begin));
    stepsize.setAttribute(QSL("size"), QString::number(size));
    if (!direction.isEmpty()) {
        stepsize.setAttribute(QSL("direction"), direction);
    }
    return stepsize;
}

void ProceduresWriter::finishAsPlugin(const data::ScriptProcedureData &data,
                                      QDomElement *toFinish)
{
    QDomDocument doc = toFinish->ownerDocument();

    //script
    toFinish->appendChild(XmlUtils::createTextElement(&doc, "script",
                          data.script()));
    //adjust_parameter
    QString adj = data.adjustParameter();
    if (!adj.isEmpty())
    {
        toFinish->appendChild(XmlUtils::createTextElement(&doc,
                              "adjust_parameter", adj));
    }

    //parameter
    data::tScriptProcedureParameterList paramList = data.customParameters();
    data::tScriptProcedureParameterList::const_iterator it;
    for (it = paramList.begin(); it != paramList.end(); it++)
    {
        QDomElement parameter = XmlUtils::createTextElement(&doc, "parameter",
                it->value);
        parameter.setAttribute(QSL("name"), it->name);
        toFinish->appendChild(parameter);
    }
}

void ProceduresWriter::fillTrialsElement
(const data::tTrialList& trials, QDomElement *theElement)
{
    QDomDocument doc = theElement->ownerDocument();

    data::tTrialList::const_iterator it;
    for (it = trials.begin(); it != trials.end(); it++)
    {
        QDomElement trialElem = doc.createElement(QSL("trial"));
        TrialData* trial = *it;

        //set the id attribute
        trialElem.setAttribute(QSL("id"), trial->GetID());

        //<answer>
        QString answer = trial->GetAnswer();
        if (!answer.isEmpty())
            trialElem.appendChild(XmlUtils::createTextElement
                                   (&doc, "answer", answer));

        //<answer_element>
        answer = trial->GetAnswerElement();
        if (!answer.isEmpty()) {
            trialElem.appendChild(XmlUtils::createTextElement
                                   (&doc, "answer_element", answer));
        }

        //<screen>
        answer = trial->GetScreen();
        if (!answer.isEmpty()) {
            QDomElement screen = doc.createElement(QSL("screen"));
            screen.setAttribute(QSL("id"), trial->GetScreen());
            trialElem.appendChild(screen);
        }

        //<stimulus>
        //NOTE see trialdata.h for tStimulusList
        tStimulusList stimuli = trial->GetStimuli();
        for (tStimulusList::const_iterator itStim = stimuli.begin();
                itStim != stimuli.end(); itStim++) {
            QString id = *itStim;
            QDomElement stimulus = doc.createElement(QSL("stimulus"));
            stimulus.setAttribute(QSL("id"), id);
            trialElem.appendChild(stimulus);
        }

        //<standard>
        tStimulusList standards = trial->GetStandards();
        for (tStimulusList::const_iterator itStan = standards.begin();
                itStan != standards.end(); itStan++)
        {
            QString id = *itStan;
            QDomElement standard = doc.createElement(QSL("standard"));
            standard.setAttribute(QSL("id"), id);
            trialElem.appendChild(standard);
        }

        theElement->appendChild(trialElem);
    }
}

QString ProceduresWriter::getTypeString(int type)
{
    QString typePrefix = "apex";
    QString typeString;

    switch (type)
    {
        case ProcedureData::AdaptiveType:

            typeString = "adaptiveProcedure";
            break;

        case ProcedureData::ConstantType:

            typeString = "constantProcedure";
            break;

        case ProcedureData::TrainingType:

            typeString = "trainingProcedure";
            break;

        case ProcedureData::MultiType:

            typeString = "multiProcedure";
            break;

        case ProcedureData::PluginType:

            typeString = "pluginProcedure";
            break;

        default:
            qFatal("WRITER: unknown procedure type (%i) \
                   returned from ProcedureData.", type);
    }

    return typePrefix + ":" + typeString;
}

QString ProceduresWriter::getOrderString(int order)
{
    QString orderString;
    switch (order) {
    case ProcedureData::RandomOrder:
        orderString = "random";
        break;
    case ProcedureData::SequentialOrder:
        orderString = "sequential";
        break;
    case ProcedureData::OneByOneOrder:
        orderString = "onebyone";
        break;
    default:
        qFatal("WRITER: unknown order (%i) returned \
                from ProcedureParameters.", order);
    }
    return orderString;
}

bool ProceduresWriter::isMultiProcedure(const ProcedureData& data)
{
    return getTypeString(data.type()).contains("multiProcedureType");
}
