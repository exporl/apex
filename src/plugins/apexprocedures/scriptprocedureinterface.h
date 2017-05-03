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

#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTPROCEDUREINTERFACE_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_SCRIPTPROCEDUREINTERFACE_H_

#include "apexdata/procedure/procedureinterface.h"
#include "apexdata/procedure/trial.h"

#include "apextools/exceptions.h"

#include <QtCore/QObject>

#include <QtScript/QScriptable>

#include <QScriptContext>
#include <QScriptEngine>

/**
 * Interface for QtScript plugin (script) procedure
 * Provides methods that alert the user that they have to be implemented in the
 * QtScript if they are not
 * A QtScript "object" is to be created that inherits from this interface
 */

class ScriptProcedureInterface: public QObject, public QScriptable {
    Q_OBJECT

    public:
        ScriptProcedureInterface(QObject *parent = 0):
            QObject(parent) {}

        virtual ~ScriptProcedureInterface() {}



    public slots:

        virtual apex::data::Trial setupNextTrial () {
            ShowError("Error: setupNextTrial should be implemented in Script procedure");
            return apex::data::Trial();
        }

        //! Prepare everything so the first trial can be started when StartOutput is called
        virtual QString firstScreen() {
            ShowError("Error: firstScreen should be implemented in Script procedure");
            return QString();
        }

        //Prepares result for XML files.
        virtual const QString resultXml() const  {
            ShowError("Error: resultXml should be implemented in Script procedure");
             return QString();
        }

        virtual apex::ResultHighlight processResult (const apex::ScreenResult*) const {
            ShowError("Error: processResult should be implemented in script procedure.");
            return apex::ResultHighlight();
        }



        /**
          * Is called before the procedure starts, can be used to check
          * whether all required parameters are present and valid
          * Return empty QString if OK, otherwise error message
          */
        virtual QString checkParameters () const {
            // Does not need to be implemented, is optional
            return QString();
        }


    private:
        void ShowError(const QString& message) const;
};


Q_DECLARE_METATYPE(ScriptProcedureInterface*)
Q_SCRIPT_DECLARE_QMETAOBJECT (ScriptProcedureInterface, QObject*)

#endif

