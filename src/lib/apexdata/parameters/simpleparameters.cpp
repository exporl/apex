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

//from libdata
#include "simpleparameters.h"

//from libtools
#include "exceptions.h"
#include <apextools.h>

#include <QDebug>

namespace apex
{

namespace data
{

//struct SimpleParametersPrivate

struct SimpleParametersPrivate
{
    ParameterList parameters;
    QString id;
    QString xsiType;

    ParameterList parametersByType(const QString& type) const;
    void printParameters() const;
};


ParameterList SimpleParametersPrivate::parametersByType(const QString& type) const
{
    ParameterList result;

    Q_FOREACH (Parameter param, parameters)
    {
        if (param.type() == type)
            result << param;
    }

    return result;
}

void SimpleParametersPrivate::printParameters() const
{
    Q_FOREACH (Parameter param, parameters)
        qDebug() << param.type() << param.defaultValue();
}


//class SimpleParameters


SimpleParameters::SimpleParameters() : d(new SimpleParametersPrivate())
{
}


SimpleParameters::SimpleParameters(const SimpleParameters& other) :
                                       d(new SimpleParametersPrivate(*other.d))
{
}


SimpleParameters::~SimpleParameters()
{
    delete d;
}

QVariant SimpleParameters::valueByType(const QString& type) const
{
    return parameterByType(type).defaultValue();
}

Parameter SimpleParameters::parameterByType(const QString& type) const
{
    Parameter result;
    ParameterList params = d->parametersByType(type);

    if (params.size() == 1)
        result = params.first();
    else if (params.size() > 1)
    {
        throw ApexStringException(QObject::tr(
                    "Error: found more than one value for parameter type %1")
                            .arg(type));
    }

    return result;

}

void SimpleParameters::setValueByType(const QString& type, const QVariant& value)
{
    //qDebug() << "setValueByType" << type << value;

    if (hasParameter(type))
    {
        int i = d->parameters.indexOf(parameterByType(type)); //can throw
        d->parameters[i].setDefaultValue(value);
    }
    else
    {
        //qDebug() << this << ": adding parameter (set) " << type;
        d->parameters.append(Parameter(id(), type, value, NO_CHANNEL, false));
    }
}

bool SimpleParameters::hasParameter(const QString& type) const
{
    Q_FOREACH (Parameter param, d->parameters)
    {
        if (param.type() == type)
            return true;
    }

    return false;
}

ParameterList SimpleParameters::parameters() const
{
    return d->parameters;
}


void SimpleParameters::addParameter(const Parameter& param)
{
    //qDebug() << "addParameter" << param.type() << param.defaultValue();
    Parameter newParam = param; //copy to set owner
    newParam.setOwner(id());

    if (!hasParameter(newParam.type()))
    {
        //qDebug() << this << ": adding parameter (add)" << param.type() << param.defaultValue();
        d->parameters.append(newParam);
    }
    else
    {
        ParameterList params = d->parametersByType(newParam.type());
        Q_ASSERT(!params.isEmpty());

        bool added = false;

        //if the given parameter has channel NO_CHANNEL, remove all existing
        //parameters of its type and add the parameter
        if (newParam.channel() == NO_CHANNEL)
        {
            Q_FOREACH (Parameter param, params)
                d->parameters.removeAll(param);

            d->parameters.append(newParam);
            added = true;
        }

        for (int i = 0; i < params.size() && !added; i++)
        {
            int channel = params[i].channel();

            if (channel == NO_CHANNEL || channel == newParam.channel())
            {
                if (channel == NO_CHANNEL)
                    Q_ASSERT(params.size() == 1);

                //if channel == NO_CHANNEL we have to overwrite it since for
                //a given param type there cannot be a param global to all
                //channels and a param for one specific channel.
                //if channel == newParam.channel() we have to overwrite it too
                //since there cannot be two params of the same type and for
                //the same channel.
                d->parameters.removeAll(params[i]);
                d->parameters.append(newParam);
                added = true;
            }
        }

        if (!added)
        {
            //none of the parameters of the same type as the given parameter
            //have the same channel; append the given parameter
            d->parameters.append(newParam);
        }
    }
}


void SimpleParameters::removeParameter(const Parameter& param)
{
    //qDebug() << this << ": removing parameter " << param.type();
    d->parameters.removeAll(param);
}

SimpleParameters& SimpleParameters::operator=(const SimpleParameters& other)
{
    if (this != &other)
        *d = *other.d;

    return *this;
}


bool SimpleParameters::operator==(const SimpleParameters& other) const
{
    return  ApexTools::haveSameContents(d->parameters, other.d->parameters) &&
            d->id == other.d->id &&
            d->xsiType == other.d->xsiType;
}

QString SimpleParameters::id() const
{
    //Q_ASSERT(!d->id.isEmpty());
    return d->id;
}

void SimpleParameters::setId(const QString& id)
{
    d->id = id;

    for (int i = 0; i < d->parameters.size(); i++)
        d->parameters[i].setOwner(id);
}

void SimpleParameters::setXsiType(const QString& type)
{
    d->xsiType = type;
}

QString SimpleParameters::xsiType() const
{
    Q_ASSERT(!d->xsiType.isEmpty());
    return d->xsiType;
}

}
}


