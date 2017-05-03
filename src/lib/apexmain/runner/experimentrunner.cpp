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

#include "apexdata/experimentdata.h"

#include "errorhandler.h"
#include "experimentparser.h"
#include "experimentrunner.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>

using namespace apex;
using namespace apex::data;

ExperimentData* ExperimentRunner::parseExperiment(const QString& fileName)
{
    qCDebug(APEX_RS, "Parsing Experiment");

    data::ExperimentData* data;

    ErrorHandler::instance()->clearCounters();

    if (!QFile(fileName).exists()) {
        qCCritical(APEX_RS, "ApexControl: Experiment file not found");
#ifndef NODIALOG
        QMessageBox::critical(NULL, tr("Error"), tr("Experiment file not found"));
#endif
        return NULL;
    }

    const QString& sConfigFileName(fileName);
    QDir dConfigFileDir(fileName);
    dConfigFileDir.cdUp();
    qCInfo(APEX_RS, "%s", qPrintable(QSL("%1: %2").arg("ApexControl" , "Parsing " + sConfigFileName)));
    try {
        data = ExperimentParser(fileName, expressions).parse(true);
    } catch (std::exception &e) {
        qCCritical(APEX_RS, "ApexControl: %s", e.what());
    } catch (...) {
        qCCritical(APEX_RS, "ApexControl: FAILED");
    }

    if (ErrorHandler::instance()->numberOfErrors())
        return NULL;

    qCWarning(APEX_RS, "ApexControl: DONE");
    return data;
}
