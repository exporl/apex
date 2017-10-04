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

#include "apexdata/procedure/proceduredata.h"
#include "apexdata/procedure/trialdata.h"

#include "apextools/exceptions.h"

#include "apextools/xml/xmlkeys.h"

#include "procedureparsersparent.h"
#include "trialparser.h"

#include <QDomElement>

using namespace apex::XMLKeys;

namespace apex
{
namespace parser
{

ProcedureParsersParent::ProcedureParsersParent() : currentConfig(NULL)
{
}

void ProcedureParsersParent::Parse(const QDomElement &p_base,
                                   data::ProcedureData *c)
{
    currentConfig = c;

    ParseTrials(p_base);
    c->SetID(p_base.attribute(gc_sID));

    QDomElement parametersNode =
        p_base.elementsByTagName("parameters").item(0).toElement();
    SetProcedureParameters(parametersNode);

    currentConfig = 0;
}

void ProcedureParsersParent::ParseTrials(const QDomElement &p_base)
{
    Q_ASSERT(currentConfig);
    // parse trials block
    QDomNode trialNode = p_base.elementsByTagName("trials").item(0).toElement();

    TrialParser trialFactory;
    for (QDomElement currentNode = trialNode.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();
        if (tag == "trial") {
            currentConfig->AddTrial(trialFactory.GetTrial(currentNode));
        } else if (tag == "plugintrials") {
            currentConfig->setHasPluginTrials(true);
        } else {
            throw ApexStringException(tr("ProcedureParsersParent::ParseTrials:"
                                         " unknown tag %1")
                                          .arg(tag));
        }
    }

    if (!trialsValid()) {
        throw ApexStringException(
            tr("For this procedure, there is an incorrect"
               " number of trials, or some required"
               " information is missing from the trials."));
    }
}
}
}
