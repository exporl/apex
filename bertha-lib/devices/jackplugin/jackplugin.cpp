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

#include "bertha/deviceinterface.h"
#include "bertha/global.h"

//#include <Qt/q3networkprotocol.h>

#include <jack/jack.h>

#include <QCoreApplication>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QSharedPointer>

#include <cstdio>
#include <cstdlib>

/**
 * This class is a driver made for jack, make sure Jack is running when using
 * the driver.
 */
class JackPlugin : public QObject, public DeviceInterface
{
    Q_OBJECT
    Q_INTERFACES(DeviceInterface)
public:
    Q_INVOKABLE JackPlugin(unsigned blockSize, unsigned sampleRate,
                           int inputPorts, int outputPorts);
    virtual ~JackPlugin();

    virtual QList<BlockOutputPort *> outputPorts();
    virtual QList<BlockInputPort *> inputPorts();

    virtual void setCallback(Callback function, void *data);

    virtual QString prepare();
    virtual QString start();
    virtual void stop();
    virtual void release();

    virtual bool processOutput();
    virtual bool processInput();

    void processOneInput(PortDataType *input, unsigned index);

private:
    static void jackShutDown(void *instance);
    static int jackCallBack(jack_nframes_t nframes, void *instance);
    static int jackXRun(void *instance);

    QString startJack();

private:
    QList<BlockInputPort *> inPorts;
    QList<BlockOutputPort *> outPorts;

    const unsigned blockSize;
    const unsigned sampleRate;
    const unsigned inputPortCount;
    const unsigned outputPortCount;

    Callback callback;
    void *callbackData;

    jack_client_t *jackClient;
    QList<jack_port_t *> jackInputPorts;
    QList<jack_port_t *> jackOutputPorts;

    QMutex stateMutex;
    volatile bool stateRunning;
};

class JackCreator : public QObject, public DeviceCreatorInterface
{
    Q_OBJECT
    Q_INTERFACES(DeviceCreatorInterface)
    Q_PLUGIN_METADATA(IID "jack")
public:
    virtual QList<BlockDescription> availableBlocks() const;
};

Q_PLUGIN_METADATA(jack)
Q_PLUGIN_METADATA(JackCreator)

static QString jackError(const QString &message, int errorCode)
{
    QStringList result;

    if (errorCode & JackFailure)
        result << JackPlugin::tr("Overall operation failed");
    if (errorCode & JackInvalidOption)
        result << JackPlugin::tr(
            "Operation contained an invalid or unsupported "
            "option");
    if (errorCode & JackNameNotUnique)
        result << JackPlugin::tr("Desired client name was not unique");
    if (errorCode & JackServerStarted)
        result << JackPlugin::tr("JACK server was started as a result of this "
                                 "operation");
    if (errorCode & JackServerFailed)
        result << JackPlugin::tr("Unable to connect to the JACK server");
    if (errorCode & JackServerError)
        result << JackPlugin::tr("Communication error with the JACK server");
    if (errorCode & JackNoSuchClient)
        result << JackPlugin::tr("Requested client does not exist");
    if (errorCode & JackLoadFailure)
        result << JackPlugin::tr("Unable to load internal client");
    if (errorCode & JackInitFailure)
        result << JackPlugin::tr("Unable to initialize client");
    if (errorCode & JackShmFailure)
        result << JackPlugin::tr("Unable to access shared memory");
    if (errorCode & JackVersionError)
        result << JackPlugin::tr("Client's protocol version does not match");

    return QString::fromLatin1("%2: %3 (%1)")
        .arg(errorCode)
        .arg(message, result.join(QLatin1String(". ")));
}

// JackPlugin ==================================================================

JackPlugin::JackPlugin(unsigned blockSize, unsigned sampleRate, int inputPorts,
                       int outputPorts)
    : blockSize(blockSize),
      sampleRate(sampleRate),
      inputPortCount(qMax(inputPorts, 0)),
      outputPortCount(qMax(outputPorts, 0)),
      callback(NULL),
      callbackData(NULL),
      jackClient(NULL),
      stateRunning(false)
{
    for (unsigned i = 0; i < inputPortCount; ++i)
        inPorts.append(
            new BlockInputPort(QString::fromLatin1("playback-%1").arg(i + 1)));
    for (unsigned i = 0; i < outputPortCount; ++i)
        outPorts.append(
            new BlockOutputPort(QString::fromLatin1("capture-%1").arg(i + 1)));
}

JackPlugin::~JackPlugin()
{
    jack_client_close(jackClient);
    qDeleteAll(inPorts);
    qDeleteAll(outPorts);
}

QString JackPlugin::prepare()
{
    jack_status_t status;

    if (jackClient)
        jack_client_close(jackClient);
    jackClient = jack_client_open("bertha", JackNullOption, &status);
    if (!jackClient)
        return jackError(tr("Unable to open JACK client"), status);

    const unsigned jackSampleRate = jack_get_sample_rate(jackClient);
    if (jackSampleRate != sampleRate)
        return tr("Sample rates do not match: "
                  "JACK: %1, required: %2")
            .arg(jackSampleRate)
            .arg(sampleRate);

    const unsigned jackBlockSize = jack_get_buffer_size(jackClient);
    if (jackBlockSize != blockSize)
        return tr("Block sizes do not match: "
                  "JACK: %1, required: %2")
            .arg(jackBlockSize)
            .arg(blockSize);

    jackOutputPorts.clear();
    for (unsigned i = 0; i < inputPortCount; ++i) {
        jack_port_t *port = jack_port_register(
            jackClient, "playback-" + QByteArray::number(i + 1),
            JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
        if (port != NULL)
            jackOutputPorts.append(port);
        else
            return tr("could not create playback port");
    }
    jackInputPorts.clear();
    for (unsigned i = 0; i < outputPortCount; ++i) {
        jack_port_t *port = jack_port_register(
            jackClient, "capture-" + QByteArray::number(i + 1),
            JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
        if (port != NULL)
            jackInputPorts.append(port);
        else
            return tr("could not create capture port");
    }

    // set process callback, xrun, shutdown threads
    jack_set_process_callback(jackClient, jackCallBack, this);
    jack_set_xrun_callback(jackClient, jackXRun, this);
    jack_on_shutdown(jackClient, jackShutDown, this);

    return QString();
}

QString JackPlugin::start()
{
    QMutexLocker locker(&stateMutex);
    stateRunning = true;
    return startJack();
}

void JackPlugin::stop()
{
    QMutexLocker locker(&stateMutex);
    stateRunning = false;
    jack_deactivate(jackClient);
}

void JackPlugin::release()
{
    if (jackClient) {
        jack_client_close(jackClient);
        jackClient = NULL;
    }
}

int JackPlugin::jackCallBack(jack_nframes_t nframes, void *instance)
{
    Q_UNUSED(nframes);
    JackPlugin *const driver = static_cast<JackPlugin *>(instance);

    QMutexLocker locker(&driver->stateMutex);
    if (!driver->stateRunning)
        return 0;

    driver->callback(driver->callbackData);
    return 0;
}

bool JackPlugin::processOutput()
{
    for (unsigned i = 0; i < outputPortCount; ++i)
        memcpy(outPorts[i]->outputData.data(),
               jack_port_get_buffer(jackInputPorts[i], blockSize),
               blockSize * sizeof(float));

    return true;
}

void JackPlugin::processOneInput(PortDataType *input, unsigned index)
{
    if (!input) {
        memset(jack_port_get_buffer(jackOutputPorts[index], blockSize), 0,
               blockSize * sizeof(float));
    } else {
        memcpy(jack_port_get_buffer(jackOutputPorts[index], blockSize), input,
               blockSize * sizeof(float));
    }
}

bool JackPlugin::processInput()
{
    for (unsigned i = 0; i < inputPortCount; ++i)
        processOneInput(inPorts[i]->inputData.toStrongRef().data(), i);

    return true;
}

void JackPlugin::jackShutDown(void *instance)
{
    Q_UNUSED(instance);
    // TODO
    fprintf(stderr, "Shutdown\n");
    return;
}

int JackPlugin::jackXRun(void *instance)
{
    Q_UNUSED(instance);
    // TODO
    fprintf(stderr, "Buffer Underrun\n");
    return 0;
}

QString JackPlugin::startJack()
{
    int result = jack_activate(jackClient);
    if (result)
        return jackError(tr("Unable to activate client"), result);

    const char **ports;

    ports = jack_get_ports(jackClient, NULL, NULL,
                           JackPortIsPhysical | JackPortIsOutput);
    if (!ports)
        return tr("Unable to find any physical capture ports");
    for (unsigned i = 0; i < outputPortCount; ++i) {
        if (!ports[i])
            return tr("Unable to find enough physical capture ports");
        result = jack_connect(jackClient, ports[i],
                              jack_port_name(jackInputPorts[i]));
        if (result)
            return jackError(tr("Unable to connect input port %1").arg(i + 1),
                             result);
    }
    free(ports);

    ports = jack_get_ports(jackClient, NULL, NULL,
                           JackPortIsPhysical | JackPortIsInput);
    if (!ports)
        return tr("Unable to find any physical playback ports");
    for (unsigned i = 0; i < inputPortCount; ++i) {
        if (!ports[i])
            return tr("Unable to find enough physical playback ports");
        result = jack_connect(jackClient, jack_port_name(jackOutputPorts[i]),
                              ports[i]);
        if (result)
            return jackError(tr("Unable to connect output port %1").arg(i + 1),
                             result);
    }
    free(ports);
    return QString();
}

QList<BlockOutputPort *> JackPlugin::outputPorts()
{
    return outPorts;
}

QList<BlockInputPort *> JackPlugin::inputPorts()
{
    return inPorts;
}

void JackPlugin::setCallback(Callback function, void *data)
{
    callback = function;
    callbackData = data;
}

// JackCreator =================================================================

QList<BlockDescription> JackCreator::availableBlocks() const
{
    return QList<BlockDescription>() << &JackPlugin::staticMetaObject;
}

#include "jackplugin.moc"
