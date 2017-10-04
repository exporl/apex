/******************************************************************************
 * Copyright (C) 2012  Michael Hofmann <mh21@piware.de>                       *
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

#include "coh/cohiterator.h"

#include "common/commonapplication.h"
#include "common/testutils.h"

#include "cohtest.h"

#include <QTest>

using namespace cmn;
using namespace coh;

int map_electrodes(int electrode)
{
    switch (electrode) {
    case -1:
        return 24;
    // this is not complete, but good enough atm
    default:
        return electrode;
    }
}

QString nic_test_xml(CohSequence *sequence)
{
    QString result;
    result += QString::fromLatin1(
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<nic:sequence xmlns:nic=\"http://www.cochlear.com/nic\" "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
        "xsi:schemaLocation=\"http://www.cochlear.com/nic  "
        "nic-stimuli.xsd\" >\n");
    CohIterator it(sequence, false);
    while (it.hasNext()) {
        CohStimulus *stimulus = it.next();
        if (CohBiphasicStimulus *biphasic =
                dynamic_cast<CohBiphasicStimulus *>(stimulus)) {
            result += QString::fromLatin1("  <stimulus><parameters>"
                                          "<ae>%1</ae>"
                                          "<re>%2</re>"
                                          "<cl>%3</cl>"
                                          "<pw>%4</pw>"
                                          "<pg>%5</pg>"
                                          "<t>%6</t>"
                                          "</parameters></stimulus>\n")
                          .arg(map_electrodes(biphasic->activeElectrode()))
                          .arg(map_electrodes(biphasic->referenceElectrode()))
                          .arg(biphasic->currentLevel())
                          .arg(biphasic->phaseWidth(), 0, 'f', 1)
                          .arg(biphasic->phaseGap(), 0, 'f', 1)
                          .arg(biphasic->period(), 0, 'f', 1);
        } else if (CohNullStimulus *null =
                       dynamic_cast<CohNullStimulus *>(stimulus)) {
            result += QString::fromLatin1("  <stimulus><parameters>"
                                          "<ae>0</ae>"
                                          "<re>0</re>"
                                          "<cl>20</cl>"
                                          "<pw>25.0</pw>"
                                          "<pg>7.0</pg>"
                                          "<t>%6</t>"
                                          "</parameters></stimulus>\n")
                          .arg(null->period(), 0, 'f', 1);
        } else if (CohCodacsStimulus *codacs =
                       dynamic_cast<CohCodacsStimulus *>(stimulus)) {
            result += QString::fromLatin1("  <stimulus><parameters>"
                                          "<ae>%1</ae>"
                                          "<re>%2</re>"
                                          "<cl>%3</cl>"
                                          "<pw>%4</pw>"
                                          "<pg>%5</pg>"
                                          "<t>%6</t>"
                                          "</parameters></stimulus>\n")
                          .arg(map_electrodes(codacs->activeElectrode()))
                          .arg(map_electrodes(codacs->referenceElectrode()))
                          .arg(codacs->currentLevel())
                          .arg(codacs->phaseWidth(), 0, 'f', 1)
                          .arg(codacs->phaseGap(), 0, 'f', 1)
                          .arg(codacs->period(), 0, 'f', 1);
        }
    }
    result += QString::fromLatin1("</nic:sequence>");
    return result;
}

QString nic_test_txt(CohSequence *sequence)
{
    QString result;
    CohIterator it(sequence, false);
    while (it.hasNext()) {
        CohStimulus *stimulus = it.next();
        if (CohBiphasicStimulus *biphasic =
                dynamic_cast<CohBiphasicStimulus *>(stimulus)) {
            result +=
                QString::fromLatin1("trigger enabled, phase width ( 100 us min "
                                    ") -->\nstandard  5cpc   %1   %2  %3 %4 "
                                    "%5 %4   0.0   0.0  %6\n")
                    .arg(map_electrodes(biphasic->activeElectrode()), 2)
                    .arg(map_electrodes(biphasic->referenceElectrode()), 2)
                    .arg(biphasic->currentLevel(), 3)
                    .arg(biphasic->phaseWidth(), 5, 'f', 1)
                    .arg(biphasic->phaseGap(), 4, 'f', 1)
                    .arg(biphasic->period(), 4, 'f', 1);
        } else if (CohNullStimulus *null =
                       dynamic_cast<CohNullStimulus *>(stimulus)) {
            result += QString::fromLatin1("standard  5cpc    0    0   20  25.0 "
                                          " 7.0  25.0   0.0   0.0  %6\n")
                          .arg(null->period(), 4, 'f', 1);
        } else if (CohCodacsStimulus *codacs =
                       dynamic_cast<CohCodacsStimulus *>(stimulus)) {
            result += QString::fromLatin1("%7high  5cpc   %1   %2  %3 %4 "
                                          "%5 %4   0.0   0.0  %6\n")
                          .arg(map_electrodes(codacs->activeElectrode()), 2)
                          .arg(28, 2)
                          .arg(codacs->currentLevel(), 3)
                          .arg(codacs->phaseWidth(), 5, 'f', 1)
                          .arg(codacs->phaseGap(), 4, 'f', 1)
                          .arg(codacs->period(), 4, 'f', 1)
                          .arg(codacs->hasTrigger()
                                   ? QL1S("trigger enabled, phase width ( 100 "
                                          "us min ) -->\n")
                                   : QString());
        }
    }
    return result;
}

int main(int argc, char *argv[])
{
    CommonCoreApplication app(argc, argv);

    TemporarySettings settings;

    CohTest test;
    QTest::qExec(&test, argc, argv);

    return 0;
}
