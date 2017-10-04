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
 * This program is distributed temp the hope that it will be useful,          *
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
#include <QMap>
#include <QSharedPointer>
#include <QStringList>
#include <QVector>
#include <cmath>
#include <iostream>

using namespace std;

#define PI 3.14159265358979323846
#define PI_2 1.57079632679489661923

class FftwfPlan
{
public:
    FftwfPlan(fftwf_plan plan) : plan(plan)
    {
    }

    ~FftwfPlan()
    {
        fftwf_destroy_plan(plan);
    }

    fftwf_plan plan;

private:
    FftwfPlan(const FftwfPlan &other);
    FftwfPlan &operator=(const FftwfPlan &other);
};

class OlaFftCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "olafft")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(olaFft, OlaFftCreator)
Q_PLUGIN_METADATA(olaFt)
Q_PLUGIN_METADATA(OlaFftCreator)

class OlaFftPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(unsigned fftLength READ getFftLength)
public:
    Q_INVOKABLE OlaFftPlugin(unsigned blockSize, unsigned sampleRate,
                             int inputPorts, int outputPorts,
                             const QVariantMap &readOnlyParameters,
                             const QVariant &toFrequencyDomain);
    virtual ~OlaFftPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output, unsigned index);

    unsigned getFftLength() const;

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;
    bool toFrequencyDomain;
    unsigned fftLength;
    float fftFactor;
    float hannNormalization;
    unsigned useHanning;
    QVector<PortDataType> temp;
    QVector<PortDataType> tempOla;
    QVector<PortDataType> previousBlocks;
    QVector<PortDataType> hanning;

    QList<QSharedPointer<FftwfPlan>> plans;
    QVector<PortDataType> in;
};

// FftPlugin ===================================================================

OlaFftPlugin::OlaFftPlugin(unsigned blockSize, unsigned sampleRate,
                           int inputPorts, int outputPorts,
                           const QVariantMap &readOnlyParameters,
                           const QVariant &toFrequencyDomain)
    : blockSize(blockSize),
      toFrequencyDomain(toFrequencyDomain.toBool()),
      fftLength(qMax(
          readOnlyParameters.contains(QLatin1String("fftLength"))
              ? readOnlyParameters.value(QLatin1String("fftLength")).toUInt()
              : 64u,
          blockSize)),
      fftFactor(1.0 / blockSize * blockSize / fftLength),
      hannNormalization((fftLength / blockSize) * 0.5), // 0.375),
      useHanning(1),
      temp(fftLength),
      tempOla(fftLength),
      previousBlocks(fftLength),
      hanning(fftLength)
{
    Q_UNUSED(sampleRate);

    if (inputPorts < 0)
        inputPorts = 1;

    if (outputPorts < 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(qMin(inputPorts, outputPorts)); ++i) {
        inPorts.append(new BlockInputPort(
            QString::fromLatin1("input-%1").arg(i + 1), 0,
            this->toFrequencyDomain ? 0 : fftLength,
            this->toFrequencyDomain ? BlockPort::TimeDomain
                                    : BlockPort::FrequencyDomain));
        outPorts.append(new BlockOutputPort(
            QString::fromLatin1("output-%1").arg(i + 1), 0,
            this->toFrequencyDomain ? fftLength : 0,
            this->toFrequencyDomain ? BlockPort::FrequencyDomain
                                    : BlockPort::TimeDomain));
    }

    in.resize(this->toFrequencyDomain ? blockSize : fftLength);
}

OlaFftPlugin::~OlaFftPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString OlaFftPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    tempOla.fill(0);
    previousBlocks.fill(0);

    for (unsigned i = 0; i < (fftLength); ++i)
        hanning[i] = sqrt(0.5 * (1 - cos(2 * PI * i / (fftLength))));

    plans.clear();
    for (unsigned i = 0; i < unsigned(inPorts.size()); ++i) {
        if (toFrequencyDomain)
            plans.append(
                QSharedPointer<FftwfPlan>(new FftwfPlan(fftwf_plan_dft_r2c_1d(
                    fftLength, temp.data(), reinterpret_cast<fftwf_complex *>(
                                                outPorts[i]->outputData.data()),
                    FFTW_ESTIMATE))));
        else
            plans.append(
                QSharedPointer<FftwfPlan>(new FftwfPlan(fftwf_plan_dft_c2r_1d(
                    fftLength, reinterpret_cast<fftwf_complex *>(in.data()),
                    temp.data(), FFTW_ESTIMATE))));
    }

    return QString();
}

void OlaFftPlugin::processOne(PortDataType *input, PortDataType *output,
                              unsigned index)
{
    if (!input) {
        memset(in.data(), 0, in.size() * sizeof(PortDataType));
    } else {
        memcpy(in.data(), input, in.size() * sizeof(PortDataType));
    }
    if (toFrequencyDomain) {
        temp.fill(0);

        if (useHanning) {
            memmove(previousBlocks.data(), previousBlocks.data() + blockSize,
                    (fftLength - blockSize) * sizeof(PortDataType));
            memcpy(previousBlocks.data() + fftLength - blockSize, in.data(),
                   blockSize * sizeof(PortDataType));
            for (unsigned k = 0; k < fftLength; k++)
                temp[k] = previousBlocks[k] * hanning[k];
        } else {
            memcpy(temp.data(), in.data(),
                   blockSize * sizeof(PortDataType)); // Rectangular Window
        }

        fftwf_execute(plans[index]->plan);

    } else {
        temp.fill(0);
        fftwf_execute(plans[index]->plan);

        if (useHanning) {
            for (unsigned j = 0; j < fftLength; ++j)
                tempOla[j] =
                    tempOla[j] +
                    fftFactor * temp[j] * (hanning[j] / hannNormalization);

        } else {
            for (unsigned j = 0; j < fftLength; ++j)
                tempOla[j] = tempOla[j] + fftFactor * temp[j];
        }

        memcpy(output, tempOla.data(), blockSize * sizeof(PortDataType));
        memmove(tempOla.data(), tempOla.data() + blockSize,
                (fftLength - blockSize) * sizeof(PortDataType));
        memset(tempOla.data() + fftLength - blockSize, 0,
               blockSize * sizeof(PortDataType));
    }
}

void OlaFftPlugin::process()
{
    for (unsigned i = 0; i < unsigned(inPorts.size()); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data(), i);
    }
}

QList<BlockInputPort *> OlaFftPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> OlaFftPlugin::outputPorts()
{
    return outPorts;
}

void OlaFftPlugin::release()
{
}

unsigned OlaFftPlugin::getFftLength() const
{
    return fftLength;
}

// OlaFftCreator ===============================================================

QList<BlockDescription> OlaFftCreator::availableBlocks() const
{
    return QList<BlockDescription>()
           << BlockDescription(&OlaFftPlugin::staticMetaObject,
                               QLatin1String("OlaFft"), true)
           << BlockDescription(&OlaFftPlugin::staticMetaObject,
                               QLatin1String("OlaIfft"), false);
}

#include "olafftplugin.moc"
