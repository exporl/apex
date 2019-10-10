/******************************************************************************
 * Copyright (C) 2019  Michael Hofmann <mh21@mh21.de>                         *
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

#include "coh/cohclient.h"
#include "coh/cohiterator.h"

#include "common/exception.h"
#include "common/random.h"
#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include "cohtest.h"

#include <QDir>
#include <QTest>

using namespace cmn;
using namespace coh;

void CohTest::rfFreeStimulus_data()
{
    QTest::addColumn<QString>("device");

    QTest::newRow("dump") << "dump: dump_test.txt";
#if !defined(Q_OS_ANDROID)
    QTest::newRow("protoslave-dump") << "protoslave: dump: dump_test.txt";
#endif

#if defined(Q_OS_WIN32)
    QTest::newRow("nic4") << "nic4: "
                             "implant=CIC4,"
                             "mode=MP1,"
                             "min_pulse_width_us=25,"
                             "go_live=false";
    QTest::newRow("protoslave-nic4") << "protoslave: "
                                        "nic4: "
                                        "implant=CIC4,"
                                        "mode=MP1,"
                                        "min_pulse_width_us=25,"
                                        "go_live=false";
#endif
}

void CohTest::rfFreeStimulus()
{
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, device);

    QScopedPointer<CohSequence> sequence(new CohSequence(1));

    CohBiphasicStimulus *stimulusStimulus =
        new CohBiphasicStimulus(Coh::CI_ELECTRODE_IC1, Coh::CI_ELECTRODE_EC1,
                                100, 25.0, 8.0, 1000.0, false);
    sequence->append(stimulusStimulus);

    CohRfFreeStimulus *rfFreeStimulus = new CohRfFreeStimulus(1115.0);
    CohSequence *rfFreeSequence = new CohSequence(250);
    rfFreeSequence->append(rfFreeStimulus);
    sequence->append(rfFreeSequence);

    stimulusStimulus =
        new CohBiphasicStimulus(Coh::CI_ELECTRODE_IC1, Coh::CI_ELECTRODE_EC1,
                                100, 25.0, 8.0, 1000.0, false);
    sequence->append(stimulusStimulus);

    TemporaryDirectory dir;
    const QString oldPath = QDir::currentPath();
    QDir::setCurrent(dir.dir().absolutePath());

    QScopedPointer<CohClient> client(CohClient::createCohClient(device));
    client->send(sequence.data());
    client->start(Coh::Immediate);
    Coh::Status status;
    do {
        QTest::qSleep(200);
        status = client->status();
    } while (status != Coh::Idle);
    client->stop();

    QDir::setCurrent(oldPath);

    if (dir.dir().exists(QL1S("dump_test.txt"))) {
        QFile file(dir.addFile(QL1S("dump_test.txt")));
        QVERIFY(file.open(QFile::ReadOnly | QFile::Text));

        static const char textPart0[] = "Sequence: repeats 1\n";
        static const char textPart1[] =
            "  Biphasic: active 1 reference -1 level 100 width 25.0 gap 8.0 "
            "period 1000.0 channel nan magnitude nan trigger 0\n";
        static const char textPart2[] = "  Rffree: period 1115.0\n";
        QByteArray result;
        result.reserve(strlen(textPart0) + 2 * strlen(textPart1) +
                       250 * strlen(textPart2));
        result += textPart0;
        result += textPart1;
        for (unsigned i = 0; i < 250; i += 1)
            result += textPart2;
        result += textPart1;
        QCOMPARE(file.readAll(), result);
    } else if (dir.dir().exists(QL1S("stream.txt"))) {
        QFile file(dir.addFile(QL1S("stream.txt")));
        QVERIFY(file.open(QFile::ReadOnly | QFile::Text));

        unsigned count = 0;
        unsigned part = 0;
        Q_FOREACH (const auto &line, file.readAll().split('\n')) {
            if (line.isEmpty() || line.startsWith("#"))
                continue;
            QList<QByteArray> list = line.simplified().split(' ');
            switch (part) {
            case 0:
                if (list.value(10) != "NO_RF")
                    part = 1;
                break;
            case 1:
                if (list.value(10) == "NO_RF")
                    part = 2;
                break;
            case 2:
                if (list.value(10) != "NO_RF")
                    part = 3;
                break;
            case 3:
                if (list.value(10) == "NO_RF")
                    part = 4;
                break;
            }
            switch (part) {
            case 0:
            case 4:
                QCOMPARE(list.value(9).data(), "250.0");
                QCOMPARE(list.value(10).data(), "NO_RF");
                break;
            case 1:
            case 3:
                QCOMPARE(list.value(1).data(), "standard");
                QCOMPARE(list.value(2).data(), "5cpc");
                QCOMPARE(list.value(3).data(), "1");
                QCOMPARE(list.value(4).data(), "24");
                QCOMPARE(list.value(5).data(), "100");
                QCOMPARE(list.value(6).data(), "25.0");
                QCOMPARE(list.value(7).data(), "8.0");
                QCOMPARE(list.value(8).data(), "25.0");
                QCOMPARE(list.value(9).data(), "1000.0");
                QCOMPARE(list.value(10).data(), "RF");
                count += 1;
                break;
            case 2:
                QCOMPARE(list.value(9).data(), "1115.0");
                QCOMPARE(list.value(10).data(), "NO_RF");
                count += 1;
                break;
            }
        }
        QCOMPARE(count, 250u + 2u);
    } else {
        qCDebug(EXPORL_COH, "Available files:");
        Q_FOREACH (const auto &entry, dir.dir().entryList(QDir::Files)) {
            qCDebug(EXPORL_COH, "  %s", qPrintable(entry));
        }
        QFAIL("Unable to find any known output file");
    }

    TEST_EXCEPTIONS_CATCH
}
