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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RUNNER_FLOWAPI_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RUNNER_FLOWAPI_H_

#include <QDir>
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QString>

namespace apex
{
class FlowRunner;
class SimpleRunner;

class FlowApi : public QObject
{
    Q_OBJECT
public:
    explicit FlowApi(FlowRunner *fr, const QDir &baseDir);
    virtual ~FlowApi();

    /*
     * Read a file from the local filesystem. When supplying a relative path,
     * the path is treated as relative to the parent folder of the apf file.
     * An empty string is returned when the path references an unexisting file.
     */
    Q_INVOKABLE QString readFile(const QString &path) const;

    /*
     * Read a file from the local filesystem. When supplying a relative path,
     * the path is treated as relative to the results folder (which can be
     * different from the parent folder of the apf file in case of a study).
     * An empty string is returned when the path references an unexisting file.
     */
    Q_INVOKABLE QString readResultsFile(const QString &path) const;

    /*
     * Write a file to the local filesystem. When supplying a relative path, the
     * path is treated as relative to the results folder (which can be different
     * from the parent folder of the apf file in case of a study).
     */
    Q_INVOKABLE void writeFile(const QString &path,
                               const QString &content) const;

    /*
     * Start an experiment. This will hide the flowrunner until the
     * `experimentDone` signal is emitted.
     * This method accepts a json object with the following values:
     * - experimentfilePath (required): The path to the experiment (apx) file.
     * When supplying a relative path, the path is treated as relative to the
     * parent folder of the apf file.
     * - resultfilePath (optional): The path to the results (apr) file that
     * should be created after the experiment. When supplying a relative path,
     * the path is treated as relative to the results folder (which can be
     * different from the parent folder of the apf file in case of a study).
     * - expressions (optional): a nested json object that contains all the
     * expressions that should be applied before starting the experiment.
     * - autoStart (optional): a boolean value that indicates whether the
     * experiment should start automatically (default: false)
     */
    Q_INVOKABLE bool runExperiment(const QJsonObject &json);

    Q_INVOKABLE bool inStudy() const;

    /*
     * Open the accompanying documentation in a separate browser window.
     */
    Q_INVOKABLE void openDocumentation() const;

signals:
    /*
     * Signal emitted when the experiment started with `runExperiment` is done.
     * Either because it's completely finished, or it has been stopped by the
     * user. The path to the result file is passed as an argument. The path is
     * empty when no result file is created.
     */
    void experimentDone(const QString &resultfilePath);
    void setResultsFilePath(QString path);

private:
    SimpleRunner *sr;
    FlowRunner *fr;
    QDir baseDir;

    QString makeResultsPath(const QString &path) const;
    QMap<QString, QString> asQMap(QVariantMap &expressions) const;
};
}

#endif
