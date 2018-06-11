/******************************************************************************
 * Copyright (C) 2016  Michael Hofmann <mh21@mh21.de>                         *
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

#include "coh/coh.h"
#include "coh/cohmapper.h"
#include "coh/cohtextdumper.h"

#include "common/testutils.h"

#include "cohtest.h"

#include <QTest>

using namespace coh;

void CohTest::mapper()
{
    TEST_EXCEPTIONS_TRY

    QString result =
        QL1S("Sequence: repeats 1\n"
             "  Null: period nan trigger 0\n"
             "  Null: period 100.0 trigger 0\n"
             "  Biphasic: active 17 reference nan level 100 width nan gap nan "
             "period nan channel nan magnitude nan trigger nan\n"
             "  Biphasic: active 17 reference -3 level 100 width nan gap nan "
             "period nan channel nan magnitude nan trigger nan\n"
             "  Biphasic: active 17 reference -3 level 100 width 25.0 gap nan "
             "period nan channel nan magnitude nan trigger nan\n"
             "  Biphasic: active 17 reference -3 level 100 width 25.0 gap 8.0 "
             "period nan channel nan magnitude nan trigger nan\n"
             "  Biphasic: active 17 reference -3 level 100 width 25.0 gap 8.0 "
             "period 100.0 channel nan magnitude nan trigger nan\n"
             "  Biphasic: active nan reference nan level nan width 25.0 gap "
             "8.0 period 100.0 channel 1 magnitude 0.5 trigger nan\n"
             "  Biphasic: active 17 reference nan level nan width 25.0 gap 8.0 "
             "period 100.0 channel 1 magnitude 0.5 trigger nan\n");

    CohNullStimulus *null = CohNullStimulus::incompleteStimulus();
    CohBiphasicStimulus *biphasic = CohBiphasicStimulus::incompleteStimulus();
    CohSequence *sequence = new CohSequence(1);
    CohSequenceMapper mapper(sequence);

    null->setTrigger(false);
    sequence->append(new CohNullStimulus(*null));
    null->setPeriod(100.0);
    sequence->append(new CohNullStimulus(*null));

    //    biphasic->setTrigger(false);
    biphasic->setCurrentLevel(100);
    biphasic->setActiveElectrode(Coh::CI_ELECTRODE_IC17);
    sequence->append(new CohBiphasicStimulus(*biphasic));
    biphasic->setReferenceElectrode(Coh::CI_ELECTRODE_EC1_2);
    sequence->append(new CohBiphasicStimulus(*biphasic));
    biphasic->setPhaseWidth(25.0);
    sequence->append(new CohBiphasicStimulus(*biphasic));
    biphasic->setPhaseGap(8.0);
    sequence->append(new CohBiphasicStimulus(*biphasic));
    biphasic->setPeriod(100.0);
    sequence->append(new CohBiphasicStimulus(*biphasic));

    biphasic = CohBiphasicStimulus::incompleteStimulus();
    biphasic->setPhaseWidth(25.0);
    biphasic->setPhaseGap(8.0);
    biphasic->setPeriod(100.0);
    biphasic->setChannelIndex(1);
    biphasic->setChannelMagnitude(0.5);
    sequence->append(new CohBiphasicStimulus(*biphasic));
    biphasic->setActiveElectrode(Coh::CI_ELECTRODE_IC17);
    sequence->append(new CohBiphasicStimulus(*biphasic));

    QCOMPARE(CohSequenceMapper(mapper.map()).needsMapping() != 0, true);
    QCOMPARE(dumpCohSequenceText(mapper.map()), result);

    QCOMPARE(CohSequenceMapper(mapper.map()).needsMapping() != 0, true);
    QCOMPARE(dumpCohSequenceText(mapper.map()), result);

    mapper.setDefaultReferenceElectrode(Coh::CI_ELECTRODE_EC1);
    result.replace(QL1S("reference nan"), QL1S("reference -1"));
    QCOMPARE(CohSequenceMapper(mapper.map()).needsMapping() != 0, true);
    QCOMPARE(dumpCohSequenceText(mapper.map()), result);

    mapper.setDefaultPhaseWidth(30.0);
    result.replace(QL1S("width nan"), QL1S("width 30.0"));
    QCOMPARE(CohSequenceMapper(mapper.map()).needsMapping() != 0, true);
    QCOMPARE(dumpCohSequenceText(mapper.map()), result);

    mapper.setDefaultPhaseGap(9.0);
    result.replace(QL1S("gap nan"), QL1S("gap 9.0"));
    QCOMPARE(CohSequenceMapper(mapper.map()).needsMapping() != 0, true);
    QCOMPARE(dumpCohSequenceText(mapper.map()), result);

    mapper.setDefaultPeriod(50.0);
    result.replace(QL1S("period nan"), QL1S("period 50.0"));
    QCOMPARE(CohSequenceMapper(mapper.map()).needsMapping() != 0, true);
    QCOMPARE(dumpCohSequenceText(mapper.map()), result);

    mapper.setDefaultTrigger(CohSequenceMapper::DefaultTrigger::AllTriggers);
    result.replace(QSL("trigger nan"), QSL("trigger 1"));
    QCOMPARE(CohSequenceMapper(mapper.map()).needsMapping() != 0, true);
    QCOMPARE(dumpCohSequenceText(mapper.map()), result);

    mapper.setChannel(1, Coh::CI_ELECTRODE_IC10, Coh::CI_ELECTRODE_EC2, 10, 30,
                      0.5);
    result.replace(QL1S("active nan"), QL1S("active 10"));
    result.replace(QL1S("level nan"), QL1S("level 15"));
    result.replace(QRegExp(QL1S("reference -1([^\n]*channel 1)")),
                   QL1S("reference -2\\1"));
    QCOMPARE(CohSequenceMapper(mapper.map()).needsMapping() != 0, false);
    QCOMPARE(dumpCohSequenceText(mapper.map()), result);

    TEST_EXCEPTIONS_CATCH
}
