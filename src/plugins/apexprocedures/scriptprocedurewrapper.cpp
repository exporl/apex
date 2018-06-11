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

#include "scriptprocedurewrapper.h"

using namespace apex;

const QString ScriptProcedureWrapper::INSTANCENAME("pluginINTERFACE");

QScriptValue
ScriptProcedureWrapper::executeLine(const QString &line,
                                    const QScriptValueList &params) const
{
    QString nline = INSTANCENAME + "." + line;
    QScriptValue result = engine->evaluate(nline).call(*classname, params);
    if (engine->hasUncaughtException()) {
        throw ApexStringException("Error executing " + nline + ": " +
                                  result.toString());
    }
    return result;
}

ScriptProcedureWrapper::ScriptProcedureWrapper(QObject *parent)
    : ScriptProcedureInterface(parent), engine(0), classname(0)
{
}

ScriptProcedureWrapper::~ScriptProcedureWrapper()
{
}

void ScriptProcedureWrapper::setEngine(QScriptEngine *p_engine)
{
    engine = p_engine;
}

void ScriptProcedureWrapper::setClassname(QScriptValue *p_classname)
{
    classname = p_classname;
}

bool ScriptProcedureWrapper::isValid()
{
    return (engine != 0 && classname != 0);
}

data::Trial ScriptProcedureWrapper::setupNextTrial()
{
    QScriptValue r = executeLine("setupNextTrial", QScriptValueList());

    data::Trial *result = qobject_cast<data::Trial *>(r.toQObject());

    if (result)
        return *(result);
    throw ApexStringException(tr("Method setupNextTrial in scriptprocedure "
                                 "should return an object of type Trial."));
}

QString ScriptProcedureWrapper::firstScreen()
{
    return executeLine("firstScreen", QScriptValueList()).toString();
}

const QString ScriptProcedureWrapper::resultXml() const
{
    return executeLine("resultXml", QScriptValueList()).toString();
}

const QString ScriptProcedureWrapper::finalResultXml() const
{
    return executeLine("finalResultXml", QScriptValueList()).toString();
}

double ScriptProcedureWrapper::progress()
{
    return executeLine("progress", QScriptValueList()).toNumber();
}

ResultHighlight
ScriptProcedureWrapper::processResult(const ScreenResult *screenResult) const
{
    QVariantMap mapS;
    QMapIterator<QString, QString> i(screenResult->get());

    for (ScreenResult::const_iterator it = screenResult->begin();
         it != screenResult->end(); ++it) {
        mapS.insert(it.key(), it.value());
    }

    QScriptValue r = executeLine(
        "processResult", QScriptValueList() << engine->toScriptValue(mapS));
    return *(dynamic_cast<ResultHighlight *>(r.toQObject()));
}

QString ScriptProcedureWrapper::checkParameters() const
{
    return executeLine("checkParameters", QScriptValueList()).toString();
}
