/******************************************************************************
 * Copyright (C) 2008  Tom Francart                                           *
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

#include <QStringList>

#ifdef Q_OS_WIN32
#include <windows.h> // Sleep()
#endif
#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

void milliSleep(unsigned millis)
{
#ifdef Q_OS_WIN32
    Sleep(millis);
#else
    usleep(1000 * millis);
#endif
}

class DelayPluginCreator : public QObject, public BlockCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockCreatorInterface)
    Q_PLUGIN_METADATA(IID "delay")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

// Q_EXPORT_PLUGIN2(delayPlugin, DelayPluginCreator)
Q_PLUGIN_METADATA(DelayPluginCreator)

class DelayPlugin : public QObject, public BlockInterface
{
    Q_OBJECT
    Q_INTERFACES(BlockInterface)
    Q_PROPERTY(int delay WRITE setDelay READ getDelay)
    Q_CLASSINFO("delay-description", Q_TR_NOOP("Delay in milliseconds"))
public:
    Q_INVOKABLE DelayPlugin(unsigned blockSize, unsigned sampleRate,
                            int inputPorts, int outputPorts);
    virtual ~DelayPlugin();

    virtual QList<BlockInputPort *> inputPorts();
    virtual QList<BlockOutputPort *> outputPorts();

    virtual QString prepare(unsigned nrOfFrames);
    virtual void process();
    virtual void release();

    void processOne(PortDataType *input, PortDataType *output);

    int getDelay() const;
    void setDelay(int value);

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    unsigned blockSize;

    int delay; // delay in ms
};

// DelayPlugin =================================================================

DelayPlugin::DelayPlugin(unsigned blockSize, unsigned sampleRate,
                         int inputPorts, int outputPorts)
    : blockSize(blockSize), delay(0)
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

DelayPlugin::~DelayPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString DelayPlugin::prepare(unsigned numberOfFrames)
{
    Q_UNUSED(numberOfFrames);

    return QString();
}

void DelayPlugin::processOne(PortDataType *input, PortDataType *output)
{
    if (!input) {
        memset(output, 0, blockSize * sizeof(PortDataType));
    } else {
        memcpy(output, input, blockSize * sizeof(PortDataType));
    }
}

void DelayPlugin::process()
{
    milliSleep(delay);
    for (int i = 0; i < inPorts.size(); ++i) {
        processOne(inPorts[i]->inputData.toStrongRef().data(),
                   outPorts[i]->outputData.data());
    }
}

QList<BlockInputPort *> DelayPlugin::inputPorts()
{
    return inPorts;
}

QList<BlockOutputPort *> DelayPlugin::outputPorts()
{
    return outPorts;
}

void DelayPlugin::release()
{
}

void DelayPlugin::setDelay(int value)
{
    delay = value;
}

int DelayPlugin::getDelay() const
{
    return delay;
}

// DelayPluginCreator ==========================================================

QList<BlockDescription> DelayPluginCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &DelayPlugin::staticMetaObject;
}

#include "delayplugin.moc"
