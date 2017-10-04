/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Hans Moorkens.                                 *
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

#include "bertha/deviceinterface.h"
#include "bertha/global.h"

#include <QCoreApplication>
#include <QUrl>
//#include <QVariant>
#include <QVector>

class ApexSoundCardPlugin : public QObject, public DeviceInterface
{
    Q_OBJECT
    Q_INTERFACES(DeviceInterface)
    Q_PROPERTY(ApexSoundCardPlugin *pointer READ getPointer)

public:
    Q_INVOKABLE ApexSoundCardPlugin(unsigned blockSize, unsigned sampleRate,
                                    int inputPorts, int outputPorts);
    virtual ~ApexSoundCardPlugin();

    virtual QList<BlockOutputPort *> outputPorts();
    virtual QList<BlockInputPort *> inputPorts();

    virtual void setCallback(Callback function, void *data);

    virtual QString prepare();
    virtual QString start();
    virtual void stop();
    virtual void release();

    virtual bool processOutput();
    virtual bool processInput();

    ApexSoundCardPlugin *getPointer();

public Q_SLOTS:
    void apexCallback(QVector<float *> *buffer);

private:
    QList<BlockInputPort *> playbackPortPointers;
    QList<BlockOutputPort *> capturePortPointers;

    const unsigned blockSize;
    const unsigned playbackPortCount;
    const unsigned capturePortCount;

    Callback callback;
    void *callbackData;

    QVector<float *> *apexBuffer;
};

class ApexSoundCardPluginCreator : public QObject, public DeviceCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(DeviceCreatorInterface)
    Q_PLUGIN_METADATA(IID "apexsoundcard")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// ApexSoundCardPlugin
// ==================================================================

ApexSoundCardPlugin::ApexSoundCardPlugin(unsigned blockSize,
                                         unsigned sampleRate, int inputPorts,
                                         int outputPorts)
    : blockSize(blockSize),
      playbackPortCount(qMax(0, inputPorts)),
      capturePortCount(qMax(0, outputPorts)),
      callback(NULL),
      callbackData(NULL)
{
    Q_UNUSED(sampleRate);

    for (unsigned i = 0; i < playbackPortCount; ++i)
        playbackPortPointers.append(
            new BlockInputPort(QString::fromLatin1("playback-%1").arg(i + 1)));
    for (unsigned i = 0; i < capturePortCount; ++i)
        capturePortPointers.append(
            new BlockOutputPort(QString::fromLatin1("capture-%1").arg(i + 1)));
}

ApexSoundCardPlugin::~ApexSoundCardPlugin()
{
    stop();

    qDeleteAll(playbackPortPointers);
    qDeleteAll(capturePortPointers);
}

QString ApexSoundCardPlugin::prepare()
{
    return QString();
}

QString ApexSoundCardPlugin::start()
{
    return QString();
}

void ApexSoundCardPlugin::stop()
{
}

void ApexSoundCardPlugin::release()
{
}

bool ApexSoundCardPlugin::processOutput()
{
    return true;
}

bool ApexSoundCardPlugin::processInput()
{
    for (unsigned i = 0; i < playbackPortCount; ++i) {
        memset(apexBuffer->value(i), 0, blockSize * sizeof(float));
        if (!playbackPortPointers[i]->inputData.isNull())
            memcpy(apexBuffer->value(i),
                   playbackPortPointers[i]->inputData.data(),
                   blockSize * sizeof(float));
    }
    return true;
}

QList<BlockOutputPort *> ApexSoundCardPlugin::outputPorts()
{
    return capturePortPointers;
}

QList<BlockInputPort *> ApexSoundCardPlugin::inputPorts()
{
    return playbackPortPointers;
}

void ApexSoundCardPlugin::setCallback(Callback function, void *data)
{
    callback = function;
    callbackData = data;
}

void ApexSoundCardPlugin::apexCallback(QVector<float *> *buffer)
{
    apexBuffer = buffer;
    callback(callbackData);
}

ApexSoundCardPlugin *ApexSoundCardPlugin::getPointer()
{
    return this;
}

// FileCreator =================================================================

QList<BlockDescription> ApexSoundCardPluginCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &ApexSoundCardPlugin::staticMetaObject;
}

#include "apexsoundcardplugin.moc"
