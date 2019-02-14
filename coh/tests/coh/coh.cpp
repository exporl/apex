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

#include "coh/codacslevelchecker.h"
#include "coh/cohcloner.h"
#include "coh/cohexpander.h"
#include "coh/cohiterator.h"
#include "coh/cohlength.h"
#include "coh/cohlevelchecker.h"
#include "coh/cohmatlabdumper.h"
#include "coh/cohprotodumper.h"
#include "coh/cohprotoloader.h"
#include "coh/cohtextdumper.h"

#include "common/listutils.h"
#include "common/testutils.h"

#include "cohtest.h"

#include <QTest>

#include <iostream>

using namespace cmn;
using namespace coh;

class CustomCommand : public CohCommand
{
public:
    CustomCommand(bool &deleted) : deleted(deleted)
    {
        deleted = false;
    }

    ~CustomCommand()
    {
        deleted = true;
    }

    virtual void accept(CohCommandVisitor *)
    {
    }

private:
    bool &deleted;
};

void CohTest::coh()
{
    TEST_EXCEPTIONS_TRY

    // Deletion
    bool deletedSequence, deleted;
    {
        QScopedPointer<CohSequence> sequence(new CohSequence(1));
        CohSequence *innerSequence = new CohSequence(1);
        sequence->append(innerSequence);
        innerSequence->append(new CustomCommand(deletedSequence));
        sequence->append(new CustomCommand(deleted));

        QCOMPARE(deleted, false);
        QCOMPARE(deletedSequence, false);
    }
    QCOMPARE(deleted, true);
    QCOMPARE(deletedSequence, true);

    // Sequence construction
    const char dump[] =
        "Sequence: repeats 1\n"
        "  Sequence: repeats 25\n"
        "    Null: period 200.0 trigger 0\n"
        "  Sequence: repeats 1\n"
        "    Meta: key string type string value Gänsefüßchen\n"
        "    Meta: key int type int value 5\n"
        "    Meta: key double type double value 10.83\n"
        "    Meta: key bool type bool value true\n"
        "  Sequence: repeats 5\n"
        "    Biphasic: active 1 reference -1 level 100 width 25.0 gap 8.0 "
        "period 1000.0 channel nan magnitude nan trigger 1\n"
        "  Codacs: amplitude 1345 period 51.0 trigger 1\n"
        "  Codacs: amplitude 1349 period 52.0 trigger 1\n";
    QString expandedDump;
    expandedDump.append(QL1S("Sequence: repeats 1\n"));
    for (unsigned i = 0; i < 25; ++i)
        expandedDump.append(QL1S("  Null: period 200.0 trigger 0\n"));
    expandedDump.append(QString::fromUtf8(
        "  Meta: key string type string value Gänsefüßchen\n"));
    expandedDump.append(QL1S("  Meta: key int type int value 5\n"));
    expandedDump.append(QL1S("  Meta: key double type double value 10.83\n"));
    expandedDump.append(QL1S("  Meta: key bool type bool value true\n"));
    for (unsigned i = 0; i < 5; ++i)
        expandedDump.append(QL1S("  Biphasic: active 1 reference -1 level 100 "
                                 "width 25.0 gap 8.0 period 1000.0 channel nan "
                                 "magnitude nan trigger 1\n"));
    expandedDump.append(
        QL1S("  Codacs: amplitude 1345 period 51.0 trigger 1\n"));
    expandedDump.append(
        QL1S("  Codacs: amplitude 1349 period 52.0 trigger 1\n"));
    QString expandedNull;
    expandedNull.append(QL1S("Sequence: repeats 1\n"));
    for (unsigned i = 0; i < 25; ++i)
        expandedNull.append(QL1S("  Null: period 200.0 trigger 0\n"));
    QString expandedNull2;
    expandedNull2.append(QL1S("Sequence: repeats 1\n"));
    expandedNull2.append(QL1S("  Null: period 200.0 trigger 0\n"));
    QString expandedMeta;
    expandedMeta.append(QL1S("Sequence: repeats 1\n"));
    expandedMeta.append(QString::fromUtf8(
        "  Meta: key string type string value Gänsefüßchen\n"));
    expandedMeta.append(QL1S("  Meta: key int type int value 5\n"));
    expandedMeta.append(QL1S("  Meta: key double type double value 10.83\n"));
    expandedMeta.append(QL1S("  Meta: key bool type bool value true\n"));
    QString expandedBiphasic;
    expandedBiphasic.append(QL1S("Sequence: repeats 1\n"));
    for (unsigned i = 0; i < 5; ++i)
        expandedBiphasic.append(
            QL1S("  Biphasic: active 1 reference -1 level 100 "
                 "width 25.0 gap 8.0 period 1000.0 channel nan magnitude nan "
                 "trigger 1\n"));
    QString expandedCodacs;
    expandedCodacs.append(QL1S("Sequence: repeats 1\n"));
    expandedCodacs.append(
        QL1S("  Codacs: amplitude 1345 period 51.0 trigger 1\n"));
    expandedCodacs.append(
        QL1S("  Codacs: amplitude 1349 period 52.0 trigger 1\n"));

    // Matlab
    const char matlab[] = "sequence = struct;\n"
                          "sequence.triggers = zeros(32, 1);\n"
                          "sequence.electrodes = zeros(32, 1);\n"
                          "sequence.references = zeros(32, 1);\n"
                          "sequence.periods = zeros(32, 1);\n"
                          "sequence.amplitudes = zeros(32, 1);\n"
                          "sequence.current_levels = zeros(32, 1);\n"
                          "sequence.phase_widths = zeros(32, 1);\n"
                          "sequence.phase_gaps = zeros(32, 1);\n"
                          "\n"
                          "sequence.triggers(26:32) = 1;\n"
                          "sequence.electrodes(26:30) = 1;\n"
                          "sequence.references(26:30) = -1;\n"
                          "sequence.periods(31) = 51;\n"
                          "sequence.periods(32) = 52;\n"
                          "sequence.periods(1:25) = 200;\n"
                          "sequence.periods(26:30) = 1000;\n"
                          "sequence.amplitudes(31) = 1345;\n"
                          "sequence.amplitudes(32) = 1349;\n"
                          "sequence.current_levels(26:30) = 100;\n"
                          "sequence.phase_widths(26:30) = 25;\n"
                          "sequence.phase_gaps(26:30) = 8;\n"
                          "\n"
                          "sequence.metas = struct;\n"
                          "sequence.metas.bool = [ true, ];\n"
                          "sequence.metas.double = [ 10.83, ];\n"
                          "sequence.metas.int = [ 5, ];\n"
                          "sequence.metas.string = { 'Gänsefüßchen', };\n";

    // Powerup
    CohSequence *powerupSequence = new CohSequence(25);
    QCOMPARE(powerupSequence->numberRepeats(), 25u);

    CohNullStimulus *powerupStimulus = new CohNullStimulus(200.0, 0);
    QCOMPARE(powerupStimulus->period(), 200.0);
    QCOMPARE(powerupStimulus->hasTrigger(), false);

    powerupSequence->append(powerupStimulus);
    QCOMPARE(powerupSequence->at(0), powerupStimulus);
    QCOMPARE(powerupSequence->size(), 1);

    // Meta data
    QScopedPointer<CohMetaData> metaCommand(
        new CohMetaData(QL1S("firstkey"), QL1S("firstvalue")));
    QCOMPARE(metaCommand->key(), QString::fromLatin1("firstkey"));
    QCOMPARE(metaCommand->value(), QVariant(QString::fromLatin1("firstvalue")));
    metaCommand->setKey(QL1S("key"));
    QCOMPARE(metaCommand->key(), QL1S("key"));
    metaCommand->setValue(QL1S("value"));
    QCOMPARE(metaCommand->value(), QVariant(QL1S("value")));
    QCOMPARE(metaCommand->valueType(), QL1S("string"));
    metaCommand->setValue(5);
    QCOMPARE(metaCommand->value(), QVariant(5));
    QCOMPARE(metaCommand->valueType(), QL1S("int"));
    metaCommand->setValue(10.83);
    QCOMPARE(metaCommand->value(), QVariant(10.83));
    QCOMPARE(metaCommand->valueType(), QL1S("double"));
    metaCommand->setValue(true);
    QCOMPARE(metaCommand->value(), QVariant(true));
    QCOMPARE(metaCommand->valueType(), QL1S("bool"));
    metaCommand->setValue(QL1S("string"), QL1S("value"));
    QCOMPARE(metaCommand->value(), QVariant(QL1S("value")));
    QCOMPARE(metaCommand->valueType(), QL1S("string"));
    metaCommand->setValue(QL1S("int"), QL1S("5"));
    QCOMPARE(metaCommand->value(), QVariant(5));
    QCOMPARE(metaCommand->valueType(), QL1S("int"));
    metaCommand->setValue(QL1S("double"), QL1S("10.83"));
    QCOMPARE(metaCommand->value(), QVariant(10.83));
    QCOMPARE(metaCommand->valueType(), QL1S("double"));
    metaCommand->setValue(QL1S("bool"), QL1S("true"));
    QCOMPARE(metaCommand->value(), QVariant(true));
    QCOMPARE(metaCommand->valueType(), QL1S("bool"));

    CohSequence *metaSequence = new CohSequence(1);
    metaSequence->append(
        new CohMetaData(QL1S("string"), QString::fromUtf8("Gänsefüßchen")));
    metaSequence->append(new CohMetaData(QL1S("int"), 5));
    metaSequence->append(new CohMetaData(QL1S("double"), 10.83));
    metaSequence->append(new CohMetaData(QL1S("bool"), true));

    // Stimuli
    CohSequence *stimulusSequence = new CohSequence(5);
    QCOMPARE(stimulusSequence->numberRepeats(), 5u);

    CohBiphasicStimulus *stimulusStimulus =
        new CohBiphasicStimulus(Coh::CI_ELECTRODE_IC1, Coh::CI_ELECTRODE_EC1,
                                100, 25.0, 8.0, 1000.0, false);
    QCOMPARE(stimulusStimulus->activeElectrode(), Coh::CI_ELECTRODE_IC1);
    QCOMPARE(stimulusStimulus->referenceElectrode(), Coh::CI_ELECTRODE_EC1);
    QCOMPARE(stimulusStimulus->currentLevel(), 100u);
    QCOMPARE(stimulusStimulus->phaseWidth(), 25.0);
    QCOMPARE(stimulusStimulus->phaseGap(), 8.0);
    QCOMPARE(stimulusStimulus->period(), 1000.0);
    QCOMPARE(stimulusStimulus->hasTrigger(), false);
    QCOMPARE(stimulusStimulus->isValid(), true);
    stimulusStimulus->setTrigger(true);
    QCOMPARE(stimulusStimulus->hasTrigger(), true);

    stimulusSequence->append(stimulusStimulus);
    QCOMPARE(stimulusSequence->at(0), stimulusStimulus);
    QCOMPARE(stimulusSequence->size(), 1);

    QScopedPointer<CohSequence> sequence(new CohSequence(1));
    QCOMPARE(sequence->numberRepeats(), 1u);
    sequence->append(powerupSequence);
    sequence->append(metaSequence);
    sequence->append(stimulusSequence);
    QCOMPARE(sequence->at(0), powerupSequence);
    QCOMPARE(sequence->at(1), metaSequence);
    QCOMPARE(sequence->at(2), stimulusSequence);
    QCOMPARE(sequence->size(), 3);

    QVERIFY(checkCohSequenceLevel(sequence.data(), 100));
    QVERIFY(!checkCohSequenceLevel(sequence.data(), 99));
    QVERIFY(!checkCohSequenceLevel(sequence.data(), 0));

    QVERIFY(checkCohSequenceLevel(
        sequence.data(),
        MapMaker<QPair<Coh::Electrode, Coh::Electrode>, unsigned>()
            << qMakePair(
                   qMakePair(Coh::CI_ELECTRODE_IC1, Coh::CI_ELECTRODE_EC1),
                   100u)));
    QVERIFY(!checkCohSequenceLevel(
        sequence.data(),
        MapMaker<QPair<Coh::Electrode, Coh::Electrode>, unsigned>()
            << qMakePair(
                   qMakePair(Coh::CI_ELECTRODE_IC2, Coh::CI_ELECTRODE_EC1),
                   100u)));
    QVERIFY(!checkCohSequenceLevel(
        sequence.data(),
        MapMaker<QPair<Coh::Electrode, Coh::Electrode>, unsigned>()
            << qMakePair(
                   qMakePair(Coh::CI_ELECTRODE_IC1, Coh::CI_ELECTRODE_EC1),
                   99u)));

    QCOMPARE(ciSequenceLength(sequence.data()), 1e4);

    CohIterator iterator(sequence.data(), true);
    while (iterator.hasNext())
        iterator.next();
    iterator.toFront();
    for (unsigned j = 0; j < 25; ++j) {
        QVERIFY(iterator.hasNext());
        QVERIFY(dynamic_cast<const CohNullStimulus *>(iterator.next()));
    }
    for (unsigned j = 0; j < 5; ++j) {
        QVERIFY(iterator.hasNext());
        QVERIFY(dynamic_cast<const CohBiphasicStimulus *>(iterator.next()));
    }
    QVERIFY(!iterator.hasNext());

    CohCodacsStimulus *codacsStimulus =
        new CohCodacsStimulus(1345, CohCodacsStimulus::defaultPeriod(), false);
    QCOMPARE(codacsStimulus->amplitude(), 1345);
    QCOMPARE(codacsStimulus->activeElectrode(), Coh::CI_ELECTRODE_IC10);
    QCOMPARE(CohCodacsStimulus::referenceElectrode(), Coh::CI_ELECTRODE_EC1_2);
    QCOMPARE(codacsStimulus->currentLevel(), 130u);
    QCOMPARE(CohCodacsStimulus::phaseWidth(), 12.0);
    QCOMPARE(CohCodacsStimulus::phaseGap(), 6.0);
    QCOMPARE(codacsStimulus->period(), 51.0);
    QCOMPARE(codacsStimulus->hasTrigger(), false);
    codacsStimulus->setTrigger(true);
    QCOMPARE(codacsStimulus->hasTrigger(), true);
    sequence->append(codacsStimulus);

    codacsStimulus = new CohCodacsStimulus(1349, 52.0, false);
    QCOMPARE(codacsStimulus->amplitude(), 1349);
    QCOMPARE(codacsStimulus->activeElectrode(), Coh::CI_ELECTRODE_IC10);
    QCOMPARE(CohCodacsStimulus::referenceElectrode(), Coh::CI_ELECTRODE_EC1_2);
    QCOMPARE(codacsStimulus->currentLevel(), 138u);
    QCOMPARE(CohCodacsStimulus::phaseWidth(), 12.0);
    QCOMPARE(CohCodacsStimulus::phaseGap(), 6.0);
    QCOMPARE(codacsStimulus->period(), 52.0);
    QCOMPARE(codacsStimulus->hasTrigger(), false);
    codacsStimulus->setTrigger(true);
    QCOMPARE(codacsStimulus->hasTrigger(), true);
    sequence->append(codacsStimulus);

    QCOMPARE(dumpCohSequenceText(sequence.data()), QString::fromUtf8(dump));

    QScopedPointer<CohSequence> sequence2(cloneCohSequence(sequence.data()));
    QCOMPARE(dumpCohSequenceText(sequence2.data()), QString::fromUtf8(dump));
    QCOMPARE(dumpCohSequenceMatlab(sequence2.data()),
             QString::fromUtf8(matlab));

    sequence2.reset(expandCohSequence(sequence.data()));
    QCOMPARE(dumpCohSequenceText(sequence2.data()), expandedDump);
    sequence2.reset(
        expandCohSequence(sequence.data(), Coh::NullStimulus | Coh::Sequence));
    QCOMPARE(dumpCohSequenceText(sequence2.data()), expandedNull);
    sequence2.reset(expandCohSequence(sequence.data(), Coh::NullStimulus));
    QCOMPARE(dumpCohSequenceText(sequence2.data()), expandedNull2);
    sequence2.reset(expandCohSequence(sequence.data(),
                                      Coh::BiphasicStimulus | Coh::Sequence));
    QCOMPARE(dumpCohSequenceText(sequence2.data()), expandedBiphasic);
    sequence2.reset(expandCohSequence(sequence.data(),
                                      Coh::CodacsStimulus | Coh::Sequence));
    QCOMPARE(dumpCohSequenceText(sequence2.data()), expandedCodacs);
    sequence2.reset(
        expandCohSequence(sequence.data(), Coh::MetaData | Coh::Sequence));
    QCOMPARE(dumpCohSequenceText(sequence2.data()), expandedMeta);

    QScopedPointer<CohSequence> sequence4(
        loadCohSequenceProto(dumpCohSequenceProto(sequence.data())));
    QCOMPARE(dumpCohSequenceText(sequence4.data()), QString::fromUtf8(dump));

    QCOMPARE(cohCurrentForCurrentLevel(0, Coh::Cic3Implant), 10.0);
    QCOMPARE(cohCurrentForCurrentLevel(255, Coh::Cic3Implant), 1750.0);
    QCOMPARE(cohCurrentForCurrentLevel(0, Coh::Cic4Implant), 17.5);
    QCOMPARE(cohCurrentForCurrentLevel(255, Coh::Cic4Implant), 1750.0);
    QCOMPARE(cohCurrentLevelForCurrent(10.0, Coh::Cic3Implant), 0u);
    QCOMPARE(cohCurrentLevelForCurrent(1750.0, Coh::Cic3Implant), 255u);
    QCOMPARE(cohCurrentLevelForCurrent(17.5, Coh::Cic4Implant), 0u);
    QCOMPARE(cohCurrentLevelForCurrent(1750.0, Coh::Cic4Implant), 255u);
    for (unsigned j = 0; j <= 255; ++j) {
        QCOMPARE(cohCurrentLevelForCurrent(
                     cohCurrentForCurrentLevel(j, Coh::Cic3Implant),
                     Coh::Cic3Implant),
                 j);
        QCOMPARE(cohCurrentLevelForCurrent(
                     cohCurrentForCurrentLevel(j, Coh::Cic4Implant),
                     Coh::Cic4Implant),
                 j);
    }

    for (unsigned j = 0; j < 32; ++j) {
        for (unsigned i = 0; i < 256; i += 2) {
            int amplitude = (j * 256 + i) / 2;
            if (amplitude > 2047)
                amplitude -= 4096;
            int raw = qAbs(amplitude);
            int sign = amplitude < 0 ? -1 : 1;
            QScopedPointer<CohCodacsStimulus> stim;
            stim.reset(new CohCodacsStimulus(
                amplitude, CohCodacsStimulus::defaultPeriod(), false));
            QCOMPARE(stim->amplitude(), amplitude);
            QCOMPARE(stim->activeElectrode(), Coh::Electrode(j));
            QCOMPARE(stim->currentLevel(), i);
            QCOMPARE(stim->expandedAmplitude(),
                     sign *
                         (raw <= 1024
                              ? ((raw - 0) * 2) + 0
                              : raw <= 1280
                                    ? ((raw - 1024) * 8) + 2048
                                    : raw <= 1536
                                          ? ((raw - 1280) * 16) + 4096
                                          : raw <= 1792
                                                ? ((raw - 1536) * 32) + 8192
                                                : ((raw - 1792) * 64) + 16384));
        }
    }

    QScopedPointer<CohSequence> codacsSequence(new CohSequence(5));
    codacsSequence->append(
        new CohCodacsStimulus(1024, CohCodacsStimulus::defaultPeriod(), false));
    codacsSequence->append(new CohCodacsStimulus(1024, 52.0, false));
    QVERIFY(checkCodacsSequenceLevel(codacsSequence.data(), 1024));
    QVERIFY(!checkCodacsSequenceLevel(codacsSequence.data(), 1023));
    QVERIFY(!checkCodacsSequenceLevel(codacsSequence.data(), 0));

    codacsSequence->append(new CohNullStimulus(200.0, false));
    QVERIFY(!checkCodacsSequenceLevel(codacsSequence.data(), 1024));

    TEST_EXCEPTIONS_CATCH
}
