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

#include "syllib/firfilter.h"

#include <QDir>
#include <QMap>
#include <QStringList>

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

class FirFilterCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "firfilter")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2 (firfilter, FirFilterCreator)
Q_PLUGIN_METADATA(firfilter)
Q_PLUGIN_METADATA(FirFilterCreator)

class FirFilterPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(bool disabled WRITE setDisabled READ getDisabled)
    Q_PROPERTY(QString filePath WRITE setFilePath READ getFilePath)
    Q_PROPERTY(unsigned limit WRITE setLimit READ getLimit)
    Q_PROPERTY(bool buffered WRITE setBuffered READ getBuffered)
    Q_CLASSINFO("disabled-description", Q_TR_NOOP("Boolean disabled"))
    Q_CLASSINFO("filePath-description", Q_TR_NOOP("filePath as string"))
    Q_CLASSINFO("limit-description", Q_TR_NOOP("limit as unsigned int"))
    Q_CLASSINFO("buffered-description", Q_TR_NOOP("Boolean buffered"))
public:
    Q_INVOKABLE FirFilterPlugin(unsigned blockSize, unsigned sampleRate,
                                int inputPorts, int outputPorts);
    virtual ~FirFilterPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    bool getDisabled() const;
    void setDisabled(bool value);

    QString getFilePath() const;
    void setFilePath(QString value);

    unsigned getLimit() const;
    void setLimit(unsigned value);

    bool getBuffered() const;
    void setBuffered(bool value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    QString filePath;
    QString newFilePath;
    QList<FirFilter> filters;
    unsigned limit;
    bool disabled;
    bool buffered; // use filters in "asynchronous" mode?
    QMap<QString, QString> parameters;
    QString workDirectory;
};

// FirFilterPlugin =============================================================

FirFilterPlugin::FirFilterPlugin(unsigned blockSize, unsigned sampleRate,
                                 int inputPorts, int outputPorts)
    : blockSize(blockSize), limit(0), disabled(false), buffered(false)
{
    Q_UNUSED(sampleRate);

    if (inputPorts < 0)
        inputPorts = 2;

    if (outputPorts < 0)
        outputPorts = 2;

    for (unsigned i = 0; i < unsigned(qMin(inputPorts, outputPorts)); ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("input-%1").arg(i + 1)));
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }
    workDirectory = QDir::currentPath();
}

FirFilterPlugin::~FirFilterPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString FirFilterPlugin::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);
    try {
        if (filePath != newFilePath) {
            filePath = newFilePath;
            QString filePath(this->filePath);
            QMapIterator<QString, QString> i(parameters);
            while (i.hasNext()) {
                i.next();
                filePath.replace(QString::fromLatin1("${") + i.key() +
                                     QString::fromLatin1("}"),
                                 i.value());
            }
            filters.clear();
            Q_FOREACH (const QVector<double> &taps,
                       FirFilter::load(filePath, limit)) {
                FirFilter filter(taps, !buffered);
                filters.append(filter);
                if (!buffered & (blockSize % filter.length() != 0))
                    return tr("Blocksize %1 is not multiple of FIR"
                              " filter length %2")
                        .arg(blockSize)
                        .arg(filter.length());
            }
        }
    } catch (const std::exception &e) {
        return tr("Unable to instantiate FIR filters: %1")
            .arg(QString::fromLocal8Bit(e.what()));
    }

    if (unsigned(filters.size()) != unsigned(inPorts.size()))
        return tr("Number of channels differ:"
                  " plugin has %1 channels, FIR filter %2")
            .arg(inPorts.size())
            .arg(filters.size());

    return QString();
}

void FirFilterPlugin::process()
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

    for (unsigned port = 1; port < ports; ++port)
        memcpy(doubleData.data()[port], doubleData.data()[0],
               blockSize * sizeof(double));
    for (unsigned port = 0; port < ports; ++port)
        filters[port].process(doubleData.data()[port], blockSize);

    doubleToFloat(doubleData.data(), ports, blockSize, floatData.data());
}

QList<BlockInputPort *> FirFilterPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> FirFilterPlugin::outputPorts()
{
    return outPorts;
}

void FirFilterPlugin::release()
{
}

bool FirFilterPlugin::getDisabled() const
{
    return disabled;
}

void FirFilterPlugin::setDisabled(bool value)
{
    disabled = value;
    parameters.insert(QString::fromLatin1("disabled"), QString(value));
}

QString FirFilterPlugin::getFilePath() const
{
    return filePath;
}

void FirFilterPlugin::setFilePath(QString value)
{
    if (QDir(value).isAbsolute())
        newFilePath = value;
    else
        newFilePath = QDir(workDirectory).absoluteFilePath(value);
    parameters.insert(QString::fromLatin1("uri"), newFilePath);
}

unsigned FirFilterPlugin::getLimit() const
{
    return limit;
}

void FirFilterPlugin::setLimit(unsigned value)
{
    limit = value;
    parameters.insert(QString::fromLatin1("limit"), QString(value));
}

bool FirFilterPlugin::getBuffered() const
{
    return buffered;
}

void FirFilterPlugin::setBuffered(bool value)
{
    buffered = value;
    parameters.insert(QString::fromLatin1("buffered"), QString(value));
}

// FirFilterCreator ============================================================

QList<BlockDescription> FirFilterCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &FirFilterPlugin::staticMetaObject;
}

#include "firfilterplugin.moc"
