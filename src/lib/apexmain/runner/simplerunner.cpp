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

#include "common/global.h"
#include "apexdata/experimentdata.h"

#include "apexdata/result/resultparameters.h"

#include "apextools/services/paths.h"

#include "gui/centralwidget.h"
#include "gui/mainwindow.h"

// TODO ANDROID resultviewer uses webkitwidgets
#ifndef Q_OS_ANDROID
#include "result/resultviewer.h"
#endif

#include "services/errorhandler.h"
#include "services/filedialog.h"

#include "apexcontrol.h"
#include "simplerunner.h"

// TODO ANDROID flowerunner uses webkitwidgets
#ifndef Q_OS_ANDROID
#include "flowrunner.h"
#endif

#include <QDebug>

using namespace apex;

SimpleRunner::SimpleRunner()
    : flowRunner(0)
{}

void SimpleRunner::selectFromDir (const QString& path)
{
    qCDebug(APEX_RS, "Selecting from Directory %s", qPrintable (path));

    QStringList filters;
    filters << "APEX  files (*.apx *.apf *.apr *.xml)"
            << "APEX experiment files (*.apx)"
            << "APEX flow (*.apf)"
            << "APEX results files (*.apr)"
            << "XML files (*.xml)"
            << "All files (*)";

    select( FileDialog::Get().mf_sGetExistingFile( path, filters ) );
}

void SimpleRunner::select (const QString& name)
{
    qCDebug(APEX_RS, "Selecting File %s", qPrintable (name));
    if (name.endsWith(QL1S(".apr"))) { // show results
        // TODO ANDROID resultviewer uses webkitwidgets
#ifndef Q_OS_ANDROID
        qCDebug(APEX_RS, "Loading resultsviewer for %s", qPrintable(name));
        data::ResultParameters rvparam;
        rvparam.setShowResultsAfter(true);
        rvparam.setSaveResults(false);
        ResultViewer rv(&rvparam, name);
        connect(&rv, SIGNAL(errorMessage(QString,QString)), this, SIGNAL(errorMessage(QString, QString)));
        QDir::setCurrent(QFileInfo(name).absolutePath());
        rv.show(false);
#endif
    } else if(name.endsWith(QLatin1String(".apf"))) {
        // TODO ANDROID rtresultsink uses webkitwidgets
#ifndef Q_OS_ANDROID
        flowRunner = new FlowRunner();
        connect(flowRunner, SIGNAL(selected(data::ExperimentData*)), this, SIGNAL(selected(data::ExperimentData*)));
        connect(flowRunner, SIGNAL(setResultsFilePath(QString)), this, SIGNAL(setResultsFilePath(QString)));
        connect(flowRunner, SIGNAL(foundExpressions(QMap<QString, QString>)), this, SLOT(setExpressions(QMap<QString, QString>)));
        connect(flowRunner, SIGNAL(opened(QString)), this, SIGNAL(opened(QString)));
        connect(this, SIGNAL(savedFile(QString)), flowRunner, SIGNAL(savedFile(QString)));
        flowRunner->select(name);
#endif
    } else if (!name.isEmpty()) {               // load experiment file
        //apex::ExperimentParser* parser = new apex::ExperimentConfigFileParser( name );
        data::ExperimentData* data = parseExperiment(name);
        emit opened(name);
        emit selected(data);
    }
}

void SimpleRunner::makeVisible() {
    // TODO ANDROID flowrunner uses webkitwidgets
#ifndef Q_OS_ANDROID
    if (flowRunner) {
        flowRunner->makeVisible();
    }
#endif
}

void SimpleRunner::setExpressions(const QMap<QString, QString>& exprs) {
    expressions = exprs;
}
