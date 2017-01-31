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

#include "services/paths.h"
#include "services/application.h"
#include "exceptions.h"
#include "pathtools.h"

#include "apextools.h"

#include <QtGlobal>

#include <QDir>
#include <QApplication>
#include <QSettings>

using namespace apex;

const QString Paths::cApexConfig( "apexconfig.xml" );
const QString Paths::cApexConfigSchema( "apexconfig.xsd" );
const QString Paths::cMainIcon( "apex_icon.png" );
const QString Paths::cPluginDir("pluginprocedures");
const QString Paths::cDataDir("bitmaps");
const QString Paths::cScriptsDir("resultsviewer");
const QString Paths::cXsltScriptsDir("xslt");
const QString Paths::cExamplesDir("examples");
const QString Paths::cDocDir("doc");
const QString Paths::cExperimentSchema("experiment.xsd");

Paths::Paths()
{
    QDir d = QDir(Application::Get().GetApplication()->applicationDirPath());

    m_sExec = d.path();

    d.cdUp();
    // check whether m_sBase looks ok, otherwise look further

    for (unsigned i = 0; i < 2; ++i) {
        if (CheckApexDir (d))
            break;
        d.cdUp();
    }
    //d.cd("data");

    m_sBase = ApexTools::MakeDirEnd (d.path());

//    qCDebug(APEX_RS, "Base path: %s", qPrintable(m_sBase));
}

bool Paths::CheckApexDir(QDir d) {
//    qCDebug(APEX_RS, "Checking path %s", qPrintable (d.path()));

    if (d.cd("data"))
        return true;


#ifdef Q_OS_UNIX
    if (d.cd("share/apex/"))
        return true;
#endif
#ifdef WIN32
    if (d.cd("schemas"))
        return true;
#endif


    return false;

}


// FIXME: to be removed, is now in ApexTools
QString Paths::findReadableFile (const QString &baseName,
        const QStringList &directories, const QStringList &extensions,
        const QStringList &prefixes)
{
    QFileInfo fileInfo;
    Q_FOREACH (const QString &directory,
            QStringList (directories) << QString()) {
        Q_FOREACH (const QString &extension,
                QStringList (extensions) << QString()) {
            Q_FOREACH (const QString &prefix,
                    QStringList (prefixes) << QString()) {
                fileInfo.setFile (QDir (directory
                            ), prefix + baseName + extension);
                qCDebug(APEX_RS, "Trying %s",
                       qPrintable(directory+"/"+prefix + baseName + extension));
                if (fileInfo.exists() && fileInfo.isFile() &&
                        fileInfo.isReadable())
                    return fileInfo.filePath();
            }
        }
    }
    return QString();
}

QStringList Paths::findReadableFiles (const QStringList &directories,
        const QStringList &extensions)
{
    QStringList mydirectories =
        ApexTools::unique(QStringList(directories)<<QString());

    QStringList result;

    QStringList nameFilters;
    Q_FOREACH (const QString &extension, extensions)
        nameFilters.append ("*" + extension);

    Q_FOREACH (const QString &directory, mydirectories)
        Q_FOREACH (const QFileInfo &fileInfo, QDir (directory).entryInfoList
                (nameFilters, QDir::Readable | QDir::Files))
            result.append (fileInfo.filePath());

    return result;
}

const QString apex::Paths::GetExperimentSchemaPath() const
{
    return PathTools::GetSchemaPath(GetBasePath(),  cExperimentSchema);

    /*QString result( GetSharePath() + "/" +  cSchemaDir + "/" + cExperimentSchema );
    if (! QFile::exists(result)) {
        qCDebug(APEX_RS, "Experiment schema not found: %s", qPrintable(result));
        throw ApexStringException(QString(tr("Experiment schema not found: %1")).arg(result));
    }
    return result;*/
}

const QString apex::Paths::GetSharePath() const
{
    return PathTools::GetSharePath(GetBasePath());
}

const QString apex::Paths::GetNonBinaryPluginPath() const
{
    QString result( GetSharePath() + "/" + cPluginDir + "/");
    return result;
}

const QStringList apex::Paths::GetBinaryPluginPaths() const
{
    QStringList result;
#ifdef WIN32
    result << GetNonBinaryPluginPath();
#else
    result << GetBasePath() + "/share/" + PathTools::cApexDir;
#endif
    result << GetExecutablePath();
    return result;
}


const QString apex::Paths::GetApexConfigFilePath() const
{
    return PathTools::GetConfigFilePath( GetBasePath(), cApexConfig );
}

const QString apex::Paths::GetApexConfigSchemaPath() const
{
    return PathTools::GetSchemaPath(GetBasePath(), cApexConfigSchema);

    /*QString result( GetSharePath() + "/" +  PathTools::cSchemaDir + "/" +
            cApexConfigSchema );
    if (! QFile::exists(result))
        throw ApexStringException(QString(tr("ApexConfig schema not found: %1")).arg(result));

    return result;*/
}

const QString apex::Paths::GetUserConfigFilePath()
{
    return PathTools::GetUserConfigFilePath();
}

const QString apex::Paths::GetDataPath() const
{
    QString result( GetSharePath() + "/" + cDataDir + "/");
    return result;
}

const QString apex::Paths::GetDocPath() const
{
    QString result(GetSharePath() + cDocDir + "/");
    if (!QFile::exists(result))
        result = GetBasePath() + cDocDir + "/";
    return result;
}

const QString apex::Paths::GetMainLogoPath() const
{
    QString result( GetSharePath() + "/" + cDataDir + "/" +
            cMainIcon);
    return result;
}

const QString apex::Paths::GetExamplesPath() const
{
    QString result( GetSharePath() + "/" + cExamplesDir + "/");
    return result;
}

const QString apex::Paths::GetScriptsPath() const
{
    QString result( GetSharePath() + "/" + cScriptsDir + "/");
    return result;
}


const QString apex::Paths::GetXsltScriptsPath() const
{
    QString result( GetSharePath() + "/" + cXsltScriptsDir + "/");
    return result;
}

const QString apex::Paths::GetExecutablePath() const
{
    return PathTools::GetExecutablePath();
    //return QDir(Application::Get().GetApplication()->applicationDirPath()).path();
}

const QString & apex::Paths::GetBasePath() const
{
    return m_sBase;
}

