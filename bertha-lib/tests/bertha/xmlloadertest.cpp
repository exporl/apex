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

#include "bertha/xmlloader.h"

#include "common/testutils.h"

#include "tests.h"

using namespace bertha;

void BerthaTest::testXmlLoader()
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
        "            <plugin>gain</plugin>\n"
        "            <parameter name=\"gain\">30</parameter>\n"
        "        </block>\n"
        "        <block id=\"block2\">\n"
        "            <plugin>gain</plugin>\n"
        "            <parameter name=\"gain\">40</parameter>\n"
        "            <parameter name=\"basegain\">20</parameter>\n"
        "        </block>\n"
        "    </blocks>\n"
        "    <connections>\n"
        "        <connection>\n"
        "            <from id=\"block1\" port=\"1\"/>\n"
        "            <to id=\"device\" port=\"1\"/>\n"
        "        </connection>\n"
        "        <connection>\n"
        "            <from id=\"block1\" port=\"1\"/>\n"
        "            <to id=\"block2\" port=\"1\"/>\n"
        "        </connection>\n"
        "    </connections>\n"
        "    <device id=\"device\">\n"
        "        <driver>file</driver>\n"
        "        <samplerate>44100</samplerate>\n"
        "        <blocksize>2</blocksize>\n"
        "        <playbackports>2</playbackports>\n"
        "        <captureports>2</captureports>\n"
        "    </device>\n"
        "</bertha:experiment>\n";

    try { // Invalid XML
        QByteArray modifiedContents(contents);
        modifiedContents.replace("</device>", "</devic>");
        XmlLoader().loadContents(modifiedContents);
        QFAIL("Exception expected");
    } catch (const std::exception &) {
        // expected
    }

    try { // Invalid schema
        QByteArray modifiedContents(contents);
        modifiedContents.replace("<device>", "<devic>");
        modifiedContents.replace("</device>", "</devic>");
        XmlLoader().loadContents(modifiedContents);
        QFAIL("Exception expected");
    } catch (const std::exception &) {
        // expected
    }

    try { // Invalid optional fields
        QString modifiedContents = QString::fromUtf8(contents);
        modifiedContents.replace(
            QRegExp(QLatin1String("<samplerate>.*</samplerate>")),
            QLatin1String("<samplerate></samplerate>"));
        XmlLoader().loadContents(modifiedContents.toUtf8());
        QFAIL("Exception expected");
    } catch (const std::exception &) {
        // expected
    }

    try { // Missing optional fields
        QString modifiedContents = QString::fromUtf8(contents);
        modifiedContents.replace(
            QRegExp(QLatin1String("<samplerate>.*</samplerate>")), QString());
        XmlLoader().loadContents(modifiedContents.toUtf8());
    } catch (const std::exception &e) {
        QFAIL(qPrintable(QString::fromLatin1("No exception expected: %1")
                             .arg(QString::fromLocal8Bit(e.what()))));
    }

    const ExperimentData testExp = XmlLoader().loadContents(contents);

    QCOMPARE(testExp.device().plugin(), QString::fromLatin1("file"));
    QCOMPARE(testExp.device().sampleRate(), 44100u);
    QCOMPARE(testExp.device().blockSize(), 2u);

    QCOMPARE(testExp.connections().size(), 2);
    QCOMPARE(testExp.connections()[0].sourceBlock(),
             QString::fromLatin1("block1"));
    QCOMPARE(testExp.connections()[0].sourcePort(), QString::fromLatin1("1"));
    QCOMPARE(testExp.connections()[0].targetBlock(),
             QString::fromLatin1("device"));
    QCOMPARE(testExp.connections()[0].targetPort(), QString::fromLatin1("1"));

    QCOMPARE(testExp.blocks().size(), 2);
    QCOMPARE(testExp.blocks()[0].plugin(), QString::fromLatin1("gain"));
    QCOMPARE(testExp.blocks()[0].parameter(QLatin1String("gain")),
             QVariant(30));
    QCOMPARE(testExp.blocks()[1].plugin(), QString::fromLatin1("gain"));
    QCOMPARE(testExp.blocks()[1].parameter(QLatin1String("gain")),
             QVariant(40));
    // Case insensitive matching
    QCOMPARE(testExp.blocks()[1].parameter(QLatin1String("baseGain")),
             QVariant(20));

    TEST_EXCEPTIONS_CATCH
}
