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

#include <QString>
#include <QApplication>

#include "datablockstester.h"
#include "devicestester.h"
#include "procedurestester.h"
#include "filterstester.h"
#include "screenstester.h"
#include "stimulitester.h"
#include "experimenttester.h"

#include "xml/xercesinclude.h"
using namespace xercesc;

#include <QStringList>

#include <iostream>


/**
 * Test apex writers
 *
 * A writer takes a data element and writes it to an XML tree.
 * To test this process, for each data element, we first use the corresponding
 * parser to create it from a given XML file and then write it back to an
 * XML file using the writer.
 * If the input file and resulting files correspond, the test is considered
 * successfull
 */
int main(int argc, char* argv[])
{
    XMLPlatformUtils::Initialize();         // setup XML environment
    QApplication app(argc, argv);

//     /*---test for datablocks---*/
//     QStringList dataDirs;
//     dataDirs << "datablocks";
//
//
//     DatablocksTester data_t;
//
//     if (!data_t.Test(dataDirs))
//         qDebug("Datablocks test FAILED");
//     else
//         qDebug("Datablocks test successfull");
//     /*---end test for datablock---*/
//
//     /*---test for devices---*/
//     QStringList devDirs;
//     devDirs << "devices";
//
//     DevicesTester dev_t;
//
//     if (!dev_t.Test(devDirs))
//         qDebug("Devices test FAILED");
//     else
//         qDebug("Devices test successfull");
//     /*---end test for devices---*/
//
//     /*---test for procedures---*/
//     QStringList procDirs;
//     procDirs << "procedures";
//
//     ProceduresTester proc_t;
//
//     if (!proc_t.Test(procDirs))
//         qDebug("Procedures test FAILED");
//     else
//         qDebug("Procedures test successfull");
//     /*---end test for procedures---*/
//
//     /*---test for filters---*/
//     QStringList filtDirs;
//     filtDirs << "filters";
//
//     FiltersTester filt_t;
//
//     if (!filt_t.Test(filtDirs))
//         qDebug("Filters test FAILED");
//     else
//         qDebug("Filters test successfull");
//     /*---end test for filters---*/
//
//     /*---test for screens---*/
//     QStringList screensDirs;
//     screensDirs << "screens";
//
//     ScreensTester screens_t;
//
//     if (!screens_t.Test(screensDirs))
//         qDebug("Screens test FAILED");
//     else
//         qDebug("Screens test successfull");
//     /*---end test for screens---*/
//
//     /*---test for stimuli---*/
//     QStringList stimuliDirs;
//     stimuliDirs << "stimuli";
//
//     StimuliTester stimuli_t;
//
//     if (!stimuli_t.Test(stimuliDirs))
//         qDebug("Stimuli test FAILED");
//     else
//         qDebug("Stimuli test successfull");
//     /*---end test for stimuli---*/

    ExperimentTester t;
    QStringList expDirs;
    expDirs << "experiments";

    if (!t.Test(expDirs))
        qDebug("Experiment test FAILED");
    else
        qDebug("Experiment test successfull");

    app.exec();
    XMLPlatformUtils::Terminate();
}





