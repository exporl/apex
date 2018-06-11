/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sanne.raymaekers@gmail.com>           *
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
#include "bertha/global.h"

#include "common/sndfilewrapper.h"

#include <QDir>
#include <QFile>
#include <QList>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>

class ApexCompatibleDataBlockPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)

    Q_PROPERTY(QString fileName WRITE setFileName READ getFileName)
    Q_CLASSINFO("fileName-description", Q_TR_NOOP("Filename as a String"))

    Q_PROPERTY(QVariantList triggers WRITE setTriggers READ getTriggers)
    Q_CLASSINFO(
        "triggers-description",
        Q_TR_NOOP(
            "Points at which the audio should restart. Specified "
            "in number of samples. This should be a multiple of blocksize"))

    Q_PROPERTY(unsigned loops WRITE setLoops READ getLoops)
    Q_CLASSINFO("loop-description",
                Q_TR_NOOP("Amount of times the block should loop"))

    Q_PROPERTY(bool donePlaying READ getDonePlaying)
    Q_CLASSINFO("donePlaying-description",
                Q_TR_NOOP("Indicates if the datablock has finished playing"))

public:
    Q_INVOKABLE ApexCompatibleDataBlockPlugin(unsigned blockSize,
                                              unsigned sampleRate,
                                              int inputPorts, int outputPorts);
    virtual ~ApexCompatibleDataBlockPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    Q_INVOKABLE void reset();

    QString getFileName() const;
    void setFileName(const QString &value);

    QVariantList getTriggers() const;
    void setTriggers(const QVariantList &values);

    unsigned getLoops() const;
    void setLoops(unsigned value);

    bool getDonePlaying() const;

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;
    unsigned sampleRate;
    unsigned sampleFrames;

    QList<unsigned> triggers;
    unsigned totalProcessed;
    unsigned totalFrames;
    unsigned currentTrigger;
    unsigned loops;

    QString fileName;
    QString oldFileName;
    cmn::SndFile inputFile;
    QString workDirectory;
};

class ApexCompatibleDataBlockCreator : public QObject,
                                       public BlockCreatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "apexcompatibledatablock")
    Q_INTERFACES(BlockCreatorInterface)
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// ApexCompatibleDataBlockPlugin ===============================================

ApexCompatibleDataBlockPlugin::ApexCompatibleDataBlockPlugin(
    unsigned blockSize, unsigned sampleRate, int inputPorts, int outputPorts)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      triggers(QList<unsigned>() << 0),
      loops(1),
      workDirectory(QDir::currentPath())
{
    if (inputPorts != 0)
        inputPorts = 0;

    if (outputPorts <= 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(outputPorts); ++i)
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
}

ApexCompatibleDataBlockPlugin::~ApexCompatibleDataBlockPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QList<BlockInputPort *> ApexCompatibleDataBlockPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> ApexCompatibleDataBlockPlugin::outputPorts()
{
    return outPorts;
}

QString ApexCompatibleDataBlockPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    totalProcessed = 0;
    currentTrigger = 0;

    if (fileName != oldFileName)
        inputFile.reset();

    if (!inputFile && !fileName.startsWith(QSL("silence:"))) {
        oldFileName = fileName;

        SF_INFO readSfinfo;
        readSfinfo.format = 0;
        inputFile.reset(oldFileName, SFM_READ, &readSfinfo);
        sampleFrames = readSfinfo.frames;
        totalFrames = (triggers.last() + sampleFrames);
        if (!inputFile)
            return tr("Unable to open input file %1: %2")
                .arg(oldFileName, QString::fromLocal8Bit(sf_strerror(NULL)));
        if (unsigned(readSfinfo.samplerate) != sampleRate)
            return tr("Source file %1 has wrong sample rate (%2)")
                .arg(oldFileName)
                .arg(readSfinfo.samplerate);
        if (unsigned(readSfinfo.channels) < unsigned(outPorts.size()))
            return tr("Source file %1 doesn't have enough channels to fill the "
                      "outputports")
                .arg(oldFileName);
    } else if (fileName.startsWith(QSL("silence:"))) {
        bool ok;
        double length = fileName.split(QSL(":")).last().toDouble(&ok);
        if (!ok)
            return tr("Unable to determine silence length");
        totalFrames = triggers.last() + ((length / 1000.0) * sampleRate);
    }

    if (loops > 1) {
        QList<unsigned> newTriggers = triggers;
        for (unsigned i = 1; i < loops; ++i)
            Q_FOREACH (const unsigned &trigger, triggers)
                newTriggers.append(trigger + totalFrames * i);
        triggers = newTriggers;
        totalFrames = totalFrames * loops;
    }

    return QString();
}

void ApexCompatibleDataBlockPlugin::process()
{
    for (unsigned i = 0; i < unsigned(outPorts.size()); ++i)
        memset(outPorts[i]->outputData.data(), 0,
               blockSize * sizeof(PortDataType));

    if (fileName.startsWith(QSL("silence:"))) {
        totalProcessed += blockSize;
        return;
    }
    bool reachedFirstTrigger =
        triggers.isEmpty()
            ? true
            : (totalProcessed + blockSize) >= triggers.first() ? true : false;
    if (!reachedFirstTrigger) {
        totalProcessed += blockSize;
        return;
    }

    QVector<float> scratch(outPorts.size() * blockSize);
    unsigned blockPos = 0;
    while (blockPos != blockSize) {
        if (!triggers.isEmpty() &&
            totalProcessed < triggers.at(currentTrigger)) {
            unsigned wait =
                (triggers.at(currentTrigger) - totalProcessed - blockPos) >
                        blockSize
                    ? blockSize - blockPos
                    : (triggers.at(currentTrigger) - totalProcessed - blockPos);
            blockPos += wait;
            totalProcessed += wait;
            continue;
        }

        const unsigned readSize = sf_readf_float(
            inputFile.get(), scratch.data(), blockSize - blockPos);
        for (unsigned j = 0; j < unsigned(outPorts.size()); ++j) {
            auto output = outPorts[j]->outputData.data();
            for (unsigned i = 0; i < readSize; ++i)
                output[blockPos + i] = scratch[i * outPorts.size() + j];
        }

        blockPos += readSize;
        totalProcessed += readSize;

        if (currentTrigger + 1 < (unsigned)triggers.size() &&
            (totalProcessed >= triggers.at(currentTrigger) + sampleFrames ||
             totalProcessed >= triggers.at(currentTrigger + 1))) {
            currentTrigger += 1;
            sf_seek(inputFile.get(), 0, SEEK_SET);
        } else if (getDonePlaying()) {
            return;
        }
    }
}

void ApexCompatibleDataBlockPlugin::release()
{
}

void ApexCompatibleDataBlockPlugin::reset()
{
    totalProcessed = 0;
    currentTrigger = 0;
    sf_seek(inputFile.get(), 0, SEEK_SET);
}

QString ApexCompatibleDataBlockPlugin::getFileName() const
{
    return fileName;
}

void ApexCompatibleDataBlockPlugin::setFileName(const QString &value)
{
    if (QDir(value).isAbsolute() || value.startsWith(QSL("silence:")))
        fileName = value;
    else
        fileName = QDir(workDirectory).absoluteFilePath(value);
}

QVariantList ApexCompatibleDataBlockPlugin::getTriggers() const
{
    QVariantList result;
    Q_FOREACH (unsigned reset, triggers)
        result.append(QVariant(reset));
    return result;
}

void ApexCompatibleDataBlockPlugin::setTriggers(const QVariantList &values)
{
    triggers.clear();
    Q_FOREACH (const QVariant &value, values)
        triggers.append(value.toUInt());
}

unsigned ApexCompatibleDataBlockPlugin::getLoops() const
{
    return loops;
}

void ApexCompatibleDataBlockPlugin::setLoops(unsigned value)
{
    loops = value;
}

bool ApexCompatibleDataBlockPlugin::getDonePlaying() const
{
    return totalProcessed >= totalFrames;
}

// ApexCompatibleDataBlockCreator ===========================================

QList<BlockDescription> ApexCompatibleDataBlockCreator::availableBlocks() const
{
    return QList<BlockDescription>()
           << &ApexCompatibleDataBlockPlugin::staticMetaObject;
}

#include "apexcompatibledatablockplugin.moc"
