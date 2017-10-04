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

#include "common/testutils.h"

#include "apexmaintest.h"

#include <sndfile.h>

#include <QScopedPointer>

#include <cmath>

namespace
{

const char noiseHrtfFile[] = "../../../bertha-lib/data/testdata/"
                             "freedome_hrtf_GHB_0_5s_90deg_16000Hz_right.bin";
const char signalHrtfFile[] = "../../../bertha-lib/data/testdata/"
                              "freedome_hrtf_GHB_0_5s_0deg_16000Hz_right.bin";
const char wienerFile[] =
    "../../../bertha-lib/data/testdata/GHB_0_5s_filter.bin";
const char beamformerFile[] =
    "../../../bertha-lib/data/testdata/FB_freedom_10.bin";
const char deemphFile[] = "../../../bertha-lib/data/testdata/filter_deemph.bin";

const char signalWaveFile[] = "../../../examples/mwffilter/preempman001.wav";
const char noiseWaveFile[] =
    "../../../examples/mwffilter/preempauditec16000.wav";
const char testWaveFile[] =
    "../../../examples/mwffilter/adapt-output_SNR-10_sentence1.wav";
const char testWaveFile2[] =
    "../../../examples/mwffilter/adapt-output_SNR-10_sentence1_10.wav";
// const char resultWaveFile[] = "adapt-generated.wav";

void cloneBlocks(double **input, unsigned channels, unsigned size,
                 double **output)
{
    for (unsigned i = 0; i < channels; ++i)
        memcpy(output[i], input[i], sizeof(double) * size);
}

void extractBlocks(float *input, unsigned channels, unsigned size,
                   double **output)
{
    for (unsigned i = 0; i < channels; ++i)
        for (unsigned j = 0; j < size; ++j)
            output[i][j] = input[j * channels + i];
}

void mergeBlocks(double **input, unsigned channels, unsigned size,
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
            plugins.append(plugin);

        QDir pluginsDir(QCoreApplication::applicationDirPath());
        Q_FOREACH (QString fileName, pluginsDir.entryList(QDir::Files)) {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (plugin)
                plugins.append(plugin);
        }
    }

    QList<QObject *> get() const
    {
        return plugins;
    }

private:
    QList<QObject *> plugins;
};

Q_GLOBAL_STATIC(Plugins, plugins)

QList<QObject *> allAvailablePlugins()
{
    return plugins()->get();
}

template <typename T>
QList<T *> availablePlugins()
{
    QList<T *> result;

    Q_FOREACH (QObject *const plugin, allAvailablePlugins())
        if (T *const casted = qobject_cast<T *>(plugin))
            result.append(casted);

    return result;
}

} // namespace

void ApexMainTest::hrtfMwfAdaptive_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<double>("offset");
    QTest::newRow("0") << testWaveFile << 0.0;
    QTest::newRow("10") << testWaveFile2 << 10.0;
}

void ApexMainTest::hrtfMwfAdaptive()
{
    TEST_EXCEPTIONS_TRY

#ifdef Q_OS_ANDROID
    QSKIP("Skipped on Android");
#endif

    QFETCH(QString, name);
    QFETCH(double, offset);

    const unsigned channels = 2;
    const unsigned blockSize = 8192;
    const unsigned sampleRate = 16000;

    const double baseGain = 15.7378580971083245;
    const double gain = -10;

    const unsigned vadLength = blockSize / 16;
    const unsigned mwfLength = 64;
    const double muInv = 0;

    // Setup modules -----------------------------------------------------------

    // Setup HRTF modules
    PluginFilterCreator *hrtfCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
               availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains(QLatin1String("firfilter"))) {
            hrtfCreator = creator;
            break;
        }
    }
    if (!hrtfCreator)
        QFAIL("Unable to find HRTF plugin.");

    QScopedPointer<PluginFilterInterface> hrtfNoiseFilter(
        hrtfCreator->createFilter(QLatin1String("firfilter"), channels,
                                  blockSize, sampleRate));
    if (!hrtfNoiseFilter.data())
        QFAIL("Unable to instantiate HRTF filter instance.");
    hrtfNoiseFilter->resetParameters();
    if (!hrtfNoiseFilter->setParameter(QLatin1String("file"), -1,
                                       QLatin1String(noiseHrtfFile)))
        QFAIL(qPrintable(hrtfNoiseFilter->errorMessage()));
    if (!hrtfNoiseFilter->prepare(0))
        QFAIL(qPrintable(hrtfNoiseFilter->errorMessage()));

    QScopedPointer<PluginFilterInterface> hrtfSignalFilter(
        hrtfCreator->createFilter(QLatin1String("firfilter"), channels,
                                  blockSize, sampleRate));
    if (!hrtfSignalFilter.data())
        QFAIL("Unable to instantiate HRTF filter instance.");
    hrtfSignalFilter->resetParameters();
    if (!hrtfSignalFilter->setParameter(QLatin1String("file"), -1,
                                        QLatin1String(signalHrtfFile)))
        QFAIL(qPrintable(hrtfSignalFilter->errorMessage()));
    if (!hrtfSignalFilter->prepare(0))
        QFAIL(qPrintable(hrtfSignalFilter->errorMessage()));

    // Setup MWF module
    PluginFilterCreator *wienerCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
               availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains(QLatin1String("wiener"))) {
            wienerCreator = creator;
            break;
        }
    }
    if (!wienerCreator)
        QFAIL("Unable to find wiener plugin.");
    QScopedPointer<PluginFilterInterface> wienerFilter(
        wienerCreator->createFilter(QLatin1String("wiener"), channels,
                                    blockSize, sampleRate));
    if (!wienerFilter.data())
        QFAIL("Unable to instantiate wiener filter instance.");
    wienerFilter->resetParameters();
    if (!wienerFilter->setParameter(QLatin1String("file"), -1,
                                    QLatin1String(wienerFile)))
        QFAIL(qPrintable(wienerFilter->errorMessage()));
    if (!wienerFilter->prepare(0))
        QFAIL(qPrintable(wienerFilter->errorMessage()));

    // Setup beamformer module
    PluginFilterCreator *beamformerCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
               availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains(QLatin1String("beamformer"))) {
            beamformerCreator = creator;
            break;
        }
    }
    if (!beamformerCreator)
        QFAIL("Unable to find beamformer plugin.");

    QScopedPointer<PluginFilterInterface> beamformerFilter(
        beamformerCreator->createFilter(QLatin1String("beamformer"), channels,
                                        blockSize, sampleRate));
    if (!beamformerFilter.data())
        QFAIL("Unable to instantiate beamformer filter instance.");
    beamformerFilter->resetParameters();
    if (!beamformerFilter->setParameter(QLatin1String("file"), -1,
                                        QLatin1String(beamformerFile)))
        QFAIL(qPrintable(beamformerFilter->errorMessage()));
    if (!beamformerFilter->prepare(0))
        QFAIL(qPrintable(beamformerFilter->errorMessage()));

    QScopedPointer<PluginFilterInterface> beamformerSnrFilter(
        beamformerCreator->createFilter(QLatin1String("beamformer"), channels,
                                        blockSize, sampleRate));
    if (!beamformerSnrFilter.data())
        QFAIL("Unable to instantiate beamformer filter instance.");
    beamformerSnrFilter->resetParameters();
    if (!beamformerSnrFilter->setParameter(QLatin1String("file"), -1,
                                           QLatin1String(beamformerFile)))
        QFAIL(qPrintable(beamformerSnrFilter->errorMessage()));
    if (!beamformerSnrFilter->prepare(0))
        QFAIL(qPrintable(beamformerSnrFilter->errorMessage()));

    // Setup deemphasis modules
    PluginFilterCreator *emphasisCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
               availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains(QLatin1String("emphasis"))) {
            emphasisCreator = creator;
            break;
        }
    }
    if (!emphasisCreator)
        QFAIL("Unable to find emphasis plugin.");

    QScopedPointer<PluginFilterInterface> deemphasisVadFilter(
        emphasisCreator->createFilter(QLatin1String("emphasis"), channels,
                                      blockSize, sampleRate));
    if (!deemphasisVadFilter.data())
        QFAIL("Unable to instantiate deemphasis filter instance.");
    deemphasisVadFilter->resetParameters();
    if (!deemphasisVadFilter->setParameter(QLatin1String("file"), -1,
                                           QLatin1String(deemphFile)))
        QFAIL(qPrintable(deemphasisVadFilter->errorMessage()));
    if (!deemphasisVadFilter->prepare(0))
        QFAIL(qPrintable(deemphasisVadFilter->errorMessage()));

    QScopedPointer<PluginFilterInterface> deemphasisSignalFilter(
        emphasisCreator->createFilter(QLatin1String("emphasis"), channels,
                                      blockSize, sampleRate));
    if (!deemphasisSignalFilter.data())
        QFAIL("Unable to instantiate deemphasis filter instance.");
    deemphasisSignalFilter->resetParameters();
    if (!deemphasisSignalFilter->setParameter(QLatin1String("file"), -1,
                                              QLatin1String(deemphFile)))
        QFAIL(qPrintable(deemphasisSignalFilter->errorMessage()));
    if (!deemphasisSignalFilter->prepare(0))
        QFAIL(qPrintable(deemphasisSignalFilter->errorMessage()));

    // Setup vad module
    PluginFilterCreator *vadCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
               availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains(QLatin1String("vad"))) {
            vadCreator = creator;
            break;
        }
    }
    if (!vadCreator)
        QFAIL("Unable to find vad plugin.");
    QScopedPointer<PluginFilterInterface> vadFilter(vadCreator->createFilter(
        QLatin1String("vad"), channels, blockSize, sampleRate));
    if (!vadFilter.data())
        QFAIL("Unable to instantiate vad filter instance.");
    vadFilter->resetParameters();
    if (!vadFilter->setParameter(QLatin1String("length"), -1,
                                 QString::number(vadLength)))
        QFAIL(qPrintable(vadFilter->errorMessage()));
    if (!vadFilter->prepare(0))
        QFAIL(qPrintable(vadFilter->errorMessage()));

    // Setup amplifier module
    PluginFilterCreator *amplifierCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
               availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains(QLatin1String("amplifier"))) {
            amplifierCreator = creator;
            break;
        }
    }
    if (!amplifierCreator)
        QFAIL("Unable to find amplifier plugin.");

    QScopedPointer<PluginFilterInterface> amplifierFilter(
        amplifierCreator->createFilter(QLatin1String("amplifier"), channels,
                                       blockSize, sampleRate));
    if (!amplifierFilter.data())
        QFAIL("Unable to instantiate amplifier filter instance.");
    amplifierFilter->resetParameters();
    if (!amplifierFilter->setParameter(QLatin1String("basegain"), -1,
                                       QString::number(baseGain)))
        QFAIL(qPrintable(amplifierFilter->errorMessage()));
    if (!amplifierFilter->setParameter(QLatin1String("gain"), -1,
                                       QString::number(gain)))
        QFAIL(qPrintable(amplifierFilter->errorMessage()));
    if (!amplifierFilter->prepare(0))
        QFAIL(qPrintable(amplifierFilter->errorMessage()));

    QScopedPointer<PluginFilterInterface> offsetAmplifierFilter(
        amplifierCreator->createFilter(QLatin1String("amplifier"), channels,
                                       blockSize, sampleRate));
    if (!offsetAmplifierFilter.data())
        QFAIL("Unable to instantiate amplifier filter instance.");
    offsetAmplifierFilter->resetParameters();
    if (!offsetAmplifierFilter->setParameter(QLatin1String("gain"), -1,
                                             QString::number(offset)))
        QFAIL(qPrintable(offsetAmplifierFilter->errorMessage()));
    if (!offsetAmplifierFilter->prepare(0))
        QFAIL(qPrintable(offsetAmplifierFilter->errorMessage()));

    // Setup wiener module
    PluginFilterCreator *adaptiveWienerCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
               availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains(
                QLatin1String("adaptivewiener"))) {
            adaptiveWienerCreator = creator;
            break;
        }
    }
    if (!adaptiveWienerCreator)
        QFAIL("Unable to find adaptivewiener plugin.");
    QScopedPointer<PluginFilterInterface> adaptiveWienerFilter(
        adaptiveWienerCreator->createFilter(QLatin1String("adaptivewiener"),
                                            2 * channels + 1, blockSize,
                                            sampleRate));
    if (!adaptiveWienerFilter.data())
        QFAIL("Unable to instantiate adaptivewiener filter instance.");
    adaptiveWienerFilter->resetParameters();
    if (!adaptiveWienerFilter->setParameter(QLatin1String("length"), -1,
                                            QString::number(mwfLength)))
        QFAIL(qPrintable(adaptiveWienerFilter->errorMessage()));
    if (!adaptiveWienerFilter->setParameter(QLatin1String("muinv"), -1,
                                            QString::number(muInv)))
        QFAIL(qPrintable(adaptiveWienerFilter->errorMessage()));
    if (!adaptiveWienerFilter->prepare(0))
        QFAIL(qPrintable(adaptiveWienerFilter->errorMessage()));

    // Load files --------------------------------------------------------------

    SF_INFO fileInfo;

    fileInfo.format = 0;
    SNDFILE *noiseFile = sf_open(noiseWaveFile, SFM_READ, &fileInfo);
    if (!noiseFile)
        QFAIL("Unable to open noise wave file.");
    if (fileInfo.channels != 1)
        QFAIL("Noise file must have exactly one channel.");
    if (fileInfo.samplerate != int(sampleRate))
        QFAIL("Input file must be 16kHz.");

    fileInfo.format = 0;
    SNDFILE *signalFile = sf_open(signalWaveFile, SFM_READ, &fileInfo);
    if (!signalFile)
        QFAIL("Unable to open signal wave file.");
    if (fileInfo.channels != 1)
        QFAIL("Signal file must have exactly one channel.");
    if (fileInfo.samplerate != int(sampleRate))
        QFAIL("Input file must be 16kHz.");

    fileInfo.format = 0;
    SNDFILE *testFile = sf_open(QFile::encodeName(name), SFM_READ, &fileInfo);
    if (!testFile)
        QFAIL("Unable to open test wave file.");
    if (fileInfo.channels != int(channels))
        QFAIL("Test file must have correct number of channels.");
    if (fileInfo.samplerate != int(sampleRate))
        QFAIL("Input file must be 16kHz.");

    fileInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    fileInfo.channels = channels;
    fileInfo.samplerate = sampleRate;
    SNDFILE *outputFile = NULL;
#if 0
    outputFile = sf_open (resultWaveFile, SFM_WRITE, &fileInfo);
    if (!outputFile)
        QFAIL ("Unable to open output file.");
#endif

    // Setup buffers -----------------------------------------------------------

    QVector<float> noiseData(blockSize * channels);
    QList<QVector<double>> noiseSplitArrays;
    QVector<double *> noiseSplitData;
    for (unsigned i = 0; i < channels; ++i) {
        noiseSplitArrays.append(QVector<double>(blockSize));
        noiseSplitData.append(noiseSplitArrays[i].data());
    }

    QVector<float> signalData(blockSize * channels);
    QList<QVector<double>> signalSplitArrays;
    QVector<double *> signalSplitData;
    for (unsigned i = 0; i < channels; ++i) {
        signalSplitArrays.append(QVector<double>(blockSize));
        signalSplitData.append(signalSplitArrays[i].data());
    }

    QList<QVector<double>> signalSnrSplitArrays;
    QVector<double *> signalSnrSplitData;
    for (unsigned i = 0; i < channels; ++i) {
        signalSnrSplitArrays.append(QVector<double>(blockSize));
        signalSnrSplitData.append(signalSnrSplitArrays[i].data());
    }

    QList<QVector<double>> vadSplitArrays;
    QVector<double *> vadSplitData;
    for (unsigned i = 0; i < channels; ++i) {
        vadSplitArrays.append(QVector<double>(blockSize));
        vadSplitData.append(vadSplitArrays[i].data());
    }

    QVector<float> testData(blockSize * channels);
    QList<QVector<double>> testSplitArrays;
    QVector<double *> testSplitData;
    for (unsigned i = 0; i < channels; ++i) {
        testSplitArrays.append(QVector<double>(blockSize));
        testSplitData.append(testSplitArrays[i].data());
    }

    QVector<double *> mwfSplitData;
    for (unsigned i = 0; i < channels; ++i)
        mwfSplitData.append(signalSplitData[i]);
    for (unsigned i = 0; i < channels; ++i)
        mwfSplitData.append(signalSnrSplitData[i]);
    mwfSplitData.append(vadSplitData[0]);

    // Processing
    unsigned beginNoise = 16384;
    unsigned endNoise = 8192;

    unsigned framesRead = 0;
    while (beginNoise || (framesRead = sf_readf_float(
                              signalFile, signalData.data(), blockSize)) ||
           endNoise) {
        // Read the signal and noise
        if (beginNoise > 0) {
            // Pad at the beginning
            framesRead = qMin(blockSize, beginNoise);
            beginNoise -= framesRead;
            memset(signalData.data(), 0, framesRead * channels * sizeof(float));
        } else if (framesRead && framesRead < blockSize) {
            // last block is padded
            memset(signalData.data() + framesRead, 0,
                   (blockSize - framesRead) * sizeof(float));
            framesRead = blockSize;
        } else if (!framesRead && endNoise) {
            // Pad at the end
            const unsigned toRead = qMin(blockSize - framesRead, endNoise);
            memset(signalData.data() + framesRead, 0, toRead * sizeof(float));
            endNoise -= toRead;
            framesRead += toRead;
        }
        if (sf_readf_float(noiseFile, noiseData.data(), blockSize) !=
            int(blockSize))
            QFAIL("Not enough noise signal");

        // Extracting the blocks
        extractBlocks(signalData.data(), 1, blockSize, signalSplitData.data());
        extractBlocks(noiseData.data(), 1, blockSize, noiseSplitData.data());

        // Processing
        hrtfSignalFilter->process(signalSplitData.data());
        hrtfNoiseFilter->process(noiseSplitData.data());

        amplifierFilter->process(signalSplitData.data());
        cloneBlocks(signalSplitData.data(), channels, blockSize,
                    signalSnrSplitData.data());
        offsetAmplifierFilter->process(signalSnrSplitData.data());
        for (unsigned i = 0; i < channels; ++i) {
            for (unsigned j = 0; j < blockSize; ++j) {
                signalSplitData[i][j] += noiseSplitData[i][j];
                signalSnrSplitData[i][j] += noiseSplitData[i][j];
            }
        }

        beamformerFilter->process(signalSplitData.data());
        beamformerSnrFilter->process(signalSnrSplitData.data());

        cloneBlocks(signalSnrSplitData.data(), channels, blockSize,
                    vadSplitData.data());
        deemphasisVadFilter->process(vadSplitData.data());

        vadFilter->process(vadSplitData.data());

        adaptiveWienerFilter->process(mwfSplitData.data());

        deemphasisSignalFilter->process(signalSplitData.data());

        // Compare with what it should be
        if (sf_readf_float(testFile, testData.data(), framesRead) !=
            int(framesRead))
            QFAIL("Not enough test signal");
        extractBlocks(testData.data(), channels, blockSize,
                      testSplitData.data());
        for (unsigned j = 0; j < blockSize; ++j)
            testSplitData[1][j] *= 32768.0 / 32767.0;
        ARRAYFUZZCOMP(signalSplitData[0], testSplitData[1], 1.529e-5,
                      framesRead);
        // Write output
        mergeBlocks(signalSplitData.data(), channels, blockSize,
                    signalData.data());
        if (outputFile)
            sf_writef_float(outputFile, signalData.data(), framesRead);
    };

    // Finish files
    sf_close(noiseFile);
    sf_close(signalFile);
    sf_close(testFile);
    if (outputFile)
        sf_close(outputFile);

    TEST_EXCEPTIONS_CATCH
}
