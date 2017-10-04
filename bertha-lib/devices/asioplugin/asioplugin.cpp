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

#include "bertha/audioconversion.h"
#include "bertha/deviceinterface.h"
#include "bertha/global.h"

#include <QCoreApplication>
#include <QObject>
#include <QScopedPointer>
#include <QVector>

#include <asiosys.h>
// Do not change the order of these includes
#include <asio.h>
#include <asiodrivers.h>
// Do not change the order of these includes
#include "iasiothiscallresolver.h"

#include <windows.h>

// This method is defined in asiodrivers.cpp and *must* be used for
// initialization, ASIOExit() (and maybe others) depends on it
bool loadAsioDriver(char *name);

using namespace bertha;

/** Exception that provides a simple error message. All derived classes must be
 * marked with BERTHA_EXPORT_DECL, otherwise exceptions will just terminate()!
 */
class BERTHA_EXPORT_DECL AsioPluginException : public std::exception
{
    Q_DECLARE_TR_FUNCTIONS(AsioPlugin)
public:
    AsioPluginException(const QString &message, const QString &device,
                        ASIOError errorCode) throw()
        : message(QString::fromLatin1("%4: %2: %3 (%1)")
                      .arg(errorCode)
                      .arg(message, description(errorCode), device)
                      .toLocal8Bit())
    {
    }

    AsioPluginException(const QString &message, ASIOError errorCode) throw()
        : message(QString::fromLatin1("%2: %3 (%1)")
                      .arg(errorCode)
                      .arg(message, description(errorCode))
                      .toLocal8Bit())
    {
    }

    AsioPluginException(const QString &message) throw()
        : message(message.toLocal8Bit())
    {
    }

    virtual ~AsioPluginException() throw()
    {
    }

    virtual const char *what() const throw()
    {
        return message;
    }

private:
    QString description(ASIOError errorCode)
    {
        switch (errorCode) {
        case ASE_OK:
            return tr("ASE_OK - Call succeeded");
        case ASE_SUCCESS:
            return tr("ASE_SUCCESS - ASIOFuture call succeeded");
        case ASE_NotPresent:
            return tr("ASE_NotPresent - Hardware input or output not present "
                      "or available");
        case ASE_HWMalfunction:
            return tr("ASE_HWMalfunction - Hardware is malfunctioning");
        case ASE_InvalidParameter:
            return tr("ASE_InvalidParameter - Input parameter invalid");
        case ASE_InvalidMode:
            return tr("ASE_InvalidMode - Hardware in a bad mode or used in a "
                      "bad mode");
        case ASE_SPNotAdvancing:
            return tr("ASE_SPNotAdvancing - Hardware is not running when "
                      "sample position is inquired");
        case ASE_NoClock:
            return tr("ASE_NoClock - Sample clock or rate cannot be "
                      "determined or not present");
        case ASE_NoMemory:
            return tr("ASE_NoMemory - Not enough memory for completing the "
                      "request");
        default:
            return tr("Unknown error code");
        }
    }

    const QByteArray message;
};

class AsioPluginPrivate;

class AsioPlugin : public QObject, public DeviceInterface
{
    Q_OBJECT
    Q_INTERFACES(DeviceInterface)
    Q_PROPERTY(QString device WRITE setDevice READ getDevice)
public:
    Q_INVOKABLE AsioPlugin(unsigned blockSize, unsigned sampleRate,
                           int inputPorts, int outputPorts);
    virtual ~AsioPlugin();

    virtual QList<BlockOutputPort *> outputPorts();
    virtual QList<BlockInputPort *> inputPorts();

    virtual void setCallback(Callback function, void *data);

    virtual QString prepare();
    virtual QString start();
    virtual void stop();
    virtual void release();

    virtual bool processOutput();
    virtual bool processInput();

private:
    QString getDevice() const;
    void setDevice(const QString &device);

    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    const unsigned blockSize;
    const unsigned sampleRate;
    const unsigned inputPortCount;
    const unsigned outputPortCount;

    Callback callback;
    void *callbackData;

    QString device;

    QScopedPointer<AsioPluginPrivate> d;

    friend class AsioPluginPrivate;
};

class AsioPluginPrivate
{
    Q_DECLARE_TR_FUNCTIONS(AsioPlugin)
public:
    AsioPluginPrivate(AsioPlugin *p, unsigned outputChannels,
                      unsigned inputChannels);
    ~AsioPluginPrivate();

    static ASIOError eh(ASIOError err, AsioPluginPrivate *instance,
                        const QString &message);

    void init();
    void start();
    void stop();

    void processOutput();
    void processInput();

    void processOneInput(PortDataType *input, unsigned index);

    static long asioMessages(long selector, long value, void *message,
                             double *opt);
    static void asioSampleRateChanged(ASIOSampleRate sampleRate);
    static void asioBufferSwitch(long bufferIndex, ASIOBool directProcess);

    const unsigned inputChannels;  // TODO
    const unsigned outputChannels; // TODO

    bool running;

public:
    static AsioPluginPrivate *instance;

private:
    AsioPlugin *p;

    QScopedPointer<AudioConversion> conversion;

    // Asio driver info obtained from the driver
    ASIODriverInfo driverInfo;
    long inputChannelsAvailable;
    long outputChannelsAvailable;
    long bufferMinSize;
    long bufferMaxSize;
    long bufferPreferredSize;
    long bufferGranularity;
    // Asio driver configuration sent to the driver
    QVector<ASIOBufferInfo> bufferInfos;
    QVector<ASIOChannelInfo> channelInfos;
    ASIOCallbacks callbacks;
    // Callback information
    long currentBuffer;
};

class AsioCreator : public QObject, public DeviceCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(DeviceCreatorInterface)
    Q_PLUGIN_METADATA(IID "asio")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

AsioPluginPrivate *AsioPluginPrivate::instance = NULL;

// Q_EXPORT_PLUGIN2(asio, AsioCreator)
Q_PLUGIN_METADATA(asio)
Q_PLUGIN_METADATA(AsioCreator)

ASIOError AsioPluginPrivate::eh(ASIOError err, AsioPluginPrivate *instance,
                                const QString &message)
{
    if (err != ASE_OK)
        throw instance ? AsioPluginException(message, instance->p->device, err)
                       : AsioPluginException(message, err);
    return err;
}

AsioPluginPrivate::AsioPluginPrivate(AsioPlugin *p, unsigned outputChannels,
                                     unsigned inputChannels)
    : inputChannels(inputChannels),
      outputChannels(outputChannels),
      running(false),
      p(p)
{
    if (instance)
        throw AsioPluginException(
            tr("Unable to instantiate second instance of the "
               "Asio sound interface"));

    QByteArray device = p->device.toLatin1();
    if (device.isEmpty()) {
        AsioDrivers drivers;
        const static unsigned nameLength = 32;
        char **namePointer = new char *[10];
        int number;
        for (int i = 0; i < 10; ++i)
            namePointer[i] = new char[nameLength];
        number = drivers.getDriverNames(namePointer, 10);
        if (number > 0) {
            device = namePointer[0];
            p->setDevice(QString::fromLatin1(device));
        } else
            throw AsioPluginException(tr("Unable to find an asio driver"));
    }

    if (!loadAsioDriver(device.data()))
        throw AsioPluginException(tr("Unable to load asio driver for %1")
                                      .arg(QString::fromLatin1(device)));

    driverInfo.asioVersion = 0;
    eh(ASIOInit(&driverInfo), NULL,
       tr("Unable to initialize driver: %1")
           .arg(QString::fromLocal8Bit(driverInfo.name)));

    try {
        init();
    } catch (...) {
        ASIOExit();
        throw;
    }

    // should be totally at the end
    instance = this;
}

AsioPluginPrivate::~AsioPluginPrivate()
{
    ASIOExit();

    instance = NULL;
}

void AsioPluginPrivate::init()
{
    // General configuration
    eh(ASIOGetChannels(&inputChannelsAvailable, &outputChannelsAvailable), this,
       tr("Unable to retrieve number of channels"));
    eh(ASIOGetBufferSize(&bufferMinSize, &bufferMaxSize, &bufferPreferredSize,
                         &bufferGranularity),
       this, tr("Unable to retrieve allowed buffer sizes"));

    eh(ASIOSetSampleRate(p->sampleRate), this, tr("Unable to set sample rate"));

    if (unsigned(bufferMinSize) > p->blockSize ||
        unsigned(bufferMaxSize) < p->blockSize)
        throw AsioPluginException(
            tr("Buffersize %1 is not compatible, minimum: %2"
               ", maximum: %3, preferred: %4")
                .arg(p->blockSize)
                .arg(bufferMinSize)
                .arg(bufferMaxSize)
                .arg(bufferPreferredSize));
    if (inputChannels > unsigned(inputChannelsAvailable))
        throw AsioPluginException(tr("Unable to allocate %1 input channels, "
                                     "only %2 available")
                                      .arg(inputChannels)
                                      .arg(inputChannelsAvailable));

    if (outputChannels > unsigned(outputChannelsAvailable))
        throw AsioPluginException(tr("Unable to allocate %1 output channels, "
                                     "only %2 available")
                                      .arg(outputChannels)
                                      .arg(outputChannelsAvailable));

    // Setup requested input and output channels
    for (unsigned i = 0; i < inputChannels; ++i) {
        ASIOBufferInfo info;
        info.isInput = ASIOTrue;
        info.channelNum = i;
        bufferInfos.append(info);
    }
    for (unsigned i = 0; i < outputChannels; ++i) {
        ASIOBufferInfo info;
        info.isInput = ASIOFalse;
        info.channelNum = i;
        bufferInfos.append(info);
    }

    // Get buffers
    callbacks.bufferSwitch = &asioBufferSwitch;
    callbacks.sampleRateDidChange = &asioSampleRateChanged;
    callbacks.asioMessage = &asioMessages;
    callbacks.bufferSwitchTimeInfo = NULL;

    eh(ASIOCreateBuffers(&bufferInfos[0], bufferInfos.size(), p->blockSize,
                         &callbacks),
       this, tr("Unable to create buffers"));

    for (unsigned i = 0; i < unsigned(bufferInfos.size()); ++i) {
        ASIOChannelInfo info;
        info.isInput = bufferInfos[i].isInput;
        info.channel = bufferInfos[i].channelNum;
        eh(ASIOGetChannelInfo(&info), this,
           tr("Unable to retrieve channel information"));
        channelInfos.append(info);
    }

    if (channelInfos[0].type == ASIOSTFloat32LSB)
        conversion.reset(
            new AudioConversion(AudioConversion::FloatSampleFormat));
    else if (channelInfos[0].type == ASIOSTInt32LSB)
        conversion.reset(
            new AudioConversion(AudioConversion::Int32SampleFormat));
    else if (channelInfos[0].type == ASIOSTInt16LSB)
        conversion.reset(
            new AudioConversion(AudioConversion::Int16SampleFormat));
    else
        throw AsioPluginException(tr("Unable to use encoding"));
}

void AsioPluginPrivate::start()
{
    running = true;

    eh(ASIOStart(), this, tr("Unable to start audio processing"));
}

void AsioPluginPrivate::stop()
{
    ASIOStop();

    running = false;
}

long AsioPluginPrivate::asioMessages(long selector, long value, void *message,
                                     double *opt)
{
    Q_UNUSED(message);
    Q_UNUSED(opt);

    switch (selector) {
    // These are harmless if unsupported
    case kAsioSelectorSupported:
    case kAsioEngineVersion:
    case kAsioSupportsTimeInfo:
    case kAsioSupportsTimeCode:
    case kAsioSupportsInputMonitor:
    case kAsioLatenciesChanged:
        return 0;
    // TODO: If those occur, some handling has to implemented
    case kAsioResetRequest:
    case kAsioBufferSizeChange:
    case kAsioResyncRequest:
    default:
        qFatal("Unsupported asio message: %lu %lu", selector, value);
        return 0;
    }
}

void AsioPluginPrivate::asioSampleRateChanged(ASIOSampleRate sampleRate)
{
    qFatal("Unsupported asio sample rate change: %f", sampleRate);
}

void AsioPluginPrivate::asioBufferSwitch(long bufferIndex,
                                         ASIOBool directProcess)
{
    Q_UNUSED(directProcess);

    instance->currentBuffer = bufferIndex;

    if (!instance->running)
        return;

    instance->p->callback(instance->p->callbackData);
}

void AsioPluginPrivate::processOutput()
{
    for (unsigned i = 0; i < p->outputPortCount; ++i)
        conversion->from(bufferInfos[i].buffers[currentBuffer],
                         p->outPorts[i]->outputData.data(), p->blockSize);
}

void AsioPluginPrivate::processOneInput(PortDataType *input, unsigned index)
{
    if (!input) {
        memset(bufferInfos[inputChannels + index].buffers[currentBuffer], 0,
               p->blockSize * conversion->sampleWidth());
    } else {
        conversion->to(
            input, bufferInfos[inputChannels + index].buffers[currentBuffer],
            p->blockSize);
    }
}

void AsioPluginPrivate::processInput()
{
    for (unsigned i = 0; i < p->inputPortCount; ++i)
        processOneInput(p->inPorts[i]->inputData.toStrongRef().data(), i);
}

// AsioPlugin ==================================================================

AsioPlugin::AsioPlugin(unsigned blockSize, unsigned sampleRate, int inputPorts,
                       int outputPorts)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      inputPortCount(qMax(inputPorts, 0)),
      outputPortCount(qMax(outputPorts, 0)),
      callback(NULL),
      callbackData(NULL)
{
    for (unsigned i = 0; i < inputPortCount; ++i)
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("playback-%1").arg(i + 1)));
    for (unsigned i = 0; i < outputPortCount; ++i)
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("capture-%1").arg(i + 1)));
}

AsioPlugin::~AsioPlugin()
{
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString AsioPlugin::prepare()
{
    try {
        d.reset(new AsioPluginPrivate(this, inputPortCount, outputPortCount));
    } catch (const std::exception &e) {
        return QString::fromLocal8Bit(e.what());
    }
    return QString();
}

QString AsioPlugin::start()
{
    try {
        d->start();
    } catch (const std::exception &e) {
        return QString::fromLocal8Bit(e.what());
    }
    return QString();
}

void AsioPlugin::stop()
{
    d->stop();
}

void AsioPlugin::release()
{
    d.reset();
}

bool AsioPlugin::processOutput()
{
    d->processOutput();
    return true;
}

bool AsioPlugin::processInput()
{
    d->processInput();
    return true;
}

QList<BlockOutputPort *> AsioPlugin::outputPorts()
{
    return outPorts;
}

QList<BlockInputPort *> AsioPlugin::inputPorts()
{
    return inPorts;
}

void AsioPlugin::setCallback(Callback function, void *data)
{
    callback = function;
    callbackData = data;
}

QString AsioPlugin::getDevice() const
{
    return device;
}

void AsioPlugin::setDevice(const QString &device)
{
    this->device = device;
}

// AsioCreator =================================================================

QList<BlockDescription> AsioCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &AsioPlugin::staticMetaObject;
}

#include "asioplugin.moc"
