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

#include "apexwriterstest.h"

#include <QDebug>

//#include "tools.h"

//libapex
#include "experimentparser.h"
#include "experimentdata.h"
#include "services/paths.h"

//libwriter
#include "experimentwriter.h"

//libtools
#include "exceptions.h"

//xercesc
#include "xml/xercesinclude.h"


QString ApexWritersTest::name() const
{
    return "libwriters";
}

QString ApexWritersTest::outfileName(const QString& infile)
{
    return "__out__" + infile;
}

void ApexWritersTest::testExperiment_data()
{
   QTest::addColumn<QString>("testFile");
   QTest::addColumn<QString>("outFile");

//    testDir.setPath(experimentsDir);
//    testDir.setPath(testDir.absolutePath());
// 
//     Q_FOREACH(QString testFile, testDir.entryList(
//                 QStringList() << "*.xml" << "*.apx")) {
//         QTest::newRow("experiment") << testFile;
//     }

    apex::Paths paths;
    QDir testDir(paths.GetBasePath());

    if (!testDir.cd("data/tests") || !testDir.cd(name()))
    {
        qDebug() << "cannot find data for the writers test";
        return;
    }

    QDir outDir = testDir;

    if (!outDir.exists("out"))
        outDir.mkdir("out");

    outDir.cd("out");

    //make a list of all dirs to check: we check 1 level down into the
    //current testDir
    QList<QDir> testDirs;
    testDirs << testDir;

    Q_FOREACH (QString dirName, testDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        if (dirName != "out")
            testDirs << QDir(testDir.absoluteFilePath(dirName));
    }

    Q_FOREACH (QDir dir, testDirs)
    {
        dir.setNameFilters(QStringList() << "*.xml" << "*.apx");
        Q_FOREACH (QString testFile, dir.entryList(QDir::AllEntries))
        {
            QString outFile = outfileName(testFile);
            QTest::newRow(qPrintable(testFile)) << dir.absoluteFilePath(testFile)
                                                << outDir.absoluteFilePath(outFile);
        }
    }
}

void ApexWritersTest::testExperiment()
{
    QFETCH(QString, testFile);
    QFETCH(QString, outFile);

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
    qDebug() << "outfile: " << outFile;
    try
    {
        apex::writer::ExperimentWriter::write(*data, outFile);
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
        apex::ExperimentParser parser2(outFile);
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

Q_EXPORT_PLUGIN2(test_libwriters, ApexWritersTest);
