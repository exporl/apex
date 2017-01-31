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

#include "simplerunner.h"
#include "experimentdata.h"
#include "services/filedialog.h"
#include "services/errorhandler.h"
#include "services/paths.h"
#include "result/resultparameters.h"
#include "result/resultviewer.h"

#include "apexcontrol.h"
#include "gui/mainwindow.h"
#include "gui/centralwidget.h"

#include <QDebug>

using namespace apex;

void SimpleRunner::SelectFromDir (const QString& path)
{
    qDebug ("Selecting from Directory %s", qPrintable (path));

     QStringList filters;
    filters << "APEX  files (*.apx *.apr *.xml)"
         << "APEX experiment files (*.apx)"
         << "APEX results files (*.apr)"
         << "XML files (*.xml)"
         << "All files (*)";

    Select( FileDialog::Get().mf_sGetExistingFile( path, filters ) );
}

void SimpleRunner::Select (const QString& name)
{
    qDebug ("Selecting File %s", qPrintable (name));
    if (name.endsWith(QLatin1String(".apr"))) { // show results
        qDebug("Loading resultsviewer for %s", qPrintable(name));
        data::ResultParameters rvparam;
        rvparam.setShowResults(true);
        rvparam.setSaveResults(false);
        ResultViewer rv(&rvparam, name, Paths::Get().GetXsltScriptsPath());
        connect(&rv, SIGNAL(errorMessage(QString,QString)), this, SIGNAL(errorMessage(QString, QString)));
        QDir::setCurrent(QFileInfo(name).absolutePath());
        rv.ProcessResult();
        rv.show(false);
    } else if (!name.isEmpty()) {               // load experiment file
        //apex::ExperimentParser* parser = new apex::ExperimentConfigFileParser( name );
        data::ExperimentData* data = parseExperiment(name);
        emit Selected (data);
    }
}

// TODO: Check when this is called
void SimpleRunner::Finished()
{
}

// TODO: Dummy implementation
void SimpleRunner::Run()
{
}
