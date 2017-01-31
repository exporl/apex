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

#include <QDebug>

#include "unittest.h"
#include "tools.h"

//libapex
#include "experimentparser.h"
#include "experimentdata.h"

//libwriter
#include "experimentwriter.h"

//libtools
#include "exceptions.h"

//xercesc
#include "xml/xercesinclude.h"

void UnitTest::initTestCase()
{
    xercesc::XMLPlatformUtils::Initialize();         // setup XML environment
    //app = new QApplication(0, 0);
}

void UnitTest::cleanupTestCase()
{
    //app->exit();
    //delete app;
    xercesc::XMLPlatformUtils::Terminate();
}

void UnitTest::testExperiment_data()
{
   QTest::addColumn<QString>("testFile");

   testDir.setPath(experimentsDir);
   testDir.setPath(testDir.absolutePath());

    Q_FOREACH(QString testFile, testDir.entryList(
                QStringList() << "*.xml" << "*.apx")) {
        QTest::newRow("experiment") << testFile;
    }

}

void UnitTest::testExperiment()
{
    QFETCH(QString, testFile);

    QString outfile = outfileName(testFile);
    testFile = testDir.absoluteFilePath(testFile);

    if (!QFile::exists(testFile))
        QFAIL(qPrintable(QString("Could not open test file: %1").arg(testFile)));

    qDebug() << "** Testing " << testFile;

    //parse the test file
    apex::data::ExperimentData* data;
    try
    {
        apex::ExperimentParser parser(testFile);
        data = parser.parse(false);
    }
    catch (ApexStringException e)
    {
        QFAIL(qPrintable(QString("Could not parse test file: %1").arg(e.what())));
    }

    qDebug() << "parsed initial experiment file";

    //write the parsed data
    QDir out("../../" + outDir);
    qDebug() << "outdir: ";
    qDebug() << out.path();
    outfile = out.absoluteFilePath(outfile);
    qDebug() << "outfile: " << outfile;
    try
    {
        apex::writer::ExperimentWriter::write(*data, outfile);
    }
    catch (xercesc::DOMException e)
    {
        qDebug() << "dom exception: " << e.getMessage() << " code: " << e.code;
        QFAIL("DOM exception during write");
    }
    catch (ApexStringException e)
    {
        qDebug() << "exception: " << e.what();
        QFAIL("Exception during write");
    }

    qDebug() << "written parsed data";

    //parse the written file
    apex::data::ExperimentData* data2;
    try
    {
        apex::ExperimentParser parser2(outfile);
        data2 = parser2.parse(false);
    }
    catch (ApexStringException e)
    {
        qDebug() << "exception: " << e.what();
        QFAIL("written xml file could not be validated");
    }

    qDebug() << "parsed written data";

    //and verify data
    QVERIFY(*data == *data2);
}
// int main()
// {
//     UnitTest t;
//     t.initTestCase();
//     t.testExperiment();
//     t.cleanupTestCase();
// }
QTEST_MAIN(UnitTest);
