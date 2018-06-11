/******************************************************************************
 * Copyright (C) 2017  Sanne Raymaekers <sanne.raymaekers@gmail.com>          *
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "berthapluginstest.h"

#include "bertha/blockdata.h"
#include "bertha/connectiondata.h"
#include "bertha/devicedata.h"
#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"

#include "common/debug.h"
#include "common/paths.h"
#include "common/sndfilewrapper.h"
#include "common/temporarydirectory.h"
#include "common/testutils.h"

#include <cmath>
#include <numeric>

using namespace bertha;
using namespace cmn;

void BerthaPluginsTest::initTestCase()
{
    enableCoreDumps(QCoreApplication::applicationFilePath());
}

void BerthaPluginsTest::apexAdapterTest()
{
    TEST_EXCEPTIONS_TRY

    TemporaryDirectory outputDir;
    const QString inputPath = Paths::searchFile(QL1S("testdata/sin1000Hz.wav"),
                                                Paths::dataDirectories());
    const QString outputPath = outputDir.addFile(QL1S("output.wav"));
    const QString comparePath = Paths::searchFile(
        QL1S("testdata/sin1000Hz_gain6dB.wav"), Paths::dataDirectories());

    ExperimentData experimentData;
    DeviceData devData(QL1S("device"), QL1S("File"));
    devData.setSampleRate(44100);
    devData.setBlockSize(44100);
    devData.setInputPorts(1);
    devData.setOutputPorts(1);
    devData.setParameter(QL1S("inputFile"), inputPath);
    devData.setParameter(QL1S("outputFile"), outputPath);
    experimentData.setDevice(devData);

    BlockData amp(QL1S("amp"), QL1S("ApexAdapter"));
    amp.setInputPorts(1);
    amp.setOutputPorts(1);
    experimentData.addBlock(amp);

    ConnectionData devToAmp(QL1S("device"), QL1S("capture-1"), QL1S("amp"),
                            QL1S("input-1"));
    ConnectionData ampToDev(QL1S("amp"), QL1S("output-1"), QL1S("device"),
                            QL1S("playback-1"));
    experimentData.addConnection(devToAmp);
    experimentData.addConnection(ampToDev);

    ExperimentProcessor processor(experimentData);

    processor.setParameterValue(QL1S("amp"), QL1S("apexFilter"),
                                QVariant(QL1S("amplifier")));
    QVariantList gainParameter;
    gainParameter << QVariant(QL1S("gain")) << QVariant(-1)
                  << QVariant(QString::number(6));
    processor.setParameterValue(QL1S("amp"), QL1S("parameter"),
                                QVariant(gainParameter));

    QSignalSpy spy(&processor,
                   SIGNAL(stateChanged(bertha::ExperimentProcessor::State)));
    processor.prepare();
    processor.start();
    QVERIFY(spy.wait(1000));
    processor.release();

    cmn::SndFile outputFilePtr;
    cmn::SndFile compareFilePtr;

    SF_INFO outputSfinfo;
    SF_INFO compareSfinfo;
    outputSfinfo.format = 0;
    compareSfinfo.format = 0;
    outputFilePtr.reset(outputPath, SFM_READ, &outputSfinfo);
    compareFilePtr.reset(comparePath, SFM_READ, &compareSfinfo);

    QVERIFY(outputFilePtr);
    QVERIFY(compareFilePtr);

    QVector<float> outputData(44100 + 1);
    const unsigned outputCount = sf_read_float(
        outputFilePtr.get(), outputData.data(), outputData.size());
    QCOMPARE(outputCount, 44100u + 1);
    QVector<float> compareData(44100 + 1);
    const unsigned compareCount = sf_read_float(
        compareFilePtr.get(), compareData.data(), compareData.size());
    QCOMPARE(compareCount, 44100u + 1);

    ARRAYFUZZCOMP(outputData, compareData, 1.0 / 32768, outputCount);

    TEST_EXCEPTIONS_CATCH
}

void BerthaPluginsTest::apexCompatibleDataBlockTest()
{
    TEST_EXCEPTIONS_TRY

    const QString inputPath = Paths::searchFile(
        QL1S("testdata/sin1000Hz_ramp.wav"), Paths::dataDirectories());

    TemporaryDirectory outputDir;
    const QString outputPath = outputDir.addFile(QL1S("output.wav"));

    ExperimentData experimentData;
    DeviceData devData(QL1S("device"), QL1S("File"));
    devData.setSampleRate(44100);
    devData.setBlockSize(2048);
    devData.setInputPorts(1);
    devData.setOutputPorts(0);
    devData.setParameter(QL1S("outputFile"), outputPath);
    experimentData.setDevice(devData);

    BlockData datablock(QL1S("datablock"), QL1S("ApexCompatibleDataBlock"));
    datablock.setOutputPorts(1);
    experimentData.addBlock(datablock);

    ConnectionData dbToDev(QL1S("datablock"), QL1S("output-1"), QL1S("device"),
                           QL1S("playback-1"));
    experimentData.addConnection(dbToDev);

    ExperimentProcessor processor(experimentData);
    processor.setParameterValue(QL1S("datablock"), QL1S("fileName"), inputPath);
    QVariantList triggers;
    triggers << QVariant(0u) << QVariant(4096u) << QVariant(4096u * 2)
             << QVariant(4096u * 3);
    processor.setParameterValue(QL1S("datablock"), QL1S("triggers"), triggers);

    processor.prepare();
    processor.start();
    QTest::qWait(250);
    processor.release();

    cmn::SndFile inputFile, outputFile;
    SF_INFO inputSfInfo, outputSfInfo;
    inputSfInfo.format = 0;
    outputSfInfo.format = 0;
    inputFile.reset(inputPath, SFM_READ, &inputSfInfo);
    outputFile.reset(outputPath, SFM_READ, &outputSfInfo);
    QVERIFY(inputFile);
    QVERIFY(outputFile);
    QVector<float> inputData(4096);
    QVector<float> outputData(4096);

    for (unsigned i = 0; i < 3; ++i) {
        sf_seek(inputFile.get(), 0, SEEK_SET);
        sf_seek(outputFile.get(), 0, SEEK_SET);

        unsigned readCount =
            sf_read_float(inputFile.get(), inputData.data(), inputData.size());
        QCOMPARE(readCount, 4096u);
        readCount = sf_read_float(outputFile.get(), outputData.data(),
                                  outputData.size());
        QCOMPARE(readCount, 4096u);

        ARRAYCOMP(inputData, outputData, (unsigned)inputData.size());
    }

    TEST_EXCEPTIONS_CATCH
}

QTEST_MAIN(BerthaPluginsTest)
