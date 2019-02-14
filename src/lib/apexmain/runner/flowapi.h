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

#include "simplerunner.h"

#include "../baseapi.h"

#include <QDir>
#include <QMap>
#include <QObject>
#include <QString>

namespace apex
{
class FlowRunner;

class FlowApi : public BaseApi
{
    Q_OBJECT
public:
    FlowApi(FlowRunner *fr, const QDir &baseDir);

public Q_SLOTS:
    bool runExperiment(const QString &filePath, const QString &resultsFilePath);

    void clearExpressions();
    void addExpression(const QString &key, const QString &value);

Q_SIGNALS:
    void setResultsFilePath(QString filePath);
    void savedFile(QString filePath);
    void experimentClosed();

private:
    SimpleRunner *sr;
    FlowRunner *fr;

    QMap<QString, QString> savedExpressions;
};
}

#endif
