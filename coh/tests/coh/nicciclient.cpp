/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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
#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include "cohtest.h"

#include <QDir>
#include <QTest>

using namespace cmn;
using namespace coh;

void CohTest::nicCiClient_data()
{
    QTest::addColumn<QString>("device");
    QTest::addColumn<unsigned>("delay");
    QTest::addColumn<bool>("needsreload");
    QTest::addColumn<double>("period");

    QTest::newRow("dump") << "dump: dump_test.txt" << 0u << false << 1115.0;
    QTest::newRow("dump-period") << "dump: dump_test.txt" << 0u << false
                                 << 1115.5;

#if !defined(Q_OS_ANDROID)
    QTest::newRow("protoslave-dump") << "protoslave: dump: dump_test.txt" << 0u
                                     << false << 1115.0;
    QTest::newRow("protoslave-dump-period") << "protoslave: dump: dump_test.txt"
                                            << 0u << false << 1115.5;
#endif

#if defined(Q_OS_WIN32)
    QTest::newRow("nic") << "nic: l34-0" << 0u << true << 1115.0;
    QTest::newRow("nic-period") << "nic: l34-0" << 0u << true << 1115.5;
    QTest::newRow("protoslave-nic") << "protoslave: nic: l34-0" << 0u << false
                                    << 1115.0;
    QTest::newRow("protoslave-nic-period") << "protoslave: nic: l34-0" << 0u
                                           << false << 1115.5;

    QTest::newRow("nic-socket") << "nic-socket: l34-0" << 0u << true << 1115.0;
    QTest::newRow("nic-socket-period") << "nic-socket: l34-0" << 0u << true
                                       << 1115.5;

    QTest::newRow("protoslave-nic-socket") << "protoslave: nic-socket: l34-0"
                                           << 0u << false << 1115.0;
    QTest::newRow("protoslave-nic-socket-period")
        << "protoslave: nic-socket: l34-0" << 0u << false << 1115.5;

    QTest::newRow("protoslave-nic3slave-nic31")
        << "protoslave: "
           "python=2.5,"
           "slave=nic3slave;" // ",verbose" for debugging
           "implant=CIC4,"
           "platform=L34,"
           "min_pulse_width_us=25.0,"
           "auto_pufs=off,"
           "go_live=false"
        << 20u << false << 1115.0;
    QTest::newRow("protoslave-nic3slave-nic31-period")
        << "protoslave: "
           "python=2.5,"
           "slave=nic3slave;" // ",verbose" for debugging
           "implant=CIC4,"
           "platform=L34,"
           "min_pulse_width_us=25.0,"
           "auto_pufs=off,"
           "go_live=false"
        << 20u << false << 1115.5;

    QTest::newRow("protoslave-nic3slave-nic32")
        << "protoslave: "
           "python=2.7,"
           "slave=nic3slave;" // ",verbose" for debugging
           "implant=CIC4,"
           "platform=L34,"
           "mode=MP1,"
           "min_pulse_width_us=25.0,"
           "flagged_electrodes=,"
           "auto_pufs=off,"
           "go_live=false"
        << 20u << false << 1115.0;
    QTest::newRow("protoslave-nic3slave-nic32-period")
        << "protoslave: "
           "python=2.7,"
           "slave=nic3slave;" // ",verbose" for debugging
           "implant=CIC4,"
           "platform=L34,"
           "mode=MP1,"
           "min_pulse_width_us=25.0,"
           "flagged_electrodes=,"
           "auto_pufs=off,"
           "go_live=false"
        << 20u << false << 1115.5;
#endif
}

void CohTest::nicCiClient()
{
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, device);
    QFETCH(unsigned, delay);
    QFETCH(bool, needsreload);
    QFETCH(double, period);

    unsigned count1 = 2500;
    unsigned count2 = 500;

    // Powerup
    CohSequence *powerupSequence = new CohSequence(count1);
    QCOMPARE(powerupSequence->numberRepeats(), count1);

    CohNullStimulus *powerupStimulus = new CohNullStimulus(period, false);
    powerupSequence->append(powerupStimulus);
    QCOMPARE(powerupSequence->numberRepeats(), count1);

    // Powerup
    CohSequence *powerupSequence2 = new CohSequence(1);
    powerupSequence2->append(powerupSequence);
    QCOMPARE(powerupSequence2->numberRepeats(), 1u);

    // Stimuli
    CohSequence *stimulusSequence = new CohSequence(count2);
    CohBiphasicStimulus *stimulusStimulus =
        new CohBiphasicStimulus(Coh::CI_ELECTRODE_IC1, Coh::CI_ELECTRODE_EC1,
                                100, 25.0, 8.0, 1000.0, false);
    stimulusStimulus->setTrigger(true);
    stimulusSequence->append(stimulusStimulus);

    QScopedPointer<CohSequence> sequence(new CohSequence(2));
    sequence->append(powerupSequence2);
    sequence->append(stimulusSequence);

    TemporaryDirectory dir;
    const QString oldPath = QDir::currentPath();
    QDir::setCurrent(dir.dir().absolutePath());

    QScopedPointer<CohClient> client(CohClient::createCohClient(device));

    {
        QCOMPARE(client->needsReloadForStop(), needsreload);
        client->send(sequence.data());
        client->start(Coh::Immediate);
        Coh::Status status;
        double currentDelay = 0.0;
        do {
            QTest::qSleep(200);
            status = client->status();
            currentDelay += 0.2;
        } while (status != Coh::Idle || (delay > 0 && currentDelay < delay));
    }

    dir.addFile(QL1S("NreSystems.log"));
    QDir::setCurrent(oldPath);

    if (dir.dir().exists(QL1S("nic_test.xml"))) {
        QFile file(dir.addFile(QL1S("nic_test.xml")));
        QVERIFY(file.open(QFile::ReadOnly | QFile::Text));

        client->stop();

        QCOMPARE(file.readAll(), nic_test_xml(sequence.data()).toUtf8());
    } else if (dir.dir().exists(QL1S("nic_test.txt"))) {
        QFile file(dir.addFile(QL1S("nic_test.txt")));
        QVERIFY(file.open(QFile::ReadOnly | QFile::Text));

        client->stop();

        QCOMPARE(file.readAll(), nic_test_txt(sequence.data()).toUtf8());
    } else if (dir.dir().exists(QL1S("stream_log.txt"))) {
        QFile file(dir.addFile(QL1S("stream_log.txt")));
        QVERIFY(file.open(QFile::ReadOnly | QFile::Text));

        client->stop();

        unsigned pos = 0;
        Q_FOREACH (const auto &line, file.readAll().split('\n')) {
            QList<QByteArray> list = line.simplified().split(' ');
            if (line.isEmpty() || list.isEmpty() || list[0] == "#")
                continue;
            if (list.size() < 4)
                qCDebug(EXPORL_COH, "-- %d: %s --", pos,
                        line.simplified().data());
            if (pos % (count1 + count2) < count1) {
                QCOMPARE(list.size(), 13);
                QCOMPARE(list[1].data(), "29");
                QCOMPARE(list[2].data(), "0");
                QCOMPARE(list[3].data(), "20");
                QCOMPARE(list[4].data(), "25.0");
                QCOMPARE(list[5].data(), "25.0");
                QCOMPARE(list[6].data(), "7.0");
                QCOMPARE(list[7],
                         QByteArray::number(roundedCohPeriod(period), 'f', 1));
                QCOMPARE(list[8].data(), "-");
                QCOMPARE(list[9].data(), "User");
                QCOMPARE(list[10].data(), "Data");
                QCOMPARE(list[11].data(), "-");
                QCOMPARE(list[12].data(), "NullStimulus");
            } else {
                QCOMPARE(list.size(),
                         pos < 2 * (count1 + count2) - 1 ? 13 : 16);
                QCOMPARE(list[1].data(), "1");
                QCOMPARE(list[2].data(), "24");
                QCOMPARE(list[3].data(), "100");
                QCOMPARE(list[4].data(), "25.0");
                QCOMPARE(list[5].data(), "25.0");
                QCOMPARE(list[6].data(), "8.0");
                QCOMPARE(list[7].data(), "1000.0");
                QCOMPARE(list[8].data(), "-");
                QCOMPARE(list[9].data(), "User");
                QCOMPARE(list[10].data(), "Data");
                QCOMPARE(list[11].data(), "-");
                QCOMPARE(list[12].data(), "Trigger");
                if (pos == 2 * (count1 + count2) - 1) {
                    QCOMPARE(list[13].data(), "-");
                    QCOMPARE(list[14].data(), "Last");
                    QCOMPARE(list[15].data(), "Frame");
                }
            }
            pos = pos + 1;
        }
        QCOMPARE(pos, 2 * (count1 + count2));
    } else /*if (dir.dir().exists("dump_test.txt"))*/ {
        QFile file(dir.addFile(QL1S("dump_test.txt")));
        QVERIFY(file.open(QFile::ReadOnly | QFile::Text));

        client->stop();

        static const char textPart0[] = "Sequence: repeats 1\n";
        static const char textPart1[] = "  Null: period 1115.0 trigger 0\n";
        static const char textPart2[] =
            "  Biphasic: active 1 reference -1 "
            "level 100 width 25.0 gap 8.0 period 1000.0 channel nan magnitude "
            "nan trigger 1\n";
        QByteArray result;
        result.reserve(strlen(textPart0) + strlen(textPart1) * count1 * 2 +
                       strlen(textPart2) * count2 * 2);
        result += textPart0;
        for (unsigned j = 0; j < 2; ++j) {
            for (unsigned i = 0; i < count1; ++i)
                result += textPart1;
            for (unsigned i = 0; i < count2; ++i)
                result += textPart2;
        }
        result.replace("1115.0",
                       QByteArray::number(roundedCohPeriod(period), 'f', 1));
        QCOMPARE(file.readAll(), result);
    }

    TEST_EXCEPTIONS_CATCH
}
