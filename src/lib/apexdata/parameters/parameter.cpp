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

#include "parameter.h"

namespace apex
{

namespace data
{

struct ParameterPrivate {
    QString owner;
    QString type;
    QVariant defaultValue;
    int channel;
    QString id;

    //! true if the parameter has an ID and can thus be changed during the
    //! experiment
    bool hasID;
};

} // ns data

} // ns apex

using namespace apex::data;

Parameter::Parameter() : d(new ParameterPrivate())
{
    d->channel = NO_CHANNEL;
}

Parameter::~Parameter()
{
    delete d;
}

Parameter::Parameter(const Parameter &other) : d(new ParameterPrivate(*other.d))
{
}

Parameter::Parameter(const QString &owner, const QString &type,
                     const QVariant &defaultValue, int channel, bool hasId,
                     const QString &id)
    : d(new ParameterPrivate())
{
    d->owner = owner;
    d->type = type;
    d->defaultValue = defaultValue;
    d->channel = channel;
    d->hasID = hasId;
    d->id = id;
}

QString Parameter::owner() const
{
    return d->owner;
}

QString Parameter::type() const
{
    return d->type;
}

void Parameter::setDefaultValue(const QVariant &value)
{
    d->defaultValue = value;
}

void Parameter::setId(const QString &id)
{
    d->id = id;
    d->hasID = true;
}

void Parameter::setOwner(const QString &owner)
{
    d->owner = owner;
}

void Parameter::setType(const QString &type)
{
    d->type = type;
}

QVariant Parameter::defaultValue() const
{
    return d->defaultValue;
}

int Parameter::channel() const
{
    return d->channel;
}

bool Parameter::hasChannel() const
{
    return d->channel != NO_CHANNEL;
}

bool Parameter::hasId() const
{
    return d->hasID;
}

QString Parameter::id() const
{
    return d->id;
}

QString Parameter::toString() const
{
    QString c;

    if (hasChannel())
        c = "-" + QString::number(d->channel);

    return d->owner + "-" + d->type + c;
}

bool Parameter::isValid() const
{
    return !d->type.isEmpty() && !(d->channel < NO_CHANNEL);
}

Parameter &Parameter::operator=(const Parameter &other)
{
    if (this != &other)
        *d = *other.d;

    return *this;
}

bool Parameter::operator<(const Parameter &p) const
{
    return toString() < p.toString(); // FIXME: not efficient?
}

bool Parameter::operator==(const Parameter &other) const
{
    return d->owner == other.d->owner && d->type == other.d->type &&
           d->defaultValue == other.d->defaultValue &&
           d->channel == other.d->channel && d->hasID == other.d->hasID &&
           d->id == other.d->id;
}

ValueReset::ValueReset() : reset(true)
{
}

ValueReset::ValueReset(const QVariant &v, bool r) : value(v), reset(r)
{
}
