/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Maarten Lambert.                               *
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

#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/testutils.h"

#include "tests.h"

using namespace bertha;

void BerthaTest::testFiles()
{
    TEST_EXCEPTIONS_TRY

    const char contents[] =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<bertha:experiment "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
        "    xsi:schemaLocation=\"http://med.kuleuven.be/"
        "exporl/bertha/1.0/experiment ../schema/experiment.xsd\"\n"
        "    xmlns:bertha=\"http://med.kuleuven.be/"
        "exporl/bertha/1.0/experiment\">\n"
        "    <blocks>\n"
        "        <block id=\"block1\">\n"
        "            <plugin>Gain</plugin>\n"
        "            <parameter name=\"gain\">20</parameter>\n"
        "        </block>\n"
        "    </blocks>\n"
        "    <connections>\n"
        "        <connection>\n"
        "            <from id=\"block1\" port=\"output-1\"/>\n"
        "            <to id=\"device\" port=\"playback-1\"/>\n"
        "        </connection>\n"
        "    </connections>\n"
        "    <device id=\"device\">\n"
        "        <driver>Files</driver>\n"
        "        <samplerate>44100</samplerate>\n"
        "        <blocksize>2</blocksize>\n"
        "        <playbackports>2</playbackports>\n"
        "        <captureports>2</captureports>\n"
        "        <parameter name=\"inputFiles\">dum1.wav,dum2.wav</parameter>\n"
        "    </device>\n"
        "</bertha:experiment>\n";

    const ExperimentData testExp = XmlLoader().loadContents(contents);

    QCOMPARE(testExp.device().plugin(), QString::fromLatin1("Files"));
    QCOMPARE(testExp.device().sampleRate(), 44100u);
    QCOMPARE(testExp.device().blockSize(), 2u);

    QCOMPARE(testExp.device().parameter(QLatin1String("inputFiles")),
             QVariant(QLatin1String("dum1.wav,dum2.wav")));

    ExperimentProcessor processor(testExp);
    QCOMPARE(processor.parameterValue(QLatin1String("device"),
                                      QLatin1String("inputFiles")),
             QVariant(QStringList() << QLatin1String("dum1.wav")
                                    << QLatin1String("dum2.wav")));

    TEST_EXCEPTIONS_CATCH
}
