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

#include "parametermanager.h"
#include "services/errorhandler.h"

#include <QDebug>

namespace apex
{

ParameterManager::ParameterManager(const data::ParameterManagerData& d) :
                                                                    data(d)
{
}

QVariant ParameterManager::parameterValue (const QString &id) const
{
    data::Parameter name(data.parameterById(id));
    if (!name.isValid())
    {
        qDebug() << "ParameterManager: cannot get parameter" << id;
        return QVariant();
    }

    if (paramValues.contains(name))
        return paramValues.value(name).value;

    return name.defaultValue();
}

data::ParameterValueMap ParameterManager::parametersForOwner(const QString& owner) const
{
    data::ParameterValueMap result;

    data::PMRuntimeSettings::const_iterator it;
    for (it = paramValues.begin(); it != paramValues.end(); ++it)
    {
            if (it.key().owner() == owner)
                    result.insert(it.key(), it.value().value);
    }

    return result;
}



void ParameterManager::setParameter(const QString& id, const QVariant& value,
                                    bool canReset)
{
    data::Parameter name(data.parameterById(id));

    if (!name.isValid())
    {
        qDebug() << "ParameterManager::SetParameter: cannot set parameter"
                 << id << "to value" << value.toString();

        apex::StatusItem info(StatusItem::Error, "ParameterManager",
                          "unknown parameter: " + id);
        showContents();
    }
    else
        paramValues.insert(name,data::ValueReset(value,canReset));
}

void ParameterManager::registerParameter(const QString& id,
                                         const data::Parameter& name)
{
    data.registerParameter(id, name);
}


void ParameterManager::showContents() const
{
    data.showContents();

    qDebug() << "Registered values:";
    qDebug() << "==================";

    data::PMRuntimeSettings::const_iterator it;
    for (it = paramValues.begin(); it != paramValues.end(); ++it)
    {
        qDebug().nospace() << "name=" << it.key().toString()
                           << ", value=" << it.value().value.toString();
    }

    qDebug() << "==================";
}

void ParameterManager::reset()
{
    QMutableMapIterator<data::Parameter,data::ValueReset> it(paramValues);
    while (it.hasNext())
    {
        it.next();
        if (it.value().reset)
            it.remove();
    }
}

void ParameterManager::forceReset()
{
    paramValues.clear();
}

void ParameterManager::resetParameter(const QString& id)
{
    paramValues.remove(data.parameterById(id));
    // we set the parameter explicitly to its default value
    // if we would just remove it from paramValues, the default parameter might not be reset
//     data::Parameter name( data.parameterById(id));
//     paramValues[name] = data::ValueReset(name.defaultValue(),
//                                      paramValues[name].reset );
//     qDebug("%s", qPrintable("Resetting parameter with id=" + id +
//             " to value " + QString::number(name.defaultValue().toInt())));
}

data::Parameter ParameterManager::parameter(const QString& id) const
{
    return data.parameterById(id);
}

data::PMRuntimeSettings ParameterManager::internalState() const
{
    return paramValues;
}

void ParameterManager::setInternalState(data::PMRuntimeSettings n)
{
    paramValues = n;
}

}
