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

#include "common/sndfilewrapper.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QStringList>
#include <QVector>

class FileSinkCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "filesink")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2 (fileSink, FileSinkCreator)
Q_PLUGIN_METADATA(fileSink)
Q_PLUGIN_METADATA(FileSinkCreator)

class FileSinkPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(QString filePath WRITE setFilePath READ getFilePath)
    Q_PROPERTY(QString format WRITE setFormat READ getFormat)
    Q_CLASSINFO("format-description", Q_TR_NOOP("format as string"))
    Q_CLASSINFO("filePath-description", Q_TR_NOOP("filepath as string"))
public:
    Q_INVOKABLE FileSinkPlugin(unsigned blockSize, unsigned sampleRate,
                               int inputPorts, int outputPorts);
    virtual ~FileSinkPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output, unsigned index);

    QString getFilePath() const;
    void setFilePath(QString value);

    QString getFormat() const;
    void setFormat(QString value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;
    unsigned sampleRate;

    unsigned count;
    cmn::SndFile file;
    QString filePath;
    QString format;
    QMap<QString, QString> parameters;
    QMap<QString, int> formats;
    QString workDirectory;

    QVector<PortDataType> scratch;
};

// FileSinkPlugin ==============================================================

FileSinkPlugin::FileSinkPlugin(unsigned blockSize, unsigned sampleRate,
                               int inputPorts, int outputPorts)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      count(0),
      format(QLatin1String("float"))
{
    formats.insert(QLatin1String("pcm8"), SF_FORMAT_PCM_S8);
    formats.insert(QLatin1String("pcm16"), SF_FORMAT_PCM_16);
    formats.insert(QLatin1String("pcm24"), SF_FORMAT_PCM_24);
    formats.insert(QLatin1String("pcm32"), SF_FORMAT_PCM_32);
    formats.insert(QLatin1String("float"), SF_FORMAT_FLOAT);
    formats.insert(QLatin1String("double"), SF_FORMAT_DOUBLE);

    if (inputPorts < 0)
        inputPorts = 1;

    outputPorts = inputPorts;

    for (unsigned i = 0; i < unsigned(inputPorts); ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("input-%1").arg(i + 1)));
    }
    for (unsigned i = 0; i < unsigned(outputPorts); ++i) {
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }
    workDirectory = QDir::currentPath();

    scratch.resize(inPorts.size() * blockSize);
}

FileSinkPlugin::~FileSinkPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString FileSinkPlugin::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);

    QString filePath(this->filePath);
    QMapIterator<QString, QString> i(parameters);
    while (i.hasNext()) {
        i.next();
        filePath.replace(QString::fromLatin1("${") + i.key() +
                             QString::fromLatin1("}"),
                         i.value());
    }
    filePath.replace(QString::fromLatin1("${count}"), QString::number(count));
    filePath.replace(QString::fromLatin1("${timestamp}"),
                     QString::number(QDateTime::currentDateTime().toTime_t()));

    SF_INFO fileInfo;
    fileInfo.format = SF_FORMAT_WAV;
    if (!formats.contains(format))
        return tr("Unknown file format %1").arg(format);
    fileInfo.format |= formats.value(format);
    fileInfo.channels = inPorts.size();
    fileInfo.samplerate = sampleRate;
    file.reset(filePath, SFM_WRITE, &fileInfo);
    if (!file)
        return tr("Unable to open wave file %1 "
                  "for writing")
            .arg(filePath);

    ++count;

    return QString();
}

void FileSinkPlugin::processOne(PortDataType *input, PortDataType *output,
                                unsigned index)
{
    if (!input) {
        memset(output, 0, blockSize * sizeof(PortDataType));
    } else {
        memcpy(output, input, blockSize * sizeof(PortDataType));
    }
    for (unsigned i = 0; i < blockSize; ++i)
        scratch[i * inPorts.size() + index] = output[i];
}

void FileSinkPlugin::process()
{
    for (unsigned i = 0; i < unsigned(inPorts.size()); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data(), i);
    }

    sf_writef_float(file.get(), scratch.data(), blockSize);
}

QList<BlockInputPort *> FileSinkPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> FileSinkPlugin::outputPorts()
{
    return outPorts;
}

void FileSinkPlugin::release()
{
    file.reset();
}

QString FileSinkPlugin::getFilePath() const
{
    return filePath;
}

void FileSinkPlugin::setFilePath(QString value)
{
    if (QDir(value).isAbsolute())
        filePath = value;
    else
        filePath = QDir(workDirectory).absoluteFilePath(value);
    parameters.insert(QString::fromLatin1("uri"), filePath);
}

QString FileSinkPlugin::getFormat() const
{
    return format;
}

void FileSinkPlugin::setFormat(QString value)
{
    format = value;
    parameters.insert(QLatin1String("format"), value);
}

// FileSinkCreator =============================================================

QList<BlockDescription> FileSinkCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &FileSinkPlugin::staticMetaObject;
}

#include "filesinkplugin.moc"
