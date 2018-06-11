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

#include "filterdata.h"

namespace apex
{

namespace data
{

FilterData::FilterData()
{
}

FilterData::~FilterData()
{
}

QString FilterData::device() const
{
    return valueByType(QString("device")).toString();
}

QString FilterData::type() const
{
    return valueByType(QString("type")).toString();
}

unsigned int FilterData::numberOfChannels() const
{
    return valueByType(QString("channels")).toInt();
}

bool FilterData::isGenerator() const
{
    return false;
}

} // ns data

} // ns apex
