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

#include "apexdata/result/resultparameters.h"

#include "apextools/apexpaths.h"
#include "apextools/apextools.h"

#include "common/global.h"

#include "gui/centralwidget.h"
#include "gui/mainwindow.h"

#include "apexcontrol.h"
#include "flowrunner.h"
#include "simplerunner.h"

#include <QFileDialog>
#include <QStandardPaths>

using namespace apex;

SimpleRunner::SimpleRunner() : flowRunner(0)
{
}

void SimpleRunner::selectFromDir(const QString &path)
{
    qCDebug(APEX_RS, "Selecting from Directory %s", qPrintable(path));

    QStringList filters;
    filters << "APEX  files (*.apx *.apf *.apr *.xml)"
            << "APEX experiment files (*.apx)"
            << "APEX flow (*.apf)"
            << "APEX results files (*.apr)"
            << "XML files (*.xml)"
            << "All files (*)";

    QFileDialog dlg(ApexControl::Get().mainWindow(), QString(), path,
                    filters.join(QL1S(";;")));
    QStringList docLocations =
        QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    if (path.isEmpty() && !docLocations.isEmpty())
        dlg.setDirectory(docLocations.first());
    ApexTools::expandWidgetToWindow(&dlg);
    if (dlg.exec() == QDialog::Accepted)
        select(dlg.selectedFiles().first());
}

bool SimpleRunner::select(const QString &name)
{
    qCDebug(APEX_RS, "Selecting File %s", qPrintable(name));
    if (name.endsWith(QL1S(".apr"))) { // show results
        data::ResultParameters rvparam;
        rvparam.setShowResultsAfter(true);
        rvparam.setSaveResults(false);
        resultViewer.reset(new ResultViewer(&rvparam, name));
        QDir::setCurrent(QFileInfo(name).absolutePath());
        resultViewer->show(false);
        return true;
    }

    if (name.endsWith(QLatin1String(".apf"))) {
        flowRunner.reset(new FlowRunner());
        connect(flowRunner.data(), SIGNAL(selected(data::ExperimentData *)),
                this, SIGNAL(selected(data::ExperimentData *)));
        connect(flowRunner.data(), SIGNAL(setResultsFilePath(QString)), this,
                SIGNAL(setResultsFilePath(QString)));
        connect(flowRunner.data(), SIGNAL(opened(QString)), this,
                SIGNAL(opened(QString)));
        connect(this, SIGNAL(savedFile(QString)), flowRunner.data(),
                SIGNAL(savedFile(QString)));
        return flowRunner->select(name);
    }

    if (!name.isEmpty()) { // load experiment file
        data::ExperimentData *data = parseExperiment(name);
        Q_EMIT opened(name);
        Q_EMIT selected(data);
        return data != Q_NULLPTR;
    }

    return false;
}

void SimpleRunner::makeVisible()
{
    if (flowRunner) {
        flowRunner->makeVisible();
    }
}

void SimpleRunner::setExpressions(const QMap<QString, QString> &exprs)
{
    expressions = exprs;
}
