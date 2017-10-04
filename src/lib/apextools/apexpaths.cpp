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

#include "common/global.h"
#include "common/paths.h"

#include "apexpaths.h"
#include "apextools.h"
#include "version.h"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QUrl>

#include <QtGlobal>

using namespace cmn;

namespace apex
{

QString ApexPaths::GetSchemaPath(const QString &filename)
{
    return Paths::searchFile(QL1S("schemas/") + filename,
                             Paths::dataDirectories());
}

QString ApexPaths::GetExperimentSchemaPath()
{
    return GetSchemaPath(QUrl(QL1S(EXPERIMENT_SCHEMA_URL)).fileName());
}

QString ApexPaths::GetApexConfigSchemaPath()
{
    return GetSchemaPath(QUrl(QL1S(CONFIG_SCHEMA_URL)).fileName());
}

QString ApexPaths::GetSpinSchemaPath()
{
    return GetSchemaPath(QUrl(QL1S(SPIN_SCHEMA_URL)).fileName());
}

QString ApexPaths::GetApexConfigFilePath()
{
    return GetConfigFilePath(QL1S("apexconfig.xml"));
}

QString ApexPaths::GetConfigFilePath(const QString &filename)
{
    QString result =
        Paths::searchFile(filename, QStringList() << GetUserConfigFilePath());
    if (result.isEmpty())
        result = Paths::searchFile(QL1S("config/") + filename,
                                   Paths::dataDirectories());
    return result;
}

QString ApexPaths::GetUserConfigFilePath()
{
    QString result;
#if defined(Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_"
                       "USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Exp"
                       "lorer\\Shell Folders",
                       QSettings::NativeFormat);
    result = settings.value("AppData").toString() + "\\ExpORL";
#elif defined(Q_OS_ANDROID)
// not supported
#else
    result = QDir::homePath() + "/.config/ExpORL";
#endif

    if (!result.isEmpty() && !QFile::exists(result))
        QDir().mkdir(result);

    return result;
}

QString ApexPaths::GetNonBinaryPluginPath()
{
    return Paths::searchDirectory(QL1S("pluginprocedures"),
                                  Paths::dataDirectories());
}

QString ApexPaths::GetScriptsPath()
{
    return Paths::searchDirectory(QL1S("resultsviewer"),
                                  Paths::dataDirectories());
}
}
