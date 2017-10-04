/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
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

#include "paths.h"

#include <QCoreApplication>
#include <QDir>
#include <QMutex>

#if defined(Q_OS_ANDROID)
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

#include <cstdlib>

namespace cmn
{

// On Unix, 4 file paths are searched in the following order:
// 1. the home direcory (XDG_DATA_HOME/program, .local/lib/program)
// 2. the directory where the program is installed (../share/program,
// ../lib/program)
// 3  or the directory where the program is located
// 4. the default directories (XDG_DATA_DIRS, /usr/local/lib:/usr/lib)
//
// On Windows, 4 file paths are searched in the following order:
// 1. the home direcory (APPDATA/program)
// 2. the common home direcory (COMMON_APPDATA/program)
// 3. the directory where the program is installed(../share, ../lib)
// 4. or the directory where the program is located

// TODO: Use Qt4.3 search paths for most of these

#ifdef Q_OS_WIN

#define _WIN32_IE 0x0400
#include <shlobj.h>

// TODO: Qt 4.4 has support to retrieve these folders
static QString windowsConfigPath(int type)
{
    QString result;

    TCHAR path[MAX_PATH];
    SHGetSpecialFolderPathW(0, path, type, FALSE);
    result = QString::fromUtf16((ushort *)path);

    if (result.isEmpty()) {
        switch (type) {
        case CSIDL_COMMON_APPDATA:
            result = QL1S("C:\\temp\\qt-common");
            break;
        case CSIDL_APPDATA:
            result = QL1S("C:\\temp\\qt-user");
            break;
        default:
            break;
        }
    }

    return QDir::fromNativeSeparators(result);
}

#endif // Q_OS_WIN

#if defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID)

static QStringList directoriesFromEnvironment(const char *name,
                                              const QString &defaultPath,
                                              const QString &suffix)
{
    QString path = QFile::decodeName(getenv(name));
    if (path.isEmpty())
        path = defaultPath;
    const QStringList dirs = QDir::fromNativeSeparators(path).split(QL1C(':'));
    QStringList result;
    Q_FOREACH (const QString &dir, dirs)
        result.append(dir + suffix);
    return result;
}

#endif // defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID)

static QStringList uniqueDirectoryList(const QStringList &list)
{
    QStringList result;
    Q_FOREACH (const QString &dir, list)
        if (!dir.isEmpty() && !result.contains(dir))
            result.append(dir);
    return result;
}

#if !defined(Q_OS_ANDROID)
// If installed returns the base directory (/usr) plus installedSubDirs.
// If not installed, returns the source root directory plus uninstalledSubDirs.
// If not installed and uninstalledSubDirs is empty, returns applicationDirPath.
static QStringList
relativeToBaseDirectory(const QStringList &installedSubDirs,
                        const QStringList &uninstalledSubDirs)
{
    QStringList result;
    QDir applicationDir(QCoreApplication::applicationDirPath());
    if (applicationDir.dirName().compare(QL1S("bin"), Qt::CaseInsensitive) ==
        0) {
        if (applicationDir.cdUp())
            Q_FOREACH (const QString &installedSubDir, installedSubDirs)
                result.append(applicationDir.path() + installedSubDir);
    } else {
        if (!uninstalledSubDirs.isEmpty()) {
            if (applicationDir.cdUp() && applicationDir.cdUp())
                Q_FOREACH (const QString &uninstalledSubDir, uninstalledSubDirs)
                    result.append(applicationDir.path() + uninstalledSubDir);
        } else {
            result.append(applicationDir.path());
        }
    }
    return result;
}

static QString mainDirectory()
{
    QCoreApplication *instance = QCoreApplication::instance();
    return QL1C('/') +
           (instance ? instance->applicationName().toLower() : QL1S("exporl"));
}
#endif

QString Paths::installedBaseDirectory()
{
    QDir applicationDir(QCoreApplication::applicationDirPath());
    if (applicationDir.dirName().compare(QL1S("bin"), Qt::CaseInsensitive) ==
            0 &&
        applicationDir.cdUp())
        return applicationDir.path();
    return QString();
}

QStringList Paths::iconDirectories()
{
    QStringList result;
#if defined(Q_OS_ANDROID)
    result << QL1S("icons");
#elif defined(Q_OS_UNIX)
    result << directoriesFromEnvironment(
        "XDG_DATA_HOME", QDir::homePath() + QL1S("/.local/share"),
        QL1S("/icons/hicolor"));
    result << relativeToBaseDirectory(
        QStringList() << QL1S("/share/icons/hicolor"),
        QStringList() << QL1S("/data/icons") << QL1S("/src/data/icon/icons")
                      << QL1S("/common/data/icon/icons")
                      << QL1S("/commongui/data/icon/icons")
                      << QL1S("/coh/data/icon/icons")
                      << QL1S("/bertha-lib/data/icons")
                      << QL1S("/contrib/data/tango/icons"));
    result << directoriesFromEnvironment("XDG_DATA_DIRS",
                                         QL1S("/usr/local/share:/usr/share"),
                                         QL1S("/icons/hicolor"));
#elif defined(Q_OS_WIN)
    result << windowsConfigPath(CSIDL_APPDATA) + mainDirectory() +
                  QL1S("/icons");
    result << windowsConfigPath(CSIDL_COMMON_APPDATA) + mainDirectory() +
                  QL1S("/icons");
    result << relativeToBaseDirectory(
        QStringList() << QL1S("/icons"),
        QStringList() << QL1S("/data/icons") << QL1S("/src/data/icon/icons")
                      << QL1S("/common/data/icon/icons")
                      << QL1S("/commongui/data/icon/icons")
                      << QL1S("/coh/data/icon/icons")
                      << QL1S("/bertha-lib/data/icons")
                      << QL1S("/contrib/data/tango/icons"));
#else
#error "No idea where to find icon directories on this platform"
#endif
    return uniqueDirectoryList(result);
}

QStringList Paths::dataDirectories()
{
    QStringList result;
#if defined(Q_OS_ANDROID)
    result << QL1S("assets:");
#elif defined(Q_OS_UNIX)
    result << directoriesFromEnvironment(
        "XDG_DATA_HOME", QDir::homePath() + QL1S("/.local/share"),
        mainDirectory());
    result << relativeToBaseDirectory(
        QStringList() << QL1S("/share") + mainDirectory(),
        QStringList() << QL1S("/src/data") << QL1S("/data") // apex data
                      << QL1S("/")                          // apex examples
                      << QL1S("/common/data") << QL1S("/commongui/data")
                      << QL1S("/coh/data") << QL1S("/bertha-lib/data"));
    result << directoriesFromEnvironment(
        "XDG_DATA_DIRS", QL1S("/usr/local/share:/usr/share"), mainDirectory());
#elif defined(Q_OS_WIN)
    result << windowsConfigPath(CSIDL_APPDATA) + mainDirectory() +
                  QL1S("/share");
    result << windowsConfigPath(CSIDL_COMMON_APPDATA) + mainDirectory() +
                  QL1S("/share");
    result << relativeToBaseDirectory(
        QStringList() << QString() << QL1S("/share"),
        QStringList() << QL1S("/src/data") << QL1S("/data") // apex data
                      << QL1S("/")                          // apex examples
                      << QL1S("/common/data") << QL1S("/commongui/data")
                      << QL1S("/coh/data") << QL1S("/bertha-lib/data"));
#else
#error "No idea where to find data directories on this platform"
#endif
    return uniqueDirectoryList(result);
}

QString Paths::dataDirectory()
{
    const QString result = dataDirectories().first();
    QDir().mkpath(result);
    return result;
}

QString Paths::filesDirectory()
{
#if defined(Q_OS_ANDROID)
    return QtAndroid::androidActivity()
        .callObjectMethod("getFilesDir", "()Ljava/io/File;")
        .toString();
#else
    return QString();
#endif
}

QStringList Paths::pluginDirectories()
{
    QStringList result;
#if defined(Q_OS_ANDROID)
    result << QDir::homePath() + QL1S("/../lib");
#elif defined(Q_OS_UNIX)
    result << QDir::homePath() + QL1S("/.local/lib") + mainDirectory();
    result << relativeToBaseDirectory(
        QStringList() << QL1S("/lib") + mainDirectory(), QStringList());
    result << QL1S("/usr/local/lib") + mainDirectory();
    result << QL1S("/usr/lib") + mainDirectory();
#elif defined(Q_OS_WIN)
    result << windowsConfigPath(CSIDL_APPDATA) + mainDirectory() + QL1S("/bin");
    result << windowsConfigPath(CSIDL_COMMON_APPDATA) + mainDirectory() +
                  QL1S("/bin");
    result << relativeToBaseDirectory(QStringList() << QL1S("/bin"),
                                      QStringList());
#else
#error "No idea where to find plugin directories on this platform"
#endif
    return uniqueDirectoryList(result);
}

QStringList Paths::binDirectories()
{
    QStringList result;
#if defined(Q_OS_ANDROID)
    result << QDir::homePath() + QL1S("/../lib");
#elif defined(Q_OS_UNIX)
    result << QDir::homePath() + QL1S("/.local/bin") + mainDirectory();
    result << relativeToBaseDirectory(
        QStringList() << QL1S("/bin") + mainDirectory(), QStringList());
    result << QL1S("/usr/local/bin") + mainDirectory();
    result << QL1S("/usr/bin") + mainDirectory();
#elif defined(Q_OS_WIN)
    result << windowsConfigPath(CSIDL_APPDATA) + mainDirectory() + QL1S("/bin");
    result << windowsConfigPath(CSIDL_COMMON_APPDATA) + mainDirectory() +
                  QL1S("/bin");
    result << relativeToBaseDirectory(QStringList() << QL1S("/bin"),
                                      QStringList());
#else
#error "No idea where to find bin directories on this platform"
#endif
    return uniqueDirectoryList(result);
}

QString Paths::tempDirectory()
{
    const QString tempDir = manualTempDirectory();
    if (tempDir.isEmpty() || !QDir(tempDir).exists())
        return QDir::tempPath();
    return tempDir;
}

namespace
{

class TempDirectoryStorage
{
public:
    QString get() const
    {
        QMutexLocker locker(&mutex);
        return value;
    }

    void set(const QString &newValue)
    {
        QMutexLocker locker(&mutex);
        value = newValue;
    }

private:
    QString value;
    mutable QMutex mutex;
};
}

Q_GLOBAL_STATIC(TempDirectoryStorage, tempDirectoryStorage)

QString Paths::manualTempDirectory()
{
    return tempDirectoryStorage()->get();
}

void Paths::setManualTempDirectory(const QString directory)
{
    tempDirectoryStorage()->set(directory);
}

static QString search(const QString &filePath, const QStringList &directories,
                      bool isFile)
{
    Q_FOREACH (const QString &directory, directories) {
        if (directory.isEmpty())
            continue;
        QFileInfo info(QDir(directory), filePath);
        if (info.exists() &&
            (isFile ? info.isFile() && info.isReadable() : info.isDir()))
            return info.absoluteFilePath();
    }
    return QString();
}

QString Paths::searchFile(const QString &filePath,
                          const QStringList &directories)
{
    return search(filePath, directories, true);
}

QString Paths::searchDirectory(const QString &filePath,
                               const QStringList &directories)
{
    return search(filePath, directories, false);
}

} // namespace cmn
