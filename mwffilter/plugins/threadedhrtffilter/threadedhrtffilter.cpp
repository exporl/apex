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

#include "syllib/threadedfirfilter.h"

#include <QMap>
#include <QMetaType>
#include <QSemaphore>
#include <QStringList>
#include <QThread>
#include <QUrl>

using namespace syl;

class ThreadedHrtfFilterCreator :
    public QObject,
    public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginFilterCreator)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "apex.threadedhrtffilter")
#endif
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter (const QString &name,
            unsigned channels, unsigned blockSize, unsigned fs) const;
};

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2 (threadedhrtffilter, ThreadedHrtfFilterCreator)
#endif

class ThreadedHrtfFilter:
    public QObject,
    public PluginFilterInterface
{
    Q_OBJECT
public:
    ThreadedHrtfFilter (unsigned channels, unsigned blockSize);
    ~ThreadedHrtfFilter();

    virtual void resetParameters();

    virtual bool isValidParameter (const QString &type, int channel) const;
    virtual bool setParameter (const QString &type, int channel,
            const QString &value);

    virtual bool prepare (unsigned numberOfFrames);

    virtual void process (double * const *data);

private:
    const unsigned channels;
    const unsigned blockSize;

    QString filePath;
    QString newFilePath;
    QList<ThreadedFirFilter*> filters;
    QList<QThread*> threads;
    QSemaphore semaphore;
    unsigned limit;
    unsigned maxThreads;
    bool disabled;
    bool buffered;                        // use filters in "asynchronous" mode?
    QMap<QString, QString> parameters;
};

// ThreadedHrtfFilter ==========================================================

ThreadedHrtfFilter::ThreadedHrtfFilter (unsigned channels, unsigned blockSize) :
    channels (channels),
    blockSize (blockSize),
    buffered(false)
{
    qRegisterMetaType<double*>("double*");
    qRegisterMetaType<QSemaphore*>("QSemaphore*");
    resetParameters();
}

ThreadedHrtfFilter::~ThreadedHrtfFilter()
{
    qDeleteAll (filters);
    Q_FOREACH (QThread *thread, threads) {
        thread->exit();
        thread->wait();
    }
}

void ThreadedHrtfFilter::resetParameters()
{
    filePath.clear();
    limit = 0;
    maxThreads = 0;
    disabled = false;
    buffered = false;
    parameters.clear();
}

bool ThreadedHrtfFilter::isValidParameter (const QString &type,
        int channel) const
{
    if (type == QLatin1String("uri") && channel == -1)
        return true;
    if (type == QLatin1String("limit") && channel == -1)
        return true;
    if (type == QLatin1String("disabled") && channel == -1)
        return true;
    if (type == QLatin1String("threads") && channel == -1)
        return true;
    if (type == QLatin1String("buffered") && channel == -1)
        return true;


    // All other parameters go to the parameters array
    if (channel == -1)
        return true;

    setErrorMessage (QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

bool ThreadedHrtfFilter::setParameter (const QString &type, int channel,
        const QString &value)
{
    if (type == QLatin1String("uri") && channel == -1) {
        newFilePath = QUrl (value).path();
        return true;
    }
    if (type == QLatin1String("limit") && channel == -1) {
        limit = value.toUInt();
        return true;
    }
    if (type == QLatin1String("threads") && channel == -1) {
        maxThreads = value.toUInt();
        return true;
    }
    if (type == QLatin1String("disabled") && channel == -1) {
        disabled = value == QLatin1String("true") || value == QLatin1String("1") ||
                   value == QLatin1String("yes")  || value == QLatin1String("disabled");
        return true;
    }

    if (type == QLatin1String("buffered") && channel == -1) {
        buffered = value == QLatin1String("true") || value == QLatin1String("1") ||
                value == QLatin1String("yes")  || value == QLatin1String("buffered");
        return true;
    }


    if (channel == -1) {
        parameters.insert (type, value);
        return true;
    }

    setErrorMessage (QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg (channel).arg (type));
    return false;
}

bool ThreadedHrtfFilter::prepare (unsigned numberOfFrames)
{
    Q_UNUSED (numberOfFrames);
    try {
        if (filePath != newFilePath) {
            filePath=newFilePath;
            QString filePath (this->filePath);
            QMapIterator<QString, QString> i (parameters);
            while (i.hasNext()) {
                i.next();
                filePath.replace (QLatin1String("${") + i.key() + QLatin1String("}"), i.value());
            }
            // Remove old filters and threads
            qDeleteAll (filters);
            filters.clear();
            Q_FOREACH (QThread *thread, threads) {
                thread->exit();
                thread->wait();
            }
            qDeleteAll (threads);
            threads.clear();
            // Create filters
            Q_FOREACH (const QVector<double> &taps,
                    FirFilter::load (filePath, limit)) {
                ThreadedFirFilter *filter = new ThreadedFirFilter (taps, true);
                filters.append (filter);
                if (blockSize % filter->length() != 0) {
                    setErrorMessage (QString::fromLatin1("Blocksize %1 is not multiple of FIR"
                    " filter length %2").arg (blockSize).arg (filter->length()));
                    return false;
                }
            }
            // Create threads
            int threadCount;
            if (maxThreads)
                threadCount = qMin (unsigned (filters.size()), maxThreads);
            else if ((threadCount = QThread::idealThreadCount()) == -1)
                threadCount = filters.size();
            for (int i = 0; i < threadCount; ++i) {
                QThread *thread = new QThread(this);
                threads.append (thread);
                thread->start();
            }
            // Move filters to threads
            for (unsigned i = 0; i < unsigned (filters.size()); ++i)
                filters[i]->moveToThread (threads[i % threads.size()]);
        }
    } catch (std::exception &e) {
        setErrorMessage (QString::fromLatin1("Unable to instantiate FIR filters: %1")
                .arg (QString::fromLocal8Bit(e.what())));
        return false;
    }

    if (unsigned (filters.size()) != channels) {
        setErrorMessage (QString::fromLatin1("Number of channels differ: plugin has %1 "
            "channels, FIR filter %2").arg (channels).arg (filters.size()));
        return false;
    }

    return true;
}

void ThreadedHrtfFilter::process (double * const *data)
{
    if (disabled)
        return;

    for (unsigned channel = 1; channel < channels; ++channel)
        memcpy (data[channel], data[0], blockSize * sizeof (double));

    for (unsigned channel = 0; channel < channels; ++channel)
        QMetaObject::invokeMethod (filters[channel], "process",
            Q_ARG (double*, data[channel]),
            Q_ARG (unsigned int, blockSize),
            Q_ARG (QSemaphore*, &semaphore));
    semaphore.acquire (channels);
}

// ThreadedHrtfFilterCreator ===================================================

QStringList ThreadedHrtfFilterCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("threadedfirfilter");
}

PluginFilterInterface *ThreadedHrtfFilterCreator::createFilter
       (const QString &name, unsigned channels, unsigned size,
        unsigned sampleRate) const
{
    Q_UNUSED (sampleRate);

    try {
        if (name == QLatin1String("threadedfirfilter"))
            return new ThreadedHrtfFilter (channels, size);
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "threadedhrtffilter.moc"
