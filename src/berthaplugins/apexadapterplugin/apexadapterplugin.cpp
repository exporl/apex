/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sann.raymaekers@gmail.com>            *
 * Copyright (C) 2017 Division of Experimental Otorhinolaryngology K.U.Leuven *
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
#include "bertha/global.h"

#include "apexmain/filter/pluginfilterinterface.h"
#include "apextools/apexpluginloader.h"

#include <QObject>
#include <QScopedPointer>
#include <QVariant>

class ApexAdapterPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)

    Q_PROPERTY(QVariantList parameters WRITE setParameters READ getParameters)
    Q_CLASSINFO(
        "parameters-description",
        Q_TR_NOOP("Since bertha only allows one parameter with the same name "
                  "in the experimentdata, this is a convenience property which "
                  "allows you to set multiple apexfilter parameters from the "
                  "experiment data. This is a list of lists, with the inner"
                  " list having format: (Qstring parameterName, int channel, "
                  "QString value)."))

    Q_PROPERTY(QVariantList parameter WRITE setParameter READ getParameter)
    Q_CLASSINFO("parameter-description",
                Q_TR_NOOP("Parameter which will be relayed to the apex"
                          "filter. List format: (Qstring parameterName, int "
                          "channel, QString value)."))

    Q_PROPERTY(QString apexFilter WRITE setApexFilter READ getApexFilter)
    Q_CLASSINFO("apexFilter-description",
                Q_TR_NOOP("Apex filter to be instantiated, set this before "
                          "setting parameters!"))

public:
    Q_INVOKABLE ApexAdapterPlugin(unsigned blockSize, unsigned sampleRate,
                                  int inputPorts, int outputPorts);
    virtual ~ApexAdapterPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    QVariantList getParameter() const;
    void setParameter(const QVariantList &parameter);

    QVariantList getParameters() const;
    void setParameters(const QVariantList &parameters);

    QString getApexFilter() const;
    void setApexFilter(const QString &filterName);

private:
    QString filterName;
    QScopedPointer<PluginFilterInterface> apexFilter;
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;
    // [channels][blockSize]
    QVector<double *> data;

    unsigned blockSize;
    unsigned sampleRate;
    int channels;
};

class ApexAdapterCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "apexadapter")
    Q_INTERFACES(BlockCreatorInterface)
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// ApexAdapterPlugin ===========================================================

ApexAdapterPlugin::ApexAdapterPlugin(unsigned blockSize, unsigned sampleRate,
                                     int inputPorts, int outputPorts)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      channels(qMin(inputPorts, outputPorts))
{
    for (int i = 0; i < channels; ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("input-%1").arg(i + 1)));
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }

    data.resize(channels);
    for (int i = 0; i < channels; ++i) {
        data[i] = (double *)malloc(sizeof(double) * blockSize);
        memset(data[i], 0, sizeof(double) * blockSize);
    }
}

ApexAdapterPlugin::~ApexAdapterPlugin()
{
    for (int i = 0; i < channels; ++i)
        free(data[i]);

    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QList<BlockInputPort *> ApexAdapterPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> ApexAdapterPlugin::outputPorts()
{
    return outPorts;
}

QString ApexAdapterPlugin::prepare(unsigned nrOfFrames)
{
    apexFilter->prepare(nrOfFrames);
    return QString();
}

void ApexAdapterPlugin::process()
{
    // Copy input to data + cast to double
    for (int i = 0; i < channels; ++i) {
        auto input = inPorts[i]->inputData.toStrongRef();
        if (input) {
            for (int j = 0; j < (int)blockSize; ++j)
                data[i][j] = (double)input.data()[j];
        }
    }

    apexFilter->process(data.data());

    // Copy data to output + cast back to float
    for (int i = 0; i < channels; ++i) {
        auto output = outPorts[i]->outputData.data();
        for (int j = 0; j < (int)blockSize; ++j) {
            output[j] = (float)data[i][j];
        }
    }
}

void ApexAdapterPlugin::release()
{
    apexFilter.reset();
}

void ApexAdapterPlugin::setParameter(const QVariantList &parameter)
{
    if (apexFilter.isNull()) {
        qCCritical(EXPORL_BERTHA,
                   "ApexFilter is null! Parameter %s could not be set.",
                   qPrintable(parameter.at(0).toString()));
        return;
    }

    if (parameter.size() == 3)
        apexFilter->setParameter(parameter.at(0).toString(),
                                 parameter.at(1).toInt(),
                                 parameter.at(2).toString());
    else
        qCCritical(
            EXPORL_BERTHA,
            "Parameter should be of size 3, received parameter of size %d",
            parameter.size());
}

QVariantList ApexAdapterPlugin::getParameter() const
{
    /* We don't know the names of the parameters of the underlying filter */
    return QVariantList();
}

void ApexAdapterPlugin::setParameters(const QVariantList &parameters)
{
    Q_FOREACH (const QVariant &parameter, parameters)
        setParameter(parameter.toList());
}

QVariantList ApexAdapterPlugin::getParameters() const
{
    /* We don't know the names of the parameters of the underlying filter */
    return QVariantList();
}

void ApexAdapterPlugin::setApexFilter(const QString &filter)
{
    filterName = filter;
    PluginFilterCreator *creator =
        apex::createPluginCreator<PluginFilterCreator>(filterName);
    apexFilter.reset(
        creator->createFilter(filterName, channels, blockSize, sampleRate));
}

QString ApexAdapterPlugin::getApexFilter() const
{
    return filterName;
}

// ApexAdapterCreator ==========================================================

QList<BlockDescription> ApexAdapterCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &ApexAdapterPlugin::staticMetaObject;
}

#include "apexadapterplugin.moc"
