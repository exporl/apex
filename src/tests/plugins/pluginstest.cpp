/******************************************************************************
 * Copyright (C) 2016 Benjamin Dieudonné <benjamin.dieudonne@kuleuven.be>     *
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
#include "tester.h"

#include <memory>

#include <sndfile.h>

namespace
{

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

void TestSyl::vocoder()
{
    QFETCH(QString, format);
    QFETCH(int, intformat);
    QFETCH(double, delta);

    const unsigned channels = 1;
    const unsigned blockSize = 1024;
    const unsigned sampleRate = 44100;

    // Load vocoder plugin
    PluginFilterCreator* vocoderCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
            availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains (QLatin1String("vocoder"))) {
            vocoderCreator = creator;
            break;
        }
    }
    if (!vocoderCreator)
        QFAIL ("Unable to find vocoder plugin.");

    QScopedPointer<PluginFilterInterface> vocoderFilter
        (vocoderCreator->createFilter (QLatin1String("vocoder"), channels, blockSize,
                                        sampleRate));
    if (!vocoderFilter)
        QFAIL ("Unable to instantiate vocoder filter instance.");
    vocoderFilter->resetParameters();

    // Set parameters and prepare filter
    QDir directory(paths.GetBasePath());
    directory.cd("data/tests/libplugins/vocodertest");
    QString filterBankFilename(directory.path() + "/filterbank.bin");
    QString lowpassFilterFilename(directory.path() + "/lowpass.bin");
    QString carriersFilename(directory.path() + "/carriers.wav");

    if (!vocoderFilter->setParameter (QLatin1String("filterbank"), -1, filterBankFilename))
        QFAIL (qPrintable (vocoderFilter->errorMessage()));
    if (!vocoderFilter->setParameter (QLatin1String("lowpass"), -1, lowpassFilterFilename))
        QFAIL (qPrintable (vocoderFilter->errorMessage()));
    if (!vocoderFilter->setParameter (QLatin1String("carriers"), -1, carriersFilename))
        QFAIL (qPrintable (vocoderFilter->errorMessage()));

    if (!vocoderFilter->prepare (0))
        QFAIL (qPrintable (vocoderFilter->errorMessage()));

    // Read asa.wav
    QString asaFilename(directory.path() + "/asa.wav");
    QByteArray filePath =
        QFile::encodeName(asaFilename);
    SF_INFO readSfinfo;
    readSfinfo.format = 0;
    SNDFILE* asaFile = sf_open(filePath, SFM_READ, &readSfinfo);
    if (!asaFile) {
        setErrorMessage(QString("Unable to open input file %1: %2")
                .arg(QFile::decodeName(filePath),
                    QString::fromLocal8Bit(sf_strerror(NULL))));
        return false;
    }
    QVector<double> asa;
    asa.resize(readSfinfo.channels*readSfinfo.frames);
    asaLength = readSfinfo.frames;
    sf_readf_double(carriersFile, asa.data(), asaLength);
    sf_close(asaFile);

    // Apply vocoder to asa.wav
    QVector<double*> asaData = asa.data();
    vocoderFilter->process (asaData.data());
    vocoderFilter.reset();

    // Read asa_vocoded.wav (vocoded in MATLAB)
    QString asaVocodedFilename(directory.path() + "/asa_vocoded.wav");
    QByteArray filePath =
        QFile::encodeName(asaVocodedFilename);
    SF_INFO readSfinfo;
    readSfinfo.format = 0;
    SNDFILE* asaVocodedFile = sf_open(filePath, SFM_READ, &readSfinfo);
    if (!asaVocodedFile) {
        setErrorMessage(QString("Unable to open input file %1: %2")
                .arg(QFile::decodeName(filePath),
                    QString::fromLocal8Bit(sf_strerror(NULL))));
        return false;
    }
    QVector<double> asaVocoded;
    asaVocoded.resize(readSfinfo.channels*readSfinfo.frames);
    asaVocodedLength = readSfinfo.frames;
    sf_readf_double(carriersFile, asaVocoded.data(), asaVocodedLength);
    sf_close(asaVocodedFile);
    QVector<double*> asaVocodedData = asaVocoded.data();

    // Compare with reference signal asa_vocoded.wav
    ARRAYFUZZCOMP (asaData, asaVocodedData, delta, asaLength);
}
