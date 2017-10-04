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

class FftwPlan
{
public:
    FftwPlan(fftw_plan plan) : plan(plan)
    {
    }

    ~FftwPlan()
    {
        fftw_destroy_plan(plan);
    }

    fftw_plan plan;

private:
    FftwPlan(const FftwPlan &other);
    FftwPlan &operator=(const FftwPlan &other);
};

class FirBlockCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "firblock")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(firBlockPlugin, FirBlockCreator)
Q_PLUGIN_METADATA(FirBlockPlugin)
Q_PLUGIN_METADATA(FirBlockCreator)

class FirBlockPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(unsigned fftLength READ getFftLength)
    Q_PROPERTY(QString filterCoeffPath READ getFilterCoeffPath WRITE
                   setFilterCoeffPath)
    Q_PROPERTY(unsigned limit READ getLimit WRITE setLimit);

public:
    Q_INVOKABLE FirBlockPlugin(unsigned blockSize, unsigned sampleRate,
                               int inputPorts, int outputPorts,
                               const QVariantMap &readOnlyParameters,
                               const QVariant &toFrequencyDomain);
    virtual ~FirBlockPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output);

    QString getFilterCoeffPath() const;
    void setFilterCoeffPath(QString value);

    unsigned getLimit() const;
    void setLimit(unsigned value);

    unsigned getFftLength() const;

    QVector<double> load(const QString &filePath, unsigned limit);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned filterLength;
    unsigned fftLength;
    unsigned limit;
    QVector<double> inCoeff;
    QVector<float> in;
    QVector<double> timeFirCoeff;
    QVector<double> freqFirCoeff;
    QString filterCoeffPath;

    QList<QSharedPointer<FftwPlan>> plans;
};

// FirBlockPlugin
// ===================================================================

FirBlockPlugin::FirBlockPlugin(unsigned blockSize, unsigned sampleRate,
                               int inputPorts, int outputPorts,
                               const QVariantMap &readOnlyParameters,
                               const QVariant &toFrequencyDomain)
    : filterLength(0),
      fftLength(qMax(
          readOnlyParameters.contains(QLatin1String("fftLength"))
              ? readOnlyParameters.value(QLatin1String("fftLength")).toUInt()
              : 64u,
          blockSize)),
      limit(0),
      inCoeff(fftLength),
      in(fftLength + 2),
      freqFirCoeff(fftLength + 2)

{
    Q_UNUSED(sampleRate);
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

FirBlockPlugin::~FirBlockPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString FirBlockPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);
    timeFirCoeff.fill(0);
    inCoeff.fill(0);
    timeFirCoeff = load(filterCoeffPath, limit);
    memcpy(inCoeff.data(), timeFirCoeff.data(),
           timeFirCoeff.size() * sizeof(double));

    plans.clear();
    for (unsigned i = 0; i < unsigned(inPorts.size()); ++i) {
        plans.append(QSharedPointer<FftwPlan>(new FftwPlan(fftw_plan_dft_r2c_1d(
            fftLength, inCoeff.data(),
            reinterpret_cast<fftw_complex *>(freqFirCoeff.data()),
            FFTW_ESTIMATE))));
    }
    for (unsigned i = 0; i < unsigned(plans.size()); ++i)
        fftw_execute(plans[i]->plan);

    return QString();
}

void FirBlockPlugin::processOne(PortDataType *input, PortDataType *output)
{
    if (!input) {
        memset(in.data(), 0, in.size() * sizeof(PortDataType));
    } else {
        memcpy(in.data(), input, in.size() * sizeof(PortDataType));
    }
    for (unsigned x = 0; x < fftLength + 2; x += 2) {
        output[x] = (in[x] * freqFirCoeff[x] - in[x + 1] * freqFirCoeff[x + 1]);
        output[x + 1] =
            (in[x] * freqFirCoeff[x + 1] + in[x + 1] * freqFirCoeff[x]);
    }
}

void FirBlockPlugin::process()
{
    for (unsigned i = 0; i < unsigned(inPorts.size()); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data());
    }
}

QList<BlockInputPort *> FirBlockPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> FirBlockPlugin::outputPorts()
{
    return outPorts;
}

void FirBlockPlugin::release()
{
}

unsigned FirBlockPlugin::getFftLength() const
{
    return fftLength;
}

QString FirBlockPlugin::getFilterCoeffPath() const
{
    return filterCoeffPath;
}

void FirBlockPlugin::setFilterCoeffPath(QString value)
{
    filterCoeffPath = value;
}

unsigned FirBlockPlugin::getLimit() const
{
    return limit;
}

void FirBlockPlugin::setLimit(unsigned value)
{
    limit = value;
}

QVector<double> FirBlockPlugin::load(const QString &filePath, unsigned limit)
{
    // TODO: use syllib if possible

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
        // throw SylError (tr("Unable to read FIR coefficients "
        //        "from '%1': %2").arg (file.fileName(), file.errorString()));
        cout << "Unable to read FIR coefficients" << endl;

    double doubleChannels;
    if (file.read(reinterpret_cast<char *>(&doubleChannels), 8) != 8)
        // throw SylError (tr("Unable to read number of channels"));
        cout << "Unable to read number of channels" << endl;
    if (doubleChannels < 1)
        // throw SylError (tr("Invalid number of channels"));
        cout << "Invalid number of channels" << endl;
    unsigned channels = qRound(doubleChannels);

    double doubleLength;
    if (file.read(reinterpret_cast<char *>(&doubleLength), 8) != 8)
        // throw SylError (tr("Unable to read filter length"));
        cout << "Unable to read filter length" << endl;
    if (doubleLength < 1 || doubleLength >= INT_MAX / 8)
        // throw SylError (tr("Invalid number of samples"));
        cout << "Invalid number of samples" << endl;
    filterLength = qRound(doubleLength);

    QVector<double> taps(filterLength);
    for (unsigned i = 0; i < channels; ++i) {
        if (file.read(reinterpret_cast<char *>(taps.data()),
                      8 * filterLength) != 8 * filterLength)
            // throw SylError (tr("Unable to read filter coefficients"));
            cout << "Unable to read filter coefficients" << endl;
        if (limit)
            taps.resize(qMin(unsigned(taps.size()), limit));
    }

    return taps;
}

// FftBlockCreator =============================================================

QList<BlockDescription> FirBlockCreator::availableBlocks() const
{
    return QList<BlockDescription>()
           //<< &FirBlockPlugin::staticMetaObject;
           << BlockDescription(&FirBlockPlugin::staticMetaObject,
                               QLatin1String("FirBlock"), true);
}

#include "firblockplugin.moc"
