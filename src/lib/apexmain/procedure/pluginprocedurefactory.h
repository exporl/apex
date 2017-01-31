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

#ifndef APEXPluginProcedureFactory_H
#define APEXPluginProcedureFactory_H

#include "apexprocedurefactory.h"
#include "pluginprocedure.h"
#include "pluginprocedureparameters.h"


#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
  class DOMElement;
};


namespace apex {

/**
@author Tom Francart,,,
*/
class PluginProcedureFactory : public ApexProcedureFactory
{
public:
    PluginProcedureFactory();

    virtual ~PluginProcedureFactory();

    virtual PluginProcedure* MakeProcedure(ExperimentRunDelegate& p_rd, ApexProcedureConfig* config);

    virtual ApexProcedureConfig* MakeProcedureConfig( XERCES_CPP_NAMESPACE::DOMElement* p_base);

        protected:
            virtual PluginProcedureParameters* MakeProcedureParameters(XERCES_CPP_NAMESPACE::DOMElement* p_paramElement);
};

}

#endif
