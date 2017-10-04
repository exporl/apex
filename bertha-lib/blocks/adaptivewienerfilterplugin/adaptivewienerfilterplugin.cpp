/*******************************************************e**********************
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

#include "syllib/adaptivewiener.h"

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

class AdaptiveWienerFilterCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "adaptivewienerfilter")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(adaptiveWienerFilter, AdaptiveWienerFilterCreator)
Q_PLUGIN_METADATA("AdaptiveWienerFilter")
Q_PLUGIN_METADATA("AdaptiveWienerFilterCreator")

class AdaptiveWienerFilterPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(unsigned length WRITE setLength READ getLenght)
    Q_PROPERTY(double muInv WRITE setMuInv READ getMuInv)
    Q_PROPERTY(bool disabled WRITE setDisabled READ getDisabled)
    Q_CLASSINFO("length-description", Q_TR_NOOP("filter length"))
    Q_CLASSINFO("muInv-description", Q_TR_NOOP("1/μ parameter of the filter"))
    Q_CLASSINFO("disabled-description", Q_TR_NOOP("Disabled as boolean"))
public:
    Q_INVOKABLE AdaptiveWienerFilterPlugin(unsigned blockSize,
                                           unsigned sampleRate, int inputPorts,
                                           int outputPorts);
    virtual ~AdaptiveWienerFilterPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    unsigned getLenght() const;
    void setLength(unsigned value);

    double getMuInv() const;
    void setMuInv(double value);

    bool getDisabled() const;
    void setDisabled(bool value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;
    unsigned sampleRate;

    QScopedPointer<AdaptiveWienerFilter> filter;
    unsigned length;
    double muInv;
    bool disabled;
};

// AdaptiveWienerFilter ========================================================

AdaptiveWienerFilterPlugin::AdaptiveWienerFilterPlugin(unsigned blockSize,
                                                       unsigned sampleRate,
                                                       int inputPorts,
                                                       int outputPorts)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      length(blockSize),
      muInv(0),
      disabled(false)
{
    if (inputPorts < 0)
        inputPorts = 5;

    if (outputPorts < 0)
        outputPorts = 5;

    for (unsigned i = 0; i < unsigned(qMin(inputPorts, outputPorts)); ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("input-%1").arg(i + 1)));
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }
}

AdaptiveWienerFilterPlugin::~AdaptiveWienerFilterPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString AdaptiveWienerFilterPlugin::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);
    try {
        if (inPorts.size() < 3)
            return tr("Need at least 3 channels");
        if (inPorts.size() % 2 == 0)
            return tr("Need an odd number of channels");
        filter.reset(new AdaptiveWienerFilter(
            sampleRate, (inPorts.size() - 1) / 2, length, muInv, true));
        if (blockSize % filter->length() != 0)
            return tr("Blocksize %1 is not multiple of Wiener filter"
                      " length %2")
                .arg(blockSize)
                .arg(filter->length());
    } catch (const std::exception &e) {
        return tr("Unable to instantiate Wiener filter: %1")
            .arg(QString::fromLocal8Bit(e.what()));
    }

    return QString();
}

void AdaptiveWienerFilterPlugin::process()
{
    const unsigned ports =
        qMin(unsigned(inPorts.size()), unsigned(outPorts.size()));

    if (disabled) {
        for (unsigned i = 0; i < ports; ++i) {
            auto inputRef = inPorts[i]->inputData.toStrongRef();
            if (!inputRef)
                memset(outPorts[i]->outputData.data(), 0,
                       blockSize * sizeof(float));
            else
                memcpy(outPorts[i]->outputData.data(), inputRef.data(),
                       blockSize * sizeof(float));
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

    const unsigned signalChannels = (inPorts.size() - 1) / 2;
    filter->process(doubleData.data(), doubleData.data() + signalChannels,
                    doubleData.data()[2 * signalChannels], blockSize);

    for (unsigned port = 1; port < unsigned(inPorts.size()); ++port)
        memset(doubleData.data()[port], 0, blockSize * sizeof(double));

    doubleToFloat(doubleData.data(), ports, blockSize, floatData.data());
}

QList<BlockInputPort *> AdaptiveWienerFilterPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> AdaptiveWienerFilterPlugin::outputPorts()
{
    return outPorts;
}

void AdaptiveWienerFilterPlugin::release()
{
}

unsigned AdaptiveWienerFilterPlugin::getLenght() const
{
    return length;
}

void AdaptiveWienerFilterPlugin::setLength(unsigned value)
{
    length = value;
}

double AdaptiveWienerFilterPlugin::getMuInv() const
{
    return muInv;
}

void AdaptiveWienerFilterPlugin::setMuInv(double value)
{
    muInv = value;
}

bool AdaptiveWienerFilterPlugin::getDisabled() const
{
    return disabled;
}

void AdaptiveWienerFilterPlugin::setDisabled(bool value)
{
    disabled = value;
}

// AdaptiveWienerFilterCreator =================================================

QList<BlockDescription> AdaptiveWienerFilterCreator::availableBlocks() const
{
    return QList<BlockDescription>()
           << &AdaptiveWienerFilterPlugin::staticMetaObject;
}

#include "adaptivewienerfilterplugin.moc"
