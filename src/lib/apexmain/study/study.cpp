/******************************************************************************
 * Copyright (C) 2017  Sanne Raymaekers <sanne.raymaekers@gmail.com>          *
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

#include "study.h"

#include "manageddirectory.h"
#include "studymanager.h"

#ifdef Q_OS_ANDROID
#include "../apexandroidnative.h"
#endif

#include "apextools/apextools.h"
#include "apextools/exceptions.h"

#include <QDateTime>
#include <QDir>
#include <QHostInfo>
#include <QRegularExpression>

namespace apex
{

class StudyPrivate
{
public:
    void openManagedDirectory(ManagedDirectory &directory, const QString &path,
                              const QString &url, const QString &branch)
    {
        QDir rootDir(rootPath);
        directory.setPath(path);
        if (directory.exists()) {
            directory.open();
        } else {
            directory.init();
            directory.setRemote(url, branch);
#ifdef Q_OS_ANDROID
            QString serial =
                android::ApexAndroidBridge::getDeviceSerialNumber();
            directory.setAuthor(
                serial, QString::fromLatin1("%1@%2").arg(serial).arg(serial));
#else
            QString user = ApexTools::getUser();
            directory.setAuthor(user,
                                QString::fromLatin1("%1@%2").arg(user).arg(
                                    QHostInfo::localHostName()));
#endif
        }
        directory.setKeyPaths(rootDir.filePath(QSL("id_rsa.pub")),
                              rootDir.filePath(QSL("id_rsa")));
    }

    QString name;
    QString experimentsUrl;
    QString experimentsBranch;
    QString resultsUrl;
    QString resultsBranch;

    QString rootPath;
    QString experimentsPath;
    QString resultsPath;

    ManagedDirectory experimentsDirectory;
    ManagedDirectory resultsDirectory;
};

Study::Study(const QString &name, const QString &experimentsUrl,
             const QString &experimentsBranch, const QString &resultsUrl,
             const QString &resultsBranch, const QString &rootPath)
    : d(new StudyPrivate)
{
    d->name = name;
    d->experimentsUrl = experimentsUrl;
    d->experimentsBranch = experimentsBranch;
    d->resultsUrl = resultsUrl;
    d->resultsBranch = resultsBranch;

    d->rootPath = rootPath;
    QDir rootDir(rootPath);
    d->experimentsPath = rootDir.filePath(name + QL1S("/experiments"));
    rootDir.mkpath(name + QL1S("/experiments"));
    if (isPrivate()) {
        d->resultsPath = rootDir.filePath(name + QL1S("/results"));
        rootDir.mkpath(name + QL1S("/results"));
    }

    connect(&d->experimentsDirectory, SIGNAL(pullDone()), this,
            SIGNAL(updateExperimentsDone()));
    connect(&d->experimentsDirectory, SIGNAL(pullFailed()), this,
            SIGNAL(updateExperimentsFailed()));
    connect(&d->experimentsDirectory, SIGNAL(progress(int, int)), this,
            SIGNAL(updateExperimentsProgress(int, int)));

    connect(&d->resultsDirectory, SIGNAL(pushDone()), this,
            SIGNAL(updateResultsDone()));
    connect(&d->resultsDirectory, SIGNAL(pushFailed()), this,
            SIGNAL(updateResultsFailed()));
    connect(&d->resultsDirectory, SIGNAL(progress(int, int)), this,
            SIGNAL(updateResultsProgress(int, int)));
}

Study::~Study()
{
}

bool Study::isPublic() const
{
    return d->resultsUrl.isEmpty();
}

bool Study::isPrivate() const
{
    return !isPublic();
}

void Study::updateExperiments()
{
    if (!d->experimentsDirectory.isOpen())
        d->openManagedDirectory(d->experimentsDirectory, d->experimentsPath,
                                d->experimentsUrl, d->experimentsBranch);
    d->experimentsDirectory.pull();
}

void Study::fetchExperiments(bool blocking)
{
    if (!d->experimentsDirectory.isOpen())
        d->openManagedDirectory(d->experimentsDirectory, d->experimentsPath,
                                d->experimentsUrl, d->experimentsBranch);
    d->experimentsDirectory.fetch(blocking ? Qt::BlockingQueuedConnection
                                           : Qt::QueuedConnection);
}

void Study::checkoutExperiments(bool blocking)
{
    if (!d->experimentsDirectory.isOpen())
        d->openManagedDirectory(d->experimentsDirectory, d->experimentsPath,
                                d->experimentsUrl, d->experimentsBranch);
    d->experimentsDirectory.checkout(blocking ? Qt::BlockingQueuedConnection
                                              : Qt::QueuedConnection);
}

void Study::addResult(const QString &path)
{
    if (isPublic())
        throw ApexStringException(tr("Results not supported by public study."));
    if (!belongsToStudy(path))
        throw ApexStringException(
            tr("Result %1 doesn't belong to study %2").arg(path).arg(name()));

    if (!d->resultsDirectory.isOpen())
        d->openManagedDirectory(d->resultsDirectory, d->resultsPath,
                                d->resultsUrl, d->resultsBranch);
    d->resultsDirectory.add(path);
    updateResults();
}

void Study::updateResults(bool blocking)
{
    if (isPublic())
        throw ApexStringException(tr("Results not supported by public study."));
    if (!d->resultsDirectory.isOpen())
        d->openManagedDirectory(d->resultsDirectory, d->resultsPath,
                                d->resultsUrl, d->resultsBranch);

    if (d->resultsDirectory.hasLocalRemote() ||
        d->resultsDirectory.commitsAheadOrigin() > 0)
        d->resultsDirectory.push(blocking ? Qt::BlockingQueuedConnection
                                          : Qt::QueuedConnection);
    else
        Q_EMIT updateResultsDone();
}

void Study::pullResults()
{
    if (isPublic())
        throw ApexStringException(tr("Results not supported by public study."));
    if (!d->resultsDirectory.isOpen())
        d->openManagedDirectory(d->resultsDirectory, d->resultsPath,
                                d->resultsUrl, d->resultsBranch);
    d->resultsDirectory.pull();
}

QString Study::name() const
{
    return d->name;
}

QString Study::experimentsUrl() const
{
    return d->experimentsUrl;
}

QString Study::experimentsBranch() const
{
    return d->experimentsBranch;
}

QString Study::resultsUrl() const
{
    return d->resultsUrl;
}

QString Study::resultsBranch() const
{
    return d->resultsBranch;
}

QString Study::experimentsPath() const
{
    return d->experimentsPath;
}

QString Study::resultsPath() const
{
    return d->resultsPath;
}

QString Study::indexExperiment() const
{
    QDir experimentsDir(d->experimentsPath);
    QStringList entryList(
        experimentsDir.entryList(QDir::NoDotAndDotDot | QDir::Files));
    if (entryList.contains(QSL("index.apf")))
        return experimentsDir.filePath(QSL("index.apf"));
    Q_FOREACH (const QString &entry, entryList)
        if (entry.endsWith(QSL(".apf")))
            return experimentsDir.filePath(entry);
    return QString();
}

QStringList Study::experiments() const
{
    return ApexTools::recursiveFind(d->experimentsPath,
                                    QRegularExpression(QSL(".*[.](apx|apf)")));
}

bool Study::belongsToStudy(const QString &path) const
{
    if (QDir::isAbsolutePath(path))
        return (path.startsWith(d->experimentsPath) &&
                QDir(d->experimentsPath).exists(path)) ||
               (isPrivate() && path.startsWith(d->resultsPath) &&
                QDir(d->resultsPath).exists(path));
    return QDir(d->experimentsPath).exists(path) ||
           (isPrivate() && QDir(d->resultsPath).exists(path));
}

QString Study::makeResultsFilePath(const QString &experimentPath) const
{
    if (isPublic())
        throw ApexStringException(tr("Results not supported by public study."));
    if (!belongsToStudy(experimentPath))
        throw ApexStringException(
            tr("ExperimentPath %1 does not belong to this study")
                .arg(experimentPath));

    QFileInfo experimentPathInfo(
        QDir::isAbsolutePath(experimentPath)
            ? experimentPath
            : QDir(d->experimentsPath).filePath(experimentPath));
    QString relativeResultPath =
        QString::fromLatin1("%1/%2/%3-%4.apr")
#ifdef Q_OS_ANDROID
            .arg(android::ApexAndroidBridge::getDeviceSerialNumber())
#else
            .arg(QString::fromLatin1("%1_%2")
                     .arg(ApexTools::getUser())
                     .arg(QHostInfo::localHostName()))
#endif
            .arg(experimentPathInfo.path().remove(d->experimentsPath))
            .arg(experimentPathInfo.baseName())
            .arg(QDateTime::currentDateTimeUtc()
                     .toString(Qt::ISODate)
                     .remove(QChar('-'))
                     .remove(QChar(':')));
    QString absoluteResultPath =
        QDir::cleanPath(QDir(d->resultsPath).filePath(relativeResultPath));

    QDir(d->resultsPath).mkpath(QFileInfo(relativeResultPath).path());
    return absoluteResultPath;
}

QString Study::statusMessage()
{
    try {
        if (!d->experimentsDirectory.isOpen())
            d->openManagedDirectory(d->experimentsDirectory, d->experimentsPath,
                                    d->experimentsUrl, d->experimentsBranch);
        return d->experimentsDirectory.lastCommitMessage();
    } catch (std::exception &e) {
        qCWarning(APEX_RS, "Unable to get study %s statusMessage: %s",
                  qPrintable(d->name), e.what());
    }
    return QString();
}
}
