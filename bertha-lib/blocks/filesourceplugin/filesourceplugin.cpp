/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Hans Moorkens.                               *
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

#include "bertha/blockinterface.h"

#include "common/sndfilewrapper.h"

#include <QDir>
#include <QFile>
#include <QSharedPointer>
#include <QVariant>
#include <QVector>

class FileSourceCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "filesource")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(fileSourcePlugin, FileSourceCreator)
Q_PLUGIN_METADATA(fileSourcePlugin)
Q_PLUGIN_METADATA(FileSourceCreator)

class FileSourcePlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(QString fileName WRITE setFileName READ getFileName)
    Q_PROPERTY(bool enabled WRITE setEnabled READ getEnabled)
    Q_PROPERTY(bool loop WRITE setLoop READ getLoop)
    Q_PROPERTY(bool zeroPadLoop WRITE setZeroPadLoop READ getZeroPadLoop)
    Q_CLASSINFO("fileName-description", Q_TR_NOOP("Filename as a String"))
    Q_CLASSINFO("enabled-description",
                Q_TR_NOOP("If not enabled, this block will output zeroes"))
    Q_CLASSINFO(
        "loop-description",
        Q_TR_NOOP(
            "If loop is true, the file will be re-read after it's finished"))
    Q_CLASSINFO("zeroPadLoop-description",
                Q_TR_NOOP("When looping and zeroPadLoop is true, if the number "
                          "of samples in the file "
                          "is not a multiple of the block size, the block "
                          "containing the end of the file"
                          "will be zero padded"))
public:
    Q_INVOKABLE FileSourcePlugin(unsigned blockSize, unsigned sampleRate,
                                 int inputPorts, int outputPorts);
    virtual ~FileSourcePlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    QString getFileName() const;
    void setFileName(const QString &value);

    bool getEnabled() const;
    void setEnabled(bool value);

    bool getLoop() const;
    void setLoop(bool value);

    bool getZeroPadLoop() const;
    void setZeroPadLoop(bool value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;
    unsigned sampleRate;

    QString fileName;
    QString oldFileName;
    bool enabled;
    bool loop;
    bool zeroPadLoop;
    cmn::SndFile inputFile;
    QString workDirectory;
};

// FileSourcePlugin ============================================================

FileSourcePlugin::FileSourcePlugin(unsigned blockSize, unsigned sampleRate,
                                   int inputPorts, int outputPorts)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      fileName(QLatin1String("input.wav")),
      enabled(true),
      loop(false),
      zeroPadLoop(false)
{
    if (inputPorts != 0)
        inputPorts = 0;

    if (outputPorts < 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(outputPorts); ++i) {
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }
    workDirectory = QDir::currentPath();
}

FileSourcePlugin::~FileSourcePlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString FileSourcePlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    if (fileName != oldFileName)
        inputFile.reset();

    if (!inputFile) {
        oldFileName = fileName;
        QByteArray filePath = QFile::encodeName(oldFileName);

        SF_INFO readSfinfo;
        readSfinfo.format = 0;
        inputFile.reset(oldFileName, SFM_READ, &readSfinfo);
        if (!inputFile)
            return tr("Unable to open input file %1: %2")
                .arg(QFile::decodeName(filePath),
                     QString::fromLocal8Bit(sf_strerror(NULL)));
        if (unsigned(readSfinfo.samplerate) != sampleRate)
            return tr("Source file %1 has wrong sample rate (%2)")
                .arg(QFile::decodeName(filePath))
                .arg(readSfinfo.samplerate);
        if (unsigned(readSfinfo.channels) != unsigned(outPorts.size()))
            return tr("Source file %1 has wrong number of "
                      "channels")
                .arg(QFile::decodeName(filePath));
    }
    return QString();
}

void FileSourcePlugin::process()
{
    for (unsigned i = 0; i < unsigned(outPorts.size()); ++i)
        memset(outPorts[i]->outputData.data(), 0,
               blockSize * sizeof(PortDataType));

    if (!enabled)
        return;

    QVector<float> scratch(outPorts.size() * blockSize);
    unsigned blockPos = 0;
    do {
        const unsigned readSize = sf_readf_float(
            inputFile.get(), scratch.data(), blockSize - blockPos);

        for (unsigned j = 0; j < unsigned(outPorts.size()); ++j) {
            auto output = outPorts[j]->outputData.data();
            for (unsigned i = 0; i < readSize; ++i)
                output[blockPos + i] = scratch[i * outPorts.size() + j];
        }
        blockPos += readSize;

        if (blockPos < blockSize && loop)
            sf_seek(inputFile.get(), 0, SEEK_SET);
    } while (loop && blockPos != blockSize && (!zeroPadLoop || blockPos == 0));
}

QList<BlockInputPort *> FileSourcePlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> FileSourcePlugin::outputPorts()
{
    return outPorts;
}

void FileSourcePlugin::release()
{
}

QString FileSourcePlugin::getFileName() const
{
    return fileName;
}

void FileSourcePlugin::setFileName(const QString &value)
{
    if (QDir(value).isAbsolute()) {
        fileName = value;
    } else {
        fileName = QDir(workDirectory).absoluteFilePath(value);
    }
}

bool FileSourcePlugin::getEnabled() const
{
    return enabled;
}

void FileSourcePlugin::setEnabled(bool value)
{
    enabled = value;
}

bool FileSourcePlugin::getLoop() const
{
    return loop;
}

void FileSourcePlugin::setLoop(bool value)
{
    loop = value;
}

bool FileSourcePlugin::getZeroPadLoop() const
{
    return zeroPadLoop;
}

void FileSourcePlugin::setZeroPadLoop(bool value)
{
    zeroPadLoop = value;
}

// FileSourceCreator ===========================================================

QList<BlockDescription> FileSourceCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &FileSourcePlugin::staticMetaObject;
}

#include "filesourceplugin.moc"
