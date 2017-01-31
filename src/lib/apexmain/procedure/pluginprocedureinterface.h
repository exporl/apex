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

#ifndef PLUGINPROCEDUREINTERFACE_H
#define PLUGINPROCEDUREINTERFACE_H

#include "exceptions.h"

#include <QtCore/QObject>
#include <QtScript/QScriptable>
#include <QScriptContext>
#include <QScriptEngine>





/**
 * Interface for QtScript plugin procedure
 * Provides methods that alert the user that they have to be implemented in the
 * QtScript if they are not
 * A QtScript "object" is to be created that inherits from this interface
 */

class PluginProcedureInterface: public QObject, public QScriptable {
    Q_OBJECT

    public:
        PluginProcedureInterface(QObject *parent = 0):
            QObject(parent) {}

        virtual ~PluginProcedureInterface() {}



    public slots:
                            //virtual void Stop();                  // moved to ApexProcedure
        //! return ID of the next trial
        virtual QString NextTrial ( const bool p_answer=true ) {
            Q_UNUSED (p_answer);
            ShowError("Error: NextTrial should be implemented in QtScript procedure plugin");
            return QString();
        };

        //! Prepare everything so the first trial can be started when StartOutput is called
        virtual QString FirstTrial() {
            ShowError("Error: FirstTrial should be implemented in QtScript procedure plugin");
            return QString();
        };

        virtual const QString GetResultXML() const  {
            ShowError("Error: GetResultXML should be implemented in QtScript procedure plugin");
             return QString();
        };
        virtual unsigned GetNumTrials() {
            ShowError("Error: GetNumTrials should be implemented in QtScript procedure plugin");
             return 0;
        };


    private:
        void ShowError(const QString& message) const;
};


Q_DECLARE_METATYPE(PluginProcedureInterface*)
Q_SCRIPT_DECLARE_QMETAOBJECT (PluginProcedureInterface, QObject*)

#endif

