/******************************************************************************
 * Copyright (C) 2016 Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>   *
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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RUNNER_FLOWAPI_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RUNNER_FLOWAPI_H_

#include "simplerunner.h"

#include <QDir>
#include <QMap>
#include <QObject>
#include <QString>

namespace apex
{
class FlowRunner;

class FlowApi : public QObject
{
    Q_OBJECT
public:
    FlowApi(FlowRunner *fr, const QDir &baseDir);

public Q_SLOTS:
    bool runExperiment(const QString &filePath, const QString &resultsFilePath);

    QString readFile(const QString &fileName);
    QString absoluteFilePath(const QString &fileName);

    void clearExpressions();
    void addExpression(const QString &key, const QString &value);

Q_SIGNALS:
    void setResultsFilePath(QString filePath);
    void savedFile(QString filePath);

private:
    SimpleRunner *sr;
    FlowRunner *fr;

    QMap<QString, QString> savedExpressions;
    const QDir baseDir;
};
}

#endif
