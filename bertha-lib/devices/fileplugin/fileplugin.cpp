/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Maarten Lambert.                               *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
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

#include "bertha/deviceinterface.h"
#include "bertha/global.h"

#include "common/sndfilewrapper.h"

#include <QCoreApplication>
#include <QFile>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include <QVector>

#include <QDebug>

class TimerThread : public QThread
{
public:
    TimerThread(QObject *receiver, const char *slot, QObject *parent = NULL)
        : QThread(parent)
    {
        connect(&timer, SIGNAL(timeout()), receiver, slot,
                Qt::DirectConnection);
        timer.moveToThread(this);
    }

protected:
    void run()
    {
        timer.start();
        exec();
        timer.stop();
    }

private:
    QTimer timer;
};

class FilePlugin : public QObject, public DeviceInterface
{
    Q_OBJECT
    Q_INTERFACES(DeviceInterface)
    Q_PROPERTY(QString inputFile READ getInputFile WRITE setInputFile)
    Q_PROPERTY(QString outputFile READ getOutputFile WRITE setOutputFile)
public:
    Q_INVOKABLE FilePlugin(unsigned blockSize, unsigned sampleRate,
                           int inputPorts, int outputPorts);
    virtual ~FilePlugin();

    virtual QList<BlockOutputPort *> outputPorts() Q_DECL_OVERRIDE;
    virtual QList<BlockInputPort *> inputPorts() Q_DECL_OVERRIDE;

    virtual void setCallback(Callback function, void *data) Q_DECL_OVERRIDE;

    virtual QString prepare() Q_DECL_OVERRIDE;
    virtual QString start() Q_DECL_OVERRIDE;
    virtual void stop() Q_DECL_OVERRIDE;
    virtual void release() Q_DECL_OVERRIDE;

    virtual bool processOutput() Q_DECL_OVERRIDE;
    virtual bool processInput() Q_DECL_OVERRIDE;

    void processOneOutput(PortDataType *output, unsigned index);
    void processOneInput(PortDataType *input, unsigned index);

public Q_SLOTS:
    void setInputFile(const QString &filePath);
    QString getInputFile() const;

    void setOutputFile(const QString &filePath);
    QString getOutputFile() const;

private Q_SLOTS:
    void timeout();

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    const unsigned blockSize;
    const unsigned sampleRate;
    const unsigned inputPortCount;
    const unsigned outputPortCount;

    QString inputFilePath;
    QString outputFilePath;

    Callback callback;
    void *callbackData;

    cmn::SndFile inputFile;
    cmn::SndFile outputFile;
    sf_count_t inputSize;

    TimerThread thread;

    QVector<PortDataType> interleavedInput;
    QVector<PortDataType> interleavedOutput;
};

class FileCreator : public QObject, public DeviceCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(DeviceCreatorInterface)
    Q_PLUGIN_METADATA(IID "file")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(file, FileCreator)
Q_PLUGIN_METADATA(file)
Q_PLUGIN_METADATA(FileCreator)

// FilePlugin ==================================================================

FilePlugin::FilePlugin(unsigned blockSize, unsigned sampleRate, int inputPorts,
                       int outputPorts)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      inputPortCount(qMax(0, inputPorts)),
      outputPortCount(qMax(0, outputPorts)),
      inputFilePath(QLatin1String("input.wav")),
      outputFilePath(QLatin1String("output.wav")),
      callback(NULL),
      callbackData(NULL),
      thread(this, SLOT(timeout()))
{
    for (unsigned i = 0; i < inputPortCount; ++i)
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("playback-%1").arg(i + 1)));
    for (unsigned i = 0; i < outputPortCount; ++i)
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("capture-%1").arg(i + 1)));

    interleavedInput.resize(inputPortCount * blockSize);
    interleavedOutput.resize(outputPortCount * blockSize);
}

FilePlugin::~FilePlugin()
{
    stop();

    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString FilePlugin::prepare()
{
    if (outputPortCount > 0) {
        QByteArray fileName = QFile::encodeName(inputFilePath);

        SF_INFO readSfinfo;
        readSfinfo.format = 0;
        inputFile.reset(inputFilePath, SFM_READ, &readSfinfo);
        if (!inputFile)
            return tr("Unable to open input file %1: %2")
                .arg(QFile::decodeName(fileName),
                     QString::fromLocal8Bit(sf_strerror(NULL)));
        if (unsigned(readSfinfo.samplerate) != sampleRate)
            return tr("Input file %1 has wrong sample rate")
                .arg(QFile::decodeName(fileName));
        if (unsigned(readSfinfo.channels) != outputPortCount)
            return tr("Input file %1 has wrong number of "
                      "channels")
                .arg(QFile::decodeName(fileName));

        inputSize = sf_seek(inputFile.get(), 0, SEEK_END);
        sf_seek(inputFile.get(), 0, SEEK_SET);
    } else {
        inputFile.reset();
        inputSize = 0;
    }

    if (inputPortCount > 0) {
        QByteArray fileName = QFile::encodeName(outputFilePath);

        SF_INFO writeSfinfo;
        writeSfinfo.samplerate = sampleRate;
        writeSfinfo.channels = inputPortCount;
        writeSfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
        outputFile.reset(outputFilePath, SFM_WRITE, &writeSfinfo);
        if (!outputFile)
            return tr("Unable to open output file %1: %2")
                .arg(QFile::decodeName(fileName),
                     QString::fromLocal8Bit(sf_strerror(NULL)));
    } else {
        outputFile.reset();
    }

    return QString();
}

QString FilePlugin::start()
{
    thread.start();
    return QString();
}

void FilePlugin::stop()
{
    thread.quit();
    if (QThread::currentThread() != &thread)
        thread.wait();
}

void FilePlugin::release()
{
    inputFile.reset();
    outputFile.reset();
}

void FilePlugin::processOneOutput(PortDataType *output, unsigned index)
{
    for (unsigned i = 0; i < blockSize; ++i)
        output[i] = interleavedOutput[i * outputPortCount + index];
}

bool FilePlugin::processOutput()
{
    if (outPorts.size() == 0)
        return true;

    const unsigned readCount =
        sf_readf_float(inputFile.get(), interleavedOutput.data(), blockSize);
    std::fill(interleavedOutput.data() + readCount * outputPortCount,
              interleavedOutput.data() + blockSize * outputPortCount, 0);
    for (unsigned j = 0; j < outputPortCount; ++j)
        processOneOutput(outPorts[j]->outputData.data(), j);

    return inputSize != sf_seek(inputFile.get(), 0, SEEK_CUR);
}

void FilePlugin::processOneInput(PortDataType *input, unsigned index)
{
    for (unsigned i = 0; i < blockSize; ++i)
        interleavedInput[i * inputPortCount + index] = input ? input[i] : 0.0f;
}

bool FilePlugin::processInput()
{
    if (inputPortCount == 0)
        return true;

    for (unsigned j = 0; j < inputPortCount; ++j)
        processOneInput(inPorts[j]->inputData.toStrongRef().data(), j);

    sf_writef_float(outputFile.get(), interleavedInput.data(), blockSize);

    return true;
}

void FilePlugin::timeout()
{
    callback(callbackData);
}

QList<BlockOutputPort *> FilePlugin::outputPorts()
{
    return outPorts;
}

QList<BlockInputPort *> FilePlugin::inputPorts()
{
    return inPorts;
}

void FilePlugin::setCallback(Callback function, void *data)
{
    callback = function;
    callbackData = data;
}

void FilePlugin::setInputFile(const QString &filePath)
{
    inputFilePath = filePath;
}

QString FilePlugin::getInputFile() const
{
    return inputFilePath;
}

void FilePlugin::setOutputFile(const QString &filePath)
{
    outputFilePath = filePath;
}

QString FilePlugin::getOutputFile() const
{
    return outputFilePath;
}

// FileCreator =================================================================

QList<BlockDescription> FileCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &FilePlugin::staticMetaObject;
}

#include "fileplugin.moc"
