/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_MAP_R126DBASESETTING_H_
#define _EXPORL_SRC_LIB_APEXMAIN_MAP_R126DBASESETTING_H_

#include <qstring.h>

namespace r126
{

//! supported R126 Database version mubers
enum eR126VersionNumber {
    UNKNOWNR126VERSION = 0,
    R126VERSION_1_3 = 1,
    R126VERSION_2_0 = 2,
    R126VERSION_2_1 = 3
};

//! contains version number and server name
//! creates the connection string
class R126DataBaseSettings
{
public:
    R126DataBaseSettings()
    {
    }
    ~R126DataBaseSettings()
    {
    }

    void mf_Set(const eR126VersionNumber ac_eVersion, const QString &ac_sServer)
    {
        m_eVersion = ac_eVersion;
        m_sServer = ac_sServer;
        mf_UpdateData();
    }

    const eR126VersionNumber mf_eGetVersion() const
    {
        return m_eVersion;
    }

    const QString &mf_sGetServerName() const
    {
        return m_sServer;
    }

    //! returns empty string if version is not supported
    const QString &mf_sGetConnectionString() const
    {
        return m_sConnectionString;
    }

private:
    void mf_UpdateData()
    {
        m_sConnectionString = "";
        switch (m_eVersion) {
        case R126VERSION_1_3:
            // User = sa; No pass
            m_sConnectionString =
                "Provider=SQLOLEDB.1;Persist Security Info = False;"
                "User ID = sa;Initial Catalog=NucleusR126_Database;Network "
                "Library=DBMSSOCN;Data Source=";
            m_sConnectionString += m_sServer;
            break;
        case R126VERSION_2_0:
            // User = sa; No pass
            m_sConnectionString =
                "Provider=SQLOLEDB.1;Persist Security Info = False;"
                "User ID = NucleusR126;Password=wallaby;Initial "
                "Catalog=NucleusR126_Database;Network Library=DBMSSOCN;Data "
                "Source=";
            m_sConnectionString += m_sServer;
            break;

        case R126VERSION_2_1:
            // User = NucleusR126; Pass = wallaby
            m_sConnectionString =
                "Provider=SQLOLEDB.1;Persist Security Info = False;"
                "User ID = NucleusR126;Password=wallaby;Initial "
                "Catalog=NucleusR126_Database;Network Library=DBMSSOCN;Data "
                "Source=";
            m_sConnectionString += m_sServer;
            m_sConnectionString += "\\COCHLEAR";
            break;
        }
    }
    eR126VersionNumber m_eVersion;
    QString m_sServer;
    QString m_sConnectionString;
};
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_MAP_R126DBASESETTING_H_
