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

#include <math.h>

#include <QStringList>
#include <QVariant>

class PulseGeneratorCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "pulsegenerator")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

Q_PLUGIN_METADATA(pulseGenerator)
Q_PLUGIN_METADATA(PulseGeneratorCreator)

class PulseGeneratorPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(unsigned pulseWidth WRITE setPulseWidth READ getPulseWidth)
    Q_PROPERTY(double amplitude WRITE setAmplitude READ getAmplitude)
    Q_PROPERTY(bool polarity WRITE setPolarity READ getPolarity)
    Q_PROPERTY(
        unsigned pulseDistance WRITE setPulseDistance READ getPulseDistance)
    Q_PROPERTY(unsigned delay WRITE setDelay READ getDelay)
    Q_PROPERTY(int incrementPulseDistance WRITE setIncrementPulseDistance READ
                   getIncrementPulseDistance)
    Q_PROPERTY(unsigned period WRITE setPeriod READ getPeriod)
    Q_CLASSINFO("pulseWidth-description", Q_TR_NOOP("pulse width in samples"))
    Q_CLASSINFO("amplitude-description", Q_TR_NOOP("amplitude, linear value"))
    Q_CLASSINFO("polarity-description",
                Q_TR_NOOP("boolean, true = positive, false = negative"))
    Q_CLASSINFO("pulseDistance-description",
                Q_TR_NOOP("distance between pulses in samples"))
    Q_CLASSINFO("delay-description",
                Q_TR_NOOP("delay in samples, delay is the sample where the "
                          "first pulse starts"))
    Q_CLASSINFO("incrementPulseDistance-description",
                Q_TR_NOOP("increment of the pulse distance in samples"))
    Q_CLASSINFO("period-description",
                Q_TR_NOOP("number of pulses after which the pulse distance is "
                          "increased by incrementPulseDistance"))

public:
    Q_INVOKABLE PulseGeneratorPlugin(unsigned blockSize, unsigned sampleRate,
                                     int inputPorts, int outputPorts);
    virtual ~PulseGeneratorPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void setPulseWidth(unsigned value);
    unsigned getPulseWidth();

    void setPolarity(bool value);
    bool getPolarity();

    void setAmplitude(double value);
    double getAmplitude();

    void setPulseDistance(unsigned value);
    unsigned getPulseDistance();

    void setDelay(unsigned value);
    unsigned getDelay();

    void setIncrementPulseDistance(int value);
    int getIncrementPulseDistance();

    void setPeriod(unsigned value);
    unsigned getPeriod();

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    unsigned pulseWidth;
    double amplitude;
    bool polarity;
    unsigned pulseDistance;
    unsigned delay;
    int incrementPulseDistance;
    unsigned period;

    unsigned position;
    unsigned count_blocks;
    unsigned count_period;
    unsigned initialPulseDistance;
};

// PulseGeneratorPlugin
// =========================================================

PulseGeneratorPlugin::PulseGeneratorPlugin(unsigned blockSize,
                                           unsigned sampleRate, int inputPorts,
                                           int outputPorts)
    : blockSize(blockSize),
      pulseWidth(1),
      amplitude(1),
      polarity(true),
      pulseDistance(unsigned(blockSize / 10)),
      delay(0),
      incrementPulseDistance(0),
      period(pulseDistance)

{
    Q_UNUSED(sampleRate);

    if (inputPorts != 0)
        inputPorts = 0;

    if (outputPorts < 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(outputPorts); ++i) {
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }
}

PulseGeneratorPlugin::~PulseGeneratorPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString PulseGeneratorPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    position = 0;
    count_blocks = 0;
    count_period = 0;
    initialPulseDistance = pulseDistance;

    return QString();
}

void PulseGeneratorPlugin::process()
{
    unsigned startidx;

    for (unsigned i = 0; i < unsigned(outPorts.size()); ++i) {
        auto output = outPorts[i]->outputData.data();
        memset(output, 0, blockSize * sizeof(PortDataType)); // sets all the
                                                             // values on the
                                                             // output port to 0
    }

    startidx = delay * (count_blocks == 0);

    for (unsigned j = startidx; j < blockSize; ++j) {
        if (position < pulseWidth) {
            for (unsigned i = 0; i < unsigned(outPorts.size()); ++i) {
                auto output = outPorts[i]->outputData.data();
                if (polarity)
                    output[j] = amplitude;
                else
                    output[j] = -amplitude;
            }
            ++position;
            ++count_period;
        } else if (position == pulseDistance) {
            position = 0;
            ++count_period;
            if (count_period <= period) {
                pulseDistance += incrementPulseDistance;
            } else {
                pulseDistance = initialPulseDistance;
                count_period = 0;
            }
        } else {
            ++position;
            ++count_period;
        }
    }
    count_blocks += 1;
}

QList<BlockInputPort *> PulseGeneratorPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> PulseGeneratorPlugin::outputPorts()
{
    return outPorts;
}

void PulseGeneratorPlugin::release()
{
}

void PulseGeneratorPlugin::setPulseWidth(unsigned value)
{
    pulseWidth = value;
}

unsigned PulseGeneratorPlugin::getPulseWidth()
{
    return pulseWidth;
}

void PulseGeneratorPlugin::setPolarity(bool value)
{
    polarity = value;
}

bool PulseGeneratorPlugin::getPolarity()
{
    return polarity;
}

void PulseGeneratorPlugin::setAmplitude(double value)
{
    amplitude = value;
}

double PulseGeneratorPlugin::getAmplitude()
{
    return amplitude;
}

void PulseGeneratorPlugin::setPulseDistance(unsigned value)
{
    pulseDistance = value;
}

unsigned PulseGeneratorPlugin::getPulseDistance()
{
    return pulseDistance;
}

void PulseGeneratorPlugin::setDelay(unsigned value)
{
    delay = value;
}

unsigned PulseGeneratorPlugin::getDelay()
{
    return delay;
}

void PulseGeneratorPlugin::setIncrementPulseDistance(int value)
{
    incrementPulseDistance = value;
}

int PulseGeneratorPlugin::getIncrementPulseDistance()
{
    return incrementPulseDistance;
}

void PulseGeneratorPlugin::setPeriod(unsigned value)
{
    period = value;
}

unsigned PulseGeneratorPlugin::getPeriod()
{
    return period;
}

// PulseGeneratorCreator
// ========================================================

QList<BlockDescription> PulseGeneratorCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &PulseGeneratorPlugin::staticMetaObject;
}

#include "pulsegeneratorplugin.moc"
