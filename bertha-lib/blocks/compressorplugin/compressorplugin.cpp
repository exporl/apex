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

#include "rmsfunction.h"

#include <QStringList>
#include <QVariant>
#include <QVector>

#include <cmath>

class CompressorPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(
        double rmsWindowLength WRITE setRmsWindowLength READ getRmsWindowLength)
    Q_PROPERTY(double calibrationValue WRITE setCalibrationValue READ
                   getCalibrationValue)
    Q_PROPERTY(double attackTime WRITE setAttackTime READ getAttackTime)
    Q_PROPERTY(double releaseTime WRITE setReleaseTime READ getReleaseTime)
    Q_PROPERTY(
        QVector<double> xBreakPoints WRITE setXBreakPoints READ getXBreakPoints)
    Q_PROPERTY(
        QVector<double> yBreakPoints WRITE setYBreakPoints READ getYBreakPoints)
    Q_PROPERTY(unsigned clippingCounter READ getClippingCounter);
    Q_CLASSINFO("rmsWindowLength-description",
                Q_TR_NOOP("RMS Window Length (ms)"))
    Q_CLASSINFO("calibrationValue-description",
                Q_TR_NOOP("Calibration Value (dB)"))
    Q_CLASSINFO("attackTime-description", Q_TR_NOOP("Attack Time (ms)"))
    Q_CLASSINFO("releaseTime-description", Q_TR_NOOP("Release Time (ms)"))
    Q_CLASSINFO("xBreakPoints-description",
                Q_TR_NOOP("Knees of the IO curve (X axis)"))
    Q_CLASSINFO("yBreakPoints-description",
                Q_TR_NOOP("Knees of the IO curve (Y axis)"))
    Q_CLASSINFO("clippingCounter-description",
                Q_TR_NOOP("Number of clipped samples"))
public:
    Q_INVOKABLE CompressorPlugin(unsigned blockSize, unsigned sampleRate,
                                 int inputPorts, int outputPorts);
    virtual ~CompressorPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output);

    double getRmsWindowLength() const;
    void setRmsWindowLength(double value);

    double getCalibrationValue() const;
    void setCalibrationValue(double value);

    double getAttackTime() const;
    void setAttackTime(double value);

    double getReleaseTime() const;
    void setReleaseTime(double value);

    QVector<double> getXBreakPoints();
    void setXBreakPoints(const QVector<double> &value);

    QVector<double> getYBreakPoints();
    void setYBreakPoints(const QVector<double> &value);

    unsigned getClippingCounter();

    // Implements the low pass filtering of the RMS level (signal smoothing)
    double attackReleaseFilter(double envelope, double rmsLevel) const;
    // Convert linear values to dB(SPL), using the calibrationValue parameter
    double linear2dbSpl(double envelope) const;
    // computes the desired output level from the input level according to the
    // defined I/O curve
    double gain(double dbSpl) const;
    // not const as it modifies clippingCounter
    double softClipping(double gain, double value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    QVector<double> xBreakPoints;
    QVector<double> yBreakPoints;
    QVector<double> userXBreakPoints;
    QVector<double> userYBreakPoints;

    RmsFunction rms;
    unsigned blockSize;
    unsigned sampleRate;
    unsigned clippingCounter;
    double rmsWindowLength;
    double attackTime;
    double attackConstant;
    double releaseTime;
    double releaseConstant;
    double calibrationValue;
    double envelope;
    double linearGain;
};

class CompressorPluginCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "compressor")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(compressorPlugin, CompressorPluginCreator)
Q_PLUGIN_METADATA(CompressorPlugin)
Q_PLUGIN_METADATA(CompressorPluginCreator)

// CompressorPlugin ============================================================

CompressorPlugin::CompressorPlugin(unsigned blockSize, unsigned sampleRate,
                                   int inputPorts, int outputPorts)
    : xBreakPoints(1),
      yBreakPoints(1),
      userXBreakPoints(1),
      userYBreakPoints(1),
      blockSize(blockSize),
      sampleRate(sampleRate),
      clippingCounter(0),
      rmsWindowLength(0),
      attackTime(0),
      attackConstant(0),
      releaseTime(0),
      releaseConstant(0),
      calibrationValue(0),
      envelope(0),
      linearGain(0)
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

CompressorPlugin::~CompressorPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString CompressorPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);
    return QString();
}

void CompressorPlugin::processOne(PortDataType *input, PortDataType *output)
{
    if (!input) {
        memset(output, 0, blockSize * sizeof(PortDataType));
    } else {
        for (unsigned j = 0; j < blockSize; ++j) {
            const double rmsLevel = rms.process(input[j]);
            envelope = attackReleaseFilter(envelope, rmsLevel);
            const double dbSpl = linear2dbSpl(envelope);
            const double compressionGaindB = gain(dbSpl) - dbSpl;
            const double compressionGain = pow(10, (compressionGaindB / 20));
            output[j] = softClipping(compressionGain, input[j]);
        }
    }
}

void CompressorPlugin::process()
{
    for (int i = 0; i < inPorts.size(); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data());
    }
}

QList<BlockInputPort *> CompressorPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> CompressorPlugin::outputPorts()
{
    return outPorts;
}

void CompressorPlugin::release()
{
}

void CompressorPlugin::setRmsWindowLength(double value)
{
    rmsWindowLength = qBound(0.0, value, 1000000.0);
    const unsigned rmsBufferSize =
        qMax(1u, unsigned(sampleRate * rmsWindowLength / 1000));
    rms.setRmsBufferSize(rmsBufferSize);
}

double CompressorPlugin::getRmsWindowLength() const
{
    return rmsWindowLength;
}

void CompressorPlugin::setAttackTime(double value)
{
    attackTime = qBound(0.0, value, 1000000.0);
    attackConstant = pow(exp(-1 / (attackTime * sampleRate / 1000)), 2);
}

double CompressorPlugin::getAttackTime() const
{
    return attackTime;
}

void CompressorPlugin::setReleaseTime(double value)
{
    releaseTime = qBound(0.0, value, 1000000.0);
    releaseConstant = pow(exp(-1 / (releaseTime * sampleRate / 1000)), 2);
}

double CompressorPlugin::getReleaseTime() const
{
    return releaseTime;
}

void CompressorPlugin::setCalibrationValue(double value)
{
    calibrationValue = value;
}

double CompressorPlugin::getCalibrationValue() const
{
    return calibrationValue;
}

void CompressorPlugin::setXBreakPoints(const QVector<double> &value)
{
    // split user/used points, as setYBreakPoints might modify the used points
    userXBreakPoints = value;
    if (userXBreakPoints.isEmpty())
        userXBreakPoints = QVector<double>(1);
    xBreakPoints = userXBreakPoints;
    yBreakPoints.resize(xBreakPoints.size());
    linearGain = yBreakPoints[0] - xBreakPoints[0];
}

QVector<double> CompressorPlugin::getXBreakPoints()
{
    return userXBreakPoints;
}

void CompressorPlugin::setYBreakPoints(const QVector<double> &value)
{
    // split user/used points, as setXBreakPoints might modify the used points
    userYBreakPoints = value;
    if (userYBreakPoints.isEmpty())
        userYBreakPoints = QVector<double>(1);
    yBreakPoints = userYBreakPoints;
    xBreakPoints.resize(yBreakPoints.size());
    linearGain = yBreakPoints[0] - xBreakPoints[0];
}

QVector<double> CompressorPlugin::getYBreakPoints()
{
    return userYBreakPoints;
}

unsigned CompressorPlugin::getClippingCounter()
{
    return clippingCounter;
}

double CompressorPlugin::attackReleaseFilter(double envelope,
                                             double rmsLevel) const
{
    if (envelope < rmsLevel)
        envelope = envelope * attackConstant + rmsLevel * (1 - attackConstant);
    else
        envelope =
            envelope * releaseConstant + rmsLevel * (1 - releaseConstant);
    return envelope;
}

double CompressorPlugin::linear2dbSpl(double envelope) const
{
    return 20 * log10(envelope) + calibrationValue;
}

double CompressorPlugin::gain(double dbSpl) const
{
    if (dbSpl <= xBreakPoints[0])
        return linearGain + dbSpl;

    if (dbSpl >= xBreakPoints[xBreakPoints.size() - 1])
        return yBreakPoints[yBreakPoints.size() - 1];

    unsigned index = 0;

    for (unsigned m = 0; m < unsigned(xBreakPoints.size()); ++m) {
        if (dbSpl <= xBreakPoints[m]) {
            index = m;
            break;
        }
    }

    Q_ASSERT(index > 0);

    const double slope = (yBreakPoints[index] - yBreakPoints[index - 1]) /
                         (xBreakPoints[index] - xBreakPoints[index - 1]);

    return slope * (dbSpl - xBreakPoints[index - 1]) + yBreakPoints[index - 1];
}

double CompressorPlugin::softClipping(double gain, double value)
{
    if (qAbs(gain * value) < 1)
        return gain * value;

    clippingCounter++;
    return gain * value >= 1 ? 0.999 : -.999;
}

// GainPluginCreator ===========================================================

QList<BlockDescription> CompressorPluginCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &CompressorPlugin::staticMetaObject;
}

#include "compressorplugin.moc"
