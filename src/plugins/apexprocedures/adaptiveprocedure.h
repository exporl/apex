/******************************************************************************
 * Copyright (C) 2010  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef _APEX_SRC_PLUGINS_APEXPROCEDURES_ADAPTIVEPROCEDURE_H_
#define _APEX_SRC_PLUGINS_APEXPROCEDURES_ADAPTIVEPROCEDURE_H_

#include "apexdata/procedure/adaptiveproceduredata.h"
#include "apexdata/procedure/procedureinterface.h"

namespace apex
{

struct AdaptiveProcedurePrivate;

class AdaptiveProcedure : public ProcedureInterface
{
    Q_DECLARE_TR_FUNCTIONS(AdaptiveProcedure)

public:
    AdaptiveProcedure(ProcedureApi *a, const data::ProcedureData *data);
    ~AdaptiveProcedure();

    data::Trial setupNextTrial();
    QString firstScreen();
    ResultHighlight processResult(const ScreenResult *screenResult);
    double progress() const;
    QString resultXml() const;
    QString finalResultXml() const;

private:
    void setNewParameter();
    void updateMinMaxValues(const data::TrialData *trial);
    bool doNextTrial() const;
    QMap<QString, QVariant> createParameters(const QString &stimulus) const;
    data::adapting_parameter
    fixedParameterValue(const QString &stimulusId) const;
    QString findStimulus(const data::TrialData *trial) const;
    QStringList findStimuli(const QStringList &list) const;
    QString resultXml(bool isVirtual) const;

    AdaptiveProcedurePrivate *const d;
    bool started;
};
}

#endif
