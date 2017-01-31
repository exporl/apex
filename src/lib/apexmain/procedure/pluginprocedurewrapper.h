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

#ifndef PluginProcedureWrapper_H
#define PluginProcedureWrapper_H

#include "../apexcontrol.h"
#include "../runner/experimentrundelegate.h"
#include "../screen/apexscreen.h"
#include "screen/screenresult.h"
#include "pluginprocedureinterface.h"

#include <QtCore/QObject>
#include <QtScript/QScriptable>
#include <QScriptContext>
#include <QScriptEngine>


namespace apex {
    class ScreenResult;
}

/**
 * Wrapper for QtScript procedure plugin
 * Calls the relevant methods from the script
 */

class PluginProcedureWrapper: public PluginProcedureInterface {
    Q_OBJECT


    public:
        static const QString INSTANCENAME;

        PluginProcedureWrapper(QObject *parent = 0):
            PluginProcedureInterface(parent),
            engine(0),
            classname(0) {}

        virtual ~PluginProcedureWrapper() {}

            void setEngine(QScriptEngine* p_engine) {
                engine=p_engine;
            }
            void setClassname(QScriptValue* p_classname) {
                classname=p_classname;
            }

            bool isValid() {
                return (engine!=0 && classname!=0 );
            }



    public slots:

        /** Check all parameters in the params object and return a QString with
         * an error message if there is an error. Otherwise return an empty QString
         */
        virtual QString CheckParameters() {
            return executeLine("CheckParameters",QScriptValueList()).toString();
        }

        //! return ID of the next trial
        //! If the procedure is finished, you should return an empty string
        QString NextTrial ( const bool p_answer, QVariantMap& screenresult) {
            QScriptValueList list;
            list.push_back(QScriptValue(engine,p_answer));
            list.push_back(engine->toScriptValue(screenresult));
            return executeLine("NextTrial", list).toString();
        };

        /** Prepare everything so the first trial can be started when StartOutput is called
         * Return the name of the first trial
         */
        virtual QString FirstTrial() {
            return executeLine("FirstTrial",QScriptValueList()).toString();
        };

        virtual const QString GetResultXML() const  {
            // TODO: Hack - pass last screenresult so that calculations done
            // here can access values on the last screen
            const apex::ScreenResult *screenResult = apex::ApexControl::Get().GetCurrentExperimentRunDelegate().modScreen()->GetLastResult();
            QVariantMap s;
            QMapIterator<QString, QString> i((*screenResult).map());
            while (i.hasNext()) {
                i.next();
                s.insert(i.key(), i.value());
            }
            return executeLine("GetResultXML", QScriptValueList() << engine->toScriptValue(s)).toString();
        };
        virtual unsigned GetNumTrials() {
            return executeLine("GetNumTrials",QScriptValueList()).toInt32();
        };

    private:
        QScriptEngine*  engine;
        QScriptValue* classname;

        QScriptValue executeLine(const QString& line, const QScriptValueList& params) const;

};



Q_DECLARE_METATYPE(PluginProcedureWrapper*)
Q_SCRIPT_DECLARE_QMETAOBJECT (PluginProcedureWrapper, QObject*)

#endif

