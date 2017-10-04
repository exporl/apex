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

#include "bertha/blockinterface.h"

#include <math.h>

#include <QStringList>
#include <QVariant>

class GainCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "gain")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(gain, GainCreator)
Q_PLUGIN_METADATA(gain)
Q_PLUGIN_METADATA(GainCreator)

class GainPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(double gain WRITE setGain READ getGain)
    Q_PROPERTY(double rampLength WRITE setRampLength READ getRampLength)
    Q_CLASSINFO("gain-description", Q_TR_NOOP("Gain in dB"))
    Q_CLASSINFO("rampLength-description",
                Q_TR_NOOP("If larger than 0, a linear ramp of this length "
                          "in seconds will be applied when changing the gain"))
public:
    Q_INVOKABLE GainPlugin(unsigned blockSize, unsigned sampleRate,
                           int inputPorts, int outputPorts);
    virtual ~GainPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output);

    double getGain() const;
    void setGain(double value);

    double getRampLength() const;
    void setRampLength(double value); // value in seconds

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;
    unsigned sampleRate;

    float newGain;     // set by user
    float oldGain;     // previous value set by user
    float currentGain; // current gain value, depending

    unsigned rampLength; // lenght of ramp in samples

    // unsigned rampPosition;
};

// GainPlugin ==================================================================

GainPlugin::GainPlugin(unsigned blockSize, unsigned sampleRate, int inputPorts,
                       int outputPorts)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      newGain(0),
      oldGain(0),
      currentGain(0),
      rampLength(0)
{
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

GainPlugin::~GainPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString GainPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    // do not ramp at start
    oldGain = newGain;
    currentGain = newGain;

    return QString();
}

void GainPlugin::processOne(PortDataType *input, PortDataType *output)
{
    if (!input) {
        memset(output, 0, blockSize * sizeof(PortDataType));
    } else {
        const double factor = pow(10.0, newGain / 20.0);
        if (rampLength == 0 || currentGain == newGain) {
            for (unsigned j = 0; j < blockSize; ++j)
                output[j] = factor * input[j];
        } else {
            float slope = (newGain - oldGain) / rampLength;
            unsigned rampLeft = (newGain - currentGain) / slope + 0.5;
            unsigned startSample = (currentGain - oldGain) / slope + 1;

            unsigned upCount = qMin(rampLeft, blockSize);
            for (unsigned j = 0; j < upCount; ++j)
                output[j] =
                    pow(10.0, (slope * (startSample + j) + oldGain) / 20.0) *
                    input[j];
            for (unsigned j = upCount; j < blockSize; ++j)
                output[j] = factor * input[j];
        }
    }
}

void GainPlugin::process()
{
    for (unsigned i = 0; i < unsigned(inPorts.size()); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data());
    }
    if (rampLength != 0 && currentGain != newGain) {
        float slope = (newGain - oldGain) / rampLength;
        unsigned rampLeft = (newGain - currentGain) / slope + 0.5;
        unsigned startSample = (currentGain - oldGain) / slope + 1;

        if (rampLeft <= blockSize)
            currentGain = newGain;
        else
            currentGain = slope * (startSample + blockSize - 1) + oldGain;
    }
}

QList<BlockInputPort *> GainPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> GainPlugin::outputPorts()
{
    return outPorts;
}

void GainPlugin::release()
{
}

void GainPlugin::setGain(double value)
{
    oldGain = currentGain;
    // rampPosition = 0;
    newGain = value;
    if (rampLength == 0)
        currentGain = value;
}

double GainPlugin::getGain() const
{
    return newGain;
}

void GainPlugin::setRampLength(double value)
{
    rampLength = value * sampleRate + 0.5; // +0.5 to round
}

double GainPlugin::getRampLength() const
{
    return rampLength / sampleRate;
}

// GainCreator =================================================================

QList<BlockDescription> GainCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &GainPlugin::staticMetaObject;
}

#include "gainplugin.moc"
