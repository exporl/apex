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

#include "apexmain/filter/pluginfilterinterface.h"

#include "common/pluginloader.h"
#include "common/testutils.h"

#include "tester.h"

#include <sndfile.h>

#include <memory>

using namespace cmn;

void TestSyl::fileSink_data()
{
    QTest::addColumn<QString> ("format");
    QTest::addColumn<int> ("intformat");
    QTest::addColumn<double> ("delta");
    QTest::newRow ("pcm16") << "pcm16" << (SF_FORMAT_PCM_16 | SF_FORMAT_WAV)
        << 1.0 / 20000;
    QTest::newRow ("pcm32") << "pcm32" << (SF_FORMAT_PCM_32 | SF_FORMAT_WAV)
        << 1.0 / 2000000000;
    QTest::newRow ("float") << "float" << (SF_FORMAT_FLOAT | SF_FORMAT_WAV)
        << 1e-5;
    QTest::newRow ("double") << "double" << (SF_FORMAT_DOUBLE | SF_FORMAT_WAV)
        << 1e-5;
}

void TestSyl::fileSink()
{
    TEST_EXCEPTIONS_TRY

    QFETCH(QString, format);
    QFETCH(int, intformat);
    QFETCH(double, delta);

    const unsigned channels = 2;
    const unsigned blockSize = 8192;
    const unsigned sampleRate = 16000;

    QTemporaryFile tempFile;
    tempFile.open();

    // Setup module
    PluginFilterCreator* fileSinkCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
            PluginLoader().availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains (QLatin1String("filesink"))) {
            fileSinkCreator = creator;
            break;
        }
    }
    if (!fileSinkCreator)
        QFAIL ("Unable to find filesink plugin.");

    QScopedPointer<PluginFilterInterface> fileSinkFilter
        (fileSinkCreator->createFilter (QLatin1String("filesink"), channels, blockSize,
                                        sampleRate));
    if (!fileSinkFilter)
        QFAIL ("Unable to instantiate fileSink filter instance.");
    fileSinkFilter->resetParameters();
    if (!fileSinkFilter->setParameter (QLatin1String("file"), -1, tempFile.fileName()))
        QFAIL (qPrintable (fileSinkFilter->errorMessage()));
    if (!fileSinkFilter->setParameter (QLatin1String("format"), -1, format))
        QFAIL (qPrintable (fileSinkFilter->errorMessage()));
    if (!fileSinkFilter->prepare (0))
        QFAIL (qPrintable (fileSinkFilter->errorMessage()));

    // Write data
    QVector<double> signalData (blockSize * channels);
    QVector<double*> signalSplitData;
    for (unsigned i = 0; i < channels; ++i)
        signalSplitData.append (signalData.data() + i * blockSize);
    for (unsigned i = 0; i < blockSize * channels; ++i)
        signalData[i] = rand() / (RAND_MAX + 1.0);

    fileSinkFilter->process (signalSplitData.data());
    fileSinkFilter.reset();

    // Compare
    SF_INFO fileInfo;

    fileInfo.format = 0;
    SNDFILE *dataFile = sf_open (QFile::encodeName (tempFile.fileName()),
            SFM_READ, &fileInfo);
    QVERIFY (dataFile);
    QCOMPARE (fileInfo.format, intformat);
    QCOMPARE (fileInfo.channels, int (channels));
    QCOMPARE (fileInfo.samplerate, int (sampleRate));

    QVector<double> readData (blockSize * channels);
    QVector<double> sortedData (blockSize * channels);

    QCOMPARE (sf_readf_double (dataFile, readData.data(), blockSize),
            sf_count_t (blockSize));
    for (unsigned i = 0; i < blockSize * channels; ++i)
        sortedData[(i % channels) * blockSize + i / channels] = readData[i];
    ARRAYFUZZCOMP (sortedData.data(), signalData.data(), delta,
            blockSize * channels);

    QCOMPARE (sf_readf_double (dataFile, readData.data(), blockSize),
            sf_count_t (0));

    sf_close (dataFile);

    TEST_EXCEPTIONS_CATCH
}
