/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
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

#include "bertha/experimentdata.h"
#include "bertha/experimentprocessor.h"
#include "bertha/xmlloader.h"

#include "common/paths.h"
#include "common/sndfilewrapper.h"
#include "common/testutils.h"

#include "tests.h"

#include <QDir>
#include <QFile>

#include <memory>

using namespace bertha;
using namespace cmn;

void BerthaTest::testFileSink_data()
{
    QTest::addColumn<QString>("format");
    QTest::addColumn<int>("intformat");
    QTest::addColumn<double>("delta");
    QTest::newRow("pcm16") << "pcm16" << (SF_FORMAT_PCM_16 | SF_FORMAT_WAV)
                           << 1.0 / 20000;
    QTest::newRow("pcm32") << "pcm32" << (SF_FORMAT_PCM_32 | SF_FORMAT_WAV)
                           << 1.0 / 2000000000;
    QTest::newRow("float") << "float" << (SF_FORMAT_FLOAT | SF_FORMAT_WAV)
                           << 1e-5;
    QTest::newRow("double") << "double" << (SF_FORMAT_DOUBLE | SF_FORMAT_WAV)
                            << 1e-5;
}

void BerthaTest::testFileSink()
{
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, format);
    QFETCH(int, intformat);
    QFETCH(double, delta);

    const unsigned channels = 1;
    const unsigned blockSize = 8192;
    const unsigned sampleRate = 16000;

    QTemporaryFile tempFile1;
    tempFile1.open();

    QTemporaryFile tempFile2;
    tempFile2.open();

    const QString xmlFile = QLatin1String("testexperiments/filesinktest.xml");
    const QString xmlPath =
        Paths::searchFile(xmlFile, Paths::dataDirectories());
    const QString inputFile = QLatin1String("testdata/man001.wav");
    const QString inputPath =
        Paths::searchFile(inputFile, Paths::dataDirectories());

    ExperimentProcessor processor(XmlLoader().loadFile(xmlPath));
    processor.setParameterValue(QString::fromLatin1("device"),
                                QString::fromLatin1("outputFiles"),
                                tempFile2.fileName());
    processor.setParameterValue(QString::fromLatin1("device"),
                                QString::fromLatin1("inputFiles"), inputPath);

    processor.setParameterValue(QString::fromLatin1("block"),
                                QString::fromLatin1("filePath"),
                                tempFile1.fileName());
    processor.setParameterValue(QString::fromLatin1("block"),
                                QString::fromLatin1("format"), format);

    processor.prepare();
    processor.start();
    QTest::qWait(250);
    processor.release();

    const QString outputPath = tempFile1.fileName();
    const QString comparePath = Paths::searchFile(
        QLatin1String("testdata/man001.wav"), Paths::dataDirectories());

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

    QCOMPARE(outputSfinfo.format, intformat);
    QCOMPARE(outputSfinfo.channels, int(channels));
    QCOMPARE(outputSfinfo.samplerate, int(sampleRate));

    QVector<float> outputData(blockSize);
    const unsigned outputCount = sf_read_float(
        outputFilePtr.get(), outputData.data(), outputData.size());
    QCOMPARE(outputCount, blockSize);
    QVector<float> compareData(blockSize);
    const unsigned compareCount = sf_read_float(
        compareFilePtr.get(), compareData.data(), compareData.size());
    QCOMPARE(compareCount, blockSize);

    for (unsigned i = 0; i < outputCount; ++i) {
        if (!(qAbs(outputData[i] - compareData[i]) < delta))
            QFAIL(
                qPrintable(QString::fromLatin1(
                               "Compared values at [%4] differ more than %1.\n"
                               "  Actual: %2\n"
                               "  Expected: %3\n")
                               .arg(delta)
                               .arg(outputData[i])
                               .arg(compareData[i])
                               .arg(i)));
    }

    TEST_EXCEPTIONS_CATCH
}
