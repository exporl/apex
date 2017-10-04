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

#include <QDebug>

namespace apex
{

ParameterManager::ParameterManager(const data::ParameterManagerData &d)
    : data(d)
{
}

QVariant ParameterManager::parameterValue(const QString &id) const
{
    data::Parameter name(data.parameterById(id));
    if (!name.isValid()) {
        qCDebug(APEX_RS) << "ParameterManager: cannot get parameter" << id;
        return QVariant();
    }

    if (paramValues.contains(name))
        return paramValues.value(name).value;

    return name.defaultValue();
}

data::ParameterValueMap
ParameterManager::parametersForOwner(const QString &owner) const
{

    data::ParameterValueMap result;

    data::PMRuntimeSettings::const_iterator it;
    for (it = paramValues.begin(); it != paramValues.end(); ++it) {
        if (it.key().owner() == owner)
            result.insert(it.key(), it.value().value);
    }

    return result;
}

void ParameterManager::setParameter(const QString &id, const QVariant &value,
                                    bool canReset)
{
    data::Parameter name(data.parameterById(id));

    if (!name.isValid()) {
        qCDebug(APEX_RS)
            << "ParameterManager::SetParameter: cannot set parameter" << id
            << "to value" << value.toString();

        qCCritical(APEX_RS, "%s",
                   qPrintable(QSL("%1: %2").arg(
                       "ParameterManager",
                       QObject::tr("unknown parameter: %1").arg(id))));
        showContents();
    } else {
        qCDebug(APEX_RS) << "ParameterManager::SetParameter: setting parameter"
                         << id << "to value" << value.toString();

        paramValues.insert(name, data::ValueReset(value, canReset));
        Q_EMIT parameterChanged(id, value);
    }
}

void ParameterManager::registerParameter(const QString &id,
                                         const data::Parameter &name)
{
    data.registerParameter(id, name);
    Q_EMIT parameterChanged(id, parameterValue(id));
}

void ParameterManager::showContents() const
{
    data.showContents();

    qCDebug(APEX_RS) << "Registered values:";
    qCDebug(APEX_RS) << "==================";

    data::PMRuntimeSettings::const_iterator it;
    for (it = paramValues.begin(); it != paramValues.end(); ++it) {
        qCDebug(APEX_RS).nospace() << "name=" << it.key().toString()
                                   << ", value=" << it.value().value.toString();
    }

    qCDebug(APEX_RS) << "==================";
}

void ParameterManager::reset()
{
    QStringList removedIds;

    QMutableMapIterator<data::Parameter, data::ValueReset> it(paramValues);
    while (it.hasNext()) {
        it.next();
        if (it.value().reset) {
            removedIds << it.key().id();
            it.remove();
        }
    }

    Q_FOREACH (QString id, removedIds)
        Q_EMIT parameterChanged(id, parameterValue(id));
}

void ParameterManager::forceReset()
{
    QList<data::Parameter> parameters = paramValues.keys();
    paramValues.clear();

    Q_FOREACH (data::Parameter parameter, parameters)
        Q_EMIT parameterChanged(parameter.id(), parameterValue(parameter.id()));
}

void ParameterManager::setAllToDefaultValue(bool force)
{
    QList<data::Parameter> parameters = paramValues.keys();
    Q_FOREACH (data::Parameter parameter, parameters)
        if (force || paramValues[parameter].reset)
            paramValues[parameter].value = parameter.defaultValue();
}

void ParameterManager::resetParameter(const QString &id)
{
    paramValues.remove(data.parameterById(id));
}

data::Parameter ParameterManager::parameter(const QString &id) const
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
