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

#include "apexdata/procedure/adaptiveproceduredata.h"
#include "apexdata/procedure/constantproceduredata.h"
#include "apexdata/procedure/multiproceduredata.h"
#include "apexdata/procedure/scriptproceduredata.h"
#include "apexdata/procedure/trainingproceduredata.h"

#include "apextools/exceptions.h"

#include "apextools/xml/xmlkeys.h"
#include "apextools/xml/xmltools.h"

#include "adaptiveprocedureparser.h"
#include "constantprocedureparser.h"
#include "multiproceduredataparser.h"
#include "multiprocedureparser.h"
#include "scriptprocedureparser.h"
#include "trainingprocedureparser.h"

using namespace apex::XMLKeys;

namespace apex
{

namespace parser
{

MultiProcedureParser::MultiProcedureParser()
{
}

data::MultiProcedureData *MultiProcedureParser::parse(const QDomElement &base)
{
    data::MultiProcedureData *config = new data::MultiProcedureData();
    currentConfig = config;
    currentConfig->SetID(base.attribute(gc_sID));

    Q_CHECK_PTR(currentConfig);

    // parse different procedures and add to config
    for (QDomElement currentNode = base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();
        if (tag == "parameters") {
            SetProcedureParameters((QDomElement)currentNode);
        } else if (tag == "procedure") {
            const QString type = currentNode.attribute(QSL("xsi:type"));
            data::ProcedureData *data;

            // FIXME This code only works for 5 types of procedures. If a user
            // defines a new plugin with new procedures, this code will fail
            // because no corresponding parser can be found.
            // In fact, there is a class ExperimentParser with a function
            // ParseProcedure that will look for every parser of procedure in a
            // plugin. So this function should be used, but this function is in
            // apexmain, while we are in apexprocedures.
            // Potential solution: use the same strategy as the function
            // initProcedures in MultiProcedures does, in more detail: the call
            // to the api. So MultiProcedureParser should have a parent that
            // exposes an api that contains a method that can return
            // ProcedureData
            // for a given DOM element. So this api should use a function like
            // ParseProcedure of ExperimentParser.
            ProcedureParsersParent *parser = NULL;

            if (type == "apex:constantProcedure") {
                parser = new ConstantProcedureParser();
                data = new data::ConstantProcedureData();
            } else if (type == "apex:pluginProcedure") {
                parser = new ScriptProcedureParser();
                data = new data::ScriptProcedureData();
            } else if (type == "apex:adaptiveProcedure") {
                parser = new AdaptiveProcedureParser();
                data = new data::AdaptiveProcedureData();
            } else if (type == "apex:trainingProcedure") {
                parser = new TrainingProcedureParser();
                data = new data::TrainingProcedureData();
            } else if (type == "apex:multiProcedure") {
                parser = new MultiProcedureParser();
                data = new data::MultiProcedureData();
            } else {
                qFatal("Unknown procedure type");
            }

            parser->Parse(currentNode, data);
            config->addProcedure(data);
        }
    }

    currentConfig = 0;
    return config;
}

bool MultiProcedureParser::trialsValid()
{
    return true;
}

void MultiProcedureParser::SetProcedureParameters(const QDomElement &p_base)
{
    data::MultiProcedureData *param =
        dynamic_cast<data::MultiProcedureData *>(currentConfig);
    parser::MultiProcedureDataParser parser;
    parser.Parse(p_base, param);

    currentConfig = param;
}
}
}
