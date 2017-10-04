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

#include <cmath>

#include <QStringList>

#define PI 3.14159265358979323846
#define PI_2 1.57079632679489661923

class FadeCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "fade")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(fade, FadeCreator)
Q_PLUGIN_METADATA(fade)
Q_PLUGIN_METADATA(FadeCreator)

class FadePlugin : public QObject, public BlockInterface
{
    // TODO: add fadeOut

    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(bool disabled WRITE setDisabled READ getDisabled)
    Q_PROPERTY(
        unsigned fadeInDuration WRITE setFadeInDuration READ getFadeInDuration)
    Q_PROPERTY(bool cosine WRITE setCosine READ getCosine)
    Q_CLASSINFO("disabled-description", Q_TR_NOOP("Boolean disabled"))
    Q_CLASSINFO("fadeInDuration-description",
                Q_TR_NOOP("fadeInDuration in number of frames"))
    Q_CLASSINFO(
        "cosine-description",
        Q_TR_NOOP("boolean specifies if cosine or linear fade should be used"))
public:
    Q_INVOKABLE FadePlugin(unsigned blockSize, unsigned sampleRate,
                           int inputPorts, int outputPorts);
    virtual ~FadePlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output);

    bool getDisabled() const;
    void setDisabled(bool value);

    unsigned getFadeInDuration() const;
    void setFadeInDuration(unsigned value);

    bool getCosine() const;
    void setCosine(bool value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    bool disabled;
    unsigned fadeInDuration;

    bool cosine;

    unsigned totalFrames;
    unsigned position;
};

// FadePlugin ==================================================================

FadePlugin::FadePlugin(unsigned blockSize, unsigned sampleRate, int inputPorts,
                       int outputPorts)
    : blockSize(blockSize),
      disabled(false),
      fadeInDuration(0.5 * sampleRate),
      cosine(false)
{
    Q_UNUSED(sampleRate);

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

FadePlugin::~FadePlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString FadePlugin::prepare(unsigned numberOfFrames)
{
    position = 0;
    totalFrames = numberOfFrames;
    fadeInDuration = qMin(totalFrames, fadeInDuration);
    return QString();
}

void FadePlugin::processOne(PortDataType *input, PortDataType *output)
{
    if (!input) {
        memset(output, 0, blockSize * sizeof(PortDataType));
    } else if (disabled) {
        memcpy(output, input, blockSize * sizeof(PortDataType));
    } else {
        for (unsigned i = 0; i < blockSize; ++i) {
            if ((position + i) < fadeInDuration) {
                double factor = (position + i) / fadeInDuration;
                if (cosine)
                    factor = 1 - cos(factor * PI_2);
                output[i] = input[i] * factor;
            } else {
                output[i] = input[i];
            }
            // TODO: add a way to process fadeout here
        }
    }
}

void FadePlugin::process()
{
    for (int i = 0; i < inPorts.size(); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data());
    }
    position += blockSize;
}

QList<BlockInputPort *> FadePlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> FadePlugin::outputPorts()
{
    return outPorts;
}

void FadePlugin::release()
{
}

bool FadePlugin::getDisabled() const
{
    return disabled;
}

void FadePlugin::setDisabled(bool value)
{
    disabled = value;
}

unsigned FadePlugin::getFadeInDuration() const
{
    return fadeInDuration;
}

void FadePlugin::setFadeInDuration(unsigned value)
{
    fadeInDuration = value;
}

bool FadePlugin::getCosine() const
{
    return cosine;
}

void FadePlugin::setCosine(bool value)
{
    cosine = value;
}

// FadeCreator =================================================================

QList<BlockDescription> FadeCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &FadePlugin::staticMetaObject;
}

#include "fadeplugin.moc"
