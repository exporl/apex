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

#include <fftw3.h>

#include <QDir>
#include <QMap>
#include <QSharedPointer>
#include <QStringList>
#include <QVector>

class FftCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "fft")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(fft, FftCreator)
Q_PLUGIN_METADATA(fft)
Q_PLUGIN_METADATA(FftCreator)

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

class FftPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
public:
    Q_INVOKABLE FftPlugin(unsigned blockSize, unsigned sampleRate,
                          int inputPorts, int outputPorts,
                          const QVariant &toFrequencyDomain);
    virtual ~FftPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output, unsigned index);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;
    bool toFrequencyDomain;
    float fftFactor;

    QList<QSharedPointer<FftwfPlan>> plans;
    QVector<PortDataType> in;
};

class FftFilterPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(QVector<double> filterCoefficients WRITE setFilterCoefficients
                   READ getFilterCoefficients)
public:
    Q_INVOKABLE FftFilterPlugin(unsigned blockSize, unsigned sampleRate,
                                int inputPorts, int outputPorts);
    virtual ~FftFilterPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output);

    QVector<double> getFilterCoefficients() const;
    void setFilterCoefficients(const QVector<double> &value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    QList<QSharedPointer<FftwfPlan>> plans;
    QVector<double> filterCoefficients;
};

// FftPlugin ===================================================================

FftPlugin::FftPlugin(unsigned blockSize, unsigned sampleRate, int inputPorts,
                     int outputPorts, const QVariant &toFrequencyDomain)
    : blockSize(blockSize),
      toFrequencyDomain(toFrequencyDomain.toBool()),
      fftFactor(1.0 / blockSize),
      in(toFrequencyDomain.toBool() ? blockSize : (blockSize / 2 + 1) * 2)
{
    Q_UNUSED(sampleRate);

    if (inputPorts < 0)
        inputPorts = 1;

    if (outputPorts < 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(qMin(inputPorts, outputPorts)); ++i) {
        inPorts.append(new BlockInputPort(
            QString::fromLatin1("input-%1").arg(i + 1), 0, 0,
            this->toFrequencyDomain ? BlockPort::TimeDomain
                                    : BlockPort::FrequencyDomain));
        outPorts.append(new BlockOutputPort(
            QString::fromLatin1("output-%1").arg(i + 1), 0, 0,
            this->toFrequencyDomain ? BlockPort::FrequencyDomain
                                    : BlockPort::TimeDomain));
    }
}

FftPlugin::~FftPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString FftPlugin::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);

    plans.clear();
    for (unsigned i = 0; i < unsigned(inPorts.size()); ++i) {
        if (toFrequencyDomain)
            plans.append(
                QSharedPointer<FftwfPlan>(new FftwfPlan(fftwf_plan_dft_r2c_1d(
                    blockSize, in.data(), reinterpret_cast<fftwf_complex *>(
                                              outPorts[i]->outputData.data()),
                    FFTW_ESTIMATE))));
        else
            plans.append(
                QSharedPointer<FftwfPlan>(new FftwfPlan(fftwf_plan_dft_c2r_1d(
                    blockSize, reinterpret_cast<fftwf_complex *>(in.data()),
                    outPorts[i]->outputData.data(), FFTW_ESTIMATE))));
    }

    return QString();
}

void FftPlugin::processOne(PortDataType *input, PortDataType *output,
                           unsigned index)
{
    if (!input) {
        memset(in.data(), 0, in.size() * sizeof(PortDataType));
    } else {
        memcpy(in.data(), input, in.size() * sizeof(PortDataType));
    }
    fftwf_execute(plans[index]->plan);
    if (!toFrequencyDomain)
        for (unsigned j = 0; j < blockSize; ++j)
            output[j] *= fftFactor;
}

void FftPlugin::process()
{
    for (unsigned i = 0; i < unsigned(inPorts.size()); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data(), i);
    }
}

QList<BlockInputPort *> FftPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> FftPlugin::outputPorts()
{
    return outPorts;
}

void FftPlugin::release()
{
}

// FftFilterPlugin =============================================================

FftFilterPlugin::FftFilterPlugin(unsigned blockSize, unsigned sampleRate,
                                 int inputPorts, int outputPorts)
    : blockSize(blockSize), filterCoefficients(blockSize / 2 + 1, 1)
{
    Q_UNUSED(sampleRate);

    if (inputPorts < 0)
        inputPorts = 1;

    if (outputPorts < 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(qMin(inputPorts, outputPorts)); ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("input-%1").arg(i + 1), 0, 0,
                               BlockPort::FrequencyDomain));
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1), 0,
                                0, BlockPort::FrequencyDomain));
    }
}

FftFilterPlugin::~FftFilterPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString FftFilterPlugin::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);

    return QString();
}

void FftFilterPlugin::processOne(PortDataType *input, PortDataType *output)
{
    if (!input) {
        memset(output, 0, 2 * (blockSize / 2 + 1) * sizeof(PortDataType));
    } else {
        for (unsigned j = 0; j < 2 * (blockSize / 2 + 1); ++j)
            output[j] = filterCoefficients[j / 2] * input[j];
    }
}

void FftFilterPlugin::process()
{
    for (unsigned i = 0; i < unsigned(inPorts.size()); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data());
    }
}

QList<BlockInputPort *> FftFilterPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> FftFilterPlugin::outputPorts()
{
    return outPorts;
}

QVector<double> FftFilterPlugin::getFilterCoefficients() const
{
    return filterCoefficients;
}

void FftFilterPlugin::setFilterCoefficients(const QVector<double> &value)
{
    filterCoefficients = value;
    filterCoefficients.resize((blockSize / 2) + 1);
    if (unsigned(value.size()) < (blockSize / 2) + 1)
        qFill(filterCoefficients.begin() + value.size(),
              filterCoefficients.end(), 1);
}

void FftFilterPlugin::release()
{
}

// FftCreator ==================================================================

QList<BlockDescription> FftCreator::availableBlocks() const
{
    return QList<BlockDescription>()
           << BlockDescription(&FftPlugin::staticMetaObject,
                               QLatin1String("Fft"), true)
           << BlockDescription(&FftPlugin::staticMetaObject,
                               QLatin1String("Ifft"), false)
           << &FftFilterPlugin::staticMetaObject;
}

#include "fftplugin.moc"
