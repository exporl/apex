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

#include "apexdata/stimulus/stimulidata.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"

#include "stimuliwriter.h"

#include <QDebug>
#include <QStringList>

using namespace XERCES_CPP_NAMESPACE;
using namespace apex::ApexXMLTools;
using namespace apex;
using namespace apex::writer;

DOMElement* StimuliWriter::addElement(DOMDocument *doc,
                                      const data::StimuliData &data)
{
    DOMElement* rootElem = doc->getDocumentElement();

    DOMElement*  stimuli = doc->createElement(X("stimuli"));
    rootElem->appendChild(stimuli);

    //fixed_parameters
    QStringList fixed_parameters = data.GetFixedParameters();

    if (fixed_parameters.count() > 0)
        stimuli->appendChild(addFixedParameters(doc, fixed_parameters));

    //add all stimuli
    Q_FOREACH(data::StimulusData stimulus, data)
        stimuli->appendChild(addStimulus(doc, stimulus));

    return stimuli;
}

DOMElement* StimuliWriter::addFixedParameters(DOMDocument *doc,
        const QStringList& params)
{
    Q_ASSERT(params.count() > 0);

    DOMElement *fixed_parameters = doc->createElement(X("fixed_parameters"));

    Q_FOREACH(QString parameter, params)
    {
        DOMElement *param = doc->createElement(X("parameter"));
        param->setAttribute(X("id"), S2X(parameter));
        fixed_parameters->appendChild(param);
    }

    return fixed_parameters;
}

DOMElement* StimuliWriter::addStimulus(DOMDocument *doc,
                                       const data::StimulusData& data)
{
    DOMElement *stimulus = doc->createElement(X("stimulus"));
    stimulus->setAttribute(X("id"), S2X(data.GetID()));

    //description
    QString description = data.description();
    if (!description.isEmpty())
    {
        stimulus->appendChild(XMLutils::CreateTextElement(doc, "description",
                              description));
    }

    //datablocks
    stimulus->appendChild(addDatablocks(doc, data.GetDatablocksContainer()));

    //variableParameters
    StimulusParameters varParams = data.GetVariableParameters();

    if (!varParams.isEmpty())
    {
        stimulus->appendChild(addParameters(doc, varParams,
                              "variableParameters"));
    }

    //fixedParameters
    StimulusParameters fixedParams = data.GetFixedParameters();

    if (!fixedParams.isEmpty())
    {
        stimulus->appendChild(addParameters(doc, fixedParams,
                              "fixedParameters"));
    }

    return stimulus;
}

DOMElement* StimuliWriter::addDatablocks(DOMDocument *doc,
        const data::StimulusDatablocksContainer& data)
{
    using data::StimulusDatablocksContainer;

    DOMElement *element = doc->createElement(S2X(data.typeName()));

    switch (data.type)
    {
        case StimulusDatablocksContainer::DATABLOCKS:

            Q_ASSERT(data.count() <= 1);
            Q_ASSERT(data.id.isEmpty());

            if (!data.isEmpty())
                element->appendChild(addDatablocks(doc, data[0]));

            break;

        case StimulusDatablocksContainer::DATABLOCK:

            Q_ASSERT(!data.id.isEmpty());

            element->setAttribute(X("id"), S2X(data.id));
            break;

        case StimulusDatablocksContainer::SEQUENTIAL:
        case StimulusDatablocksContainer::SIMULTANEOUS:

            Q_ASSERT(data.id.isEmpty());

            Q_FOREACH(StimulusDatablocksContainer datablock, data)
                element->appendChild(addDatablocks(doc, datablock));

            break;

        default:
            qFatal("stimulus writer: unknown datablocks type");
    }

    return element;
}

DOMElement* StimuliWriter::addParameters(DOMDocument *doc,
                                    const data::StimulusParameters& params,
                                    const QString& name)
{
    DOMElement *element = doc->createElement(S2X(name));

    QMap<QString,QVariant>::const_iterator it;
    for (it = params.map().begin(); it != params.map().end(); it++)
    {
        DOMElement *parameter = XMLutils::CreateTextElement(doc,
                "parameter", it.value().toString());
        parameter->setAttribute(X("id"), S2X(it.key()));
        element->appendChild(parameter);
    }

    return element;
}

























