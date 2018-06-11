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

#ifndef _EXPORL_SRC_LIB_APEXDATA_FILTER_FILTERDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_FILTER_FILTERDATA_H_

#include "../parameters/simpleparameters.h"

namespace apex
{

namespace data
{

/**
 * @author Tom Francart,,, <tom.francart@med.kuleuven.be>
 */
class APEXDATA_EXPORT FilterData : public SimpleParameters
{
public:
    FilterData();
    virtual ~FilterData();

    QString device() const;
    QString type() const;
    unsigned numberOfChannels() const;
    virtual bool isGenerator() const;
};

} // ns data

} // ns apex

#endif
