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

#define PI 3.14159265358979323846
#define PI_2 1.57079632679489661923

class SineGeneratorCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "sinegenerator")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(sineGenerator, SineGeneratorCreator)
Q_PLUGIN_METADATA(sineGenerator)
Q_PLUGIN_METADATA(SineGeneratorCreator)

class SineGeneratorPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(double frequency WRITE setFrequency READ getFrequency)
    Q_PROPERTY(double amplitude WRITE setAmplitude READ getAmplitude)
    Q_PROPERTY(double phase WRITE setPhase READ getPhase)
    Q_CLASSINFO("frequency-description", Q_TR_NOOP("Frequency in hertz"))
    Q_CLASSINFO("frequency-unit", "linear")
    Q_CLASSINFO("frequency-minimum", "0")
    Q_CLASSINFO("frequency-maximum", "20000")
    Q_CLASSINFO("amplitude-description", Q_TR_NOOP("Amplitude in"))
    Q_CLASSINFO("amplitude-unit", "linear")
    Q_CLASSINFO("amplitude-minimum", "0")
    Q_CLASSINFO("amplitude-maximum", "20")
    Q_CLASSINFO("phase-description", Q_TR_NOOP("Phase in degrees"))
    Q_CLASSINFO("phase-unit", "linear")
    Q_CLASSINFO("phase-minimum", "0")
    Q_CLASSINFO("phase-maximum", "1");

public:
    Q_INVOKABLE SineGeneratorPlugin(unsigned blockSize, unsigned sampleRate,
                                    int inputPorts, int outputPorts);
    virtual ~SineGeneratorPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    double getFrequency() const;
    void setFrequency(double value);

    double getAmplitude() const;
    void setAmplitude(double value);

    double getPhase() const;
    void setPhase(double value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;
    unsigned sampleRate;

    double frequency;
    double amplitude;
    double phase;

    unsigned position;

    QVector<PortDataType> scratch;
};

// SineGeneratorPlugin =========================================================

SineGeneratorPlugin::SineGeneratorPlugin(unsigned blockSize,
                                         unsigned sampleRate, int inputPorts,
                                         int outputPorts)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      frequency(1),
      amplitude(1),
      phase(0),
      position(0)
{
    if (inputPorts != 0)
        inputPorts = 0;

    if (outputPorts < 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(outputPorts); ++i) {
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }

    scratch.resize(blockSize);
}

SineGeneratorPlugin::~SineGeneratorPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString SineGeneratorPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    return QString();
}

void SineGeneratorPlugin::process()
{
    for (unsigned j = 0; j < blockSize; ++j) {
        scratch[j] = amplitude *
                     sin((2 * PI * frequency * (double(position) / sampleRate) +
                          phase * PI / 180));
        ++position;
    }

    for (int i = 0; i < outPorts.size(); ++i)
        memcpy(outPorts[i]->outputData.data(), scratch.data(),
               scratch.size() * sizeof(PortDataType));

    if (position > sampleRate)
        position -= sampleRate;
}

QList<BlockInputPort *> SineGeneratorPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> SineGeneratorPlugin::outputPorts()
{
    return outPorts;
}

void SineGeneratorPlugin::release()
{
}

double SineGeneratorPlugin::getFrequency() const
{
    return frequency;
}

void SineGeneratorPlugin::setFrequency(double value)
{
    frequency = value;
}

double SineGeneratorPlugin::getAmplitude() const
{
    return amplitude;
}

void SineGeneratorPlugin::setAmplitude(double value)
{
    amplitude = value;
}

double SineGeneratorPlugin::getPhase() const
{
    return phase;
}

void SineGeneratorPlugin::setPhase(double value)
{
    phase = value;
}

// SineGeneratorCreator ========================================================

QList<BlockDescription> SineGeneratorCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &SineGeneratorPlugin::staticMetaObject;
}

#include "sinegeneratorplugin.moc"
