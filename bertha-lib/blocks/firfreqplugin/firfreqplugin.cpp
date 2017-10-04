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

#include <fftw3.h>

#include <QDir>
#include <QFile>
#include <QMap>
#include <QSharedPointer>
#include <QStringList>
#include <QVector>
#include <climits>
#include <iostream>

using namespace std;

class FirFreqCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "firfreq")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

Q_PLUGIN_METADATA(FirFreqCreator)

class FirFreqPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(unsigned fftLength READ getFftLength)
    Q_PROPERTY(unsigned lowerCutoffFreq READ getLowerCutoffFreq WRITE
                   setLowerCutoffFreq)
    Q_PROPERTY(unsigned upperCutoffFreq READ getUpperCutoffFreq WRITE
                   setUpperCutoffFreq);

public:
    Q_INVOKABLE FirFreqPlugin(unsigned blockSize, unsigned sampleRate,
                              int inputPorts, int outputPorts,
                              const QVariantMap &readOnlyParameters,
                              const QVariant &toFrequencyDomain);
    virtual ~FirFreqPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output);

    unsigned getFftLength() const;
    unsigned getLowerCutoffFreq() const;
    unsigned getUpperCutoffFreq() const;
    void setLowerCutoffFreq(unsigned value);
    void setUpperCutoffFreq(unsigned value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned sampleRate;
    unsigned fftLength;
    unsigned lowerCutoffFreq;
    unsigned upperCutoffFreq;
    int cut1;
    int cut2;
    QVector<float> in;         // store input block in this Vector
    QVector<int> freqFirCoeff; // store frequency coefficients in this block

    //    FILE *fichero;
};

// FirFreqPlugin
// ===================================================================

FirFreqPlugin::FirFreqPlugin(unsigned blockSize, unsigned sampleRate,
                             int inputPorts, int outputPorts,
                             const QVariantMap &readOnlyParameters,
                             const QVariant &toFrequencyDomain)
    : sampleRate(sampleRate),
      fftLength(qMax(
          readOnlyParameters.contains(QLatin1String("fftLength"))
              ? readOnlyParameters.value(QLatin1String("fftLength")).toUInt()
              : 64u,
          blockSize)),
      lowerCutoffFreq(0),
      upperCutoffFreq(0),
      cut1(0),
      cut2(0),
      in(fftLength + 2),
      freqFirCoeff((fftLength / 2) + 1)
{
    Q_UNUSED(blockSize);
    Q_UNUSED(toFrequencyDomain);

    if (inputPorts < 0)
        inputPorts = 1;

    if (outputPorts < 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(qMin(inputPorts, outputPorts)); ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("input-%1").arg(i + 1), 0,
                               fftLength, BlockPort::FrequencyDomain));
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1), 0,
                                fftLength, BlockPort::FrequencyDomain));
    }
}

FirFreqPlugin::~FirFreqPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString FirFreqPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    /*        if (lowerCutoffFreq == 0)
            fichero = fopen("fichero.txt","w");
        if (lowerCutoffFreq == 1000)
            fichero = fopen("fichero2.txt","w");
        if (lowerCutoffFreq > 2990 && lowerCutoffFreq < 3010)
            fichero = fopen("fichero3.txt","w");
        if (lowerCutoffFreq > 8990 && lowerCutoffFreq < 9010)
            fichero = fopen("fichero4.txt","w");*/

    freqFirCoeff.fill(0);

    cut1 = lowerCutoffFreq * (fftLength / 2) / (sampleRate / 2);
    cut2 = upperCutoffFreq * (fftLength / 2) / (sampleRate / 2);

    for (int x = 0; x < (cut2 - cut1); x++)
        freqFirCoeff[cut1 + x] = 1;

    //    for (int x = 0; x <  freqFirCoeff.size(); x++)
    //       fprintf(fichero,"%i\n",freqFirCoeff[x]);

    return QString();
}

void FirFreqPlugin::processOne(PortDataType *input, PortDataType *output)
{
    if (!input) {
        memset(in.data(), 0, in.size() * sizeof(PortDataType));
    } else {
        memcpy(in.data(), input, in.size() * sizeof(PortDataType));
    }
    for (unsigned x = 0; x < (fftLength / 2) + 1; x++) {
        output[x * 2] = in[x * 2] * freqFirCoeff[x];
        output[x * 2 + 1] = in[x * 2 + 1] * freqFirCoeff[x];
    }
}

void FirFreqPlugin::process()
{
    for (unsigned i = 0; i < unsigned(inPorts.size()); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data());
    }
}

QList<BlockInputPort *> FirFreqPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> FirFreqPlugin::outputPorts()
{
    return outPorts;
}

void FirFreqPlugin::release()
{
    //    fclose(fichero);
}

unsigned FirFreqPlugin::getFftLength() const
{
    return fftLength;
}

unsigned FirFreqPlugin::getLowerCutoffFreq() const
{
    return lowerCutoffFreq;
}

void FirFreqPlugin::setLowerCutoffFreq(unsigned value)
{
    lowerCutoffFreq = value;
}

unsigned FirFreqPlugin::getUpperCutoffFreq() const
{
    return upperCutoffFreq;
}

void FirFreqPlugin::setUpperCutoffFreq(unsigned value)
{
    upperCutoffFreq = value;
}

// FirFreqCreator =============================================================

QList<BlockDescription> FirFreqCreator::availableBlocks() const
{
    return QList<BlockDescription>()
           //<< &FirBlockPlugin::staticMetaObject;
           << BlockDescription(&FirFreqPlugin::staticMetaObject,
                               QLatin1String("FirFreq"), true);
}

#include "firfreqplugin.moc"
