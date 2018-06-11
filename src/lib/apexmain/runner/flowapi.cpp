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

#include "flowapi.h"
#include "accessmanager.h"
#include "apextools/apexpaths.h"

#include <QFile>
#include <QUrl>

using namespace apex;

FlowApi::FlowApi(FlowRunner *fr, const QDir &baseDir)
    : BaseApi(nullptr, baseDir), fr(fr)
{
    sr = new SimpleRunner;

    connect(sr, SIGNAL(selected(data::ExperimentData *)), fr,
            SLOT(select(data::ExperimentData *)));
    connect(this, SIGNAL(setResultsFilePath(QString)), fr,
            SIGNAL(setResultsFilePath(QString)));
}

bool FlowApi::runExperiment(const QString &filePath,
                            const QString &resultsFilePath)
{
    if (!resultsFilePath.isEmpty())
        Q_EMIT setResultsFilePath(resultsFilePath);

    AccessManager am;
    QUrl url = am.transformApexUrl(QUrl(filePath));

    sr->setExpressions(savedExpressions);
    savedExpressions.clear();

    fr->makeInvisible();
    bool result = sr->select(url.toLocalFile());
    if (!result)
        fr->makeVisible();
    return result;
}

void FlowApi::clearExpressions()
{
    savedExpressions.clear();
}

void FlowApi::addExpression(const QString &key, const QString &value)
{
    savedExpressions.insert(key, value);
}
