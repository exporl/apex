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

#include "apexmain/filter/pluginfilterinterface.h"

#include "apextools/global.h"

#include "common/random.h"

#include <sndfile.h>

#include <QDir>
#include <QFile>
#include <QMap>

class DataLoopGeneratorCreator : public QObject, public PluginFilterCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginFilterCreator)
    Q_PLUGIN_METADATA(IID "apex.dataloopgenerator")
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFilterInterface *createFilter(const QString &name,
                                                unsigned channels,
                                                unsigned blocksize,
                                                unsigned fs) const;
};

class DataLoopGenerator : public QObject, public PluginFilterInterface
{
    Q_OBJECT
public:
    DataLoopGenerator(unsigned channels, unsigned blockSize,
                      unsigned sampleRate);
    ~DataLoopGenerator();

    virtual void resetParameters();
    virtual bool isValidParameter(const QString &type, int channel) const;
    virtual bool setParameter(const QString &type, int channel,
                              const QString &value);

    virtual bool prepare(unsigned numberOfFrames);
    virtual void process(double *const *data);

private:
    bool parseDouble(double &result, const QString &value, const QString &type);
    unsigned &getPosition();

    const unsigned channels;
    const unsigned blockSize;
    const unsigned sampleRate;

    QString id;
    QString workDirectory;
    QString fileName;
    SNDFILE *inputFile;
    unsigned totalFrames;
    double jump;
    bool randomJump;
    double baseGain;
    double gain;
    unsigned loops;
    bool deterministic;
};

static QMap<QString, unsigned> positions;

// DataLoopGenerator
// ==============================================================

DataLoopGenerator::DataLoopGenerator(unsigned channels, unsigned blockSize,
                                     unsigned sampleRate)
    : channels(channels),
      blockSize(blockSize),
      sampleRate(sampleRate),
      workDirectory(QDir::currentPath()),
      inputFile(Q_NULLPTR)
{
    resetParameters();
}

DataLoopGenerator::~DataLoopGenerator()
{
    if (inputFile)
        sf_close(inputFile);
}

void DataLoopGenerator::resetParameters()
{
    fileName.clear();
    if (inputFile)
        sf_close(inputFile);
    inputFile = Q_NULLPTR;
    totalFrames = 0;
    jump = 0.0;
    randomJump = false;
    baseGain = 0.0;
    gain = 0.0;
    loops = 1;
    deterministic = false;
}

bool DataLoopGenerator::isValidParameter(const QString &type, int channel) const
{
    if (type == QL1S("fileName") && channel == -1)
        return true;
    if (type == QL1S("jump") && channel == -1)
        return true;
    if (type == QL1S("randomjump") && channel == -1)
        return true;
    if (type == QL1S("basegain") && channel == -1)
        return true;
    if (type == QL1S("gain") && channel == -1)
        return true;
    if (type == QL1S("loops") && channel == -1)
        return true;
    if (type == QL1S("deterministic") && channel == -1)
        return true;
    if (type == QL1S("id") && channel == -1)
        return true;

    setErrorMessage(
        QString::fromLatin1("Unknown parameter %2 or invalid channel %1")
            .arg(channel)
            .arg(type));
    return false;
}

bool DataLoopGenerator::setParameter(const QString &type, int channel,
                                     const QString &value)
{
    if (!isValidParameter(type, channel))
        return false;

    if (type == QL1S("fileName")) {
        if (QDir(value).isAbsolute())
            fileName = value;
        else
            fileName = QDir(workDirectory).absoluteFilePath(value);
        return true;
    }

    if (type == QL1S("jump")) {
        if (randomJump) {
            setErrorMessage(QString::fromLatin1(
                "Cannot random jump and fixed jump at the same time."));
            return false;
        }
        return parseDouble(jump, value, type);
    }

    if (type == QL1S("randomjump")) {
        if (value == QL1S("true")) {
            if (jump) {
                setErrorMessage(QString::fromLatin1(
                    "Cannot random jump and fixed jump at the same time."));
                return false;
            }
            randomJump = true;
            return true;
        }
        if (value == QL1S("false")) {
            randomJump = false;
            return false;
        }
        setErrorMessage(
            QString::fromLatin1("Couldn't parse value %1 for parameter %2")
                .arg(value)
                .arg(type));
    }

    if (type == QL1S("basegain"))
        return parseDouble(baseGain, value, type);

    if (type == QL1S("gain"))
        return parseDouble(gain, value, type);

    if (type == QL1S("loops")) {
        bool ok;
        loops = value.toUInt(&ok);
        if (!ok) {
            setErrorMessage(
                QString::fromLatin1("Unable to convert loops value %1")
                    .arg(value));
            return false;
        }
        return true;
    }

    if (type == QL1S("id")) {
        id = value;
        return true;
    }

    if (type == QL1S("deterministic")) {
        deterministic = value == QL1S("true");
        return true;
    }

    return false;
}

bool DataLoopGenerator::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);

    if (!QFile::exists(fileName)) {
        setErrorMessage(
            QString::fromLatin1("File %1 does not exist").arg(fileName));
        return false;
    }

    QByteArray filePath = QFile::encodeName(fileName);
    SF_INFO readSfinfo;
    readSfinfo.format = 0;
    inputFile = sf_open(filePath, SFM_READ, &readSfinfo);
    totalFrames = readSfinfo.frames;
    if (!inputFile) {
        setErrorMessage(QString::fromLatin1("Unable to open input file %1: %2")
                            .arg(fileName)
                            .arg(QString::fromLocal8Bit(sf_strerror(NULL))));
        return false;
    }
    if ((unsigned)readSfinfo.samplerate != sampleRate) {
        setErrorMessage(
            QString::fromLatin1(
                "Samplerate %1 doesn't match input file's samplerate %2")
                .arg(QString::number(readSfinfo.samplerate))
                .arg(QString::number(sampleRate)));
        return false;
    }
    if ((unsigned)readSfinfo.channels != channels) {
        setErrorMessage(
            QString::fromLatin1(
                "Channels %1 doesn't match input file's channels %2")
                .arg(QString::number(readSfinfo.channels))
                .arg(QString::number(channels)));
        return false;
    }

    bool ran = positions.contains(id);
    if (jump && !ran) {
        if (jump > (totalFrames / sampleRate)) {
            setErrorMessage(
                QString::fromLatin1("Cannot jump past end of file"));
            return false;
        }
        getPosition() = jump * sampleRate;
        sf_seek(inputFile, getPosition(), SEEK_SET);
    } else if (randomJump && !ran) {
        if (deterministic)
            getPosition() = cmn::Random(0).nextUInt(totalFrames);
        else
            getPosition() = cmn::Random().nextUInt(totalFrames);
        sf_seek(inputFile, getPosition(), SEEK_SET);
    } else if (ran) {
        sf_seek(inputFile, getPosition(), SEEK_SET);
    }

    return true;
}

void DataLoopGenerator::process(double *const *data)
{
    QVector<float> scratch(channels * blockSize);
    unsigned blockPos = 0;
    do {
        unsigned readSize =
            sf_readf_float(inputFile, scratch.data(), blockSize - blockPos);
        const double gainMult = std::pow(10.0, (gain + baseGain) / 20);
        for (unsigned j = 0; j < unsigned(channels); ++j) {
            for (unsigned i = 0; i < readSize; ++i)
                data[j][blockPos + i] = gainMult * scratch[i * channels + j];
        }
        blockPos += readSize;
        getPosition() += readSize;

        if (blockPos < blockSize) {
            getPosition() = 0;
            sf_seek(inputFile, getPosition(), SEEK_SET);
        }
    } while (blockPos != blockSize);
}

bool DataLoopGenerator::parseDouble(double &result, const QString &value,
                                    const QString &type)
{
    bool ok;
    double parsedValue = value.toDouble(&ok);
    if (ok)
        result = parsedValue;
    else
        setErrorMessage(
            QString::fromLatin1("Unable to parse value %1 for parameter %2")
                .arg(value)
                .arg(type));
    return ok;
}

unsigned &DataLoopGenerator::getPosition()
{
    return positions[id];
}

// DataLoopGeneratorCreator ====================================================

QStringList DataLoopGeneratorCreator::availablePlugins() const
{
    return QStringList() << QLatin1String("dataloop");
}

PluginFilterInterface *
DataLoopGeneratorCreator::createFilter(const QString &name, unsigned channels,
                                       unsigned size, unsigned sampleRate) const
{
    Q_UNUSED(sampleRate);

    if (name == QLatin1String("dataloop"))
        return new DataLoopGenerator(channels, size, sampleRate);

    return NULL;
}

#include "dataloopgenerator.moc"
