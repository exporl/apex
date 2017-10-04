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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_APEXMAPWIZARD_H_
#define _EXPORL_SRC_LIB_APEXMAIN_GUI_APEXMAPWIZARD_H_

#include "gui/mapwizardbase.h"

namespace r126
{
class R126DataBase;
class R126PatientRecord;
class R126NucleusMAPRecord;
}
using namespace r126;

namespace apex
{

//! Wizard allowing user to select a map from a R126 database
//! has only two pages which are defined through designer
//! for simplicity, ApexMapWizard handles everything happening on any page
class ApexMapWizard : public MapWizardBase
{
public:
    ApexMapWizard();
    ~ApexMapWizard();

    // returns selected map or 0 if cancelled
    R126NucleusMAPRecord *GetSelectedMap() const;

    // base overrides
    int exec();

protected slots:
    void back();
    void next();
    void help();
    void PatientSelected(QListBoxItem *);
    void MapSelected(QListBoxItem *);
    void R126SettingsClicked();

private:
    void FillPatientList();
    void FillMapList();
    QString m_sSelectedPatient; // GUID
    QString m_sSelectedMap;     //
    R126DataBase *m_pDataBase;
};
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_APEXMAPWIZARD_H_
