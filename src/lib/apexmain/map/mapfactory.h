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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_MAP_MAPFACTORY_H_
#define _EXPORL_SRC_LIB_APEXMAIN_MAP_MAPFACTORY_H_

#include "r126nucleusmaprecord.h"

#include <QCoreApplication>

using namespace r126;

class QDomElement;

namespace apex
{

namespace data
{
class ApexMap;
}

/**
Creates maps based on XML data or the Mapping wizard

@author Tom Francart,,,
*/
class MapFactory
{
    Q_DECLARE_TR_FUNCTIONS(MapFactory)
public:
    MapFactory();

    virtual data::ApexMap* GetMap(const QDomElement &p_base);

    virtual ~MapFactory();

    static data::ApexMap* R126ToApexMap(const R126NucleusMAPRecord* p_map);
};

}

#endif
