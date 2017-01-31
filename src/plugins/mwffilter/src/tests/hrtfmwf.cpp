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

#include "pluginfilterinterface.h"
#include "tester.h"

#include <memory>

#include <sndfile.h>

namespace
{

const char noiseHrtfFile[] =
    "data/tests/freedome_hrtf_GHB_0_5s_90deg_16000Hz_right.bin";
const char signalHrtfFile[] =
    "data/tests/freedome_hrtf_GHB_0_5s_0deg_16000Hz_right.bin";
const char wienerFile[] =
    "data/tests/GHB_0_5s_filter_FB10_N64_vu_man_0deg_babble_90_180_270deg"
    "_16000Hz_w00_muinv_0.5_mis_0dB_0_deg_right_SNR-10.bin";
const char signalWaveFile[] = "data/tests/man001.wav";
const char noiseWaveFile[] = "data/tests/auditec16000.wav";
const char testWaveFile[] = "data/tests/output_SNR-10_sentence1.wav";
const char resultWaveFile[] = "data/tests/result-generated.wav";

void extractBlocks (float *input, unsigned channels, unsigned size,
        double **output)
{
    for (unsigned i = 0; i < channels; ++i)
        for (unsigned j = 0; j < size; ++j)
            output[i][j] = input[j * channels + i];
}

void mergeBlocks (double **input, unsigned channels, unsigned size,
        float *output)
{
    for (unsigned i = 0; i < channels; ++i)
        for (unsigned j = 0; j < size; ++j)
            output[j * channels + i] = input[i][j];
}

class Plugins
{
public:
    Plugins()
    {
        Q_FOREACH (QObject *plugin, QPluginLoader::staticInstances())
            plugins.append (plugin);

        QDir pluginsDir (QCoreApplication::applicationDirPath());
        Q_FOREACH (QString fileName, pluginsDir.entryList (QDir::Files)) {
            QPluginLoader loader (pluginsDir.absoluteFilePath (fileName));
            QObject *plugin = loader.instance();
            if (plugin)
                plugins.append (plugin);
        }
    }

    QList<QObject*> get() const
    {
        return plugins;
    }

private:
    QList<QObject*> plugins;
};

Q_GLOBAL_STATIC (Plugins, plugins)

QList<QObject*> allAvailablePlugins()
{
    return plugins()->get();
}

template <typename T>
QList<T*> availablePlugins()
{
    QList<T*> result;

    Q_FOREACH (QObject * const plugin, allAvailablePlugins())
        if (T * const casted = qobject_cast<T*> (plugin))
            result.append (casted);

    return result;
}

} // namespace

void TestSyl::hrtfMwf_data()
{
    QTest::addColumn<QString> ("hrtf");
    QTest::newRow ("simple") << "firfilter";
    QTest::newRow ("threaded") << "threadedfirfilter";
}

void TestSyl::hrtfMwf()
{
    QFETCH(QString, hrtf);

    const unsigned channels = 2;
    const unsigned blockSize = 8192;
    const unsigned sampleRate = 16000;

    // Setup HRTF modules
    PluginFilterCreator* hrtfCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
            availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains (hrtf)) {
            hrtfCreator = creator;
            break;
        }
    }
    if (!hrtfCreator)
        QFAIL ("Unable to find HRTF plugin.");

    std::auto_ptr<PluginFilterInterface> hrtfNoiseFilter
        (hrtfCreator->createFilter (hrtf, channels, blockSize, sampleRate));
    if (!hrtfNoiseFilter.get())
        QFAIL ("Unable to instantiate HRTF filter instance.");
    hrtfNoiseFilter->resetParameters();
    if (!hrtfNoiseFilter->setParameter ("uri", -1, noiseHrtfFile))
        QFAIL (qPrintable (hrtfNoiseFilter->errorMessage()));
    if (!hrtfNoiseFilter->prepare (0))
        QFAIL (qPrintable (hrtfNoiseFilter->errorMessage()));

    std::auto_ptr<PluginFilterInterface> hrtfSignalFilter
        (hrtfCreator->createFilter (hrtf, channels, blockSize, sampleRate));
    if (!hrtfSignalFilter.get())
        QFAIL ("Unable to instantiate HRTF filter instance.");
    hrtfSignalFilter->resetParameters();
    if (!hrtfSignalFilter->setParameter ("uri", -1, signalHrtfFile))
        QFAIL (qPrintable (hrtfSignalFilter->errorMessage()));
    if (!hrtfSignalFilter->prepare (0))
        QFAIL (qPrintable (hrtfSignalFilter->errorMessage()));

    // Setup MWF module
    PluginFilterCreator* wienerCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
            availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains ("wiener")) {
            wienerCreator = creator;
            break;
        }
    }
    if (!wienerCreator)
        QFAIL ("Unable to find wiener plugin.");
    std::auto_ptr<PluginFilterInterface> wienerFilter
        (wienerCreator->createFilter ("wiener", channels, blockSize,
                                      sampleRate));
    if (!wienerFilter.get())
        QFAIL (qPrintable (wienerFilter->errorMessage()));
    wienerFilter->resetParameters();
    if (!wienerFilter->setParameter ("uri", -1, wienerFile))
        QFAIL (qPrintable (wienerFilter->errorMessage()));
    if (!wienerFilter->prepare (0))
        QFAIL (qPrintable (wienerFilter->errorMessage()));

    // Load files
    SF_INFO fileInfo;

    fileInfo.format = 0;
    SNDFILE *noiseFile = sf_open (noiseWaveFile, SFM_READ, &fileInfo);
    if (!noiseFile)
        QFAIL ("Unable to open noise wave file.");
    if (fileInfo.channels != 1)
        QFAIL ("Noise file must have exactly one channel.");
    if (fileInfo.samplerate != int (sampleRate))
        QFAIL ("Input file must be 16kHz.");

    fileInfo.format = 0;
    SNDFILE *signalFile = sf_open (signalWaveFile, SFM_READ, &fileInfo);
    if (!signalFile)
        QFAIL ("Unable to open signal wave file.");
    if (fileInfo.channels != 1)
        QFAIL ("Signal file must have exactly one channel.");
    if (fileInfo.samplerate != int (sampleRate))
        QFAIL ("Input file must be 16kHz.");

    fileInfo.format = 0;
    SNDFILE *testFile = sf_open (testWaveFile, SFM_READ, &fileInfo);
    if (!testFile)
        QFAIL ("Unable to open test wave file.");
    if (fileInfo.channels != int (channels))
        QFAIL ("Test file must have correct number of channels.");
    if (fileInfo.samplerate != int (sampleRate))
        QFAIL ("Input file must be 16kHz.");

    // Setup output file
    fileInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    fileInfo.channels = channels;
    fileInfo.samplerate = sampleRate;
    SNDFILE *outputFile = NULL;
#if 0
    outputFile = sf_open (resultWaveFile, SFM_WRITE, &fileInfo);
    if (!outputFile)
        QFAIL ("Unable to open output file.");
#endif

    // Setup buffers
    QVector<float> noiseData (blockSize * channels);
    QList<QVector<double> > noiseSplitArrays;
    QVector<double*> noiseSplitData;
    for (unsigned i = 0; i < channels; ++i) {
        noiseSplitArrays.append (QVector<double> (blockSize));
        noiseSplitData.append (noiseSplitArrays[i].data());
    }

    QVector<float> signalData (blockSize * channels);
    QList<QVector<double> > signalSplitArrays;
    QVector<double*> signalSplitData;
    for (unsigned i = 0; i < channels; ++i) {
        signalSplitArrays.append (QVector<double> (blockSize));
        signalSplitData.append (signalSplitArrays[i].data());
    }

    QVector<float> testData (blockSize * channels);
    QList<QVector<double> > testSplitArrays;
    QVector<double*> testSplitData;
    for (unsigned i = 0; i < channels; ++i) {
        testSplitArrays.append (QVector<double> (blockSize));
        testSplitData.append (testSplitArrays[i].data());
    }

    // Processing
    unsigned beginNoise = 8192;
    unsigned endNoise = 8192;

    unsigned framesRead = 0;
    while (beginNoise || (framesRead = sf_readf_float (signalFile,
                    signalData.data(), blockSize)) || endNoise) {
        // Read the signal and noise
        if (beginNoise > 0) {
            // Pad at the beginning
            framesRead = qMin (blockSize, beginNoise);
            beginNoise -= framesRead;
            memset (signalData.data(), 0,
                    framesRead * channels * sizeof (float));
        } else if (framesRead && framesRead < blockSize) {
            // Fill last block
            memset (signalData.data() + framesRead, 0,
                    (blockSize - framesRead) * sizeof (float));
            framesRead = blockSize;
        } else if (framesRead < blockSize && endNoise) {
            // Pad at the end
            const unsigned toRead = qMin (blockSize - framesRead, endNoise);
            memset (signalData.data() + framesRead, 0, toRead * sizeof (float));
            endNoise -= toRead;
            framesRead += toRead;
        }
        if (sf_readf_float (noiseFile, noiseData.data(), framesRead) !=
                int (framesRead))
            QFAIL ("Not enough noise signal");

        // Extracting the blocks
        extractBlocks (signalData.data(), 1, blockSize, signalSplitData.data());
        extractBlocks (noiseData.data(), 1, blockSize, noiseSplitData.data());

        // Processing
        hrtfSignalFilter->process (signalSplitData.data());
        hrtfNoiseFilter->process (noiseSplitData.data());
        for (unsigned i = 0; i < channels; ++i)
            for (unsigned j = 0; j < blockSize; ++j)
                signalSplitData[i][j] += noiseSplitData[i][j];
        wienerFilter->process (signalSplitData.data());

        // Compare with what it should be
        if (sf_readf_float (testFile, testData.data(), framesRead) !=
                int (framesRead))
            QFAIL ("Not enough test signal");
        extractBlocks (testData.data(), channels, blockSize,
                testSplitData.data());
        for (unsigned j = 0; j < blockSize; ++j)
            testSplitData[1][j] *= 32768.0 / 32767.0;
        ARRAYFUZZCOMP (signalSplitData[0], testSplitData[1],
                1.0 / (65536 - 2 /* FUZZ */), framesRead);
        // Write output
        mergeBlocks (signalSplitData.data(), channels, blockSize,
                signalData.data());
        if (outputFile)
            sf_writef_float (outputFile, signalData.data(), framesRead);
    };

    // Finish files
    sf_close (noiseFile);
    sf_close (signalFile);
    sf_close (testFile);
    if (outputFile)
        sf_close (outputFile);
}
