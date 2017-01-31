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

#include <audiofile.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>

#include <cstdio>
#include <memory>

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

int main (int argc, char *argv[])
{
    QCoreApplication application (argc, argv);

    if (argc < 7) {
        fprintf (stderr, "Usage: %s input.wav output.wav numchannels "
                "coefficients-%%1.dat angle maxcoefficients\n", argv[0]);
        exit (1);
    }

    PluginFilterCreator* hrtfCreator = NULL;
    Q_FOREACH (PluginFilterCreator *creator,
            availablePlugins<PluginFilterCreator>()) {
        if (creator->availablePlugins().contains (QLatin1String("hrtf"))) {
            hrtfCreator = creator;
            break;
        }
    }
    if (!hrtfCreator) {
        fprintf (stderr, "Unable to find HRTF plugin.\n");
        exit (2);
    }

    AFfilehandle inputFile = afOpenFile (argv[1], "r", NULL);
    if (inputFile == AF_NULL_FILEHANDLE) {
        fprintf (stderr, "Cannot open input file %s.\n", argv[1]);
        exit (3);
    }
    if (afGetVirtualChannels (inputFile, AF_DEFAULT_TRACK) > 1) {
        fprintf (stderr, "Input file must have exactly one channel.\n");
        exit (4);
    }
    int sampleFormat, sampleWidth;
    afGetVirtualSampleFormat (inputFile, AF_DEFAULT_TRACK, &sampleFormat,
            &sampleWidth);
    if (sampleFormat != AF_SAMPFMT_FLOAT || sampleWidth != 32) {
        fprintf (stderr, "Input file must be encoded with 32bit float.\n");
        exit (5);
    }

    const unsigned channels = QByteArray (argv[3]).toUInt();
    const unsigned sampleRate =
        unsigned (afGetRate (inputFile, AF_DEFAULT_TRACK));
    const unsigned blockSize = 8192;

    // Setup output file
    AFfilesetup setup = afNewFileSetup();
    afInitFileFormat (setup, AF_FILE_WAVE);
    afInitSampleFormat (setup, AF_DEFAULT_TRACK, AF_SAMPFMT_FLOAT, 32);
    afInitByteOrder (setup, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
    afInitChannels (setup, AF_DEFAULT_TRACK, channels);
    afInitRate (setup, AF_DEFAULT_TRACK, sampleRate);
    AFfilehandle outputFile = afOpenFile (argv[2], "w", setup);
    if (outputFile == AF_NULL_FILEHANDLE) {
        fprintf (stderr, "Cannot open output file %s.\n", argv[2]);
        exit (6);
    }

    // Setup buffers
    QVector<float> data (blockSize * channels);
    QList<QVector<double> > split;
    QVector<double*> splitData;
    for (unsigned i = 0; i < channels; ++i) {
        split.append (QVector<double> (blockSize));
        splitData.append (split[i].data());
    }

    // Setup module
    QScopedPointer<PluginFilterInterface> hrtfFilter (hrtfCreator->createFilter
        (QLatin1String("hrtf"), channels, blockSize, sampleRate));
    if (!hrtfFilter) {
        fprintf (stderr, "Unable to instantiate filter instance.\n");
        exit (6);
    }
    hrtfFilter->resetParameters();
    if (!hrtfFilter->setParameter (QLatin1String("uri"), -1, QString::fromLocal8Bit(argv[4]))) {
        fprintf (stderr, "Unable to set coefficient file path: %s\n",
            qPrintable (hrtfFilter->errorMessage()));
        exit (7);
    }
    if (!hrtfFilter->setParameter (QLatin1String("angle"), -1, QString::fromLocal8Bit(argv[5]))) {
        fprintf (stderr, "Unable to set angle: %s\n",
            qPrintable (hrtfFilter->errorMessage()));
        exit (8);
    }
    if (!hrtfFilter->setParameter (QLatin1String("limit"), -1, QString::fromLocal8Bit(argv[6]))) {
        fprintf (stderr, "Unable to set coefficient limit: %s\n",
            qPrintable (hrtfFilter->errorMessage()));
        exit (9);
    }
    if (!hrtfFilter->prepare (0)) {
        fprintf (stderr, "Unable to prepare module: %s\n",
            qPrintable (hrtfFilter->errorMessage()));
        exit (10);
    }

    // Processing
    unsigned framesRead;
    while ((framesRead = afReadFrames (inputFile, AF_DEFAULT_TRACK, data.data(),
                    blockSize))) {
        // Padding
        memset (data.data() + framesRead * channels, 0,
                (blockSize - framesRead) * channels * sizeof (float));
        extractBlocks (data.data(), 1, blockSize, splitData.data());
        hrtfFilter->process (splitData.data());
        mergeBlocks (splitData.data(), channels, blockSize, data.data());
        afWriteFrames (outputFile, AF_DEFAULT_TRACK, data.data(), framesRead);
    };

    // Finish files
    afCloseFile (inputFile);
    afCloseFile (outputFile);
}

