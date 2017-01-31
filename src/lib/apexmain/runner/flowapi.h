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

#ifndef FLOWAPI_H
#define FLOWAPI_H

#include "simplerunner.h"
#include "flowrunner.h"
#include <QString>
#include <QObject>
#include <QDebug>
#include <QQueue>
#include <QDir>


namespace apex {
class FlowApi : public QObject
{
    Q_OBJECT
public:
    FlowApi(FlowRunner *fr, const QDir &baseDir);
public slots:
    Q_INVOKABLE virtual void runExperiment(const QString& filePath, const QVariantMap& expressions = QVariantMap(), const QString& resultsFilepath = "");
    Q_INVOKABLE virtual QString readFile(const QString& fileName);
    Q_INVOKABLE virtual QString fromCurrentDir(const QString& fileName);
private:
    SimpleRunner *sr;
    FlowRunner *fr;
    const QDir baseDir;

signals:
    void setResultsFilePath(QString filePath);
    void savedFile(QString filePath);

};
} // ns apex

#endif // FLOWAPI_H
