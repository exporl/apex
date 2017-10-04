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

#include <QStringList>

#include <cmath>

class AmplifierCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "amplifier")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(amplifier, AmplifierCreator)
Q_PLUGIN_METADATA(amplifier)
Q_PLUGIN_METADATA(AmplifierCreator)

class AmplifierPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(bool disabled WRITE setDisabled READ getDisabled)
    Q_PROPERTY(double baseGain WRITE setBaseGain READ getBaseGain)
    Q_PROPERTY(double gain WRITE setAmplifier READ getAmplifier)
    Q_CLASSINFO("disabled-description", Q_TR_NOOP("Boolean disabled"))
    Q_CLASSINFO("baseGain-description", Q_TR_NOOP("Base gain in dB"))
    Q_CLASSINFO("gain-description", Q_TR_NOOP("Delay in dB"))
public:
    Q_INVOKABLE AmplifierPlugin(unsigned blockSize, unsigned sampleRate,
                                int inputPorts, int outputPorts);
    virtual ~AmplifierPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output);

    bool getDisabled() const;
    void setDisabled(bool value);

    double getBaseGain() const;
    void setBaseGain(double value);

    double getAmplifier() const;
    void setAmplifier(double value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    bool disabled;
    // gains in dB
    double baseGain;
    double gain;
};

// AmplifierPlugin =============================================================

AmplifierPlugin::AmplifierPlugin(unsigned blockSize, unsigned sampleRate,
                                 int inputPorts, int outputPorts)
    : blockSize(blockSize), disabled(false), baseGain(0), gain(0)
{
    Q_UNUSED(sampleRate);

    if (inputPorts < 0)
        inputPorts = 1;

    if (outputPorts < 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(qMin(inputPorts, outputPorts)); ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("input-%1").arg(i + 1)));
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }
}

AmplifierPlugin::~AmplifierPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString AmplifierPlugin::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);
    return QString();
}

void AmplifierPlugin::processOne(PortDataType *input, PortDataType *output)
{
    if (!input) {
        memset(output, 0, blockSize * sizeof(PortDataType));
    } else if (disabled) {
        memcpy(output, input, blockSize * sizeof(PortDataType));
    } else {
        const double gainMult = std::pow(10.0, (gain + baseGain) / 20);
        for (unsigned i = 0; i < blockSize; ++i)
            output[i] = gainMult * input[i];
    }
}

void AmplifierPlugin::process()
{
    for (int i = 0; i < inPorts.size(); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data());
    }
}

QList<BlockInputPort *> AmplifierPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> AmplifierPlugin::outputPorts()
{
    return outPorts;
}

void AmplifierPlugin::release()
{
}

bool AmplifierPlugin::getDisabled() const
{
    return disabled;
}

void AmplifierPlugin::setDisabled(bool value)
{
    disabled = value;
}

double AmplifierPlugin::getBaseGain() const
{
    return baseGain;
}

void AmplifierPlugin::setBaseGain(double value)
{
    baseGain = value;
}

double AmplifierPlugin::getAmplifier() const
{
    return gain;
}

void AmplifierPlugin::setAmplifier(double value)
{
    gain = value;
}

// AmplifierCreator ============================================================

QList<BlockDescription> AmplifierCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &AmplifierPlugin::staticMetaObject;
}

#include "amplifierplugin.moc"
