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

#ifndef _APEX_SRC_LIB_APEXTOOLS_APEXPATHS_H_
#define _APEX_SRC_LIB_APEXTOOLS_APEXPATHS_H_

#include "global.h"

#include <QString>

namespace apex
{

class APEXTOOLS_EXPORT ApexPaths
{
public:
    static QString GetExperimentSchemaPath();
    static QString GetApexConfigSchemaPath();
    static QString GetSpinSchemaPath();

    static QString GetApexConfigFilePath();
    static QString GetUserConfigFilePath();
    static QString GetConfigFilePath(const QString &filename);

    static QString GetNonBinaryPluginPath();
    static QString GetScriptsPath();
    static QString GetStudiesDirectory();

private:
    static QString GetSchemaPath(const QString &filename);
};
}

#endif //#ifndef _APEX_SRC_LIB_APEXTOOLS_APEXPATHS_H_
