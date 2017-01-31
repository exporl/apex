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

#ifndef _EXPORL_SRC_LIB_APEXDATA_PARAMETERS_PARAMETERMANAGERDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_PARAMETERS_PARAMETERMANAGERDATA_H_

#include "apextools/global.h"

#include <QMap>

namespace apex
{

namespace data
{

class Parameter;

typedef QMap<QString,Parameter> IdParameterMap;

class APEXDATA_EXPORT ParameterManagerData
{
    public:

        void registerParameter(const QString& id, const Parameter& pn);
        Parameter parameterById(const QString& id) const;

        void showContents() const;

        bool operator==(const ParameterManagerData& other) const;

    private:

        //! Connects parameter IDs to internal unique parameter names
        IdParameterMap idMap;
};


}





}




#endif

