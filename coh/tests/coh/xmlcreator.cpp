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

// Test the behaviour of the NIC xml generator

#include "coh/coh.h"
#include "coh/cohnicxmldumper.h"
#include "coh/cohxmldumper.h"
#include "coh/cohxmlloader.h"

#include "common/exception.h"
#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include "cohtest.h"

#include <QTest>

#include <QtDebug>

using namespace cmn;
using namespace coh;

void CohTest::nicXmlCreator()
{
    TEST_EXCEPTIONS_TRY

    // Powerup
    CohSequence *powerupSequence = new CohSequence(25);
    QCOMPARE(powerupSequence->numberRepeats(), 25u);

    CohNullStimulus *powerupStimulus = new CohNullStimulus(200.0, 0);
    powerupSequence->append(powerupStimulus);
    QCOMPARE(powerupSequence->numberRepeats(), 25u);

    // Powerup
    CohSequence *powerupSequence2 = new CohSequence(1);
    powerupSequence2->append(powerupSequence);
    QCOMPARE(powerupSequence2->numberRepeats(), 1u);

    // Meta data
    CohSequence *metaSequence = new CohSequence(1);
    metaSequence->append(
        new CohMetaData(QL1S("string"), QString::fromUtf8("Gänsefüßchen")));
    metaSequence->append(new CohMetaData(QL1S("int"), 5));
    metaSequence->append(new CohMetaData(QL1S("double"), 10.83));
    metaSequence->append(new CohMetaData(QL1S("bool"), true));

    // Stimuli
    CohSequence *stimulusSequence = new CohSequence(5);
    CohBiphasicStimulus *stimulusStimulus =
        new CohBiphasicStimulus(Coh::CI_ELECTRODE_IC1, Coh::CI_ELECTRODE_EC1,
                                100, 25.0, 8.0, 1000.0, false);
    stimulusStimulus->setTrigger(true);
    stimulusSequence->append(stimulusStimulus);

    QScopedPointer<CohSequence> sequence(new CohSequence(2));
    sequence->append(powerupSequence2);
    sequence->append(metaSequence);
    sequence->append(stimulusSequence);

    CohNullStimulus *stimulus = new CohNullStimulus(234.567, false);
    sequence->append(stimulus);
    CohCodacsStimulus *codacsStimulus =
        new CohCodacsStimulus(1345, CohCodacsStimulus::defaultPeriod(), false);
    sequence->append(codacsStimulus);
    codacsStimulus = new CohCodacsStimulus(1349, 52.0, false);
    sequence->append(codacsStimulus);

    QByteArray xml;

    xml += "      <nic:sequence>\n"
           "         <nic:sequence>\n";

    for (unsigned i = 0; i < 25; ++i)
        xml += "            <stimulus>\n"
               "               <parameters><ae>0</ae><re>0</re><t>200.0</t>"
               "</parameters>\n"
               "            </stimulus>\n";

    xml += "         </nic:sequence>\n"
           "      </nic:sequence>\n"
           "      <nic:sequence>\n"
           "      </nic:sequence>\n"
           "      <nic:sequence>\n";

    for (unsigned i = 0; i < 5; ++i)
        xml +=
            "         <trigger>\n"
            "            <parameters><ae>1</ae><re>-1</re><cl>100</cl><pw>25.0"
            "</pw><pg>8.0</pg><t>1000.0</t></parameters>\n"
            "         </trigger>\n";

    xml +=
        "      </nic:sequence>\n"
        "      <stimulus>\n"
        "         <parameters><ae>0</ae><re>0</re><t>234.6</t></parameters>\n"
        "      </stimulus>\n";

    xml += "      <stimulus>\n"
           "         <parameters><ae>10</ae><re>-3</re><cl>130</cl><pw>12.0"
           "</pw><pg>6.0</pg><t>51.0</t></parameters>\n"
           "      </stimulus>\n";

    xml += "      <stimulus>\n"
           "         <parameters><ae>10</ae><re>-3</re><cl>138</cl><pw>12.0"
           "</pw><pg>6.0</pg><t>52.0</t></parameters>\n"
           "      </stimulus>\n";

    xml += xml;

    xml += "   </nic:sequence>\n"
           "</nic:sequence>\n";

    xml = "<nic:sequence xmlns:nic='http://www.cochlear.com/nic'>\n"
          "   <nic:sequence>\n" +
          xml;

    static const char xml2[] =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<!DOCTYPE cisequence>\n"
        "<cisequence version=\"1.0\">\n"
        "    <sequence repeats=\"2\">\n"
        "        <sequence>\n"
        "            <sequence repeats=\"25\">\n"
        "                <stimulus type=\"null\">\n"
        "                    <parameter name=\"trigger\">false</parameter>\n"
        "                    <parameter name=\"period\">200.0</parameter>\n"
        "                </stimulus>\n"
        "            </sequence>\n"
        "        </sequence>\n"
        "        <sequence>\n"
        "            <metadata>\n"
        "                <key>string</key>\n"
        "                <value type=\"string\">Gänsefüßchen</value>\n"
        "            </metadata>\n"
        "            <metadata>\n"
        "                <key>int</key>\n"
        "                <value type=\"int\">5</value>\n"
        "            </metadata>\n"
        "            <metadata>\n"
        "                <key>double</key>\n"
        "                <value type=\"double\">10.83</value>\n"
        "            </metadata>\n"
        "            <metadata>\n"
        "                <key>bool</key>\n"
        "                <value type=\"bool\">true</value>\n"
        "            </metadata>\n"
        "        </sequence>\n"
        "        <sequence repeats=\"5\">\n"
        "            <stimulus type=\"biphasic\">\n"
        "                <parameter name=\"trigger\">true</parameter>\n"
        "                <parameter name=\"active\">1</parameter>\n"
        "                <parameter name=\"reference\">-1</parameter>\n"
        "                <parameter name=\"currentlevel\">100</parameter>\n"
        "                <parameter name=\"phasewidth\">25.0</parameter>\n"
        "                <parameter name=\"phasegap\">8.0</parameter>\n"
        "                <parameter name=\"period\">1000.0</parameter>\n"
        "            </stimulus>\n"
        "        </sequence>\n"
        "        <stimulus type=\"null\">\n"
        "            <parameter name=\"trigger\">false</parameter>\n"
        "            <parameter name=\"period\">234.6</parameter>\n"
        "        </stimulus>\n"
        "        <stimulus type=\"codacs\">\n"
        "            <parameter name=\"trigger\">false</parameter>\n"
        "            <parameter name=\"amplitude\">1345</parameter>\n"
        "            <parameter name=\"period\">51.0</parameter>\n"
        "        </stimulus>\n"
        "        <stimulus type=\"codacs\">\n"
        "            <parameter name=\"trigger\">false</parameter>\n"
        "            <parameter name=\"amplitude\">1349</parameter>\n"
        "            <parameter name=\"period\">52.0</parameter>\n"
        "        </stimulus>\n"
        "    </sequence>\n"
        "</cisequence>\n";

    QCOMPARE(QString::fromUtf8(dumpCohSequenceNicXml(sequence.data())),
             QString::fromUtf8(xml));
    QCOMPARE(QString::fromUtf8(dumpCohSequenceXml(sequence.data())),
             QString::fromUtf8(xml2));
    QScopedPointer<CohSequence> sequence2(loadCohSequenceXml(xml2));
    QCOMPARE(QString::fromUtf8(dumpCohSequenceXml(sequence2.data())),
             QString::fromUtf8(xml2));

    TEST_EXCEPTIONS_CATCH
}
