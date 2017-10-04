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

#include "bertha/blockdata.h"
#include "bertha/connectiondata.h"
#include "bertha/devicedata.h"
#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"
#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include "tests.h"

#include <cmath>
#include <numeric>

using namespace bertha;
using namespace cmn;

void BerthaTest::testProcessor()
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
        "        <block id=\"block2\">\n"
        "            <plugin>Gain</plugin>\n"
        "            <parameter name=\"gain\">20</parameter>\n"
        "        </block>\n"
        "    </blocks>\n"
        "    <connections>\n"
        "        <connection>\n"
        "            <from id=\"block1\" port=\"output-1\"/>\n"
        "            <to id=\"device\" port=\"playback-1\"/>\n"
        "        </connection>\n"
        "        <connection>\n"
        "            <from id=\"block1\" port=\"output-1\"/>\n"
        "            <to id=\"block2\" port=\"input-1\"/>\n"
        "        </connection>\n"
        "        <connection>\n"
        "            <from id=\"device\" port=\"capture-1\"/>\n"
        "            <to id=\"block1\" port=\"input-1\"/>\n"
        "        </connection>\n"
        "    </connections>\n"
        "    <device id=\"device\">\n"
        "        <driver>File</driver>\n"
        "        <samplerate>44100</samplerate>\n"
        "        <blocksize>44100</blocksize>\n"
        "        <playbackports>1</playbackports>\n"
        "        <captureports>2</captureports>\n"
        "        <parameter name=\"inputFile\">INPUTFILE</parameter>\n"
        "        <parameter name=\"outputFile\">OUTPUTFILE</parameter>\n"
        "    </device>\n"
        "</bertha:experiment>\n";

    TemporaryDirectory outputDir;
    const QString inputPath = Paths::searchFile(
        QLatin1String("testdata/sinus.wav"), Paths::dataDirectories());
    const QString outputPath = outputDir.addFile(QLatin1String("output.wav"));

    QByteArray data(contents);
    data.replace("INPUTFILE", inputPath.toUtf8());
    data.replace("OUTPUTFILE", outputPath.toUtf8());
    {
        ExperimentProcessor processor(XmlLoader().loadContents(data));
        QSignalSpy spy(&processor, SIGNAL(stateChanged(
                                       bertha::ExperimentProcessor::State)));
        processor.prepare();
        QCOMPARE(spy.count(), 1);
        QCOMPARE(
            qvariant_cast<ExperimentProcessor::State>(spy.takeFirst().first()),
            ExperimentProcessor::ExperimentPrepared);
        processor.start();
        QCOMPARE(spy.count(), 1);
        QCOMPARE(
            qvariant_cast<ExperimentProcessor::State>(spy.takeFirst().first()),
            ExperimentProcessor::ExperimentRunning);
        QVERIFY(spy.wait(1000));
        // Automatically stopped
        QCOMPARE(spy.count(), 1);
        QCOMPARE(
            qvariant_cast<ExperimentProcessor::State>(spy.takeFirst().first()),
            ExperimentProcessor::ExperimentPrepared);
        processor.release();
        QCOMPARE(spy.count(), 1);
        QCOMPARE(
            qvariant_cast<ExperimentProcessor::State>(spy.takeFirst().first()),
            ExperimentProcessor::ExperimentLoaded);
    }

    cmn::SndFile inputFile;
    SF_INFO readSfinfo;
    readSfinfo.format = 0;
    inputFile.reset(outputPath, SFM_READ, &readSfinfo);
    QVERIFY(inputFile);
    QCOMPARE(readSfinfo.samplerate, 44100);
    QCOMPARE(readSfinfo.channels, 1);
    QVector<float> sampleData(2 * 44100 + 1);
    const unsigned readCount =
        sf_read_float(inputFile.get(), sampleData.data(), sampleData.size());
    QCOMPARE(readCount, 44100u * 2);
    const double sum =
        sqrt(std::inner_product(sampleData.begin(), sampleData.end(),
                                sampleData.begin(), 0.0) /
             (2 * 44100));
    QVERIFY(10 * sqrt(2.0) / 2 - sum < 1e-5);

    TEST_EXCEPTIONS_CATCH
}

void BerthaTest::testParameterList()
{
    TEST_EXCEPTIONS_TRY

    QTemporaryFile file;
    file.open();

    ExperimentData data;
    DeviceData devData(QString::fromLatin1("device"),
                       QString::fromLatin1("File"));
    devData.setSampleRate(44100);
    devData.setBlockSize(400);
    devData.setInputPorts(1);
    devData.setOutputPorts(0);
    devData.setParameter(QString::fromLatin1("outputFile"), file.fileName());
    data.setDevice(devData);

    BlockData blockData(QString::fromLatin1("sin"),
                        QString::fromLatin1("SineGenerator"));
    blockData.setInputPorts(0);
    blockData.setOutputPorts(1);
    data.addBlock(blockData);
    ConnectionData conData(
        QString::fromLatin1("sin"), QString::fromLatin1("output-1"),
        QString::fromLatin1("device"), QString::fromLatin1("playback-1"));
    data.addConnection(conData);

    ExperimentProcessor processor(data);

    QMap<QString, QMap<QString, QVariant>> parameters;
    QString blockId = QString::fromLatin1("sin");
    parameters[blockId].insert(QString::fromLatin1("frequency"), QVariant(1));

    parameters[blockId].insert(QString::fromLatin1("amplitude"), QVariant(2));
    parameters[blockId].insert(QString::fromLatin1("phase"), QVariant(0.3));

    processor.setParameterList(parameters);
    Q_FOREACH (QString str, parameters[blockId].keys())
        QCOMPARE(processor.parameterValue(QString::fromLatin1("sin"), str),
                 parameters[blockId][str]);

    processor.prepare();
    processor.start();

    parameters.clear();
    parameters[blockId][QString::fromLatin1("frequency")] = QVariant(4);
    parameters[blockId][QString::fromLatin1("amplitude")] = QVariant(5);
    parameters[blockId][QString::fromLatin1("phase")] = QVariant(0.6);

    processor.setParameterList(parameters);
    QTest::qWait(20);

    Q_FOREACH (QString str, parameters[blockId].keys())
        QCOMPARE(processor.parameterValue(QString::fromLatin1("sin"), str),
                 parameters[blockId][str]);
    processor.release();

    file.close();

    TEST_EXCEPTIONS_CATCH
}

namespace
{
QString addBlockToExperimentData(ExperimentData &data, const QString &type,
                                 const QString &source, const QString &target,
                                 const QString &targetPort)
{
    BlockData block(source, type);
    data.addBlock(block);
    if (!target.isEmpty())
        data.addConnection(
            ConnectionData(source, QL1S("output-1"), target, targetPort));
    return block.id();
}
QStringList readConnectionsFromFile(const QString &path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QStringList verification;
    while (!file.atEnd())
        verification << QL1S(file.readLine().trimmed());
    return verification;
}
}

void BerthaTest::testProcessorLeafNodeSelection()
{
    TEST_EXCEPTIONS_TRY

    QDir blockConfigurations(Paths::searchDirectory(
        QL1S("testdata/blockconfigurations"), Paths::dataDirectories()));

    QTemporaryFile file;
    file.open();

    ExperimentData data;
    DeviceData devData(QString::fromLatin1("device"),
                       QString::fromLatin1("File"));
    devData.setSampleRate(44100);
    devData.setBlockSize(400);
    devData.setInputPorts(2);
    devData.setOutputPorts(2);
    devData.setParameter(QString::fromLatin1("inputFile"),
                         Paths::searchFile(QLatin1String("testdata/sinus.wav"),
                                           Paths::dataDirectories())
                             .toUtf8());
    devData.setParameter(QString::fromLatin1("outputFile"), file.fileName());
    data.setDevice(devData);

    addBlockToExperimentData(data, QL1S("Gain"), QL1S("gain3"), QL1S("device"),
                             QL1S("playback-2"));
    data.addConnection(ConnectionData(QL1S("device"), QL1S("capture-1"),
                                      QL1S("gain3"), QL1S("input-1")));

    QSet<QString> sinBlocks;
    QSet<QString> sin2Blocks;

    sinBlocks +=
        addBlockToExperimentData(data, QL1S("SineGenerator"), QL1S("sin"),
                                 QL1S("gain"), QL1S("input-1"));
    sin2Blocks +=
        addBlockToExperimentData(data, QL1S("SineGenerator"), QL1S("sin2"),
                                 QL1S("gain2"), QL1S("input-1"));
    sinBlocks += addBlockToExperimentData(data, QL1S("Gain"), QL1S("gain"),
                                          QL1S("sum"), QL1S("input-1"));
    sin2Blocks += addBlockToExperimentData(data, QL1S("Gain"), QL1S("gain2"),
                                           QL1S("sum"), QL1S("input-2"));
    sinBlocks += addBlockToExperimentData(data, QL1S("Summation"), QL1S("sum"),
                                          QL1S("device"), QL1S("playback-1"));

    ExperimentProcessor processor(data);
    // all blocks should be in there except for gain3
    QVERIFY(processor.blockNames().toSet().contains(sinBlocks));
    QVERIFY(processor.blockNames().toSet().contains(sin2Blocks));

    processor.prepare();
    processor.start();
    QTest::qWait(20);

    processor.setActiveLeafNodes(QStringList() << QL1S("sin"));
    // only the blocks needed for sin1 should be in there
    QVERIFY(processor.blockNames().toSet().contains(sinBlocks));
    QVERIFY(!processor.blockNames().toSet().contains(sin2Blocks));
    QTest::qWait(20);

    processor.setActiveLeafNodes(QStringList() << QL1S("sin") << QL1S("sin2"));
    // all blocks should be in there except for gain3
    QVERIFY(processor.blockNames().toSet().contains(sinBlocks));
    QVERIFY(processor.blockNames().toSet().contains(sin2Blocks));
    QTest::qWait(20);

    // with recreate
    processor.setActiveLeafNodes(
        QStringList() << QL1S("sin"),
        QMap<QString, QList<QPair<QString, QVariant>>>(), true);
    // only the blocks needed for sin1 should be in there
    QVERIFY(processor.blockNames().toSet().contains(sinBlocks));
    QVERIFY(!processor.blockNames().toSet().contains(sin2Blocks));
    QStringList activeConnections(processor.dumpActiveConnections());
    QStringList verify(readConnectionsFromFile(blockConfigurations.filePath(
        QL1S("leafnodeselection_recreate_sin1.txt"))));
    QVERIFY(activeConnections.removeDuplicates() == 0);
    QCOMPARE(activeConnections.toSet(), verify.toSet());
    QTest::qWait(20);

    processor.setActiveLeafNodes(
        QStringList() << QL1S("sin") << QL1S("sin2") << QL1S("device"),
        QMap<QString, QList<QPair<QString, QVariant>>>(), true);
    // all blocks should be in there
    QVERIFY(processor.blockNames().toSet().contains(sinBlocks));
    QVERIFY(processor.blockNames().contains(QL1S("gain3")));
    QVERIFY(processor.blockNames().toSet().contains(sin2Blocks));
    activeConnections = processor.dumpActiveConnections();
    verify = readConnectionsFromFile(blockConfigurations.filePath(
        QL1S("leafnodeselection_recreate_all.txt")));
    QVERIFY(activeConnections.removeDuplicates() == 0);
    QCOMPARE(activeConnections.toSet(), verify.toSet());
    QTest::qWait(20);

    processor.setActiveLeafNodes(
        QStringList() << QL1S("sin"),
        QMap<QString, QList<QPair<QString, QVariant>>>(), true);
    // only the blocks needed for sin1 should be in there
    QVERIFY(processor.blockNames().toSet().contains(sinBlocks));
    QVERIFY(!processor.blockNames().toSet().contains(sin2Blocks));
    activeConnections = processor.dumpActiveConnections();
    verify = readConnectionsFromFile(blockConfigurations.filePath(
        QL1S("leafnodeselection_recreate_sin1.txt")));
    QVERIFY(activeConnections.removeDuplicates() == 0);
    QCOMPARE(activeConnections.toSet(), verify.toSet());
    QTest::qWait(20);

    processor.release();
    file.close();

    TEST_EXCEPTIONS_CATCH
}

void BerthaTest::testProcessorLeafNodeSelectionWithParameters()
{
    TEST_EXCEPTIONS_TRY

    QTemporaryFile file;
    file.open();

    ExperimentData data;
    DeviceData devData(QString::fromLatin1("device"),
                       QString::fromLatin1("File"));
    devData.setSampleRate(44100);
    devData.setBlockSize(400);
    devData.setInputPorts(1);
    devData.setOutputPorts(0);
    devData.setParameter(QString::fromLatin1("outputFile"), file.fileName());
    data.setDevice(devData);

    QSet<QString> blocks;
    blocks += addBlockToExperimentData(data, QL1S("SineGenerator"), QL1S("sin"),
                                       QL1S("sum"), QL1S("input-1"));
    blocks +=
        addBlockToExperimentData(data, QL1S("SineGenerator"), QL1S("sin2"),
                                 QL1S("sum"), QL1S("input-2"));

    blocks += addBlockToExperimentData(data, QL1S("Summation"), QL1S("sum"),
                                       QL1S("device"), QL1S("playback-1"));

    ExperimentProcessor processor(data);
    QMap<QString, QList<QPair<QString, QVariant>>> parameters;
    parameters[QSL("sin")].append(qMakePair(QSL("frequency"), QVariant(1000)));

    // default value
    QVERIFY(processor.blockNames().toSet().contains(blocks));
    QCOMPARE(processor.sanitizedParameterValue(QSL("sin"), QSL("frequency")),
             QVariant(1));

    // setting parameter via leafnode while not running
    processor.setActiveLeafNodes(QStringList() << QL1S("sin"), parameters);
    QCOMPARE(processor.sanitizedParameterValue(QSL("sin"), QSL("frequency")),
             QVariant(1000));

    // last added parameter with same name wins while not running
    parameters[QSL("sin")].append(qMakePair(QSL("frequency"), QVariant(500)));
    processor.setActiveLeafNodes(QStringList() << QL1S("sin"), parameters);
    QCOMPARE(processor.sanitizedParameterValue(QSL("sin"), QSL("frequency")),
             QVariant(500));

    // setting parameter back to 1
    processor.setParameterValue(QSL("sin"), QSL("frequency"), QVariant(1));
    QCOMPARE(processor.sanitizedParameterValue(QSL("sin"), QSL("frequency")),
             QVariant(1));

    processor.start();
    // setting parameter via leafnode while running
    processor.setActiveLeafNodes(QStringList() << QL1S("sin"), parameters);
    QCOMPARE(processor.sanitizedParameterValue(QSL("sin"), QSL("frequency")),
             QVariant(500));

    // last added parameter with same name wins while running
    parameters[QSL("sin")].append(qMakePair(QSL("frequency"), QVariant(1000)));
    processor.setActiveLeafNodes(QStringList() << QL1S("sin"), parameters);
    QCOMPARE(processor.sanitizedParameterValue(QSL("sin"), QSL("frequency")),
             QVariant(1000));

    // both leafnodes active
    parameters[QSL("sin2")].append(qMakePair(QSL("frequency"), QVariant(1000)));
    processor.setActiveLeafNodes(QStringList() << QL1S("sin") << QL1S("sin2"),
                                 parameters);
    QCOMPARE(processor.sanitizedParameterValue(QSL("sin"), QSL("frequency")),
             QVariant(1000));
    QCOMPARE(processor.sanitizedParameterValue(QSL("sin2"), QSL("frequency")),
             QVariant(1000));

    // setactiveleafnodes without setting parameters and recreate on results in
    // default parameters because all blocks get reconstructed
    processor.setActiveLeafNodes(
        QStringList() << QL1S("sin") << QL1S("sin2"),
        QMap<QString, QList<QPair<QString, QVariant>>>(), true);
    QCOMPARE(processor.sanitizedParameterValue(QSL("sin"), QSL("frequency")),
             QVariant(1));
    QCOMPARE(processor.sanitizedParameterValue(QSL("sin2"), QSL("frequency")),
             QVariant(1));

    processor.release();
    file.close();

    TEST_EXCEPTIONS_CATCH
}

void BerthaTest::testProcessorBlockConnections()
{
    TEST_EXCEPTIONS_TRY

    QDir blockConfigurations(Paths::searchDirectory(
        QL1S("testdata/blockconfigurations"), Paths::dataDirectories()));
    QTemporaryFile outputFile;
    outputFile.open();
    QFile experimentFile(Paths::searchFile(
        QL1S("testexperiments/dummyexperiment.xml"), Paths::dataDirectories()));
    experimentFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data(experimentFile.readAll());
    data.replace("%OUTPUTFILEPLACEHOLDER%", outputFile.fileName().toUtf8());

    ExperimentProcessor processor(XmlLoader().loadContents(data), true);

    // lazy init so no connections are made
    QVERIFY(processor.dumpActiveConnections().isEmpty());

    // device, leaf1
    processor.setActiveLeafNodes(QStringList() << QL1S("leaf1")
                                               << QL1S("device"));
    QStringList activeConnections(processor.dumpActiveConnections());
    QStringList verify(readConnectionsFromFile(
        blockConfigurations.filePath(QL1S("dummyexperiment_leaf1.txt"))));
    QVERIFY(activeConnections.removeDuplicates() == 0);
    QCOMPARE(activeConnections.toSet(), verify.toSet());

    // test all
    processor.setActiveLeafNodes(QStringList() << QL1S("leaf1") << QL1S("leaf2")
                                               << QL1S("device"));
    activeConnections = processor.dumpActiveConnections();
    verify = readConnectionsFromFile(
        blockConfigurations.filePath(QL1S("dummyexperiment_all.txt")));
    QVERIFY(activeConnections.removeDuplicates() == 0);
    QCOMPARE(activeConnections.toSet(), verify.toSet());

    // just device and leaf1 again
    processor.setActiveLeafNodes(QStringList() << QL1S("leaf1")
                                               << QL1S("device"));
    activeConnections = processor.dumpActiveConnections();
    verify = readConnectionsFromFile(
        blockConfigurations.filePath(QL1S("dummyexperiment_leaf1.txt")));
    QVERIFY(activeConnections.removeDuplicates() == 0);
    QCOMPARE(activeConnections.toSet(), verify.toSet());

    // device, leaf2
    processor.setActiveLeafNodes(QStringList() << QL1S("leaf2")
                                               << QL1S("device"));
    activeConnections = processor.dumpActiveConnections();
    verify = readConnectionsFromFile(
        blockConfigurations.filePath(QL1S("dummyexperiment_leaf2.txt")));
    QVERIFY(activeConnections.removeDuplicates() == 0);
    QCOMPARE(activeConnections.toSet(), verify.toSet());

    // test all one list time
    processor.setActiveLeafNodes(processor.leafNodes());
    activeConnections = processor.dumpActiveConnections();
    verify = readConnectionsFromFile(
        blockConfigurations.filePath(QL1S("dummyexperiment_all.txt")));
    QVERIFY(activeConnections.removeDuplicates() == 0);
    QCOMPARE(activeConnections.toSet(), verify.toSet());

    TEST_EXCEPTIONS_CATCH
}
