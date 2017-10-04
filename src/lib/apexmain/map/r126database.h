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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_MAP_R126DATABASE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_MAP_R126DATABASE_H_

#include "r126dbasesetting.h"
#include <vector>
using namespace std;

namespace r126
{

class R126PatientRecord;
class R126NucleusMAPRecord;
class R126DataBaseSettings;

//! R126 Database class
//! keeps it's own records
class R126DataBase
{
public:
    R126DataBase();
    ~R126DataBase();

    //! connection
    void mf_SetDataBaseSettings(const eR126VersionNumber ac_eVersion,
                                const QString &ac_sServer);
    const R126DataBaseSettings *mf_pGetDataBaseSettings() const;
    const bool mf_bTestConnection() const;

    //! data
    std::vector<R126PatientRecord *> &mf_FillPatientRecords();

    //! only fills in: date, rates, maxima, strategy, mode,.. not the channels
    //! usefull for displaying data
    std::vector<R126NucleusMAPRecord *> &
    mf_FillNucleusMapsInfo(const QString &ac_sPatientGUID);

    //! returns full database record which has to be deleted by client
    R126NucleusMAPRecord *mf_pGetNucleusMap(const QString &ac_sMapGUID);

private:
    static bool sm_bPatientsRecorded;
    std::vector<R126PatientRecord *> m_aPatients;
    std::vector<R126NucleusMAPRecord *> m_aMaps;
    R126DataBaseSettings *m_pConnectionSettings;
};
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_MAP_R126DATABASE_H_
