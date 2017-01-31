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

#ifndef APEXAPEXCONSTANTPROCEDUREPARAMETERS_H
#define APEXAPEXCONSTANTPROCEDUREPARAMETERS_H

//from libdata
#include "procedure/apexprocedureparameters.h"

#include <map>

namespace apex
{
namespace data
{
/**
@author Tom Francart,,,
*/
    class APEXDATA_EXPORT ApexConstantProcedureParameters : public ApexProcedureParameters
{
    public:
        ApexConstantProcedureParameters(XERCES_CPP_NAMESPACE::DOMElement* p_paramElement);
        ApexConstantProcedureParameters();
        ~ApexConstantProcedureParameters();

        virtual t_type GetType() const;

        friend class ApexConstantProcedure;

    private:
        virtual bool SetParameter(const QString& p_name, const QString& p_id, const QString& p_value, XERCES_CPP_NAMESPACE::DOMElement*);

};

}
}

#endif
