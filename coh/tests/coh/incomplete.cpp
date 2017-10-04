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
#include "coh/cohtextdumper.h"

#include "common/testutils.h"

#include "cohtest.h"

#include <QTest>

using namespace coh;

void CohTest::incomplete()
{
    TEST_EXCEPTIONS_TRY

    {
        const char ref[] =
            "Sequence: repeats 1\n"
            "  Null: period nan trigger nan\n"
            "  Null: period 100.0 trigger 1\n"
            "  Biphasic: active nan reference nan level nan width nan gap nan "
            "period nan channel nan magnitude nan trigger nan\n"
            "  Biphasic: active nan reference nan level nan width nan gap nan "
            "period nan channel nan magnitude nan trigger 0\n"
            "  Biphasic: active 17 reference nan level nan width nan gap nan "
            "period nan channel nan magnitude nan trigger 0\n"
            "  Biphasic: active 17 reference -3 level nan width nan gap nan "
            "period nan channel nan magnitude nan trigger 0\n"
            "  Biphasic: active 17 reference -3 level 100 width nan gap nan "
            "period nan channel nan magnitude nan trigger 0\n"
            "  Biphasic: active 17 reference -3 level 100 width 25.0 gap nan "
            "period nan channel nan magnitude nan trigger 0\n"
            "  Biphasic: active 17 reference -3 level 100 width 25.0 gap 8.0 "
            "period nan channel nan magnitude nan trigger 0\n"
            "  Biphasic: active 17 reference -3 level 100 width 25.0 gap 8.0 "
            "period 100.0 channel nan magnitude nan trigger 0\n"
            "  Biphasic: active 17 reference -3 level 100 width 25.0 gap 8.0 "
            "period 100.0 channel 1 magnitude nan trigger 0\n"
            "  Biphasic: active 17 reference -3 level 100 width 25.0 gap 8.0 "
            "period 100.0 channel 1 magnitude 0.5 trigger 0\n";

        CohNullStimulus *null = CohNullStimulus::incompleteStimulus();
        CohBiphasicStimulus *biphasic =
            CohBiphasicStimulus::incompleteStimulus();
        CohSequence *sequence = new CohSequence(1);

        QCOMPARE(null->availableProperties(), Coh::CommandProperties(0));
        QCOMPARE(null->availableProperties(Coh::Trigger), false);
        QCOMPARE(null->availableProperties(Coh::Period), false);
        QCOMPARE(null->isValid(), false);
        sequence->append(new CohNullStimulus(*null));

        null->setTrigger(true);
        QCOMPARE(null->availableProperties(),
                 Coh::CommandProperties(Coh::Trigger));
        QCOMPARE(null->availableProperties(Coh::Trigger), true);
        QCOMPARE(null->isValid(), false);

        null->setPeriod(100.0);
        QCOMPARE(null->availableProperties(),
                 Coh::CommandProperties(Coh::Period | Coh::Trigger));
        QCOMPARE(null->availableProperties(Coh::Trigger), true);
        QCOMPARE(null->availableProperties(Coh::Period), true);
        QCOMPARE(null->isValid(), true);
        sequence->append(new CohNullStimulus(*null));

        QCOMPARE(biphasic->availableProperties(), Coh::CommandProperties(0));
        QCOMPARE(biphasic->availableProperties(Coh::Trigger), false);
        QCOMPARE(biphasic->availableProperties(Coh::ActiveElectrode), false);
        QCOMPARE(biphasic->availableProperties(Coh::ReferenceElectrode), false);
        QCOMPARE(biphasic->availableProperties(Coh::CurrentLevel), false);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseWidth), false);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseGap), false);
        QCOMPARE(biphasic->availableProperties(Coh::Period), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelIndex), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelMagnitude), false);
        QCOMPARE(biphasic->isValid(), false);
        sequence->append(new CohBiphasicStimulus(*biphasic));

        biphasic->setTrigger(false);
        QCOMPARE(biphasic->availableProperties(),
                 Coh::CommandProperties(Coh::Trigger));
        QCOMPARE(biphasic->availableProperties(Coh::Trigger), true);
        QCOMPARE(biphasic->availableProperties(Coh::ActiveElectrode), false);
        QCOMPARE(biphasic->availableProperties(Coh::ReferenceElectrode), false);
        QCOMPARE(biphasic->availableProperties(Coh::CurrentLevel), false);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseWidth), false);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseGap), false);
        QCOMPARE(biphasic->availableProperties(Coh::Period), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelIndex), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelMagnitude), false);
        QCOMPARE(biphasic->isValid(), false);
        sequence->append(new CohBiphasicStimulus(*biphasic));

        biphasic->setActiveElectrode(Coh::CI_ELECTRODE_IC17);
        QCOMPARE(biphasic->availableProperties(),
                 Coh::CommandProperties(Coh::Trigger | Coh::ActiveElectrode));
        QCOMPARE(biphasic->availableProperties(Coh::Trigger), true);
        QCOMPARE(biphasic->availableProperties(Coh::ActiveElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::ReferenceElectrode), false);
        QCOMPARE(biphasic->availableProperties(Coh::CurrentLevel), false);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseWidth), false);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseGap), false);
        QCOMPARE(biphasic->availableProperties(Coh::Period), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelIndex), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelMagnitude), false);
        QCOMPARE(biphasic->isValid(), false);
        sequence->append(new CohBiphasicStimulus(*biphasic));

        biphasic->setReferenceElectrode(Coh::CI_ELECTRODE_EC1_2);
        QCOMPARE(biphasic->availableProperties(),
                 Coh::CommandProperties(Coh::Trigger | Coh::ActiveElectrode |
                                        Coh::ReferenceElectrode));
        QCOMPARE(biphasic->availableProperties(Coh::Trigger), true);
        QCOMPARE(biphasic->availableProperties(Coh::ActiveElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::ReferenceElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::CurrentLevel), false);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseWidth), false);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseGap), false);
        QCOMPARE(biphasic->availableProperties(Coh::Period), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelIndex), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelMagnitude), false);
        QCOMPARE(biphasic->isValid(), false);
        sequence->append(new CohBiphasicStimulus(*biphasic));

        biphasic->setCurrentLevel(100);
        QCOMPARE(biphasic->availableProperties(),
                 Coh::CommandProperties(Coh::Trigger | Coh::ActiveElectrode |
                                        Coh::ReferenceElectrode |
                                        Coh::CurrentLevel));
        QCOMPARE(biphasic->availableProperties(Coh::Trigger), true);
        QCOMPARE(biphasic->availableProperties(Coh::ActiveElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::ReferenceElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::CurrentLevel), true);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseWidth), false);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseGap), false);
        QCOMPARE(biphasic->availableProperties(Coh::Period), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelIndex), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelMagnitude), false);
        QCOMPARE(biphasic->isValid(), false);
        sequence->append(new CohBiphasicStimulus(*biphasic));

        biphasic->setPhaseWidth(25.0);
        QCOMPARE(biphasic->availableProperties(),
                 Coh::CommandProperties(Coh::Trigger | Coh::ActiveElectrode |
                                        Coh::ReferenceElectrode |
                                        Coh::CurrentLevel | Coh::PhaseWidth));
        QCOMPARE(biphasic->availableProperties(Coh::Trigger), true);
        QCOMPARE(biphasic->availableProperties(Coh::ActiveElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::ReferenceElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::CurrentLevel), true);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseWidth), true);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseGap), false);
        QCOMPARE(biphasic->availableProperties(Coh::Period), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelIndex), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelMagnitude), false);
        QCOMPARE(biphasic->isValid(), false);
        sequence->append(new CohBiphasicStimulus(*biphasic));

        biphasic->setPhaseGap(8.0);
        QCOMPARE(biphasic->availableProperties(),
                 Coh::CommandProperties(
                     Coh::Trigger |

                     Coh::ActiveElectrode | Coh::ReferenceElectrode |
                     Coh::CurrentLevel | Coh::PhaseWidth | Coh::PhaseGap));
        QCOMPARE(biphasic->availableProperties(Coh::Trigger), true);
        QCOMPARE(biphasic->availableProperties(Coh::ActiveElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::ReferenceElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::CurrentLevel), true);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseWidth), true);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseGap), true);
        QCOMPARE(biphasic->availableProperties(Coh::Period), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelIndex), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelMagnitude), false);
        QCOMPARE(biphasic->isValid(), false);
        sequence->append(new CohBiphasicStimulus(*biphasic));

        biphasic->setPeriod(100.0);
        QCOMPARE(biphasic->availableProperties(),
                 Coh::CommandProperties(Coh::Trigger | Coh::ActiveElectrode |
                                        Coh::ReferenceElectrode |
                                        Coh::CurrentLevel | Coh::PhaseWidth |
                                        Coh::PhaseGap | Coh::Period));
        QCOMPARE(biphasic->availableProperties(Coh::Trigger), true);
        QCOMPARE(biphasic->availableProperties(Coh::ActiveElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::ReferenceElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::CurrentLevel), true);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseWidth), true);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseGap), true);
        QCOMPARE(biphasic->availableProperties(Coh::Period), true);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelIndex), false);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelMagnitude), false);
        QCOMPARE(biphasic->isValid(), true);
        sequence->append(new CohBiphasicStimulus(*biphasic));

        biphasic->setChannelIndex(1);
        QCOMPARE(biphasic->availableProperties(),
                 Coh::CommandProperties(Coh::Trigger | Coh::ActiveElectrode |
                                        Coh::ReferenceElectrode |
                                        Coh::CurrentLevel | Coh::PhaseWidth |
                                        Coh::PhaseGap | Coh::Period |
                                        Coh::ChannelIndex));
        QCOMPARE(biphasic->availableProperties(Coh::Trigger), true);
        QCOMPARE(biphasic->availableProperties(Coh::ActiveElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::ReferenceElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::CurrentLevel), true);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseWidth), true);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseGap), true);
        QCOMPARE(biphasic->availableProperties(Coh::Period), true);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelIndex), true);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelMagnitude), false);
        QCOMPARE(biphasic->isValid(), true);
        sequence->append(new CohBiphasicStimulus(*biphasic));

        biphasic->setChannelMagnitude(0.5);
        QCOMPARE(biphasic->availableProperties(),
                 Coh::CommandProperties(
                     Coh::Trigger | Coh::ActiveElectrode |
                     Coh::ReferenceElectrode | Coh::CurrentLevel |
                     Coh::PhaseWidth | Coh::PhaseGap | Coh::Period |
                     Coh::ChannelIndex | Coh::ChannelMagnitude));
        QCOMPARE(biphasic->availableProperties(Coh::Trigger), true);
        QCOMPARE(biphasic->availableProperties(Coh::ActiveElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::ReferenceElectrode), true);
        QCOMPARE(biphasic->availableProperties(Coh::CurrentLevel), true);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseWidth), true);
        QCOMPARE(biphasic->availableProperties(Coh::PhaseGap), true);
        QCOMPARE(biphasic->availableProperties(Coh::Period), true);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelIndex), true);
        QCOMPARE(biphasic->availableProperties(Coh::ChannelMagnitude), true);
        QCOMPARE(biphasic->isValid(), true);
        sequence->append(new CohBiphasicStimulus(*biphasic));

        QCOMPARE(dumpCohSequenceText(sequence), QString::fromUtf8(ref));
    }

    TEST_EXCEPTIONS_CATCH
}
