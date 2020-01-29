/******************************************************************************
 * Copyright (C) 2016 Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>   *
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

#include "apexmain/resultsink/resultfilepathcreator.h"

#include "flowapi.h"
#include "flowrunner.h"
#include "simplerunner.h"

#ifdef Q_OS_ANDROID
#include "apexmain/apexandroidnative.h"
#else
#include <QDesktopServices>
#endif

using namespace apex;

FlowApi::FlowApi(FlowRunner *fr, const QDir &baseDir)
    : sr(new SimpleRunner), fr(fr), baseDir(baseDir)
{
    connect(sr, SIGNAL(selected(data::ExperimentData *)), fr,
            SLOT(select(data::ExperimentData *)));
    connect(this, SIGNAL(setResultsFilePath(QString)), fr,
            SIGNAL(setResultsFilePath(QString)));
}

FlowApi::~FlowApi()
{
}

bool FlowApi::runExperiment(const QJsonObject &json)
{
    QString experimentfilePath = json["experimentfilePath"].toString();

    QString absoluteExperimentfilePath(
        QDir::isRelativePath(experimentfilePath)
            ? baseDir.absoluteFilePath(experimentfilePath)
            : experimentfilePath);

    QString absoluteResultfilePath =
        ResultfilePathCreator().createAbsolutePathForFlow(
            baseDir, absoluteExperimentfilePath,
            json["resultfilePath"].toString());

    if (!absoluteResultfilePath.isEmpty())
        Q_EMIT setResultsFilePath(absoluteResultfilePath);

    QVariantMap expressions = json["expressions"].toObject().toVariantMap();
    sr->setExpressions(asQMap(expressions));

    fr->makeInvisible();

    bool result =
        sr->select(absoluteExperimentfilePath, json["autoStart"].toBool(false));
    if (!result)
        fr->makeVisible();
    return result;
}

QMap<QString, QString> FlowApi::asQMap(QVariantMap &expressions) const
{
    QMap<QString, QString> expressionsMap;
    for (QVariantMap::const_iterator iter = expressions.begin();
         iter != expressions.end(); ++iter) {
        expressionsMap.insert(iter.key(), iter.value().toString());
    }
    return expressionsMap;
}

QString FlowApi::readFile(const QString &path) const
{
    QFile file(QDir::isRelativePath(path) ? baseDir.absoluteFilePath(path)
                                          : path);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    in.setCodec("UTF-8");
    return in.readAll();
}

QString FlowApi::readResultsFile(const QString &path) const
{
    return readFile(makeResultsPath(path));
}

void FlowApi::writeFile(const QString &path, const QString &content) const
{
    QString resultsPath = makeResultsPath(path);

    QDir(QFileInfo(resultsPath).dir()).mkpath(".");

    QFile file(resultsPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate |
                  QIODevice::Text)) {
        file.write(content.toUtf8());
        file.close();
    } else {
        qCWarning(APEX_RS, "Could not write file \"%s\"",
                  qPrintable(resultsPath));
    }

    StudyManager::instance()->afterExperiment(resultsPath);
}

QString FlowApi::makeResultsPath(const QString &path) const
{
    if (QDir::isAbsolutePath(path))
        return path;

    if (StudyManager::instance()->belongsToActiveStudy(baseDir.absolutePath()))
        return StudyManager::instance()->makeResultsPath(path);

    return baseDir.absoluteFilePath(path);
}

bool FlowApi::inStudy() const
{
    return StudyManager::instance()->belongsToActiveStudy(
        baseDir.absolutePath());
}

void FlowApi::openDocumentation() const
{
#ifdef Q_OS_ANDROID
    QString documentationPath =
        inStudy()
            ? StudyManager::instance()->activeStudy()->getDocumentationPath()
            : baseDir.filePath("documentation");
    android::ApexAndroidBridge::openBrowser(QSL("file://") + documentationPath +
                                            QSL("/index.html"));
#else
    QDesktopServices::openUrl(
        QUrl::fromLocalFile(baseDir.filePath("documentation/index.html")));
#endif
}