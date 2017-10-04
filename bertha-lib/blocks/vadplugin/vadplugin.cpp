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

#include "syllib/vad.h"

#include <QMap>
#include <QStringList>
#include <QVector>

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

class VadCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "vad")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2 (vad, VadCreator)
Q_PLUGIN_METADATA(vad)
Q_PLUGIN_METADATA(VadCreator)

class VadPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(unsigned length WRITE setLength READ getLenght)
    Q_PROPERTY(bool disabled WRITE setDisabled READ getDisabled)
    Q_CLASSINFO("length-description", Q_TR_NOOP("filter length"))
    Q_CLASSINFO("disabled-description", Q_TR_NOOP("Disabled as boolean"))
public:
    Q_INVOKABLE VadPlugin(unsigned blockSize, unsigned sampleRate,
                          int inputPorts, int outputPorts);
    virtual ~VadPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    unsigned getLenght() const;
    void setLength(unsigned value);

    bool getDisabled() const;
    void setDisabled(bool value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    QList<VoiceActivityDetector> filters;
    unsigned length;
    bool disabled;
};

// VadPlugin ===================================================================

VadPlugin::VadPlugin(unsigned blockSize, unsigned sampleRate, int inputPorts,
                     int outputPorts)
    : blockSize(blockSize), length(blockSize), disabled(false)
{
    Q_UNUSED(sampleRate);

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
}

VadPlugin::~VadPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString VadPlugin::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);
    try {
        VoiceActivityDetector filter(length, true);
        filter.setWeirdRMSLimiting(true);
        filters.clear();
        while (unsigned(filters.size()) < unsigned(inPorts.size()))
            filters.append(filter);
    } catch (const std::exception &e) {
        return tr("Unable to instantiate VAD: %1")
            .arg(QString::fromLocal8Bit(e.what()));
    }

    return QString();
}

void VadPlugin::process()
{
    const unsigned ports =
        qMin(unsigned(inPorts.size()), unsigned(outPorts.size()));

    if (disabled) {
        for (unsigned port = 0; port < unsigned(outPorts.size()); ++port) {
            auto outputData = outPorts[port]->outputData.data();
            for (unsigned i = 0; i < blockSize; ++i)
                outputData[i] = 1;
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

    for (unsigned port = 0; port < ports; ++port)
        filters[port].process(doubleData.data()[port], blockSize);

    doubleToFloat(doubleData.data(), ports, blockSize, floatData.data());
}

QList<BlockInputPort *> VadPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> VadPlugin::outputPorts()
{
    return outPorts;
}

void VadPlugin::release()
{
}

unsigned VadPlugin::getLenght() const
{
    return length;
}

void VadPlugin::setLength(unsigned value)
{
    length = value;
}

bool VadPlugin::getDisabled() const
{
    return disabled;
}

void VadPlugin::setDisabled(bool value)
{
    disabled = value;
}

// VadCreator ==================================================================

QList<BlockDescription> VadCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &VadPlugin::staticMetaObject;
}

#include "vadplugin.moc"
