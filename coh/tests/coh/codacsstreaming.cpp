/******************************************************************************
 * Copyright (C) 2007-2013  Michael Hofmann <mh21@mh21.de>                    *
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

#include "common/exception.h"
#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include "cohtest.h"

#include <QDir>
#include <QTest>

using namespace cmn;
using namespace coh;

void CohTest::codacsStreaming_data()
{
    QTest::addColumn<QString>("device");
    QTest::addColumn<double>("period");
    QTest::addColumn<unsigned>("delay");
    QTest::addColumn<bool>("needsreload");

    QTest::newRow("dump") << "dump: dump_test.txt"
                          << CohCodacsStimulus::defaultPeriod() << 0u << false;
    QTest::newRow("dump-52") << "dump: dump_test.txt" << 52.0 << 0u << false;

#if !defined(Q_OS_ANDROID)
    QTest::newRow("protoslave-dump") << "protoslave: dump: dump_test.txt"
                                     << CohCodacsStimulus::defaultPeriod() << 0u
                                     << false;
    QTest::newRow("protoslave-dump-52") << "protoslave: dump: dump_test.txt"
                                        << 52.0 << 0u << false;
#endif

#if defined(Q_OS_WIN32)
    QTest::newRow("nic") << "nic: l34-0" << CohCodacsStimulus::defaultPeriod()
                         << 0u << true;
    QTest::newRow("protoslave-nic") << "protoslave: nic: l34-0"
                                    << CohCodacsStimulus::defaultPeriod() << 0u
                                    << false;

    QTest::newRow("nic-socket") << "nic-socket: l34-0"
                                << CohCodacsStimulus::defaultPeriod() << 0u
                                << true;
    QTest::newRow("protoslave-nic-socket") << "protoslave: nic-socket: l34-0"
                                           << CohCodacsStimulus::defaultPeriod()
                                           << 0u << false;

    QTest::newRow("protoslave-nic3slave-nic31")
        << "protoslave: "
           "python=2.5,"
           "slave=nic3slave,"
           "verbose=true,"
           "args=flexibleframeperiods extendedamplituderange "
           "allowimplantcommands;"
           "implant=CIC4,"
           "platform=L34,"
           "min_pulse_width_us=12.0,"
           "auto_pufs=off,"
           "go_live=false"
        << CohCodacsStimulus::defaultPeriod() << 20u << false;

    /*
    QTest::newRow("protoslave-nic3slave-nic32")
        << "protoslave: "
           "python=2.7,"
           "slave=nic3slave,"
           "verbose=true,"
           "args=flexibleframeperiods extendedamplituderange "
           "allowimplantcommands;"
           "implant=CIC4,"
           "platform=L34,"
           "mode=MP1+2,"
           "min_pulse_width_us=12.0,"
           "flagged_electrodes=,"
           "auto_pufs=off,"
           "go_live=false"
        << CohCodacsStimulus::defaultPeriod() << 20u << false;
    */

    QTest::newRow("nic-52") << "nic: l34-0" << 52.0 << 0u << true;
    QTest::newRow("protoslave-nic-52") << "protoslave: nic: l34-0" << 52.0 << 0u
                                       << false;

    QTest::newRow("nic-socket-52") << "nic-socket: l34-0" << 52.0 << 0u << true;
    QTest::newRow("protoslave-nic-socket-52") << "protoslave: nic-socket: l34-0"
                                              << 52.0 << 0u << false;

    QTest::newRow("protoslave-nic3slave-nic31-52")
        << "protoslave: "
           "python=2.5,"
           "slave=nic3slave," // "verbose," for debugging
           "args=flexibleframeperiods extendedamplituderange "
           "allowimplantcommands;"
           "implant=CIC4,"
           "platform=L34,"
           "min_pulse_width_us=12.0,"
           "auto_pufs=off,"
           "go_live=false"
        << 52.0 << 20u << false;

/*
QTest::newRow("protoslave-nic3slave-nic32-52")
    << "protoslave: "
       "python=2.7,"
       "slave=nic3slave," // "verbose," for debugging
       "args=flexibleframeperiods extendedamplituderange "
       "allowimplantcommands;"
       "implant=CIC4,"
       "platform=L34,"
       "mode=MP1+2,"
       "min_pulse_width_us=12.0,"
       "flagged_electrodes=,"
       "auto_pufs=off,"
       "go_live=false"
    << 52.0 << 20u << false;
*/
#endif
}

void CohTest::codacsStreaming()
{
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, device);
    QFETCH(double, period);
    QFETCH(unsigned, delay);
    QFETCH(bool, needsreload);

    unsigned count1 = 2500;
    unsigned count2 = 500;

    // Powerup
    CohSequence *powerupSequence = new CohSequence(count1);
    QCOMPARE(powerupSequence->numberRepeats(), count1);

    CohCodacsStimulus *powerupStimulus =
        new CohCodacsStimulus(128, period, false);
    powerupSequence->append(powerupStimulus);
    QCOMPARE(powerupSequence->numberRepeats(), count1);

    // Powerup
    CohSequence *powerupSequence2 = new CohSequence(1);
    powerupSequence2->append(powerupSequence);
    QCOMPARE(powerupSequence2->numberRepeats(), 1u);

    // Stimuli
    CohSequence *stimulusSequence = new CohSequence(count2);
    CohCodacsStimulus *stimulusStimulus =
        new CohCodacsStimulus(1345, period, false);
    stimulusStimulus->setTrigger(true);
    stimulusSequence->append(stimulusStimulus);

    QScopedPointer<CohSequence> sequence(new CohSequence(2));
    sequence->append(powerupSequence2);
    sequence->append(stimulusSequence);

    try {
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
            } while (status != Coh::Idle ||
                     (delay > 0 && currentDelay < delay));
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
                    QCOMPARE(list.size(), 11);
                    QCOMPARE(list[1].data(), "1");
                    QCOMPARE(list[2].data(),
                             "1"); // reference electrode is ignored
                    QCOMPARE(list[3].data(), "0");
                    QCOMPARE(list[4].data(), "12.0");
                    QCOMPARE(list[5].data(), "12.0");
                    QCOMPARE(list[6].data(), "6.0");
                    QCOMPARE(list[7], QByteArray::number(period, 'f', 1));
                    QCOMPARE(list[8].data(), "-");
                    QCOMPARE(list[9].data(), "User");
                    QCOMPARE(list[10].data(), "Data");
                } else {
                    QCOMPARE(list.size(),
                             pos < 2 * (count1 + count2) - 1 ? 13 : 16);
                    QCOMPARE(list[1].data(), "10");
                    QCOMPARE(list[2].data(),
                             "1"); // reference electrode is ignored
                    QCOMPARE(list[3].data(), "130");
                    QCOMPARE(list[4].data(), "12.0");
                    QCOMPARE(list[5].data(), "12.0");
                    QCOMPARE(list[6].data(), "6.0");
                    QCOMPARE(list[7], QByteArray::number(period, 'f', 1));
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
        } else if (dir.dir().exists(QL1S("dump_test.txt"))) {
            QFile file(dir.addFile(QL1S("dump_test.txt")));
            QVERIFY(file.open(QFile::ReadOnly | QFile::Text));

            client->stop();

            static const char textPart0[] = "Sequence: repeats 1\n";
            static const char textPart1[] =
                "  Codacs: amplitude 128 period 51.0 trigger 0\n";
            static const char textPart2[] =
                "  Codacs: amplitude 1345 period 51.0 trigger 1\n";
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
            result = result.replace("51.0", QByteArray::number(period, 'f', 1));
            QCOMPARE(file.readAll(), result);
        } else {
            QSKIP("Unable to find output file");
        }
    } catch (const std::exception &e) {
        QFAIL(qPrintable(QString::fromLatin1("Exception thrown: %1")
                             .arg(QString::fromLocal8Bit(e.what()))));
    }

    TEST_EXCEPTIONS_CATCH
}
