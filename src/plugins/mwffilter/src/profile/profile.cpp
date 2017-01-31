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

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include <QThread>
#include <QVector>

#include <memory>

#ifdef Q_OS_UNIX
    #include <sys/time.h>

    #define START_TIMER(t)                                                     \
        timeval oldtime##t;                                                    \
        gettimeofday (&oldtime##t, NULL);
    #define END_TIMER(t)                                                       \
        timeval newtime##t;                                                    \
        gettimeofday (&newtime##t, NULL);                                      \
        qDebug ("Elapsed: %f", (newtime##t.tv_sec * 1e6 + newtime##t.tv_usec - \
                oldtime##t.tv_sec * 1e6 - oldtime##t.tv_usec) / 1000.0);
#else
    #include <windows.h>

    #define START_TIMER(t)                                                     \
        LARGE_INTEGER frequency##t, oldtime##t;                                \
        QueryPerformanceFrequency (&frequency##t);                             \
        QueryPerformanceCounter (&oldtime##t);
    #define END_TIMER(t)                                                       \
        LARGE_INTEGER newtime##t;                                              \
        QueryPerformanceCounter (&newtime##t);                                 \
        qDebug ("Elapsed: %f", (newtime##t.QuadPart - oldtime##t.QuadPart) /   \
                double (frequency##t.QuadPart));
#endif

const char hrtfFile[] =
    "data/profile/freedome_hrtf_GHB_0_5s_16000Hz_right.bin";

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

void test (const QString &hrtf, unsigned threads)
{
    qDebug ("Module: %s, %u thread(s)", qPrintable (hrtf), threads);

    const unsigned channels = 4;
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
        qFatal ("Unable to find HRTF plugin.");

    std::auto_ptr<PluginFilterInterface> hrtfFilter
        (hrtfCreator->createFilter (hrtf, channels, blockSize, sampleRate));
    if (!hrtfFilter.get())
        qFatal ("Unable to instantiate HRTF filter instance.");
    hrtfFilter->resetParameters();
    if (!hrtfFilter->setParameter ("uri", -1, hrtfFile))
        qFatal ("%s", qPrintable (hrtfFilter->errorMessage()));
    hrtfFilter->setParameter ("threads", -1, QString::number (threads));
    if (!hrtfFilter->prepare (0))
        qFatal ("%s", qPrintable (hrtfFilter->errorMessage()));

    // Setup buffers
    QVector<float> data (blockSize * channels);
    QList<QVector<double> > splitArrays;
    QVector<double*> splitData;
    for (unsigned i = 0; i < channels; ++i) {
        splitArrays.append (QVector<double> (blockSize));
        splitData.append (splitArrays[i].data());
    }

    for (unsigned i = 0; i < 10; ++i) {
        START_TIMER (0);
        for (unsigned j = 0; j < 50; ++j)
            hrtfFilter->process (splitData.data());
        END_TIMER (0);
    }
}

int main (int argc, char *argv[])
{
    QCoreApplication app (argc, argv);

    qDebug ("Ideal thread count: %i", QThread::idealThreadCount());

    test ("hrtf", 1);
    test ("threadedfir", 0);
    test ("threadedfir", 1);
    test ("threadedfir", 2);
    test ("threadedfir", 3);
    test ("threadedfir", 4);

    return 0;
}
