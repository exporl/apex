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

#include "bertha/blockinterface.h"

#include "syllib/threadedfirfilter.h"

#include <QDir>
#include <QMap>
#include <QMetaType>
#include <QSemaphore>
#include <QStringList>
#include <QThread>

void doubleToFloat(double **input, unsigned ports, unsigned size,
                   float **output)
{
    for (unsigned i = 0; i < ports; ++i)
        for (unsigned j = 0; j < size; ++j)
            output[i][j] = input[i][j];
}

void floatToDouble(float **input, unsigned ports, unsigned size,
                   double **output)
{
    for (unsigned i = 0; i < ports; ++i)
        for (unsigned j = 0; j < size; ++j)
            output[i][j] = input[i][j];
}

using namespace syl;

class ThreadedFirFilterCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "threadedfirfilter")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2 (threadedfirfilter, ThreadedFirFilterCreator)
Q_PLUGIN_METADATA(threadedfirfilter)
Q_PLUGIN_METADATA(ThreadedFirFilterCreator)

class ThreadedFirFilterPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(bool disabled WRITE setDisabled READ getDisabled)
    Q_PROPERTY(QString filePath WRITE setFilePath READ getFilePath)
    Q_PROPERTY(unsigned limit WRITE setLimit READ getLimit)
    Q_PROPERTY(bool buffered WRITE setBuffered READ getBuffered)
    Q_PROPERTY(unsigned maxThreads WRITE setMaxThreads READ getMaxThreads)
    Q_CLASSINFO("disabled-description", Q_TR_NOOP("Boolean disabled"))
    Q_CLASSINFO("filePath-description", Q_TR_NOOP("filePath as string"))
    Q_CLASSINFO("limit-description", Q_TR_NOOP("limit as unsigned int"))
    Q_CLASSINFO("buffered-description", Q_TR_NOOP("Boolean buffered"))
    Q_CLASSINFO("maxThreads-description",
                Q_TR_NOOP("maxThreads as unsigned int"))
public:
    Q_INVOKABLE ThreadedFirFilterPlugin(unsigned blockSize, unsigned sampleRate,
                                        int inputPorts, int outputPorts);
    virtual ~ThreadedFirFilterPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    bool getDisabled() const;
    void setDisabled(bool value);

    QString getFilePath() const;
    void setFilePath(QString value);

    unsigned getLimit() const;
    void setLimit(unsigned value);

    bool getBuffered() const;
    void setBuffered(bool value);

    unsigned getMaxThreads() const;
    void setMaxThreads(unsigned value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    QString filePath;
    QString newFilePath;
    QList<ThreadedFirFilter *> filters;
    QList<QThread *> threads;
    QSemaphore semaphore;
    unsigned limit;
    unsigned maxThreads;
    bool disabled;
    bool buffered; // use filters in "asynchronous" mode?
    QMap<QString, QString> parameters;
    QString workDirectory;
};

// ThreadedFirFilterPlugin =====================================================

ThreadedFirFilterPlugin::ThreadedFirFilterPlugin(unsigned blockSize,
                                                 unsigned sampleRate,
                                                 int inputPorts,
                                                 int outputPorts)
    : blockSize(blockSize),
      limit(0),
      maxThreads(0),
      disabled(false),
      buffered(false)
{
    Q_UNUSED(sampleRate);

    qRegisterMetaType<double *>("double*");
    qRegisterMetaType<QSemaphore *>("QSemaphore*");

    if (inputPorts < 0)
        inputPorts = 2;

    if (outputPorts < 0)
        outputPorts = 2;

    for (unsigned i = 0; i < unsigned(qMin(inputPorts, outputPorts)); ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("input-%1").arg(i + 1)));
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }
    workDirectory = QDir::currentPath();
}

ThreadedFirFilterPlugin::~ThreadedFirFilterPlugin()
{
    qDeleteAll(filters);
    Q_FOREACH (QThread *thread, threads) {
        thread->exit();
        thread->wait();
    }
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString ThreadedFirFilterPlugin::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);
    try {
        if (filePath != newFilePath) {
            filePath = newFilePath;
            QString filePath(this->filePath);
            QMapIterator<QString, QString> i(parameters);
            while (i.hasNext()) {
                i.next();
                filePath.replace(QLatin1String("${") + i.key() +
                                     QLatin1String("}"),
                                 i.value());
            }
            // Remove old filters and threads
            qDeleteAll(filters);
            filters.clear();
            Q_FOREACH (QThread *thread, threads) {
                thread->exit();
                thread->wait();
            }
            qDeleteAll(threads);
            threads.clear();
            // Create filters
            Q_FOREACH (const QVector<double> &taps,
                       FirFilter::load(filePath, limit)) {
                ThreadedFirFilter *filter = new ThreadedFirFilter(taps, true);
                filters.append(filter);
                if (blockSize % filter->length() != 0)
                    return tr("Blocksize %1 is "
                              "not multiple of FIR filter length %2")
                        .arg(blockSize)
                        .arg(filter->length());
            }
            // Create threads
            int threadCount;
            if (maxThreads)
                threadCount = qMin(unsigned(filters.size()), maxThreads);
            else if ((threadCount = QThread::idealThreadCount()) == -1)
                threadCount = filters.size();
            for (int i = 0; i < threadCount; ++i) {
                QThread *thread = new QThread(this);
                threads.append(thread);
                thread->start();
            }
            // Move filters to threads
            for (unsigned i = 0; i < unsigned(filters.size()); ++i)
                filters[i]->moveToThread(threads[i % threads.size()]);
        }
    } catch (const std::exception &e) {
        return tr("Unable to instantiate FIR filters: %1")
            .arg(QString::fromLocal8Bit(e.what()));
    }

    if (unsigned(filters.size()) != unsigned(inPorts.size()))
        return tr("Number of channels differ: plugin has %1 channels"
                  ", FIR filter %2")
            .arg(inPorts.size())
            .arg(filters.size());

    return QString();
}

void ThreadedFirFilterPlugin::process()
{
    const unsigned ports =
        qMin(unsigned(inPorts.size()), unsigned(outPorts.size()));

    if (disabled) {
        for (unsigned i = 0; i < ports; ++i) {
            auto inputRef = inPorts[i]->inputData.toStrongRef();
            if (!inputRef)
                memset(outPorts[i]->outputData.data(), 0,
                       blockSize * sizeof(float));
            else
                memcpy(outPorts[i]->outputData.data(), inputRef.data(),
                       blockSize * sizeof(float));
        }
        return;
    }

    QVector<float *> floatData(inPorts.size());
    for (unsigned i = 0; i < unsigned(ports); ++i) {
        auto inputRef = inPorts[i]->inputData.toStrongRef();
        if (!inputRef)
            memset(outPorts[i]->outputData.data(), 0,
                   blockSize * sizeof(float));
        else
            memcpy(outPorts[i]->outputData.data(), inputRef.data(),
                   blockSize * sizeof(float));
        floatData.data()[i] = outPorts[i]->outputData.data();
    }

    QList<QVector<double>> doubleArrays;
    QVector<double *> doubleData;
    for (unsigned i = 0; i < ports; ++i) {
        doubleArrays.append(QVector<double>(blockSize));
        doubleData.append(doubleArrays[i].data());
    }

    floatToDouble(floatData.data(), ports, blockSize, doubleData.data());

    for (unsigned port = 1; port < ports; ++port)
        memcpy(doubleData.data()[port], doubleData.data()[0],
               blockSize * sizeof(double));

    for (unsigned port = 0; port < ports; ++port)
        QMetaObject::invokeMethod(
            filters[port], "process", Q_ARG(double *, doubleData.data()[port]),
            Q_ARG(unsigned int, blockSize), Q_ARG(QSemaphore *, &semaphore));
    semaphore.acquire(ports);

    doubleToFloat(doubleData.data(), ports, blockSize, floatData.data());
}
QList<BlockInputPort *> ThreadedFirFilterPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> ThreadedFirFilterPlugin::outputPorts()
{
    return outPorts;
}

void ThreadedFirFilterPlugin::release()
{
}

bool ThreadedFirFilterPlugin::getDisabled() const
{
    return disabled;
}

void ThreadedFirFilterPlugin::setDisabled(bool value)
{
    disabled = value;
    parameters.insert(QLatin1String("disabled"), QString(value));
}

QString ThreadedFirFilterPlugin::getFilePath() const
{
    return filePath;
}

void ThreadedFirFilterPlugin::setFilePath(QString value)
{
    if (QDir(value).isAbsolute())
        newFilePath = value;
    else
        newFilePath = QDir(workDirectory).absoluteFilePath(value);
    parameters.insert(QString::fromLatin1("uri"), newFilePath);
}

unsigned ThreadedFirFilterPlugin::getLimit() const
{
    return limit;
}

void ThreadedFirFilterPlugin::setLimit(unsigned value)
{
    limit = value;
    parameters.insert(QLatin1String("limit"), QString(value));
}

bool ThreadedFirFilterPlugin::getBuffered() const
{
    return buffered;
}

void ThreadedFirFilterPlugin::setBuffered(bool value)
{
    buffered = value;
    parameters.insert(QLatin1String("buffered"), QString(value));
}

unsigned ThreadedFirFilterPlugin::getMaxThreads() const
{
    return maxThreads;
}

void ThreadedFirFilterPlugin::setMaxThreads(unsigned value)
{
    maxThreads = value;
    parameters.insert(QLatin1String("threads"), QString(value));
}

// ThreadedFirFilterCreator ====================================================

QList<BlockDescription> ThreadedFirFilterCreator::availableBlocks() const
{
    return QList<BlockDescription>()
           << &ThreadedFirFilterPlugin::staticMetaObject;
}

#include "threadedfirfilterplugin.moc"
