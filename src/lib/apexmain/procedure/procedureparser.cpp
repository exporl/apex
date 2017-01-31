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

#include "adaptiveprocedureparser.h"
#include "constantprocedureparser.h"
#include "exceptions.h"
#include "multiprocedureparser.h"
#include "pluginprocedureparser.h"
#include "procedureparser.h"
#include "procedureparsersparent.h"
#include "trainingprocedureparser.h"

#include "xml/xercesinclude.h"

//from libdata
#include "procedure/apexmultiprocedureconfig.h"
#include "procedure/apexprocedureconfig.h"

using namespace xercesc;

#include "xml/xmlkeys.h"
#include "xml/apexxmltools.h"
using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;

namespace apex
{
namespace parser
{

ProcedureParser::ProcedureParser(QWidget* parent) :
    m_parent(parent)
{
}

data::ApexProcedureConfig* ProcedureParser::Parse ( XERCES_CPP_NAMESPACE::DOMElement* p_base,
        const QString& scriptLibraryFile, const QVariantMap& scriptParameters)
{
    QString procType = XMLutils::GetAttribute ( p_base, "xsi:type" );
    data::ApexProcedureConfig* conf;

    if ( procType == "apex:adaptiveProcedureType" )
    {
        AdaptiveProcedureParser parser(m_parent);
        conf = new data::ApexProcedureConfig();
        parser.Parse ( p_base, conf, scriptLibraryFile, scriptParameters);
    }
    else if ( procType == "apex:constantProcedureType" )
    {
        ConstantProcedureParser parser(m_parent);
        conf = new data::ApexProcedureConfig();
        parser.Parse ( p_base, conf, scriptLibraryFile, scriptParameters);
    }
    else if ( procType == "apex:trainingProcedureType" )
    {
        TrainingProcedureParser parser(m_parent);
        conf = new data::ApexProcedureConfig();
        parser.Parse ( p_base, conf, scriptLibraryFile, scriptParameters);
    }
    else if ( procType == "apex:multiProcedureType" )
    {
        MultiProcedureParser parser(m_parent);
        data::ApexMultiProcedureConfig* multiConf = new data::ApexMultiProcedureConfig();
        parser.Parse( p_base, multiConf, scriptLibraryFile, scriptParameters);
        multiConf->ShowChildren();
        return multiConf;
    }
    else if ( procType == "apex:pluginProcedureType" )
    {
        PluginProcedureParser parser(m_parent);
        conf = new data::ApexProcedureConfig();
        parser.Parse ( p_base, conf, scriptLibraryFile, scriptParameters);
    }
    else
    {
        throw ( ApexStringException (
                    QString ( "PARSER: unknowns procedure type: %1" )
                    .arg ( procType ) ) );
    }

    return conf;
}

}//ns parser
}//ns apex


























