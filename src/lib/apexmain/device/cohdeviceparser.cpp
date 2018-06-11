/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexdata/device/cohdevicedata.h"

#include "apexdata/map/apexmap.h"

#include "apextools/exceptions.h"

#include "map/mapfactory.h"

#include "cohdeviceparser.h"

namespace apex
{

namespace parser
{

CohDeviceParser::CohDeviceParser() : SimpleParametersParser()
{
}

CohDeviceParser::~CohDeviceParser()
{
}

void CohDeviceParser::AddParameter(data::SimpleParameters *p,
                                   const QDomElement &e, const QString &owner,
                                   const QString &type, const QString &id,
                                   const QVariant &value, const int channel)
{
    SimpleParametersParser::AddParameter(p, e, owner, type, id, value, channel);

    data::CohDeviceData *q = dynamic_cast<data::CohDeviceData *>(p);
    Q_ASSERT(q);

    if (type == "defaultmap") {
        MapFactory mapfac;
        data::ApexMap *pmap = mapfac.GetMap(e);
        if (!pmap)
            throw ApexStringException("Could not parse map for " + p->id());
        q->setMap(pmap);
        // the following are handled by SimpleParametersParser
    } else if (type == "trigger") {
    } else if (type == "powerup_count") {
    } else if (type == "device") {
    } else if (type == "volume") {
    } else {
        throw ApexStringException("Unknown tag " + type +
                                  " in CohDeviceData::SetParameter");
    }
}
}
}
