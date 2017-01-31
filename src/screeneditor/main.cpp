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
 
#include <qapplication.h>
#include <qpushbutton.h>

#include "screeneditor.h"
#include "experimentfilemanager.h"

#include <QFileDialog>

using apex::editor::ExperimentFileManager;

/**
 * This is the main function of the screeneditor application.
 */
int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    QString fileName = QFileDialog::getOpenFileName(
        0, QObject::tr( "Open Experiment" ), QString(),
      QObject::tr( "Apex Experiment Files(*.xml *.apx)" ) );
    if ( !fileName.isEmpty() )
    {
        ExperimentFileManager* fm = new ExperimentFileManager(
            qApp, fileName );
        fm->openScreenEditors();
        return a.exec();
    }
    return 0;
}

