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

#include "gui/calibrationdialog.h"
#include "gui/calibrationsetupsdialog.h"
#include "gui/mainwindow.h"
#include "runner/experimentrundelegate.h"
#include "calibrationdatabase.h"
#include "calibrationio.h"
#include "calibrator.h"
#include "parameters/parametermanager.h"
#include "autocalibration.h"

#include "services/errorhandler.h"

#include <QMessageBox>
#include <QPointer>

//from libdata
#include "calibration/calibrationdata.h"

namespace apex
{

using namespace data;

class CalibratedParameter
{
    public:
        CalibratedParameter() :
                targetAmplitude (0),
                outputParameter (0)
        {
        }

        CalibratedParameter (double targetAmplitude, double outputParameter) :
                targetAmplitude (targetAmplitude),
                outputParameter (outputParameter)
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
        CalibratorPrivate (ExperimentRunDelegate *runDelegate,
                           const data::CalibrationData &data);
        ~CalibratorPrivate();

        void initAutoCalibration();

    private Q_SLOTS:
        void parameterNameChanged (const QString &value);
        void outputParameterChanged();
        void targetAmplitudeChanged();
        void hardwareSetupChanged (const QString &value);
        void stimulusChanged (const QString &value);
        void manageHardwareSetups();
        void correctOutputParameter (double amplitude);
        void saveCalibration();
        void mutingChanged (const QString &name, bool muted);

        void playingChanged (bool playing);
        void autoCalibrateAll ();
        void autoCalibrateSingle (const QString& parameter);

        void autoCalibrateSingleFinished(bool succes);
        void calibrateManually();
        void beforeCancel();

    private:
        void updateWidgetStates();
        void updateWidgetsFromCalibration();
        void updateCalibrationFromWidgets();
        void updateDatabaseState();
        void updateCompleteState();

        double outputParameter (const QString &name) const;
        double targetAmplitude (const QString &name) const;
        bool isInCalibrationDatabase (const QString &name) const;
        bool isNewlyCalibrated (const QString &name) const;
        bool calibrationComplete() const;
        void mergeCalibrationResults();
        void discardCalibrationResults();


    public:
        ExperimentRunDelegate *runDelegate;
        QPointer<CalibrationIO> calibrationIO;
        QPointer<CalibrationDialog> dialog;
        std::auto_ptr<AutoCalibration> autoCalibration;
        bool autoCalibrationEnabled;
        const data::CalibrationData &data;
        bool playing;
        bool switchAutoManual;

        QString hardwareSetup;
        QMap<QString, CalibratedParameter> results;

        friend class Calibrator;
};

// CalibratorPrivate ===========================================================

CalibratorPrivate::CalibratorPrivate (ExperimentRunDelegate *runDelegate,
        const data::CalibrationData &data) :
    runDelegate (runDelegate),
    autoCalibration(0),
    autoCalibrationEnabled(false),
    data (data),
    playing (false),
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
        if (data.soundLevelMeterData() && !autoCalibration.get())
	   autoCalibration.reset(
	           new AutoCalibration(runDelegate,data,hardwareSetup));
		autoCalibrationEnabled=true;
    } catch (ApexStringException &e) {
         ErrorHandler::Get().addWarning(tr("Calibrator"), e.what());
         ErrorHandler::Get().addWarning(tr("Calibrator"),
                 tr("Could not load sound level meter plugin, "
                     "disabling automatic calibration"));

		autoCalibrationEnabled=false;
    }
}


CalibratorPrivate::~CalibratorPrivate()
{
}

double CalibratorPrivate::outputParameter (const QString &name) const
{
    if (isNewlyCalibrated (name))
        return results.value (name).outputParameter;

    if (isInCalibrationDatabase (name))
        return CalibrationDatabase().outputParameter (hardwareSetup,
                data.calibrationProfile(), name);

    return data.parameters().value (name).defaultParameter();
}

double CalibratorPrivate::targetAmplitude (const QString &name) const
{
    if (isNewlyCalibrated (name))
        return results.value (name).targetAmplitude;

    if (isInCalibrationDatabase (name))
        return CalibrationDatabase().targetAmplitude (hardwareSetup,
                data.calibrationProfile(), name);

    return data.parameters().value (name).defaultTargetAmplitude();
}

bool CalibratorPrivate::isInCalibrationDatabase (const QString &name) const
{
    return CalibrationDatabase().isCalibrated (hardwareSetup,
            data.calibrationProfile(), name);
}

bool CalibratorPrivate::isNewlyCalibrated (const QString &name) const
{
    return results.contains (name);
}

bool CalibratorPrivate::calibrationComplete() const
{
    Q_FOREACH (const QString &name, data.parameters().keys())
    if (!isInCalibrationDatabase (name) && !isNewlyCalibrated (name))
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
        if (isNewlyCalibrated (name))
        {
            const CalibratedParameter &parameter = results[name];
            database.calibrate (hardwareSetup, profile, name,
                                parameter.targetAmplitude, parameter.outputParameter);
        }
        else if (isInCalibrationDatabase (name))
        {
            results.insert (name, CalibratedParameter
                            (database.targetAmplitude (hardwareSetup, profile, name),
                             database.outputParameter (hardwareSetup, profile, name)));
        }
        else
        {
            qFatal ("Request to merge results but not completely calibrated");
        }
    }
}

void CalibratorPrivate::discardCalibrationResults()
{
    results.clear();
}

void CalibratorPrivate::stimulusChanged (const QString &name)
{
    try
    {
        calibrationIO->setStimulus (name);
        calibrationIO->setLooping (dialog->isLooping());
    }
    catch (std::exception &e)
    {
        QMessageBox::critical (dialog, tr ("Calibration Error"),
                               tr ("Unable to set stimulus:\n%1").arg (e.what()));
    }
}

void CalibratorPrivate::parameterNameChanged (const QString &value)
{
    const CalibrationParameterData param = data.parameters().value (value);
    dialog->setMinimumOutputParameter (param.minimumParameter());
    dialog->setMaximumOutputParameter (param.maximumParameter());
    dialog->setMeasuredAmplitude (QString());

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
        calibrationIO->setParameter (parameterName, outputParameter.toDouble());

    updateWidgetStates();
}

void CalibratorPrivate::hardwareSetupChanged (const QString &value)
{
    discardCalibrationResults();
    hardwareSetup = value;

    updateWidgetsFromCalibration();
    updateWidgetStates();
    updateCompleteState();
}

void CalibratorPrivate::mutingChanged (const QString &name, bool muted)
{
    //qDebug ("Muting change %s %u", qPrintable (name), muted);
    // Currently selected parameter switched on
    const QString parameterName = dialog->parameterName();
    if (parameterName == name && !muted)
    {
        const QString outputParameter = dialog->outputParameter();
        if (!outputParameter.isEmpty()) {
            /*qDebug ("(Un)muting current parameter %s to %f", qPrintable (name),
                    outputParameter.toDouble());*/
            calibrationIO->setParameter (name, outputParameter.toDouble());
        }
        return;
    }
    // all other cases
    const CalibrationParameterData parameter = data.parameters()[name];
    /*qDebug ("(Un)muting parameter %s to %f", qPrintable (name),
            muted ? parameter.muteParameter() : outputParameter (name));*/
    calibrationIO->setParameter (name,
                                 muted ? parameter.muteParameter() : outputParameter (name));
}

void CalibratorPrivate::updateCompleteState()
{
    dialog->setComplete (calibrationComplete());
}

void CalibratorPrivate::updateWidgetStates()
{
    dialog->setPlaying (playing);
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

    results.insert (parameterName, CalibratedParameter
                    (targetAmplitude.toDouble(), outputParameter.toDouble()));

    updateDatabaseState();
    updateCompleteState();
}

void CalibratorPrivate::updateWidgetsFromCalibration()
{
    const QString name = dialog->parameterName();

    dialog->setTargetAmplitude (QString::number (targetAmplitude (name)));
    dialog->setOutputParameter (QString::number (outputParameter (name)));

    updateDatabaseState();
}

void CalibratorPrivate::updateDatabaseState()
{
    const QString name = dialog->parameterName();

    QString text;
    if (isNewlyCalibrated (name))
    {
        text = "<span style=\"background-color:#0f0\">";
        text += tr ("Newly calibrated");
    }
    else if (isInCalibrationDatabase (name))
    {
        text = "<span style=\"background-color:#0f0\">";
        text += tr ("Found in calibration database");
    }
    else
    {
        text = "<span style=\"background-color:#ff0\">";
        text += tr ("Not calibrated");
    }
    text += "</span>";
    dialog->setDatabaseStatus (text);
}

void CalibratorPrivate::correctOutputParameter (double currentAmplitude)
{
    bool targetAmplitudeOk, currentParameterOk;

    const double targetAmplitude =
        dialog->targetAmplitude().toDouble (&targetAmplitudeOk);
    const double currentParameter =
        dialog->outputParameter().toDouble (&currentParameterOk);
    if (!targetAmplitudeOk || !currentParameterOk)
        return;

    const double correctedParameter = qBound
                                      (dialog->minimumOutputParameter(),
                                       currentParameter + targetAmplitude - currentAmplitude,
                                       dialog->maximumOutputParameter());

    dialog->setOutputParameter (QString::number (correctedParameter));
    dialog->setMeasuredAmplitude (QString());

    dialog->setDatabaseStatus ("<span style=\"background-color:#ff0\">"
                               + tr ("Click apply to keep") + "</span>"
                              );



}

void CalibratorPrivate::playingChanged (bool value)
{
    playing = value;
	if (dialog)
        updateWidgetStates();
}

void CalibratorPrivate::manageHardwareSetups()
{
    CalibrationSetupsDialog setupsDialog (dialog);
    setupsDialog.exec();

    dialog->setHardwareSetups (CalibrationDatabase().hardwareSetups());
}


void CalibratorPrivate::autoCalibrateAll()
{
	Q_ASSERT(autoCalibrationEnabled);
    Q_ASSERT(data.soundLevelMeterData());
    CalibrationDatabase().setCurrentHardwareSetup (hardwareSetup);
    switchAutoManual = true;
    dialog->reject();
}


void CalibratorPrivate::autoCalibrateSingle(const QString &parameter)
{
    Q_ASSERT(data.soundLevelMeterData());
	Q_ASSERT(autoCalibrationEnabled);

    dialog->setDatabaseStatus ("<span style=\"background-color:#3ff\">"
                               + tr (QString("Calibrating %1 for %2 seconds")
							   .arg(parameter)
							   .arg(data.soundLevelMeterData()->valueByType("time").toInt()))
							   + "</span>");
    autoCalibration->setHardwareSetup (hardwareSetup);

    if(autoCalibration->autoCalibrateSingle (true,parameter))
        dialog->disable();
    else
        QMessageBox::information(0,"AutoCalibrateSingle failed",
                                      autoCalibration->errorString());

}


void CalibratorPrivate::autoCalibrateSingleFinished(bool succes)
{
    Q_ASSERT(data.soundLevelMeterData());
    if(succes)
    {
        qDebug("Measurement succesfull");
		qDebug("%s", qPrintable(QString::fromLatin1("Measured value: %1").arg(autoCalibration->result())));
		dialog->setMeasuredAmplitude (QString::number(autoCalibration->result()));
        correctOutputParameter (autoCalibration->result());
    }
    else
    {
        qDebug("Measurement failed: %s", + qPrintable(autoCalibration->errorString()));
        dialog->setDatabaseStatus (
			tr(QString::fromLatin1("<span style=\"color:#fff;background-color:#f00\">%1</span>")
            .arg(autoCalibration->errorString())));
	}
	dialog->enable();

}

void CalibratorPrivate::calibrateManually()
{
    this->switchAutoManual = true;
    CalibrationDatabase().setCurrentHardwareSetup (autoCalibration->hardwareSetup);
}

void CalibratorPrivate::beforeCancel()
{
	qDebug("beforeCancel()");
	bool save = true;
	if (!results.size() == 0){
        QMessageBox::StandardButton  answer =
           QMessageBox::question(0, "Save calibration?",
                                  "You have made changes to the calibration.\n"
                                  "Do you want to save the results?",
                                  QMessageBox::Discard | QMessageBox::Save,
                                  QMessageBox::Discard);
        if (answer== QMessageBox::Discard){
            discardCalibrationResults();
		    save = false;
        }
    }
    else {
        discardCalibrationResults();
        save = false;
    }
	dialog->finalize(save);
}


// Calibrator ==================================================================

Calibrator::Calibrator (ExperimentRunDelegate *runDelegate,
                        const data::CalibrationData &data) :
        ApexModule (*runDelegate),
        d (new CalibratorPrivate (runDelegate, data))
{
}

Calibrator::~Calibrator()
{
}

bool Calibrator::calibrate (bool forceRecalibration)
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
        Q_ASSERT(d->autoCalibration.get());
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
    if (d->calibrationComplete() && !forceRecalibration)
    {
        d->mergeCalibrationResults();
        return true;
    }

    // reset parametermanager and keep its state
    PMRuntimeSettings pmstate =
            d->runDelegate->GetParameterManager()-> internalState();
    d->runDelegate->GetParameterManager()->forceReset();

    std::auto_ptr<CalibrationIO> io (new CalibrationIO (d->runDelegate, true));
    d->calibrationIO = io.get();

    std::auto_ptr<CalibrationDialog> dialog (new CalibrationDialog
            (d->runDelegate->parentWidget()));
    d->dialog = dialog.get();

    connect (io.get(), SIGNAL (playingChanged(bool)),
             d.get(), SLOT (playingChanged(bool)));

    connect (io.get(), SIGNAL (clippingOccured(bool)),
             d->dialog, SLOT (setClipping(bool)));

    connect (d->dialog, SIGNAL (startOutput()),
             io.get(), SLOT (startOutput()));
    connect (d->dialog, SIGNAL (stopOutput()),
             io.get(), SLOT (stopOutput()));
    connect (d->dialog, SIGNAL (loopingChanged(bool)),
             io.get(), SLOT (setLooping(bool)));

    connect (d->dialog, SIGNAL (stimulusChanged(QString)),
             d.get(), SLOT (stimulusChanged(QString)));
    connect (d->dialog, SIGNAL (parameterNameChanged(QString)),
             d.get(), SLOT (parameterNameChanged(QString)));
    connect (d->dialog, SIGNAL (targetAmplitudeChanged(QString)),
             d.get(), SLOT (targetAmplitudeChanged()));
    connect (d->dialog, SIGNAL (outputParameterChanged(QString)),
             d.get(), SLOT (outputParameterChanged()));
    connect (d->dialog, SIGNAL (correctOutputParameter(double)),
             d.get(), SLOT (correctOutputParameter(double)));
    connect (d->dialog, SIGNAL (hardwareSetupChanged(QString)),
             d.get(), SLOT (hardwareSetupChanged(QString)));
    connect (d->dialog, SIGNAL (manageHardwareSetups()),
             d.get(), SLOT (manageHardwareSetups()));
    connect (d->dialog, SIGNAL (saveCalibration()),
             d.get(), SLOT (saveCalibration()));
    connect (d->dialog, SIGNAL (mutingChanged(QString,bool)),
             d.get(), SLOT (mutingChanged(QString,bool)));
    connect (d->dialog, SIGNAL(autoCalibrateAll()),
             d.get(), SLOT(autoCalibrateAll()));
    connect (d->dialog, SIGNAL(autoCalibrateSingle(QString)),
             d.get(), SLOT(autoCalibrateSingle(QString)));
	connect (d->dialog, SIGNAL(beforeCancel()),
			 d.get(), SLOT(beforeCancel()));

    d->initAutoCalibration();
    if (d->autoCalibrationEnabled) {
        connect(d->autoCalibration.get(), SIGNAL(measurement_finished(bool)),
                d.get(),SLOT(autoCalibrateSingleFinished(bool)));

	    connect(d->autoCalibration.get(), SIGNAL(startOutput()),
                io.get(),SLOT(startOutput()));
        connect(d->autoCalibration.get(), SIGNAL(stopOutput()),
                io.get(),SLOT(stopOutput()));
		connect(d->autoCalibration.get(), SIGNAL(setParameter (QString, double)),
				io.get(),SLOT(setParameter(QString, double)));
		connect (io.get(), SIGNAL (clippingOccured(bool)),
				d->autoCalibration.get(), SLOT (clippingOccured(bool)));
		connect(d->autoCalibration.get(), SIGNAL(calibrateManually()),
				d.get(),SLOT(calibrateManually()));
		connect(d->dialog, SIGNAL(stopOutput()),
				d->autoCalibration.get(), SLOT(stopCalibrating()));
	}

    dialog->setStimuli (d->data.availableStimuli());
    dialog->setHardwareSetups (CalibrationDatabase().hardwareSetups());
    dialog->setParameterNames (d->data.parameters().keys());

    if (d->autoCalibrationEnabled)
        dialog->setAutoCalibrateEnabled(true);

    d->updateCompleteState();

    if (!d->data.availableStimuli().isEmpty())
        dialog->setStimulus (d->data.availableStimuli().first());
    dialog->setHardwareSetup (CalibrationDatabase().currentHardwareSetup());
    if (!d->data.parameters().isEmpty())
        dialog->setParameterName (d->data.parameters().begin().key());

    if (!dialog->stimulus().isEmpty())
        dialog->setLooping (true);

    dialog->setModal(true);
    dialog->show();
    if (dialog->hardwareSetups().isEmpty())
    {
        d->manageHardwareSetups();
        if (!dialog->hardwareSetups().isEmpty())
            dialog->setHardwareSetup (dialog->hardwareSetups().first());
    }

	const int returnValue = dialog->exec();

	d->runDelegate->GetParameterManager()-> setInternalState(pmstate);

	if (returnValue != QDialog::Accepted )
	{
			d->discardCalibrationResults();
			return false;
	}
	CalibrationDatabase().setCurrentHardwareSetup (d->hardwareSetup);
	d->mergeCalibrationResults();
	return true;
}

void Calibrator::updateParameters()
{
    Q_FOREACH (const QString &name, d->data.parameters().keys())
    {
        Q_ASSERT (d->isNewlyCalibrated (name));
        const CalibrationParameterData parameter = d->data.parameters()[name];
        const CalibratedParameter &result = d->results[name];
        d->runDelegate->GetParameterManager()->setParameter (name, result.outputParameter +
                parameter.finalTargetAmplitude() - result.targetAmplitude, false);
    }
}

QString Calibrator::GetEndXML() const
{
    QStringList result;

    result << "<calibration>";

    Q_FOREACH (const QString &name, d->data.parameters().keys())
    {
        Q_ASSERT (d->isNewlyCalibrated (name));
        const CalibrationParameterData parameter = d->data.parameters()[name];
        const CalibratedParameter &value = d->results[name];
        result << "<parameter id=\"" + name + "\">" +
        QString::number(value.outputParameter +
                        parameter.finalTargetAmplitude() - value.targetAmplitude)
        + "</parameter>";
    }

    result <<  "</calibration>";
    return result.join ("\n");
}

QString Calibrator::GetResultXML() const
{
    // No results to save
    return QString();
}

} // namespace apex

#include "calibrator.moc"
