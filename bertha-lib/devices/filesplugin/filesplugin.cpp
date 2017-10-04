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
#include <QDir>
#include <QFile>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include <QVector>

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

class FilesPlugin : public QObject, public DeviceInterface
{
    Q_OBJECT
    Q_INTERFACES(DeviceInterface)
    Q_PROPERTY(QStringList inputFiles READ getInputFiles WRITE setInputFiles)
    Q_PROPERTY(QStringList outputFiles READ getOutputFiles WRITE setOutputFiles)
public:
    Q_INVOKABLE FilesPlugin(unsigned blockSize, unsigned sampleRate,
                            int inputPorts, int outputPorts);
    virtual ~FilesPlugin();

    virtual QList<BlockOutputPort *> outputPorts();
    virtual QList<BlockInputPort *> inputPorts();

    virtual void setCallback(Callback function, void *data);

    virtual QString prepare();
    virtual QString start();
    virtual void stop();
    virtual void release();

    virtual bool processOutput();
    virtual bool processInput();

    void processOneInput(PortDataType *input, unsigned index);

public Q_SLOTS:
    void setInputFiles(const QStringList &filePaths);
    QStringList getInputFiles() const;

    void setOutputFiles(const QStringList &filePaths);
    QStringList getOutputFiles() const;

private Q_SLOTS:
    void timeout();

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    const unsigned blockSize;
    const unsigned sampleRate;
    const unsigned inputPortCount;
    const unsigned outputPortCount;

    QStringList inputFilePaths;
    QStringList outputFilePaths;

    Callback callback;
    void *callbackData;

    QList<cmn::SndFile> inputFiles;
    QList<cmn::SndFile> outputFiles;
    QList<sf_count_t> inputSizes;

    TimerThread thread;

    QString workDirectory;
};

class FilesCreator : public QObject, public DeviceCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(DeviceCreatorInterface)
    Q_PLUGIN_METADATA(IID "files")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(files, FilesCreator)
Q_PLUGIN_METADATA(files)
Q_PLUGIN_METADATA(FilesCreator)

// FilesPlugin =================================================================

FilesPlugin::FilesPlugin(unsigned blockSize, unsigned sampleRate,
                         int inputPorts, int outputPorts)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      inputPortCount(qMax(0, inputPorts)),
      outputPortCount(qMax(0, outputPorts)),
      callback(NULL),
      callbackData(NULL),
      thread(this, SLOT(timeout()))
{
    for (unsigned i = 0; i < inputPortCount; ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("playback-%1").arg(i + 1)));
        inputFilePaths.append(QString::fromLatin1("input-%1.wav").arg(i + 1));
    }
    for (unsigned i = 0; i < outputPortCount; ++i) {
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("capture-%1").arg(i + 1)));
        outputFilePaths.append(QString::fromLatin1("output-%1.wav").arg(i + 1));
    }
    workDirectory = QDir::currentPath();
}

FilesPlugin::~FilesPlugin()
{
    stop();

    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString FilesPlugin::prepare()
{
    inputFiles.clear();
    inputFiles.clear();
    outputFiles.clear();

    for (unsigned i = 0; i < outputPortCount; ++i) {
        QString inputFilePath;
        if (QDir(inputFilePaths.value(i)).isAbsolute())
            inputFilePath = inputFilePaths.value(i);
        else
            inputFilePath =
                QDir(workDirectory).absoluteFilePath(inputFilePaths.value(i));
        cmn::SndFile inputFile;
        sf_count_t inputSize = 0;

        if (!inputFilePath.isEmpty()) {
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
            if (readSfinfo.channels != 1)
                return tr("Input file %1 has wrong number of "
                          "channels")
                    .arg(QFile::decodeName(fileName));

            inputSize = sf_seek(inputFile.get(), 0, SEEK_END);
            sf_seek(inputFile.get(), 0, SEEK_SET);
        }

        inputFiles.append(inputFile);
        inputSizes.append(inputSize);
    }

    for (unsigned i = 0; i < inputPortCount; ++i) {
        QString outputFilePath;
        if (QDir(outputFilePaths.value(i)).isAbsolute())
            outputFilePath = outputFilePaths.value(i);
        else
            outputFilePath =
                QDir(workDirectory).absoluteFilePath(outputFilePaths.value(i));

        cmn::SndFile outputFile;

        if (!outputFilePath.isEmpty()) {
            QByteArray fileName = QFile::encodeName(outputFilePath);

            SF_INFO writeSfinfo;
            writeSfinfo.samplerate = sampleRate;
            writeSfinfo.channels = 1;
            writeSfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
            outputFile.reset(outputFilePath, SFM_WRITE, &writeSfinfo);
            if (!outputFile)
                return tr("Unable to open output file %1: %2")
                    .arg(QFile::decodeName(fileName),
                         QString::fromLocal8Bit(sf_strerror(NULL)));
        }

        outputFiles.append(outputFile);
    }
    return QString();
}

QString FilesPlugin::start()
{
    thread.start();
    return QString();
}

void FilesPlugin::stop()
{
    thread.quit();
    if (QThread::currentThread() != &thread)
        thread.wait();
}

void FilesPlugin::release()
{
    inputFiles.clear();
    outputFiles.clear();
}

bool FilesPlugin::processOutput()
{
    bool completed = false;

    for (unsigned i = 0; i < outputPortCount; ++i) {
        cmn::SndFile inputFile = inputFiles.value(i);
        float *data = outPorts[i]->outputData.data();
        unsigned readCount = 0;
        if (inputFile) {
            readCount = sf_readf_float(inputFile.get(), data, blockSize);
            completed |=
                inputSizes.value(i) == sf_seek(inputFile.get(), 0, SEEK_CUR);
        }
        qFill(data + readCount, data + blockSize, 0);
    }

    return !completed;
}

void FilesPlugin::processOneInput(PortDataType *input, unsigned index)
{
    cmn::SndFile outputFile = outputFiles.value(index);
    if (outputFile && input)
        sf_writef_float(outputFile.get(), input, blockSize);
}

bool FilesPlugin::processInput()
{
    for (unsigned i = 0; i < inputPortCount; ++i)
        processOneInput(inPorts[i]->inputData.toStrongRef().data(), i);

    return true;
}

void FilesPlugin::timeout()
{
    callback(callbackData);
}

QList<BlockOutputPort *> FilesPlugin::outputPorts()
{
    return outPorts;
}

QList<BlockInputPort *> FilesPlugin::inputPorts()
{
    return inPorts;
}

void FilesPlugin::setCallback(Callback function, void *data)
{
    callback = function;
    callbackData = data;
}

void FilesPlugin::setInputFiles(const QStringList &filePaths)
{
    inputFilePaths = filePaths;
}

QStringList FilesPlugin::getInputFiles() const
{
    return inputFilePaths;
}

void FilesPlugin::setOutputFiles(const QStringList &filePaths)
{
    outputFilePaths = filePaths;
}

QStringList FilesPlugin::getOutputFiles() const
{
    return outputFilePaths;
}

// FilesCreator ================================================================

QList<BlockDescription> FilesCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &FilesPlugin::staticMetaObject;
}

#include "filesplugin.moc"
