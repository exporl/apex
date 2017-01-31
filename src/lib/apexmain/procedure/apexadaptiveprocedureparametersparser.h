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

#ifndef APEXADAPTIVEPROCEDUREPARAMETERSPARSERH
#define APEXADAPTIVEPROCEDUREPARAMETERSPARSERH

#include "apexprocedureparametersparser.h"

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
class DOMElement;
};


namespace apex
{
namespace data
{
class ApexAdaptiveProcedureParameters;
}

namespace parser
{

class ApexAdaptiveProcedureParametersParser: public ApexProcedureParametersParser
{
    public:

        ApexAdaptiveProcedureParametersParser();

        /**
        * Parse the given xml data structure into the given parameters structure
        * does not take ownership of p_parameters
         */
        virtual void Parse(XERCES_CPP_NAMESPACE::DOMElement* p_base,
                   data::ApexAdaptiveProcedureParameters* p_parameters);

        bool CheckParameters(data::ApexAdaptiveProcedureParameters* param);

    protected:
        virtual bool SetParameter(const QString tag,
                                  const QString id, const QString value ,
                                  XERCES_CPP_NAMESPACE::DOMElement* node);


    private:
        bool ParseStepSizes(XERCES_CPP_NAMESPACE::DOMElement* p_base);



};


}
}



#endif
