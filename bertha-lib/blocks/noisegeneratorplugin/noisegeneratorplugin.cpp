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

#include "random.h"

#include <math.h>

#include <QStringList>
#include <QVariant>

class NoiseGeneratorCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "noisegenerator")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(noiseGenerator, NoiseGeneratorCreator)
Q_PLUGIN_METADATA(noiseGenerator)
Q_PLUGIN_METADATA(NoiseGeneratorCreator)

class NoiseGeneratorPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(double amplitude WRITE setAmplitude READ getAmplitude)
    Q_CLASSINFO("amplitude-description",
                Q_TR_NOOP("Amplitude of the generated noise"))
    Q_CLASSINFO("amplitude-unit", "linear")
    Q_CLASSINFO("amplitude-minimum", "0")
    Q_CLASSINFO("amplitude-maximum", "20");

public:
    Q_INVOKABLE NoiseGeneratorPlugin(unsigned blockSize, unsigned sampleRate,
                                     int inputPorts, int outputPorts);
    virtual ~NoiseGeneratorPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    double getAmplitude() const;
    void setAmplitude(double value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    double amplitude;
    Random random;
};

// NoiseGeneratorPlugin ========================================================

NoiseGeneratorPlugin::NoiseGeneratorPlugin(unsigned blockSize,
                                           unsigned sampleRate, int inputPorts,
                                           int outputPorts)
    : blockSize(blockSize), amplitude(1)
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

NoiseGeneratorPlugin::~NoiseGeneratorPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString NoiseGeneratorPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    return QString();
}

void NoiseGeneratorPlugin::process()
{
    for (unsigned i = 0; i < unsigned(outPorts.size()); ++i) {
        auto output = outPorts[i]->outputData.data();
        for (unsigned j = 0; j < blockSize; ++j) {
            output[j] = amplitude * 2 * (random.nextDouble() - .5);
        }
    }
}

QList<BlockInputPort *> NoiseGeneratorPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> NoiseGeneratorPlugin::outputPorts()
{
    return outPorts;
}

void NoiseGeneratorPlugin::release()
{
}

double NoiseGeneratorPlugin::getAmplitude() const
{
    return amplitude;
}

void NoiseGeneratorPlugin::setAmplitude(double value)
{
    amplitude = value;
}

// NoiseGeneratorCreator =======================================================

QList<BlockDescription> NoiseGeneratorCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &NoiseGeneratorPlugin::staticMetaObject;
}

#include "noisegeneratorplugin.moc"
