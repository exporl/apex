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

#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_PARAMETERDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_PARAMETERDATA_H_

#include <QString>

#include "apextools/global.h"

namespace apex
{
namespace data
{

// FIXME [job refactory] make this a class with private data
struct APEXDATA_EXPORT ParameterData {
    ParameterData(const QString &p_id, const QString &p_name,
                  const QString &p_expression)
        : id(p_id), name(p_name), expression(p_expression){};

    ParameterData(){};

    const QString GetName() const
    {
        if (name.isEmpty())
            return id;
        else
            return name;
    }
    QString id;
    QString name;
    QString expression;

    bool operator==(const ParameterData &other) const
    {
        return id == other.id && name == other.name &&
               expression == other.expression;
    }
};
}
}

#endif
