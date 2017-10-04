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

#include "apexwriters/experimentwriter.h"

#include "common/debug.h"
#include "common/paths.h"
#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include "apexwriterstest.h"

using namespace apex;
using namespace apex::data;
using namespace apex::writer;
using namespace cmn;

void ApexWritersTest::initTestCase()
{
    enableCoreDumps(QCoreApplication::applicationFilePath());
}

void ApexWritersTest::testExperiment_data()
{
    QTest::addColumn<QString>("testFile");

    QDir testDir(Paths::searchDirectory(QL1S("tests/libwriters"),
                                        Paths::dataDirectories()));
    testDir.setNameFilters(QStringList() << "*.xml"
                                         << "*.apx");
    Q_FOREACH (QString testFile, testDir.entryList(QDir::AllEntries))
        QTest::newRow(qPrintable(testFile))
            << testDir.absoluteFilePath(testFile);
}

void ApexWritersTest::testExperiment()
{
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, testFile);

    TemporaryDirectory dir;
    QString outFile = dir.addFile(QSL("out.apx"));

    QScopedPointer<ExperimentData> data(
        ExperimentParser(testFile).parse(false));
    ExperimentWriter::write(*data, outFile);
    QScopedPointer<ExperimentData> data2(
        ExperimentParser(outFile).parse(false));

    QVERIFY(*data == *data2);

    TEST_EXCEPTIONS_CATCH
}

QTEST_MAIN(ApexWritersTest)
