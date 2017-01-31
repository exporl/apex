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

#include "apexdata/calibration/calibrationdata.h"

#include "gui/calibrationdialog.h"
#include "gui/calibrationsetupsdialog.h"
#include "gui/mainwindow.h"

#include "parameters/parametermanager.h"

#include "runner/experimentrundelegate.h"

#include "services/errorhandler.h"

#include "autocalibration.h"
#include "calibrationdatabase.h"
#include "calibrationio.h"
#include "calibrator.h"

#include <QMessageBox>
#include <QPointer>

namespace apex
{

using namespace data;

class CalibratedParameter
{
public:
    CalibratedParameter() :
        targetAmplitude(0),
        outputParameter(0)
    {
    }

    CalibratedParameter(double targetAmplitude, double outputParameter) :
        targetAmplitude(targetAmplitude),
        outputParameter(outputParameter)
    {
    }

public:
    double targetAmplitude;
    double outputParameter;
};

class CalibratorPrivate : public QObject
{
    Q_OBJECT
public:
        CalibratorPrivate(ExperimentRunDelegate *runDelegate,
                const data::CalibrationData &data);
        ~CalibratorPrivate();

        void initAutoCalibration();

        private Q_SLOTS:
            void parameterNameChanged(const QString &value);
        void outputParameterChanged();
        void targetAmplitudeChanged();
        void hardwareSetupChanged(const QString &value);
        void stimulusChanged(const QString &value);
        void manageHardwareSetups();
        void correctOutputParameter(double amplitude);
        void saveCalibration();
        void mutingChanged(const QString &name, bool muted);

        void playingChanged(bool playing);
        void autoCalibrateAll();
        void autoCalibrateSingle(const QString& parameter);

        void autoCalibrateSingleFinished(bool success);
        void calibrateManually();
        void beforeCancel();

private:
        void updateWidgetStates();
        void updateWidgetsFromCalibration();
        void updateCalibrationFromWidgets();
        void updateDatabaseState();
        void updateCompleteState();

        double outputParameter(const QString &name) const;
        double targetAmplitude(const QString &name) const;
        double experimentAmplitude(const QString &name) const;
        bool isInCalibrationDatabase(const QString &name) const;
        bool isNewlyCalibrated(const QString &name) const;
        bool calibrationComplete() const;
        void mergeCalibrationResults();
        void discardCalibrationResults();

        void setGlobal(bool global);
        bool isGlobal() const;
        bool global;


public:
        ExperimentRunDelegate *runDelegate;
        QPointer<CalibrationIO> calibrationIO;
        QPointer<CalibrationDialog> dialog;
        QScopedPointer<AutoCalibration> autoCalibration;
        bool autoCalibrationEnabled;
        const data::CalibrationData &data;
        bool playing;
        bool switchAutoManual;

        QString hardwareSetup;
        QMap<QString, CalibratedParameter> results;

        friend class Calibrator;
};

// CalibratorPrivate ===========================================================

CalibratorPrivate::CalibratorPrivate(ExperimentRunDelegate *runDelegate,
        const data::CalibrationData &data) :
    runDelegate(runDelegate),
    autoCalibrationEnabled(false),
    data(data),
    playing(false),
    switchAutoManual(false)
{

}

void CalibratorPrivate::initAutoCalibration()
{
    if (!data.soundLevelMeterData()) {
         autoCalibrationEnabled=false;
         return;
    }
    try {
        if (data.soundLevelMeterData() && !autoCalibration)
            autoCalibration.reset(new AutoCalibration(runDelegate, data, hardwareSetup));
        autoCalibrationEnabled = true;
    } catch (ApexStringException &e) {
        ErrorHandler::Get().addWarning(tr("Calibrator"), e.what());
        ErrorHandler::Get().addWarning(tr("Calibrator"),
                tr("Could not load sound level meter plugin, "
                    "disabling automatic calibration"));
        autoCalibrationEnabled = false;
    }
}


CalibratorPrivate::~CalibratorPrivate()
{
}

double CalibratorPrivate::outputParameter(const QString &name) const
{
    if (isNewlyCalibrated (name))
        return results.value(name).outputParameter;

    if (isInCalibrationDatabase(name))
        return CalibrationDatabase().outputParameter(hardwareSetup,
                data.calibrationProfile(), name);

    return data.parameters().value(name).defaultParameter();
}

double CalibratorPrivate::targetAmplitude(const QString &name) const
{
    if (isNewlyCalibrated(name))
        return results.value(name).targetAmplitude;

    if (isInCalibrationDatabase(name))
        return CalibrationDatabase().targetAmplitude(hardwareSetup,
                data.calibrationProfile(), name);

    return data.parameters().value(name).defaultTargetAmplitude();
}

double CalibratorPrivate::experimentAmplitude(const QString &name) const
{
    return data.parameters().value(name).finalTargetAmplitude();
}

bool CalibratorPrivate::isInCalibrationDatabase(const QString &name) const
{
    return CalibrationDatabase().isCalibrated(hardwareSetup,
            data.calibrationProfile(), name);
}

bool CalibratorPrivate::isNewlyCalibrated(const QString &name) const
{
    return results.contains(name);
}

bool CalibratorPrivate::calibrationComplete() const
{
    Q_FOREACH (const QString &name, data.parameters().keys())
    if (!isInCalibrationDatabase(name) && !isNewlyCalibrated(name))
        return false;

    return true;
}

void CalibratorPrivate::mergeCalibrationResults()
{
    // Fill the calibration database, prepare the calibration result
    CalibrationDatabase database;
    const QString profile = data.calibrationProfile();
    Q_FOREACH (const QString &name, data.parameters().keys())
    {
        if (isNewlyCalibrated(name))
        {
            const CalibratedParameter &parameter = results[name];
            database.calibrate(hardwareSetup, profile, name,
                                parameter.targetAmplitude, parameter.outputParameter);
        }
        else if (isInCalibrationDatabase(name))
        {
            results.insert(name, CalibratedParameter
                            (database.targetAmplitude(hardwareSetup, profile, name),
                             database.outputParameter(hardwareSetup, profile, name)));
        }
        else
        {
            qFatal("Request to merge results but not completely calibrated");
        }
    }
}

void CalibratorPrivate::discardCalibrationResults()
{
    results.clear();
}

void CalibratorPrivate::stimulusChanged(const QString &name)
{
    try
    {
        calibrationIO->setStimulus(name);
        calibrationIO->setLooping(dialog->isLooping());
    }
    catch (std::exception &e)
    {
        QMessageBox::critical(dialog, tr("Calibration Error"),
                               tr("Unable to set stimulus:\n%1").arg(e.what()));
    }
}

void CalibratorPrivate::parameterNameChanged(const QString &value)
{
    const CalibrationParameterData param = data.parameters().value(value);
    dialog->setMinimumOutputParameter(param.minimumParameter());
    dialog->setMaximumOutputParameter(param.maximumParameter());
    dialog->setMeasuredAmplitude(QString());

    updateWidgetsFromCalibration();
    updateWidgetStates();
}

void CalibratorPrivate::targetAmplitudeChanged()
{
    updateWidgetStates();
}

void CalibratorPrivate::outputParameterChanged()
{
    const QString outputParameter = dialog->outputParameter();
    const QString parameterName = dialog->parameterName();

    if (!parameterName.isEmpty() && !outputParameter.isEmpty())
        calibrationIO->setParameter(parameterName, outputParameter.toDouble());

    updateWidgetStates();
}

void CalibratorPrivate::hardwareSetupChanged(const QString &value)
{
    discardCalibrationResults();
    hardwareSetup = value;

    updateWidgetsFromCalibration();
    updateWidgetStates();
    updateCompleteState();
}

void CalibratorPrivate::mutingChanged(const QString &name, bool muted)
{
    //qCDebug(APEX_RS, "Muting change %s %u", qPrintable(name), muted);
    // Currently selected parameter switched on
    const QString parameterName = dialog->parameterName();
    if (parameterName == name && !muted) {
        const QString outputParameter = dialog->outputParameter();
        if (!outputParameter.isEmpty()) {
            /*qCDebug(APEX_RS, "(Un)muting current parameter %s to %f", qPrintable(name),
                    outputParameter.toDouble());*/
            calibrationIO->setParameter(name, outputParameter.toDouble());
        }
        return;
    }
    // all other cases
    const CalibrationParameterData parameter = data.parameters()[name];
    /*qCDebug(APEX_RS, "(Un)muting parameter %s to %f", qPrintable(name),
            muted ? parameter.muteParameter() : outputParameter(name));*/
    calibrationIO->setParameter(name,
                                 muted ? parameter.muteParameter() : outputParameter(name));
}

void CalibratorPrivate::updateCompleteState()
{
    dialog->setComplete(calibrationComplete());
}

void CalibratorPrivate::updateWidgetStates()
{
    dialog->setPlaying(playing);
    if (dialog->hardwareSetup().isEmpty())
        dialog->disable();
    else
        dialog->enable();
}

void CalibratorPrivate::saveCalibration()
{
    const QString targetAmplitude = dialog->targetAmplitude();
    const QString outputParameter = dialog->outputParameter();
    const QString parameterName = dialog->parameterName();

    if (targetAmplitude.isEmpty() || outputParameter.isEmpty() ||
            parameterName.isEmpty())
        return;

    results.insert(parameterName, CalibratedParameter
                    (targetAmplitude.toDouble(), outputParameter.toDouble()));

    updateDatabaseState();
    updateCompleteState();
}

void CalibratorPrivate::setGlobal(bool global) {
    dialog->setGlobal(global);
    this->global = global;
}

bool CalibratorPrivate::isGlobal() const {
    return global;
}

void CalibratorPrivate::updateWidgetsFromCalibration()
{
    const QString name = dialog->parameterName();

    dialog->setTargetAmplitude(QString::number(targetAmplitude(name)));
    dialog->setExperimentAmplitude(QString::number(experimentAmplitude(name)));
    dialog->setOutputParameter(QString::number(outputParameter(name)));

    setGlobal(CalibrationDatabase().isGlobal(hardwareSetup));
    updateDatabaseState();
}

void CalibratorPrivate::updateDatabaseState()
{
    const QString name = dialog->parameterName();

    QString text;
    if (isNewlyCalibrated(name)) {
        text = "<span style=\"background-color:#0f0\">";
        text += tr ("Newly calibrated");
    } else if (isInCalibrationDatabase(name)) {
        if(isGlobal()) {
            text = "<span style=\"background-color:#00f; color:#fff\">";
            text += tr("Found in calibration database (global)");
        } else {
            text = "<span style=\"background-color:#0f0\">";
            text += tr("Found in calibration database");
        }
    } else {
        text = "<span style=\"background-color:#ff0\">";
        text += tr("Not calibrated");
    }
    text += "</span>";
    dialog->setDatabaseStatus(text);
}

void CalibratorPrivate::correctOutputParameter(double currentAmplitude)
{
    bool targetAmplitudeOk, currentParameterOk;

    const double targetAmplitude =
        dialog->targetAmplitude().toDouble(&targetAmplitudeOk);
    const double currentParameter =
        dialog->outputParameter().toDouble(&currentParameterOk);
    if (!targetAmplitudeOk || !currentParameterOk)
        return;

    const double correctedParameter = qBound
                                      (dialog->minimumOutputParameter(),
                                       currentParameter + targetAmplitude - currentAmplitude,
                                       dialog->maximumOutputParameter());

    dialog->setOutputParameter(QString::number(correctedParameter));
    dialog->setMeasuredAmplitude(QString());

    dialog->setDatabaseStatus("<span style=\"background-color:#ff0\">"
                               + tr("Click apply to keep") + "</span>"
                              );



}

void CalibratorPrivate::playingChanged(bool value)
{
    playing = value;
    if (dialog)
        updateWidgetStates();
}

void CalibratorPrivate::manageHardwareSetups()
{
    CalibrationSetupsDialog setupsDialog(dialog);
    setupsDialog.exec();

    dialog->setHardwareSetups(CalibrationDatabase().hardwareSetups());
}


void CalibratorPrivate::autoCalibrateAll()
{
    Q_ASSERT(autoCalibrationEnabled);
    Q_ASSERT(data.soundLevelMeterData());
    CalibrationDatabase().setCurrentHardwareSetup(hardwareSetup);
    switchAutoManual = true;
    dialog->reject();
}


void CalibratorPrivate::autoCalibrateSingle(const QString &parameter)
{
    Q_ASSERT(data.soundLevelMeterData());
    Q_ASSERT(autoCalibrationEnabled);

    dialog->setDatabaseStatus("<span style=\"background-color:#3ff\">"
                               + tr("Calibrating %1 for %2 seconds")
                                                           .arg(parameter)
                               .arg(data.soundLevelMeterData()->valueByType("time").toInt())
                               + "</span>");
    autoCalibration->setHardwareSetup(hardwareSetup);

    if(autoCalibration->autoCalibrateSingle(true,parameter))
        dialog->disable();
    else
        QMessageBox::information(0,"AutoCalibrateSingle failed",
                autoCalibration->errorString());
}


void CalibratorPrivate::autoCalibrateSingleFinished(bool success)
{
    Q_ASSERT(data.soundLevelMeterData());
    if(success) {
        qCDebug(APEX_RS, "Measurement succesfull");
                qCDebug(APEX_RS, "%s", qPrintable(QString::fromLatin1("Measured value: %1").arg(autoCalibration->result())));
                dialog->setMeasuredAmplitude(QString::number(autoCalibration->result()));
        correctOutputParameter(autoCalibration->result());
    } else {
        qCDebug(APEX_RS, "Measurement failed: %s", + qPrintable(autoCalibration->errorString()));
        dialog->setDatabaseStatus(
                tr("<span style=\"color:#fff;background-color:#f00\">%1</span>")
                .arg(autoCalibration->errorString()));
    }
    dialog->enable();
}

void CalibratorPrivate::calibrateManually()
{
    this->switchAutoManual = true;
    CalibrationDatabase().setCurrentHardwareSetup(autoCalibration->hardwareSetup);
}

void CalibratorPrivate::beforeCancel()
{
    qCDebug(APEX_RS, "beforeCancel()");
    bool save = true;
    if (!results.size() == 0) {
        QMessageBox::StandardButton answer =
           QMessageBox::question(0, "Save calibration?",
                                  "You have made changes to the calibration.\n"
                                  "Do you want to save the results?",
                                  QMessageBox::Discard | QMessageBox::Save,
                                  QMessageBox::Discard);
        if (answer == QMessageBox::Discard) {
            discardCalibrationResults();
            save = false;
        }
    } else {
        discardCalibrationResults();
        save = false;
    }
    dialog->finalize(save);
}


// Calibrator ==================================================================

Calibrator::Calibrator(ExperimentRunDelegate *runDelegate,
                        const data::CalibrationData &data) :
        ApexModule(*runDelegate),
        d(new CalibratorPrivate(runDelegate, data))
{
}

Calibrator::~Calibrator()
{
}

bool Calibrator::calibrate(bool forceRecalibration)
{
    bool success = false;
    while(true)
    {
        d->switchAutoManual = false;
        success = calibrateManually(forceRecalibration);
        if(!d->switchAutoManual)
            break;
        d->switchAutoManual = false;
        if(!d->autoCalibrationEnabled)
            break;
        Q_ASSERT(d->autoCalibration);
        success = d->autoCalibration->autoCalibrateAll();
        if(!d->switchAutoManual)
            break;
    }
    return success;
}

bool Calibrator::calibrateManually(bool forceRecalibration)
{
    d->discardCalibrationResults();

    // FIXME TOM show a warning if more parameters are available in the DB than
    // we need
    d->hardwareSetup = CalibrationDatabase().currentHardwareSetup();
    if (d->calibrationComplete() && !forceRecalibration) {
        d->mergeCalibrationResults();
        return true;
    }

    // reset parametermanager and keep its state
    PMRuntimeSettings pmstate =
            d->runDelegate->GetParameterManager()-> internalState();
    d->runDelegate->GetParameterManager()->setAllToDefaultValue(true);

    QScopedPointer<CalibrationIO> io(new CalibrationIO(d->runDelegate, true));
    d->calibrationIO = io.data();

    QScopedPointer<CalibrationDialog> dialog(new CalibrationDialog
            (d->runDelegate->parentWidget()));
    d->dialog = dialog.data();

    connect(io.data(), SIGNAL(playingChanged(bool)),
             d.data(), SLOT(playingChanged(bool)));

    connect(io.data(), SIGNAL(clippingOccured(bool)),
             d->dialog, SLOT(setClipping(bool)));

    connect(d->dialog, SIGNAL(startOutput()),
             io.data(), SLOT(startOutput()));
    connect(d->dialog, SIGNAL(stopOutput()),
             io.data(), SLOT(stopOutput()));
    connect(d->dialog, SIGNAL(loopingChanged(bool)),
             io.data(), SLOT(setLooping(bool)));

    connect(d->dialog, SIGNAL(stimulusChanged(QString)),
             d.data(), SLOT(stimulusChanged(QString)));
    connect(d->dialog, SIGNAL(parameterNameChanged(QString)),
             d.data(), SLOT(parameterNameChanged(QString)));
    connect(d->dialog, SIGNAL(targetAmplitudeChanged(QString)),
             d.data(), SLOT(targetAmplitudeChanged()));
    connect(d->dialog, SIGNAL(outputParameterChanged(QString)),
             d.data(), SLOT(outputParameterChanged()));
    connect(d->dialog, SIGNAL(correctOutputParameter(double)),
             d.data(), SLOT(correctOutputParameter(double)));
    connect(d->dialog, SIGNAL(hardwareSetupChanged(QString)),
             d.data(), SLOT(hardwareSetupChanged(QString)));
    connect(d->dialog, SIGNAL(manageHardwareSetups()),
             d.data(), SLOT(manageHardwareSetups()));
    connect(d->dialog, SIGNAL(saveCalibration()),
             d.data(), SLOT(saveCalibration()));
    connect(d->dialog, SIGNAL(mutingChanged(QString,bool)),
             d.data(), SLOT(mutingChanged(QString,bool)));
    connect(d->dialog, SIGNAL(autoCalibrateAll()),
             d.data(), SLOT(autoCalibrateAll()));
    connect(d->dialog, SIGNAL(autoCalibrateSingle(QString)),
             d.data(), SLOT(autoCalibrateSingle(QString)));
    connect(d->dialog, SIGNAL(beforeCancel()),
             d.data(), SLOT(beforeCancel()));

    d->initAutoCalibration();
    if (d->autoCalibrationEnabled) {
        connect(d->autoCalibration.data(), SIGNAL(measurement_finished(bool)),
                d.data(),SLOT(autoCalibrateSingleFinished(bool)));

            connect(d->autoCalibration.data(), SIGNAL(startOutput()),
                io.data(),SLOT(startOutput()));
        connect(d->autoCalibration.data(), SIGNAL(stopOutput()),
                io.data(),SLOT(stopOutput()));
                connect(d->autoCalibration.data(), SIGNAL(setParameter(QString, double)),
                                io.data(),SLOT(setParameter(QString, double)));
                connect(io.data(), SIGNAL(clippingOccured(bool)),
                                d->autoCalibration.data(), SLOT(clippingOccured(bool)));
                connect(d->autoCalibration.data(), SIGNAL(calibrateManually()),
                                d.data(),SLOT(calibrateManually()));
                connect(d->dialog, SIGNAL(stopOutput()),
                                d->autoCalibration.data(), SLOT(stopCalibrating()));
        }

    dialog->setStimuli(d->data.availableStimuli());
    dialog->setHardwareSetups(CalibrationDatabase().hardwareSetups());
    dialog->setParameterNames(d->data.parameters().keys());

    if (d->autoCalibrationEnabled)
        dialog->setAutoCalibrateEnabled(true);

    d->updateCompleteState();

    if (!d->data.availableStimuli().isEmpty())
        dialog->setStimulus(d->data.availableStimuli().first());
    dialog->setHardwareSetup(CalibrationDatabase().currentHardwareSetup());
    if (!d->data.parameters().isEmpty())
        dialog->setParameterName(d->data.parameters().begin().key());

    if (!dialog->stimulus().isEmpty())
        dialog->setLooping(true);

    dialog->setModal(true);
    dialog->show();
    if (dialog->hardwareSetups().isEmpty()) {
        d->manageHardwareSetups();
        if (!dialog->hardwareSetups().isEmpty())
            dialog->setHardwareSetup(dialog->hardwareSetups().first());
    }

    const int returnValue = dialog->exec();

    d->runDelegate->GetParameterManager()-> setInternalState(pmstate);

    if (returnValue != QDialog::Accepted ) {
        d->discardCalibrationResults();
        return false;
    }
    CalibrationDatabase().setCurrentHardwareSetup(d->hardwareSetup);
    d->mergeCalibrationResults();
    return true;
}

void Calibrator::updateParameters()
{
    Q_FOREACH (const QString &name, d->data.parameters().keys()) {
        Q_ASSERT(d->isNewlyCalibrated(name));
        const CalibrationParameterData parameter = d->data.parameters()[name];
        const CalibratedParameter &result = d->results[name];
        d->runDelegate->GetParameterManager()->setParameter(name, result.outputParameter +
                parameter.finalTargetAmplitude() - result.targetAmplitude, false);
    }
}

QString Calibrator::GetEndXML() const
{
    QStringList result;

    result << "<calibration>";

    Q_FOREACH (const QString &name, d->data.parameters().keys()) {
        Q_ASSERT(d->isNewlyCalibrated(name));
        const CalibrationParameterData parameter = d->data.parameters()[name];
        const CalibratedParameter &value = d->results[name];
        result << "<parameter id=\"" + name + "\">" +
        QString::number(value.outputParameter +
                        parameter.finalTargetAmplitude() - value.targetAmplitude)
        + "</parameter>";
    }

    result <<  "</calibration>";
    return result.join("\n");
}

QString Calibrator::GetResultXML() const
{
    // No results to save
    return QString();
}

} // namespace apex

#include "calibrator.moc"
