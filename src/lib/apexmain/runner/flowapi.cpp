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

#include "flowapi.h"
#include "apextools/apexpaths.h"
#include "accessmanager.h"

#include <QFile>

using namespace apex;

FlowApi::FlowApi(FlowRunner *fr, const QDir& baseDir)
    : fr(fr)
    , baseDir(baseDir)
{
    sr = new SimpleRunner;

    connect(sr, SIGNAL(selected(data::ExperimentData*)),
            fr, SLOT(select(data::ExperimentData*)));
    connect(fr, SIGNAL(savedFile(QString)),
            this, SIGNAL(savedFile(QString)));
    connect(this, SIGNAL(setResultsFilePath(QString)),
            fr, SIGNAL(setResultsFilePath(QString)));
}

void FlowApi::runExperiment(const QString& filePath, const QVariantMap& expressions, const QString& results) {
    if(!results.isEmpty()) {
        Q_EMIT setResultsFilePath(results);
    }
    if(!expressions.isEmpty()) {
        QMap<QString, QString> m;
        foreach(QString key, expressions.keys()) {
            QString value = expressions.value(key).toString();
            m.insert(key, value);
        }
        sr->setExpressions(m);
    }

    AccessManager am;
    QUrl url = am.transformApexUrl(QUrl(filePath));

    sr->select(url.toLocalFile());
}

QString FlowApi::readFile(const QString& fileName) {
    QFile file(fileName);
    return file.readAll();
}

QString FlowApi::fromCurrentDir(const QString& fileName) {
    return baseDir.absoluteFilePath(fileName);
}
