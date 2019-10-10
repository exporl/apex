/******************************************************************************
 * Copyright (C) 2018  Sanne Raymaekers <sanne.raymaekers@gmail.com>          *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
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

#include "studymodule.h"

#include "../apexcontrol.h"
#include "../runner/experimentrundelegate.h"
#include "study.h"
#include "studymanager.h"

#include "apexdata/experimentdata.h"

#include <QSharedPointer>

namespace apex
{

StudyModule::StudyModule(ExperimentRunDelegate &runDelegate)
    : ApexModule(runDelegate)
{
}

StudyModule::~StudyModule()
{
}

QString StudyModule::GetResultXML() const
{
    return QString();
}

QString StudyModule::GetEndXML() const
{
    StudyManager *manager = StudyManager::instance();
    if (!manager->belongsToActiveStudy(
            ApexControl::Get().GetCurrentExperiment().fileName()))
        return QString();

    QStringList result;
    result << "<study>";
    result << "\t<name>" + manager->activeStudy()->name() + "</name>";
    result << "\t<branch>" + manager->activeStudy()->experimentsBranch() +
                  "</branch>";
    result << "</study>";
    return result.join('\n');
}

void StudyModule::savedResultfile(const QString &resultfilePath)
{
    StudyManager::instance()->afterExperiment(resultfilePath);
}
}
