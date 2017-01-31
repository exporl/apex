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

#include "pluginprocedurefactory.h"
#include "pluginprocedure.h"
#include "pluginprocedureconfig.h"
#include "pluginprocedureparser.h"
#include "apexpluginprocedureparametersparser.h"
#include "pluginprocedureparameters.h"

#include "xml/xercesinclude.h"
using namespace XERCES_CPP_NAMESPACE;

namespace apex {

    PluginProcedureFactory::PluginProcedureFactory()
    : ApexProcedureFactory()
    {
    }


    PluginProcedureFactory::~PluginProcedureFactory()
    {
    }


    PluginProcedure* PluginProcedureFactory::MakeProcedure (ExperimentRunDelegate& p_rd, ApexProcedureConfig* config)
    {
        Q_CHECK_PTR(config);
        return new PluginProcedure(p_rd, config);
    }


    PluginProcedureParameters * PluginProcedureFactory::MakeProcedureParameters( DOMElement * p_paramElement )
    {
        Q_CHECK_PTR(p_paramElement);
        /*PluginProcedureParameters* param = new PluginProcedureParameters(p_paramElement);
        param->Parse(p_paramElement);*/

        PluginProcedureParameters* param = new PluginProcedureParameters();
        parser::ApexPluginProcedureParametersParser parser;
        parser.Parse(p_paramElement,param);
        return param;
    }

    ApexProcedureConfig * PluginProcedureFactory::MakeProcedureConfig( DOMElement* p_base )
    {
        /*Q_CHECK_PTR(p_base);
        PluginProcedureParameters* param = MakeProcedureParameters(GetParamElement(p_base));
        return new ApexProcedureConfig(param, p_base,this);*/

        PluginProcedureConfig* config = new PluginProcedureConfig();
        parser::PluginProcedureParser parser;
        parser.Parse(p_base,config);
        return config;
    }


}
