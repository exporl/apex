/******************************************************************************
 * Copyright (C) 2015  Michael Hofmann <mh21@mh21.de>                         *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "coh/aseqloader.h"
#include "coh/cohmatlabdumper.h"
#include "coh/cohtextdumper.h"
#include "coh/cohxmldumper.h"

#include "common/listutils.h"
#include "common/paths.h"
#include "common/testutils.h"

#include "cohtest.h"

#include <QDir>
#include <QFile>
#include <QTest>

using namespace cmn;
using namespace coh;

void CohTest::aseq_data()
{
    QTest::addColumn<QString>("filePath");

    const QDir dir(Paths::searchDirectory(QL1S("cohregressiontests"),
                                          Paths::dataDirectories()));
    const QStringList files =
        dir.entryList(QStringList() << QL1S("*.aseq"), QDir::Files);
    Q_FOREACH (const auto &file, files)
        QTest::newRow(file.toUtf8()) << dir.absoluteFilePath(file);
    QVERIFY(!files.isEmpty());
}

void CohTest::aseq()
{
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, filePath);

    QFile aseq(filePath);
    QVERIFY(aseq.open(QIODevice::ReadOnly));
    QScopedPointer<CohSequence> seq(loadCohSequenceAseq(aseq.readAll()));

    QScopedPointer<QFile> reference;

    reference.reset(
        new QFile(QString(filePath).replace(QL1S(".aseq"), QL1S(".txt"))));
    QVERIFY(reference->open(QIODevice::ReadOnly | QIODevice::Text));
    QCOMPARE(dumpCohSequenceText(seq.data()),
             QString::fromUtf8(reference->readAll()));

    reference.reset(
        new QFile(QString(filePath).replace(QL1S(".aseq"), QL1S(".m"))));
    QVERIFY(reference->open(QIODevice::ReadOnly | QIODevice::Text));
    QCOMPARE(dumpCohSequenceMatlab(seq.data()),
             QString::fromUtf8(reference->readAll()));

    reference.reset(
        new QFile(QString(filePath).replace(QL1S(".aseq"), QL1S(".xml"))));
    QVERIFY(reference->open(QIODevice::ReadOnly | QIODevice::Text));
    QCOMPARE(dumpCohSequenceXml(seq.data()), reference->readAll());

    TEST_EXCEPTIONS_CATCH
}
