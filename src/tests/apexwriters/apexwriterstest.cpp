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

#include "apexmain/experimentparser.h"

#include "apextools/exceptions.h"

#include "apextools/services/paths.h"

#include "apextools/xml/xercesinclude.h"

#include "apexwriters/experimentwriter.h"

#include "apexwriterstest.h"

#include <QDebug>

#include <iostream>

void ApexWritersTest::initTestCase()
{
    xercesc::XMLPlatformUtils::Initialize();
}

QString ApexWritersTest::outfileName(const QString& infile)
{
    return "__out__" + infile;
}

void ApexWritersTest::testExperiment_data()
{
   QTest::addColumn<QString>("testFile");
   QTest::addColumn<QString>("outFile");

    apex::Paths paths;
    QDir testDir(paths.GetBasePath());

    if (!testDir.cd("data/tests") || !testDir.cd("libwriters"))
    {
        qCDebug(APEX_RS) << "cannot find data for the writers test";
        return;
    }

    QDir outDir = testDir;

    if (!outDir.exists("out"))
        outDir.mkdir("out");

    outDir.cd("out");

    testDir.setNameFilters(QStringList() << "*.xml" << "*.apx");
    Q_FOREACH (QString testFile, testDir.entryList(QDir::AllEntries))
    {
        QString outFile = outfileName(testFile);
        QTest::newRow(qPrintable(testFile)) << testDir.absoluteFilePath(testFile)
                                            << outDir.absoluteFilePath(outFile);
    }
}

void ApexWritersTest::testExperiment()
{
    QFETCH(QString, testFile);
    QFETCH(QString, outFile);

    if (!QFile::exists(testFile))
        QFAIL(qPrintable(QString("Could not open test file: %1").arg(testFile)));

    qCDebug(APEX_RS) << "** Testing " << testFile;

    //parse the test file
    apex::data::ExperimentData* data;
    try
    {
//        std::cout<<"exception in the constructor"<<std::endl;
        apex::ExperimentParser parser(testFile);
//        std::cout<<"exception in the parse function"<<std::endl;

        data = parser.parse(false);
//        std::cout<<"jump"<<endl;
    }
    catch (const std::exception& e)
    {
        QFAIL(qPrintable(QString("Could not parse test file: %1").arg(e.what())));
    }
    catch (...)
    {
        QFAIL(qPrintable(QString("Unknown exception while parsing %1").arg(testFile)));
    }

    qCDebug(APEX_RS) << "parsed initial experiment file";

    //write the parsed data
    try
    {
        apex::writer::ExperimentWriter::write(*data, outFile);
    }
    catch (const xercesc::DOMException& e)
    {
        qCDebug(APEX_RS) << "dom exception: " << X2S(e.getMessage()) << " code: " << e.code;
        QFAIL("DOM exception during write");
    }
    catch (const std::exception& e)
    {
        qCDebug(APEX_RS) << "exception: " << e.what();
        qCDebug(APEX_RS) <<"Output file: "<< outFile;
        QFAIL("Exception during write");
    }

    qCDebug(APEX_RS) << "written parsed data";

    //parse the written file
    apex::data::ExperimentData* data2;
    try
    {
        apex::ExperimentParser parser2(outFile);
        data2 = parser2.parse(false);
    }
    catch (const std::exception& e)
    {
        qCDebug(APEX_RS) << "exception: " << e.what();
        qCDebug(APEX_RS) << "output file: "<< outFile;
        QFAIL("written xml file could not be validated");
    }
    catch (...)
    {
        QFAIL(qPrintable(QString("Unknown exception while parsing %1").arg(outFile)));
    }

    qCDebug(APEX_RS) << "parsed written data2";

    //and verify data
    QVERIFY(*data == *data2);
}

void ApexWritersTest::cleanupTestCase()
{
    xercesc::XMLPlatformUtils::Terminate();
}

//generate standalone binary for the test case
QTEST_MAIN(ApexWritersTest)
