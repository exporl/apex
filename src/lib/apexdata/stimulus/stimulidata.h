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

#ifndef STIMULIDATAH
#define STIMULIDATAH

#include "stimulusdata.h"

#include "global.h"

#include <QMap>
#include <QStringList>

namespace apex
{
class StatusReporter;

namespace data
{

typedef  QStringList FixedParameterList;


class APEXDATA_EXPORT StimuliData:
            public QMap<QString,StimulusData>
{
    public:
        void setFixedParameters(FixedParameterList p);
        /** Check whether each fixed parameter is defined in each stimulus.
         * if so, return true, otherwise return false
         **/
        bool CheckFixedParameters(StatusReporter* err);

        const FixedParameterList& GetFixedParameters() const;
        
        bool operator==(const StimuliData& other) const;

    private:
        FixedParameterList m_fixedParameters;
};

}
}




#endif
