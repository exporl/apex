/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexdata/device/cohdevicedata.h"

#include "apexmain/mainconfigfileparser.h"

#include "apexmain/stimulus/filter.h"

#include "apextools/exceptions.h"

#include "coh/cohclient.h"
#include "coh/cohlength.h"

#include "common/exception.h"
#include "common/global.h"
#include "common/utils.h"

#include "cohdatablock.h"
#include "cohdevice.h"

#include <QMessageBox>
#include <QThread>

using namespace cmn;
using namespace coh;

Q_DECLARE_LOGGING_CATEGORY(APEX_COH)
Q_LOGGING_CATEGORY(APEX_COH, "apex.coh")

namespace apex
{

namespace stimulus
{

class CohDeviceThread : public QThread
{
public:
    CohDeviceThread();
    virtual ~CohDeviceThread();
};

Q_GLOBAL_STATIC(CohDeviceThread, cohDeviceThread)

// AllDone is called in the DeviceCheckThread, which breaks the communication
// with the CohClients; put all calls the clients in a separate thread, and use
// asynchronous Qt signal/slot magic with some macro sugar to relay to it
#define RELAY(obj, slot) RELAY_ARGS(obj, slot, QGenericReturnArgument())
#define RELAY_ARGS(obj, slot, ...)                                             \
    do {                                                                       \
        if (!QMetaObject::invokeMethod(                                        \
                &obj, slot, Qt::BlockingQueuedConnection, __VA_ARGS__)) {      \
            qCCritical(APEX_COH, "Unable to relay to thread");                 \
        } else if (!obj.p->commandError.isEmpty()) {                           \
            throw Exception(obj.p->commandError);                              \
        }                                                                      \
    } while (0)
#define RELAY_WRAP(command)                                                    \
    Q_ASSERT(QThread::currentThread() == cohDeviceThread);                     \
    p->commandError.clear();                                                   \
    QMutexLocker locker(&p->deviceLock);                                       \
    try {                                                                      \
        command;                                                               \
    } catch (const std::exception &e) {                                        \
        p->commandError = QString::fromLocal8Bit(e.what());                    \
    }

class CohDeviceWorker : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void setup();
    void teardown();
    void send(coh::CohSequence *sequence);
    void start(coh::Coh::TriggerMode trigger);
    void stop();
    coh::Coh::Status status();

public:
    CohDevicePrivate *p;
};

class CohDevicePrivate : public QObject
{
    Q_OBJECT
public:
    bool waitUntil(Coh::Status state, int timeout);
    bool waitUntil(const QSet<Coh::Status> &states, int timeout);
    void showStatus();

public:
    CohDeviceWorker worker;
    QString commandError;

    bool isPlaying;
    // is the device ready to be started?
    bool isReady;
    data::ApexMap *map;
    int triggerType;
    QString deviceName;
    // how many powerup frames to send before starting streaming
    int powerupCount;
    // is powerup already sent for this sequence?
    bool isPowerupSent;
    // length of the current stimulus in us
    int stimulusLength;
    // timer that will be started when the stimulus starts
    QTime timeSinceStart;
    // current output volume, in % (0-100)
    float volume;
    QString driverName;
    QScopedPointer<CohClient> device;
    QMutex deviceLock;
};

// CohDeviceThread =============================================================

CohDeviceThread::CohDeviceThread()
{
    qCDebug(APEX_THREADS, "Constructing CohDeviceThread");
}

CohDeviceThread::~CohDeviceThread()
{
    qCDebug(APEX_THREADS, "Destroying CohDeviceThread");
    // Wait for the thread to finish before we destruct it
    this->quit();
    this->wait();
}

// CohDeviceWorker =============================================================

void CohDeviceWorker::setup()
{
    RELAY_WRAP(p->device.reset(CohClient::createCohClient(p->driverName)));
}

void CohDeviceWorker::teardown()
{
    RELAY_WRAP(p->device.reset());
}

void CohDeviceWorker::send(CohSequence *sequence)
{
    RELAY_WRAP(p->device->send(sequence));
}

void CohDeviceWorker::start(Coh::TriggerMode trigger)
{
    RELAY_WRAP(p->device->start(trigger));
}

void CohDeviceWorker::stop()
{
    RELAY_WRAP(p->device->stop());
}

Coh::Status CohDeviceWorker::status()
{
    RELAY_WRAP(return p->device->status());
    return Coh::Status(0);
}

// CohDevicePrivate ============================================================

void CohDevicePrivate::showStatus()
{
    Coh::Status state = Coh::Status(0);
    RELAY_ARGS(worker, "status", Q_RETURN_ARG(coh::Coh::Status, state));
    qCDebug(APEX_COH, "Status of device %s is %s", qPrintable(deviceName),
            qPrintable(enumValueToKey(Coh::staticMetaObject, "Status", state)));
}

bool CohDevicePrivate::waitUntil(Coh::Status state, int timeout)
{
    return waitUntil(QSet<Coh::Status>() << state, timeout);
}

bool CohDevicePrivate::waitUntil(const QSet<Coh::Status> &states, int timeout)
{
    qCDebug(APEX_COH) << "waitUntil for device" << deviceName << "in states"
                      << states << "until" << timeout;

    QTime timer;
    timer.start();

    Q_FOREVER {
        Coh::Status state = Coh::Status(0);
        RELAY_ARGS(worker, "status", Q_RETURN_ARG(coh::Coh::Status, state));
        showStatus();

        if (states.contains(state))
            return true;

        if (state == Coh::Error)
            throw Exception(tr("error received"));

        if (timeout && timer.elapsed() > timeout)
            return false;

        QThread::msleep(250);
    }
}

// CohDevice ===================================================================

CohDevice::CohDevice(data::CohDeviceData *params)
    : OutputDevice(params), dataPtr(new CohDevicePrivate)
{
    E_D(CohDevice);

    d->isPlaying = false;
    d->isReady = false;
    d->map = params->map();
    d->triggerType = params->triggerType();
    d->deviceName = params->device();
    d->powerupCount = params->powerupCount();
    d->isPowerupSent = false;
    d->stimulusLength = 0;
    d->volume = params->volume();
#ifdef Q_OS_WIN32
    d->driverName =
        apex::MainConfigFileParser::Get().data().cohDriverName(d->deviceName);
#else
    static int dumpFileCounter = 0;
    d->driverName =
        QString::fromLatin1("protoslave: dump: coh-stimulus-dump-%1.txt")
            .arg(++dumpFileCounter);
#endif

    d->worker.p = d;
    d->worker.moveToThread(cohDeviceThread);
    cohDeviceThread->start();

    qCDebug(APEX_COH, "Initializing %s (%s)", qPrintable(d->driverName),
            qPrintable(d->deviceName));

    RELAY(d->worker, "setup");

    d->showStatus();
}

CohDevice::~CohDevice()
{
    E_D(CohDevice);

    try {
        RELAY(d->worker, "teardown");
    } catch (...) {
        // ignore all exceptions in destructor
    }

    delete dataPtr;
}

void CohDevice::StopAll()
{
    E_D(CohDevice);

    qCDebug(APEX_COH, "StopAll");

    RELAY(d->worker, "stop");
    d->stimulusLength = 0;
    d->isPlaying = false;
    d->isReady = false;
}

void CohDevice::RemoveAll()
{
    qCDebug(APEX_COH, "RemoveAll");

    StopAll();
}

void CohDevice::SetSequence(const DataBlockMatrix *sequence)
{
    E_D(CohDevice);

    qCDebug(APEX_COH, "SetSequence");

    RELAY(d->worker, "stop");
    d->stimulusLength = 0;

    const unsigned nSeq = sequence->mf_nGetBufferSize();
    const unsigned nPar = sequence->mf_nGetChannelCount();

    if (nSeq == 0 && nPar == 0) {
        qCDebug(APEX_COH, "not playing, playmatrix empty");
        return;
    }

    Q_ASSERT(nPar <= 1);

    for (unsigned j = 0; j < nPar; ++j) {
        for (unsigned i = 0; i < nSeq; ++i) {
            CohDataBlock *pCur = (CohDataBlock *)(*sequence)(j, i);
            if (pCur) {
                AddInput(*pCur);
                d->isReady = true;
            }
        }
    }
}

void CohDevice::AddInput(const DataBlock &dataBlock)
{
    E_D(CohDevice);

    Q_ASSERT(dataBlock.GetDevice() == GetID());

    if (!d->isPowerupSent && d->powerupCount > 0) {
        float powerup_period = 70;
        QScopedPointer<CohSequence> powerups(new CohSequence(d->powerupCount));
        powerups->append(new CohNullStimulus(powerup_period, false));
        RELAY_ARGS(d->worker, "send", QGenericReturnArgument(),
                   Q_ARG(coh::CohSequence *, powerups.data()));
        d->stimulusLength += d->powerupCount * powerup_period;
        d->isPowerupSent = true;
    }

    QScopedPointer<CohSequence> sequence(
        static_cast<const CohDataBlock &>(dataBlock).mappedData(d->map,
                                                                d->volume));
    d->stimulusLength += ciSequenceLength(sequence.data());
    RELAY_ARGS(d->worker, "send", QGenericReturnArgument(),
               Q_ARG(coh::CohSequence *, sequence.data()));

    d->showStatus();
}

void CohDevice::PlayAll(void)
{
    E_D(CohDevice);

    qCDebug(APEX_COH, "PlayAll");

    if (!d->isReady) {
        qCCritical(APEX_COH, "PlayAll called while not ready");
        return;
    }

    if (d->isPlaying) {
        qCCritical(APEX_COH, "PlayAll called while playing");
        return;
    }

    RELAY_ARGS(
        d->worker, "start", QGenericReturnArgument(),
        Q_ARG(coh::Coh::TriggerMode, d->triggerType == data::TRIGGER_IN
                                         ? Coh::External
                                         : d->triggerType == data::TRIGGER_OUT
                                               ? Coh::Bilateral
                                               : Coh::Immediate));
    d->showStatus();

    if ((d->triggerType == data::TRIGGER_IN &&
         !d->waitUntil(Coh::Waiting, 5000)) ||
        (d->triggerType != data::TRIGGER_IN &&
         !d->waitUntil(Coh::Streaming, 5000))) {
        RELAY(d->worker, "stop");
        d->isPlaying = false;

        QMessageBox::critical(
            0, "Timeout",
            QL1S("Timeout waiting for L34\n") +
                (d->triggerType == data::TRIGGER_IN
                     ? QL1S("Check the trigger switch on the POD and restart "
                            "the experiment.")
                     : QL1S("Check your stimuli and the hardware.")),
            QMessageBox::Abort, QMessageBox::NoButton);

        // Exception below will not stop the experiment, so try to reset the
        // device sensibly (for debugging)
        d->timeSinceStart.restart();
        d->isPlaying = true;
        d->isReady = false;
        d->isPowerupSent = false;

        throw Exception(tr("L34 timeout passed"));
    }

    d->timeSinceStart.restart();
    d->isPlaying = true;
    d->isReady = false;
    d->isPowerupSent = false;
}

bool CohDevice::AllDone()
{
    E_D(CohDevice);

    qCDebug(APEX_COH, "AllDone");

    if (d->isPlaying == false)
        return true;

    if (d->stimulusLength) {
        int sleeptime = d->stimulusLength / 1000 - d->timeSinceStart.elapsed();
        if (sleeptime > 0) {
            qCDebug(APEX_COH, "Waiting %d ms for stimulation to end (total "
                              "stimulus duration=%d ms)",
                    sleeptime, d->stimulusLength / 1000);
            QThread::msleep(sleeptime);
            qCDebug(APEX_COH, "End wait");
        }
    }

    if (!d->waitUntil(QSet<Coh::Status>() << Coh::Stopped << Coh::Idle,
                      60000)) {
        if (d->triggerType == data::TRIGGER_IN) {
            qCDebug(APEX_COH, "Timeout waiting for 'stopped' status");
            qCDebug(APEX_COH, "Trigger did probably not occur.");
            QMessageBox::critical(
                0, "Timeout",
                QString("Timeout waiting for 'stopped' status\n"
                        "Check that the soundcard is set as master device."),
                QMessageBox::Abort, QMessageBox::NoButton);
        } else {
            qCDebug(APEX_COH, "Timeout waiting for 'stopped' status");
            qCDebug(APEX_COH, "Check all connections.");
        }
    }

    RELAY(d->worker, "stop");

    return true;
}

bool CohDevice::SetParameter(const QString &type, int channel,
                             const QVariant &value)
{
    E_D(CohDevice);

    Q_UNUSED(channel);

    qCDebug(APEX_COH, "SetParameter %s=%s", qPrintable(type),
            qPrintable(value.toString()));

    if (type == "volume") {
        bool ok;
        float volume = value.toDouble(&ok);
        if (!ok)
            return false;
        d->volume = qBound(0.0f, volume, 100.0f);
        qCDebug(APEX_COH, "Set volume to %s",
                qPrintable(QString::number(d->volume)));
        mv_bNeedsRestore = true;
        return true;
    }

    return false;
}

void CohDevice::Reset()
{
    E_D(CohDevice);

    d->volume = 100;
}

void CohDevice::Prepare()
{
}

bool CohDevice::CanSequence() const
{
    return true;
}

void CohDevice::AddFilter(Filter &)
{
    qCCritical(APEX_COH, "AddFilter not implemented");
}

void CohDevice::SetSilenceBefore(double)
{
    qCCritical(APEX_COH, "SetSilenceBefore not implemented");
}
}
}

#include "cohdevice.moc"
