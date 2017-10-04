/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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
#include "apexdata/stimulus/stimulusdata.h"
#include "apexdata/stimulus/stimulusparameters.h"

#include "apextools/exceptions.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "parser/scriptexpander.h"

#include "playmatrix.h"
#include "stimuliparser.h"

using apex::data::StimulusParameters;
using apex::data::StimuliData;
using apex::data::StimulusDatablocksContainer;
using namespace apex::XMLKeys;

namespace apex
{
namespace parser
{

StimuliParser::StimuliParser(QWidget *parent) : currentData(0), m_parent(parent)
{
}

void StimuliParser::Parse(const QString &fileName, const QDomElement &p_base,
                          data::StimuliData *c,
                          const QString &scriptLibraryFile,
                          const QVariantMap &scriptParameters, bool expand)
{
    currentData = c;

    if (expand) {
        // find plugin stimuli and expand them
        for (QDomElement currentNode = p_base.firstChildElement();
             !currentNode.isNull();
             currentNode = currentNode.nextSiblingElement()) {
            const QString tag(currentNode.tagName());
            if (tag == "pluginstimuli") {
                ScriptExpander expander(fileName, scriptLibraryFile,
                                        scriptParameters, m_parent);
                expander.ExpandScript(currentNode, "getStimuli");
            }
        }
    }

    for (QDomElement currentNode = p_base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();
        if (tag == "stimulus") {
            ParseStimulus(currentNode);
        } else if (tag == "pluginstimuli") {
            c->setHasPluginStimuli(true);
        } else if (tag == "fixed_parameters") {
            // create QStringList from defined parameters
            data::FixedParameterList params;
            for (QDomElement cur = currentNode.firstChildElement();
                 !cur.isNull(); cur = cur.nextSiblingElement()) {
                QString id = cur.attribute(QSL("id"));
                params.push_back(id);
            }
            c->setFixedParameters(params);
        } else {
            throw ApexStringException("Unknown tag: " + tag);
        }
    }

    currentData = 0;
}

void apex::parser::StimuliParser::ParseStimulus(const QDomElement &p_base)
{
    data::StimulusData target;

    target.m_id = p_base.attribute(gc_sID);

    for (QDomElement currentNode = p_base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();
        const QString value = currentNode.text();
        if (tag == "description") {
            target.m_description = value;
        } else if (tag == "datablocks") {
            target.m_datablocksContainer =
                CreateDatablocksContainer(currentNode);
        } else if (tag == "variableParameters") {
            target.m_varParams = CreateStimulusParameters(currentNode);
        } else if (tag == "fixedParameters") {
            target.m_fixParams = CreateStimulusParameters(currentNode);
        } else {
            throw ApexStringException("Unknown tag: " + tag);
        }
    }

    currentData->insert(target.m_id, target);
}

StimulusParameters
StimuliParser::CreateStimulusParameters(const QDomElement &base)
{
    StimulusParameters param;

    for (QDomElement currentNode = base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString id = currentNode.attribute(gc_sID);
        const QString value = XmlUtils::richText(
            currentNode); // can contain markup (eg fixed parameter with <b>)
        param.insert(id, value);
    }

    return param;
}

StimulusDatablocksContainer
StimuliParser::CreateDatablocksContainer(const QDomElement &data)
{
    StimulusDatablocksContainer::Type type;

    QString tag = data.tagName();
    if (tag == "datablocks")
        type = StimulusDatablocksContainer::DATABLOCKS;
    else if (tag == "sequential")
        type = StimulusDatablocksContainer::SEQUENTIAL;
    else if (tag == "simultaneous")
        type = StimulusDatablocksContainer::SIMULTANEOUS;
    else
        qFatal("illegal tag in stimulus datablock");

    StimulusDatablocksContainer container(type);

    for (QDomElement child = data.firstChildElement(); !child.isNull();
         child = child.nextSiblingElement()) {
        QString tag = child.tagName();
        if (tag == "datablock") {
            StimulusDatablocksContainer datablock(
                StimulusDatablocksContainer::DATABLOCK);
            datablock.id = child.attribute(QSL("id"));
            container.append(datablock);
        } else if (tag == "sequential") {
            container.append(CreateDatablocksContainer(child));
        } else if (tag == "simultaneous") {
            container.append(CreateDatablocksContainer(child));
        } else {
            qFatal("illegal tag in stimulus datablock");
        }
    }

    return container;
}
}
}
