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

#include "apexdata/parameters/parameter.h"

#include "parameters/parametermanager.h"

#include "stimulus/filter.h"

namespace apex
{
    namespace stimulus
    {

        void Filter::SetParameters ( ParameterManager* pm )
        {
            // TODO:
            // get our own parametrs from parametermanager
            // call setparameter for each

            data::ParameterValueMap params = pm->parametersForOwner ( GetID() );

            for ( data::ParameterValueMap::const_iterator it=params.begin(); it!=params.end(); ++it )
            {
                SetParameter ( it.key().type(), it.key().channel(), it.value() );
            }
        }


        bool Filter::SetParameter ( data::Parameter name, QVariant value) {
            if (name.owner()!=GetID())
                return false;
            SetParameter(name.type(), name.channel(), value);
            return true;


        }


    }
}


