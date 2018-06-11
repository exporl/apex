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

#include "apexdata/experimentdata.h"

#include "apexdata/stimulus/stimulusparameters.h"

#include "parameters/parametermanager.h"

#include "runner/experimentrundelegate.h"

#include "stimulus/datablock.h"
#include "stimulus/stimulusoutput.h"

#include "wavstimulus/berthabuffer.h"
#include "wavstimulus/wavdevice.h"

#include "streamapp/utils/checks.h"

#include "calibrationio.h"

#if defined(Q_OS_ANDROID)
#include "../apexandroidnative.h"
#endif

#include <QMessageBox>
#include <QThread>
#include <QTimer>

//#include <appcore/threads/waitableobject.h>
// from libtools
using namespace apex::stimulus;
using namespace utils;

namespace apex
{

class EventThread : public QThread
{
public:
    EventThread()
    {
        qCDebug(APEX_THREADS, "Constructing EventThread");
    }
    ~EventThread()
    {
        qCDebug(APEX_THREADS, "Destroying EventThread");
    }

protected:
    void run()
    {
        exec();
    }
};

class ClippingNotifier : public QObject
{
    Q_OBJECT
public:
    ClippingNotifier(const stimulus::tDeviceMap &devices)
        : devices(devices), clipped(false), timer(new QTimer(this))
    {
        connect(timer, SIGNAL(timeout()), this, SLOT(clipCheck()));
        timer->start(1000);
    }

private Q_SLOTS:
    void clipCheck()
    {
        using namespace apex::device;

        Q_FOREACH (stimulus::tDeviceMap::mapped_type device, devices) {
            tUnsignedBoolMap clippings;
            if (!device->GetInfo(IApexDevice::mc_eClipping, &clippings))
                continue;
            Q_FOREACH (tUnsignedBoolMap::value_type clipping, clippings) {
                if (clipping.second) {
                    Q_EMIT clippingOccured(true);
                    clipped = true;
                    return;
                }
            }
        }
        if (clipped) {
            Q_EMIT clippingOccured(false);
            clipped = false;
        }
    }

Q_SIGNALS:
    void clippingOccured(bool clipping);

private:
    const stimulus::tDeviceMap devices;
    bool clipped;
    QTimer *timer;
};

class CalibrationIOPrivate
{
public:
    CalibrationIOPrivate(ExperimentRunDelegate *runDelegate)
        : runDelegate(runDelegate),
          stimulusOutput(PtrCheck(
              runDelegate->modOutput() /*ApexControl::Get().GetOutput()*/)),
          looping(false)
    {
    }

    ExperimentRunDelegate *runDelegate;
    stimulus::StimulusOutput *const stimulusOutput;
    QString currentStimulus;
    QScopedPointer<ClippingNotifier> clippingNotifier;
    bool looping;
    bool playing;
};

// CalibrationIO ===============================================================

CalibrationIO::CalibrationIO(ExperimentRunDelegate *runDelegate,
                             bool clipChecking)
    : d(new CalibrationIOPrivate(runDelegate))
{

    if (clipChecking) {
        d->clippingNotifier.reset(
            new ClippingNotifier(runDelegate->modOutput()->GetDevices()));
        connect(d->clippingNotifier.data(), SIGNAL(clippingOccured(bool)), this,
                SIGNAL(clippingOccured(bool)));
    }

    // disable continuous for all devices
    Q_FOREACH (const tDeviceMap::mapped_type &dev,
               d->runDelegate->GetDevices()) {
        dev->EnableContinuousMode(false);
    }
    d->playing = false;
}

CalibrationIO::~CalibrationIO()
{
    stopOutput();

    // re-enable continuous mode for all devices
    Q_FOREACH (const tDeviceMap::mapped_type &dev,
               d->runDelegate->GetDevices()) {
        dev->EnableContinuousMode(true);
    }

    // FIXME: set loop the the original value for ALL stimuli
    setLooping(false);

    // d->runDelegate->GetModOutput()->GetStimulusEnd().mp_ResetObject();
}

void CalibrationIO::setStimulus(const QString &name)
{
    // TODO CALIB restart output if running
    // TODO CALIB looping handling broken
    const bool oldLooping = d->looping;
    setLooping(false);
    d->currentStimulus = name;
    setLooping(oldLooping);
}

void CalibrationIO::queueParameter(const QString &name, double value)
{
    ParameterManager *manager = d->runDelegate->GetParameterManager();
    manager->setParameter(name, value, false);
    d->runDelegate->GetModOutput()->SendParametersToClients();
    d->runDelegate->GetModOutput()->PrepareClients();
}

void CalibrationIO::setParameter(const QString &name, double value)
{
    if (!d->playing) {
        queueParameter(name, value);
    } else {
        d->runDelegate->GetModOutput()->HandleParam(name, value);
        d->runDelegate->GetModOutput()->PrepareClients();
    }
}

void CalibrationIO::setLooping(bool looping)
{
    // TODO CALIB no idea if this works during playback
    // TODO CALIB modifies stimuli directly
    // TODO CALIB looping handling broken
    d->looping = looping;

    if (d->currentStimulus.isEmpty())
        return;

    try {
        QMap<QString, tQStringVector> dataBlockMap(
            d->runDelegate->GetStimulus(d->currentStimulus)
                ->ModDeviceDataBlocks());
        Q_FOREACH (const tQStringVector &dataBlocks, dataBlockMap)
            Q_FOREACH (const QString &dataBlock, dataBlocks)
                d->runDelegate->GetDatablock(dataBlock)->SetParameter(
                    "loop", looping ? QString::number(UINT_MAX) : "1");
    } catch (std::exception &e) {
        QMessageBox::critical(
            NULL, tr("Calibration Error"),
            tr("Unable to modify loop state:\n%1").arg(e.what()));
    }
}

void CalibrationIO::startOutput()
{
    qCDebug(APEX_RS, "startOutput()");
#if defined(Q_OS_ANDROID)
    apex::android::ApexAndroidBridge::forceAudioVolumeToMaximum();
#endif
    try {
        stopOutput();

        /* Bertha needs this every start */
        if (d->runDelegate->usingBertha()) {
            BerthaBuffer *berthaBuffer =
                static_cast<WavDevice *>(
                    d->runDelegate->GetDevice(
                        d->runDelegate->getBerthaExperimentData()
                            .device()
                            .id()))
                    ->getWavDeviceIo()
                    ->berthaBuffer.data();
            /* We're not using UINT_MAX since some of
             * apexcompatibledatablockplugin's
             * members would overflow
             */
            QMap<QString, tQStringVector> dataBlockMap(
                d->runDelegate->GetStimulus(d->currentStimulus)
                    ->ModDeviceDataBlocks());
            Q_FOREACH (const tQStringVector &dataBlocks, dataBlockMap)
                Q_FOREACH (const QString &dataBlock, dataBlocks)
                    berthaBuffer->queueParameter(dataBlock, QSL("loops"),
                                                 d->looping ? QSL("5000")
                                                            : "1");
        }

        Stimulus *stimulus = d->runDelegate->GetStimulus(d->currentStimulus);

        /* Parameters are set from ExperimentControl,
         * so all parameters from the calibrationstimulus are set here.
         */
        const QVariantMap &variableParameters(
            stimulus->GetVarParameters()->map());
        Q_FOREACH (const QString &key, variableParameters.keys())
            d->runDelegate->GetParameterManager()->setParameter(
                key, variableParameters.value(key), true);

        const QVariantMap &fixedParameters(stimulus->GetFixParameters()->map());
        Q_FOREACH (const QString &key, fixedParameters.keys())
            d->runDelegate->GetParameterManager()->setParameter(
                key, fixedParameters.value(key), true);

        qCDebug(APEX_RS, "loadStimulus()");
        d->stimulusOutput->LoadStimulus(stimulus, false);
        qCDebug(APEX_RS, "resumeDevides()");
        // use resume since this is not blocking
        d->stimulusOutput->ResumeDevices();
        d->playing = true;
        // TODO CALIB this should be a real notification
        Q_EMIT playingChanged(true);
    } catch (std::exception &e) {
        QMessageBox::critical(
            NULL, tr("Calibration Error"),
            tr("Unable to start calibration:\n%1").arg(e.what()));
    }
}

void CalibrationIO::stopOutput()
{
    try {
        d->stimulusOutput->StopDevices();
        d->stimulusOutput->UnLoadStimulus();

        d->playing = false;
        // TODO CALIB this should be a real notification
        Q_EMIT playingChanged(false);
    } catch (std::exception &e) {
        QMessageBox::critical(
            NULL, tr("Calibration Error"),
            tr("Unable to stop calibration:\n%1").arg(e.what()));
    }
}

} // namespace apex

#include "calibrationio.moc"
