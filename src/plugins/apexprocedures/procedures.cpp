/******************************************************************************
 * Copyright (C) 2008  Tom Francart                                           *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "apexdata/procedure/procedureinterface.h"

#include "adaptiveprocedure.h"
#include "adaptiveprocedureparser.h"
#include "constantprocedure.h"
#include "constantprocedureparser.h"
#include "multiprocedure.h"
#include "multiprocedureparser.h"
#include "scriptprocedure.h"
#include "scriptprocedureparser.h"
#include "trainingprocedure.h"
#include "trainingprocedureparser.h"

#include <QStringList>

#define STR_ADAPTIVE "apex:adaptiveProcedure"
#define STR_CONSTANT "apex:constantProcedure"
#define STR_TRAINING "apex:trainingProcedure"
#define STR_MULTI    "apex:multiProcedure"
#define STR_PLUGIN   "apex:pluginProcedure"

using namespace apex;

class ProcedureCreator : public QObject, public ProcedureCreatorInterface
{
        Q_OBJECT
        Q_INTERFACES(apex::ProcedureCreatorInterface)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "apex.procedurecreator")
#endif
    public:

        QStringList availableProcedurePlugins() const;

        ProcedureInterface* createProcedure(const QString& name,
                                            ProcedureApi* api,
                                            const data::ProcedureData* config);

        ProcedureParserInterface* createProcedureParser(const QString& name);
};

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2 (procedurecreator, ProcedureCreator)
#endif

QStringList ProcedureCreator::availableProcedurePlugins() const
{
    return QStringList()
        << STR_ADAPTIVE
        << STR_CONSTANT
        << STR_TRAINING
        << STR_MULTI
        << STR_PLUGIN;
}


ProcedureInterface* ProcedureCreator::createProcedure(const QString& name,
                                                      ProcedureApi* api,
                                                      const data::ProcedureData* config)
{
    if (name == STR_ADAPTIVE)
        return new AdaptiveProcedure(api, config);
    else if (name == STR_CONSTANT)
        return new ConstantProcedure(api, config);
    else if (name == STR_TRAINING)
        return new TrainingProcedure(api, config);
    else if (name == STR_MULTI)
        return new MultiProcedure(api, config);
    else if (name == STR_PLUGIN)
        return new ScriptProcedure(api, config);
    else
        qFatal("Invalid name in createProcedure");

    return 0;           // keep compiler happy
}


ProcedureParserInterface *ProcedureCreator::createProcedureParser(const QString &name)
{
    if (name == STR_ADAPTIVE)
        return new parser::AdaptiveProcedureParser();
    else if (name == STR_CONSTANT)
        return new parser::ConstantProcedureParser();
    else if (name == STR_TRAINING)
        return new parser::TrainingProcedureParser();
    else if (name == STR_MULTI)
        return new parser::MultiProcedureParser();
    else if (name == STR_PLUGIN)
        return new parser::ScriptProcedureParser();
    else
        qFatal("Invalid name in createProcedureParser");

    return 0;           // keep compiler happy
}


#include "procedures.moc"
