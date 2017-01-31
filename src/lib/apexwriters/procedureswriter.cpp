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

/**
* @author Job Noorman
*/

#include "procedureswriter.h"
#include "procedure/apexprocedureconfig.h"
#include "procedure/apexmultiprocedureconfig.h"
#include "procedure/apexmultiprocedureparameters.h"
#include "procedure/apexprocedureparameters.h"
#include "procedure/apexadaptiveprocedureparameters.h"
#include "procedure/pluginprocedureparameters.h"
#include "procedure/apextrial.h"

//from libtools
#include "apextools.h"
#include "xml/apexxmltools.h"

#include "xml/xercesinclude.h"
#include <vector>

using namespace apex;
using namespace apex::ApexXMLTools;
using namespace XERCES_CPP_NAMESPACE;

using apex::data::ApexTrial;
using apex::data::ApexProcedureConfig;
using apex::data::ApexMultiProcedureConfig;
using apex::data::ApexProcedureParameters;
using apex::data::ApexAdaptiveProcedureParameters;
using apex::data::PluginProcedureParameters;
using apex::data::ApexAnswer;
using apex::writer::ProceduresWriter;

DOMElement* ProceduresWriter::addElement(DOMDocument* doc,
        const ApexProcedureConfig& data)
{
    DOMElement* root = doc->getDocumentElement();
    DOMElement* procedure = doc->createElement(X("procedure"));
    root->appendChild(procedure);

    if (!isMultiProcedure(data))
        fillProcedure(procedure, data);
    else
        fillMultiProcedure(procedure,
                           dynamic_cast<const ApexMultiProcedureConfig&>(data));

    return procedure;
}

void ProceduresWriter::fillProcedure(DOMElement* theElement,
                                     const ApexProcedureConfig& data)
{
    DOMDocument* doc = theElement->getOwnerDocument();

    DOMElement* parameters = doc->createElement(X("parameters"));
    //get the parameters
    const ApexProcedureParameters& procParams = data.GetParameters();

    //get the type of the parameters
    QString type = getTypeString(procParams.GetType());
    theElement->setAttribute(X("xsi:type"), S2X(type));

    //check if the procedure has an id attribute
    //FIXME cannot get the id this way and GetID() has been
    //removed from ApexProcedureParameters
    if (procParams.HasParameter("id"))
        theElement->setAttribute(X("id"),
                                 S2X(procParams.GetParameter("id")));

    //fill them and append to procedure
    fillParametersElement(procParams, parameters);
    theElement->appendChild(parameters);

    DOMElement* trials = doc->createElement(X("trials"));
    //get the trials
    const std::vector<ApexTrial*>& procTrials = data.GetTrials();
    //fill them and append to procedure
    fillTrialsElement(procTrials, trials);
    theElement->appendChild(trials);
}

void ProceduresWriter::fillMultiProcedure(DOMElement* theElement,
        const ApexMultiProcedureConfig& data)
{
    DOMDocument* doc = theElement->getOwnerDocument();

    theElement->setAttribute(X("xsi:type"), X("apex:multiProcedureType"));

    //<parameters>
    DOMElement* params = doc->createElement(X("parameters"));
    theElement->appendChild(params);
    //  <order>
    QString order = getOrderString(data.GetParameters().GetOrder());
    params->appendChild(XMLutils::CreateTextElement(doc, "order", order));

    //get all procedures and append one by one
    //NOTE typedef std::vector<ApexProcedureConfig*>
    //tProcConfigList in apexmultiprocedureconfig.h
    data::tProcConfigList procs = data.GetProcedures();
    data::tProcConfigList::const_iterator it;

    for (it = procs.begin(); it != procs.end(); it++)
    {
        DOMElement* procedure = doc->createElement(X("procedure"));

        const ApexProcedureConfig& d = **it;

        if (!isMultiProcedure(d))
            fillProcedure(procedure, d);
        else
            fillMultiProcedure(procedure,
                               dynamic_cast<const ApexMultiProcedureConfig&>(d));

        theElement->appendChild(procedure);
    }
}

void ProceduresWriter::fillParametersElement
(const ApexProcedureParameters& params, DOMElement* theElement)
{
    int n; //used for temp integers

    //get the document this element will live in
    DOMDocument* doc = theElement->getOwnerDocument();

    //<presentations>
    n = params.GetPresentations();
    if (n > 0)
    {
        theElement->appendChild(XMLutils::CreateTextElement
                (doc, "presentations", n));
    }

    //<skip>
    theElement->appendChild(XMLutils::CreateTextElement
            (doc, "skip", params.GetSkip()));

    //<order>
    QString order = getOrderString(params.GetOrder());
    theElement->appendChild(XMLutils::CreateTextElement
            (doc, "order", order));

    //<defaultstandard>
    QString ds = params.GetDefaultStandard();
    if (!ds.isEmpty())
    {
        theElement->appendChild(XMLutils::CreateTextElement
                (doc, "defaultstandard", ds));
    }

    //<uniquestandard>
    theElement->appendChild(XMLutils::CreateTextElement
            (doc, "uniquestandard", params.GetUniqueStandard()));

    if (params.GetType() == ApexProcedureParameters::TYPE_MULTI)
        return; //only order parameter for multiprocedure

    //<choices>
    n = params.GetChoices().nChoices;
    if (n >= 0)
    {
        theElement->appendChild(XMLutils::CreateTextElement
                                (doc, "choices", n));
    }

    //<pause_between_stimuli>
    n = params.GetPauseBetweenStimuli();
    if (n > 0)
        theElement->appendChild(XMLutils::CreateTextElement
                                (doc, "pause_between_stimuli", n));

    //<time_before_first_trial>
    theElement->appendChild(XMLutils::CreateTextElement
                            (doc, "time_before_first_trial",
                             params.GetTimeBeforeFirstStimulus()));

    //<input_during_stimulus>
    theElement->appendChild(XMLutils::CreateTextElement
                            (doc, "input_during_stimulus",
                             params.GetInputDuringStimulus()));

    //add type specific parameters
    switch (params.GetType())
    {
        case ApexProcedureParameters::TYPE_ADAPTIVE:

            finishAsAdaptive(*dynamic_cast<
                    const ApexAdaptiveProcedureParameters*>(&params), theElement);
            break;

        case ApexProcedureParameters::TYPE_CONSTANT:
        case ApexProcedureParameters::TYPE_TRAINING:
            break;//nothing special here
        case ApexProcedureParameters::TYPE_PLUGIN:

            finishAsPlugin(*dynamic_cast<
                    const PluginProcedureParameters*>(&params), theElement);
            break;

        default:
            qFatal("illegal procedure parameters type");
    }
}

void ProceduresWriter::finishAsAdaptive(const ApexAdaptiveProcedureParameters&
        params, DOMElement* toFinish)
{
    DOMDocument* doc = toFinish->getOwnerDocument();

    //nUp
    toFinish->appendChild(XMLutils::CreateTextElement(doc, "nUp", params.nUp()));
    //nDown
    toFinish->appendChild(XMLutils::CreateTextElement(doc, "nDown",
                          params.nDown()));
    //adapt_parameter
    Q_FOREACH(QString adap, params.adaptingParameters())
    {
        toFinish->appendChild(XMLutils::CreateTextElement(doc,
                              "adapt_parameter", adap));
    }

    //start_value
    toFinish->appendChild(XMLutils::CreateTextElement(doc, "start_value",
                          params.startValue()));
    //stop_after_type
    toFinish->appendChild(XMLutils::CreateTextElement(doc, "stop_after_type",
                          params.stopAfterTypeString()));
    //stop_after
    toFinish->appendChild(XMLutils::CreateTextElement(doc, "stop_after",
                          params.stopAfter()));
    //min_value
    if (params.hasMinValue())
    {
        toFinish->appendChild(XMLutils::CreateTextElement(doc, "min_value",
                              params.minValue()));
    }
    //max_value
    if (params.hasMaxValue())
    {
        toFinish->appendChild(XMLutils::CreateTextElement(doc, "max_value",
                              params.maxValue()));
    }

    //TODO rev_for_mean
    //larger_is_easier
    toFinish->appendChild(XMLutils::CreateTextElement(doc, "larger_is_easier",
                          ApexTools::boolToString(params.largerIsEasier())));
    //repeat_first_until_correct
    if (params.repeatFirstUntilCorrect())
    {
        toFinish->appendChild(XMLutils::CreateTextElement(doc,
                              "repeat_first_until_correct", "true"));
    }

    //stepsizes
    DOMElement* stepsizes = doc->createElement(X("stepsizes"));
    stepsizes->appendChild(XMLutils::CreateTextElement(doc, "change_after",
                           params.changeStepsizeAfterString()));

    std::map<int,float> stepsizesMap = params.stepsizes();
    std::map<int,float>::const_iterator it;
    for (it = stepsizesMap.begin(); it != stepsizesMap.end(); it++)
    {
        DOMElement* stepsize = doc->createElement(X("stepsize"));
        stepsize->setAttribute(X("begin"), S2X(QString::number(it->first)));
        stepsize->setAttribute(X("size"), S2X(QString::number(it->second)));
        stepsizes->appendChild(stepsize);
    }

    toFinish->appendChild(stepsizes);
}

void ProceduresWriter::finishAsPlugin(const data::PluginProcedureParameters&
        params, DOMElement* toFinish)
{
    DOMDocument* doc = toFinish->getOwnerDocument();

    //script
    toFinish->appendChild(XMLutils::CreateTextElement(doc, "script",
                          params.GetScript()));
    //adjust_parameter
    QString adj = params.GetAdjustParameter();
    if (!adj.isEmpty())
    {
        toFinish->appendChild(XMLutils::CreateTextElement(doc,
                              "adjust_parameter", adj));
    }

    //parameter
    data::tPluginProcedureParameterList paramList = params.GetCustomParameters();
    data::tPluginProcedureParameterList::const_iterator it;
    for (it = paramList.begin(); it != paramList.end(); it++)
    {
        DOMElement* parameter = XMLutils::CreateTextElement(doc, "parameter",
                it->value);
        parameter->setAttribute(X("name"), S2X(it->name));
        toFinish->appendChild(parameter);
    }
}

void ProceduresWriter::fillTrialsElement
(const std::vector<ApexTrial*>& trials, DOMElement* theElement)
{
    DOMDocument* doc = theElement->getOwnerDocument();

    std::vector<ApexTrial*>::const_iterator it;
    for (it = trials.begin(); it != trials.end(); it++)
    {
        QString temp;

        DOMElement* trialElem = doc->createElement(X("trial"));
        ApexTrial* trial = *it;

        //set the id attribute
        trialElem->setAttribute(X("id"), S2X(trial->GetID()));

        //<answer>
        ApexAnswer answer = trial->GetAnswer();
        temp = answer.string();
        if (!temp.isEmpty())
            trialElem->appendChild(XMLutils::CreateTextElement
                                   (doc, "answer", temp));

        //<answer_element>
        temp = trial->GetAnswerElement();
        if (!temp.isEmpty())
            trialElem->appendChild(XMLutils::CreateTextElement
                                   (doc, "answer_element", temp));

        //<screen>
        temp = trial->GetScreen();
        if (!temp.isEmpty())
        {
            DOMElement* screen = doc->createElement(X("screen"));
            screen->setAttribute(X("id"), S2X(temp));
            trialElem->appendChild(screen);
        }

        //<stimulus>
        //NOTE see apextrial.h for tStimulusList
        tStimulusList stimuli = trial->GetStimuli();
        for (tStimulusList::const_iterator itStim = stimuli.begin();
                itStim != stimuli.end(); itStim++)
        {
            QString id = *itStim;
            DOMElement* stimulus = doc->createElement(X("stimulus"));
            stimulus->setAttribute(X("id"), S2X(id));
            trialElem->appendChild(stimulus);
        }

        //<standard>
        tStimulusList standards = trial->GetStandards();
        for (tStimulusList::const_iterator itStan = standards.begin();
                itStan != standards.end(); itStan++)
        {
            QString id = *itStan;
            DOMElement* standard = doc->createElement(X("standard"));
            standard->setAttribute(X("id"), S2X(id));
            trialElem->appendChild(standard);
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
        case ApexProcedureParameters::TYPE_ADAPTIVE:

            typeString = "adaptiveProcedureType";
            break;

        case ApexProcedureParameters::TYPE_CONSTANT:

            typeString = "constantProcedureType";
            break;

        case ApexProcedureParameters::TYPE_TRAINING:

            typeString = "trainingProcedureType";
            break;

        case ApexProcedureParameters::TYPE_MULTI:

            typeString = "multiProcedureType";
            break;

        case ApexProcedureParameters::TYPE_PLUGIN:

            typeString = "pluginProcedureType";
            break;

        default:
            qFatal("WRITER: unknown procedure type (%i) \
                   returned from ApexProcedureParameters.", type);
    }

    return typePrefix + ":" + typeString;
}

QString ProceduresWriter::getOrderString(int order)
{
    QString orderString;

    switch (order)
    {
        case ApexProcedureParameters::ORDER_RANDOM:

            orderString = "random";
            break;

        case ApexProcedureParameters::ORDER_SEQUENTIAL:

            orderString = "sequential";
            break;

        case ApexProcedureParameters::ORDER_ONEBYONE:

            orderString = "onebyone";
            break;

        default:
            qFatal("WRITER: unknown order (%i) returned \
                   from ApexProcedureParameters.", order);
    }

    return orderString;
}

bool ProceduresWriter::isMultiProcedure(const ApexProcedureConfig& data)
{
    return getTypeString(data.GetParameters().GetType())
           .contains("multiProcedureType");
}




























