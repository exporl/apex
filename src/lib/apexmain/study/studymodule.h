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

#ifndef _APEX_SRC_LIB_APEXMAIN_STUDY_STUDYMODULE_H_
#define _APEX_SRC_LIB_APEXMAIN_STUDY_STUDYMODULE_H_

#include "../apexmodule.h"

#include "studymanager.h"

namespace apex
{

class APEX_EXPORT StudyModule : public ApexModule
{
    Q_OBJECT
public:
    StudyModule(ExperimentRunDelegate &runDelegate);
    virtual ~StudyModule();

    virtual QString GetResultXML() const Q_DECL_OVERRIDE;
    virtual QString GetEndXML() const Q_DECL_OVERRIDE;

public Q_SLOTS:
    void savedResultfile(const QString &resultfilePath);
};
}

#endif
