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

#include "apexconstantprocedureparameters.h"
#include <iostream>

#include "xml/xercesinclude.h"
using namespace xercesc;
using namespace apex::data;

ApexConstantProcedureParameters::ApexConstantProcedureParameters(DOMElement* p_paramElement)
        : ApexProcedureParameters(p_paramElement)
{
}


ApexConstantProcedureParameters::ApexConstantProcedureParameters():
        ApexProcedureParameters()
{

}

ApexConstantProcedureParameters::~ApexConstantProcedureParameters()
{
}

t_type ApexConstantProcedureParameters::GetType() const
{
	return TYPE_CONSTANT;
}

bool ApexConstantProcedureParameters::SetParameter(const  QString & p_name,const  QString & p_id,const  QString & p_value, DOMElement* d)
{
        if ( ApexProcedureParameters::SetParameter(p_name, p_id, p_value,d) == true)
                return true;



        std::cerr<< "ApexConstantProcedureParameters::SetParameter: unknown tag " << p_name.toAscii().data()<< std::endl;
        return false;
}



