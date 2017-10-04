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
#include "bertha/global.h"
#include "lsl_cpp.h"

using namespace lsl;

#include <math.h>

#include <QStringList>
#include <QVariant>
#include <typeinfo>
#include <vector>

class LslCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "lsl")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

Q_PLUGIN_METADATA(lsl)
Q_PLUGIN_METADATA(LslCreator)

class LslPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    //    Q_PROPERTY(QString lslserver WRITE setServer READ getServer)
public:
    Q_INVOKABLE LslPlugin(unsigned blockSize, unsigned sampleRate,
                          int inputPorts, int outputPorts);
    virtual ~LslPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    double getLsl() const;
    void setLsl(double value);

    double getRampLength() const;
    void setRampLength(double value); // value in seconds

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;
    unsigned sampleRate;

    QScopedPointer<stream_info> lslStreamInfo;
    QScopedPointer<stream_outlet> lslStreamOutlet;
};

// LslPlugin ==================================================================

LslPlugin::LslPlugin(unsigned blockSize, unsigned sampleRate, int inputPorts,
                     int outputPorts)
    : blockSize(blockSize), sampleRate(sampleRate)
{
    qCDebug(EXPORL_BERTHA, "LslPlugin initialised");

    if (inputPorts < 0)
        inputPorts = 1;

    if (outputPorts < 0)
        outputPorts = 1;

    for (unsigned i = 0; i < unsigned(qMin(inputPorts, outputPorts)); ++i) {
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("input-%1").arg(i + 1)));
    }

    for (unsigned i = 0; i < unsigned(qMin(inputPorts, outputPorts)); ++i) {
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("output-%1").arg(i + 1)));
    }

    lslStreamInfo.reset(
        new stream_info("BERTHA", "Audio", inputPorts, sampleRate, cf_float32));
}

LslPlugin::~LslPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString LslPlugin::prepare(unsigned nrOfFrames)
{
    Q_UNUSED(nrOfFrames);

    qCDebug(EXPORL_BERTHA, "LslPlugin::prepare");

    lslStreamOutlet.reset(new stream_outlet(*lslStreamInfo.data(), blockSize));

    return QString();
}

void LslPlugin::process()
{
    std::vector<std::vector<double>> mychunk(blockSize);

    for (unsigned t = 0; t < blockSize; t++) {
        std::vector<double> s(unsigned(inPorts.size()));

        for (unsigned portidx = 0; portidx < unsigned(inPorts.size());
             portidx++) {
            auto inputRef = inPorts[portidx]->inputData.toStrongRef();
            s[portidx] = inputRef.data()[t];
        }
        mychunk[t] = s;
    }

    // send it to LSL
    lslStreamOutlet->push_chunk(mychunk);
}

QList<BlockInputPort *> LslPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> LslPlugin::outputPorts()
{
    return outPorts;
}

void LslPlugin::release()
{
}

// LslCreator =================================================================

QList<BlockDescription> LslCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &LslPlugin::staticMetaObject;
}

#include "lslplugin.moc"
