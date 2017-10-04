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

#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTPROCEDUREWRAPPER_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTPROCEDUREWRAPPER_H_

#include "apexdata/screen/screenresult.h"

#include "apexdata/procedure/procedureinterface.h"

#include "scriptprocedureinterface.h"

#include <QtCore/QObject>

#include <QtScript/QScriptable>

#include <QScriptContext>
#include <QScriptEngine>

namespace apex
{
class ScreenResult;
}

/**
 * Wrapper for QtScript (Qt Script) procedure
 * Calls the relevant methods from the script
 */

class ScriptProcedureWrapper : public ScriptProcedureInterface
{
    Q_OBJECT

public:
    static const QString INSTANCENAME;

    ScriptProcedureWrapper(QObject *parent = 0);

    virtual ~ScriptProcedureWrapper();

    void setEngine(QScriptEngine *p_engine);
    void setClassname(QScriptValue *p_classname);

    bool isValid();

public slots:

    /**
        * Return a Trial structure with screens and stimuli to be presented
        * when finished, return an empty Trial (with no screens added)
        */
    virtual apex::data::Trial setupNextTrial() Q_DECL_OVERRIDE;

    /**
         * Return the first screen; this is the screen that should be shown
         * before the user presses start if there is no intro screen.
         */
    virtual QString firstScreen() Q_DECL_OVERRIDE;

    /**
         * Return XML for the results file
         */
    virtual const QString resultXml() const Q_DECL_OVERRIDE;
    virtual const QString finalResultXml() const;

    /**
        * Return the current progress in %, e.g., trial 5 of 10 would yield 50%
        */
    virtual double progress();

    virtual apex::ResultHighlight
    processResult(const apex::ScreenResult *screenResult) const Q_DECL_OVERRIDE;

    /**
      * Is called before the procedure starts, can be used to check
      * whether all required parameters are present and valid
      * Return empty QString if OK, otherwise error message
      */
    virtual QString checkParameters() const Q_DECL_OVERRIDE;

private:
    QScriptEngine *engine;
    QScriptValue *classname;

    QScriptValue executeLine(const QString &line,
                             const QScriptValueList &params) const;
};

Q_DECLARE_METATYPE(ScriptProcedureWrapper *)
Q_SCRIPT_DECLARE_QMETAOBJECT(ScriptProcedureWrapper, QObject *)

#endif
