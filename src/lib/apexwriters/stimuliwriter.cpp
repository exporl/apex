/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "apexdata/stimulus/stimulidata.h"

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "stimuliwriter.h"

#include <QDebug>
#include <QStringList>

using namespace apex;
using namespace apex::writer;

QDomElement StimuliWriter::addElement(QDomDocument *doc,
                                      const data::StimuliData &data)
{
    QDomElement rootElem = doc->documentElement();

    QDomElement stimuli = doc->createElement(QSL("stimuli"));
    rootElem.appendChild(stimuli);

    // fixed_parameters
    QStringList fixed_parameters = data.GetFixedParameters();

    if (fixed_parameters.count() > 0)
        stimuli.appendChild(addFixedParameters(doc, fixed_parameters));

    // add all stimuli
    Q_FOREACH (data::StimulusData stimulus, data)
        stimuli.appendChild(addStimulus(doc, stimulus));

    if (data.hasPluginStimuli())
        stimuli.appendChild(doc->createElement(QSL("pluginstimuli")));

    return stimuli;
}

QDomElement StimuliWriter::addFixedParameters(QDomDocument *doc,
                                              const QStringList &params)
{
    Q_ASSERT(params.count() > 0);

    QDomElement fixed_parameters = doc->createElement(QSL("fixed_parameters"));

    Q_FOREACH (QString parameter, params) {
        QDomElement param = doc->createElement(QSL("parameter"));
        param.setAttribute(QSL("id"), parameter);
        fixed_parameters.appendChild(param);
    }

    return fixed_parameters;
}

QDomElement StimuliWriter::addStimulus(QDomDocument *doc,
                                       const data::StimulusData &data)
{
    QDomElement stimulus = doc->createElement(QSL("stimulus"));
    stimulus.setAttribute(QSL("id"), data.GetID());

    // description
    QString description = data.description();
    if (!description.isEmpty()) {
        stimulus.appendChild(
            XmlUtils::createTextElement(doc, "description", description));
    }

    // datablocks
    stimulus.appendChild(addDatablocks(doc, data.GetDatablocksContainer()));

    // variableParameters
    StimulusParameters varParams = data.GetVariableParameters();

    if (!varParams.isEmpty()) {
        stimulus.appendChild(
            addParameters(doc, varParams, "variableParameters"));
    }

    // fixedParameters
    StimulusParameters fixedParams = data.GetFixedParameters();

    if (!fixedParams.isEmpty()) {
        stimulus.appendChild(
            addParameters(doc, fixedParams, "fixedParameters"));
    }

    return stimulus;
}

QDomElement
StimuliWriter::addDatablocks(QDomDocument *doc,
                             const data::StimulusDatablocksContainer &data)
{
    using data::StimulusDatablocksContainer;

    QDomElement element = doc->createElement(data.typeName());

    switch (data.type) {
    case StimulusDatablocksContainer::DATABLOCKS:

        Q_ASSERT(data.count() <= 1);
        Q_ASSERT(data.id.isEmpty());

        if (!data.isEmpty())
            element.appendChild(addDatablocks(doc, data[0]));

        break;

    case StimulusDatablocksContainer::DATABLOCK:

        Q_ASSERT(!data.id.isEmpty());

        element.setAttribute(QSL("id"), data.id);
        break;

    case StimulusDatablocksContainer::SEQUENTIAL:
    case StimulusDatablocksContainer::SIMULTANEOUS:

        Q_ASSERT(data.id.isEmpty());

        Q_FOREACH (StimulusDatablocksContainer datablock, data)
            element.appendChild(addDatablocks(doc, datablock));

        break;

    default:
        qFatal("stimulus writer: unknown datablocks type");
    }

    return element;
}

QDomElement StimuliWriter::addParameters(QDomDocument *doc,
                                         const data::StimulusParameters &params,
                                         const QString &name)
{
    QDomElement element = doc->createElement(name);

    QMap<QString, QVariant>::const_iterator it;
    for (it = params.map().begin(); it != params.map().end(); it++) {
        QDomElement parameter = XmlUtils::createRichTextElement(
            doc, "parameter", it.value().toString());
        parameter.setAttribute(QSL("id"), it.key());
        element.appendChild(parameter);
    }

    return element;
}
