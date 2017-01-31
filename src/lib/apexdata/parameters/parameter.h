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

#ifndef PARAMETERNAMEH
#define PARAMETERNAMEH

#include <QString>
#include <QMap>
#include <QVariant>

//from libtools
#include "global.h"

namespace apex
{
namespace data
{

struct ParameterPrivate;

const int NO_CHANNEL = -1;

/**
 * Used to refer in a unique way to a parameter, even if it has no ID
 */
class APEXDATA_EXPORT Parameter
{

public:

    Parameter();
    Parameter(const Parameter& other);
    Parameter(const QString& owner,
              const QString& type,
              const QVariant& defaultValue,
              int channel,
              bool hasID,
              const QString& id = QString());
    ~Parameter();

    //getters
    QString owner() const;
    QString type() const;
    QVariant defaultValue() const;
    int channel() const;
    bool hasChannel() const;
    bool hasId() const;
    QString id() const;

    //setters
    void setDefaultValue(const QVariant& value);
    void setId(const QString& id);
    void setOwner(const QString& owner);
    void setType(const QString& type);

    QString toString() const;
    bool isValid() const;

    Parameter& operator=(const Parameter& other);
    bool operator<(const Parameter& p) const;
    bool operator==(const Parameter& other) const;

private:

    ParameterPrivate* d;
};


struct APEXDATA_EXPORT ValueReset
{
    QVariant value;
    bool    reset;

    ValueReset();

    ValueReset(const QVariant& v, bool r);
};

typedef QMap<Parameter, QVariant> ParameterValueMap;
typedef QMap<Parameter, ValueReset> PMRuntimeSettings;

}// ns data
}        // ns apex


#endif
