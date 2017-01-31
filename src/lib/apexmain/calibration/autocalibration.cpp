/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "autocalibration.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMap>
#include <QDir>
#include <QMessageBox>
#include <QProgressDialog>
#include "calibrationdatabase.h"
#include "gui/calibrationsetupsdialog.h"
#include "services/pluginloader.h"

namespace apex
{

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

//AutoCalibration ===========================================================


AutoCalibration::AutoCalibration(ExperimentRunDelegate *runDelegate,
                                 const CalibrationData &data,
                                 const QString &hardwareSetup) :
    runDelegate(runDelegate),
    timer(new QTimer(this)),
    intervalTimer(new QTimer(this)),
    slmData(*data.soundLevelMeterData()),
    data(data),
    measurementResult(0),
    hardwareSetup(hardwareSetup)
{
	error.clear();
    connect(timer, SIGNAL(timeout()), this, SLOT(afterMeasurement()));
    connect(intervalTimer, SIGNAL(timeout()), this, SLOT(timerInterval()));
    SLM.reset(loadSoundLevelMeterPlugin(slmData.valueByType("plugin").toString()));
	if(!SLM.get()){
		throw ApexStringException(error);
	}
    qDebug("Connecting..");
    if(!SLM->connect()){
        error = SLM->errorString();
        QMessageBox::information(0, "Autocalibration - construction failed",
                                      errorString());
        return;
    }

    qDebug("List of available transducers:");
    QStringList trans = SLM->transducers();
    for(int i = 0; i < trans.size() ; ++i)
        qDebug("%s", qPrintable(trans.at(i)));

    if (!slmData.valueByType("transducer").toString().isEmpty()) {

        if(!SLM->setTransducer(slmData.valueByType("transducer").toString())){
            error = SLM->errorString();
            QMessageBox::information(0, "Autocalibration - construction failed",
                                        errorString());
            return;
        }
    } // FIXME: else: allow user to choose transducer
}

AutoCalibration::~AutoCalibration()
{

}

bool AutoCalibration::autoCalibrateAll()
{

    this->hardwareSetup = CalibrationDatabase().currentHardwareSetup();

	std::auto_ptr<CalibrationIO> io (new CalibrationIO (runDelegate, true));
    calibrationIO = io.get();
	calibrationIO->setStimulus (data.availableStimuli().first());
	calibrationIO->setLooping(true);

	std::auto_ptr<AutoCalibrationDialog> newdialog (new AutoCalibrationDialog
        (runDelegate->GetMainWindow()));
    dialog = newdialog.get();
    Q_ASSERT(dialog);
    setParameters();
    setSlmParameters();
    dialog->setParameterList(parameters);
	dialog->setTargetAmplitude(QString::number(data.parameters().begin().value().defaultTargetAmplitude()));


    connect (dialog, SIGNAL (hardwareSetupChanged(QString)),
             this, SLOT (hardwareSetupChanged(QString)));
    connect (dialog, SIGNAL (manageHardwareSetups()),
             this, SLOT (manageHardwareSetups()));

    connect(dialog, SIGNAL(calibrateAllParameters()),
        this, SLOT(calibratingAllParameters()));
    connect(dialog, SIGNAL(stopAutoCalibration()),
        this, SLOT(stopCalibrating()));
    connect(dialog, SIGNAL(slmParameterChanged(QString, QString)),
        this, SLOT(slmParameterChanged(QString, QString)));
    connect(dialog, SIGNAL(calibrateManually()),
        this, SLOT(goCalibrateManually()));
	connect(dialog, SIGNAL(advancedCalibrationAmplitudeUnchecked()),
		this, SLOT(advancedCalibrationAmplitudeUnchecked()));
	connect(dialog, SIGNAL(beforeCancel()),
		this, SLOT(beforeCancel()));
	connect (io.get(), SIGNAL (clippingOccured(bool)),
		this, SLOT (clippingOccured(bool)));
	dialog->setHardwareSetups (CalibrationDatabase().hardwareSetups());

    dialog->show();

    dialog->setHardwareSetup(hardwareSetup);
	dialog->selectAllParamersByDefault();

    const int returnValue = dialog->exec();

    if (returnValue != QDialog::Accepted ){
		discardCalibrationResults();
        return false;
    }
    CalibrationDatabase().setCurrentHardwareSetup (hardwareSetup);
    mergeCalibrationResults();
    return false; //TODO --> in apexcontrol.cpp -> updateparameters get's called (result doesn't contain them)
	//is this the final save?
}

SoundLevelMeter* AutoCalibration::loadSoundLevelMeterPlugin(const QString &name)
{
    qDebug("Loading SLM plugin: %s", qPrintable(name));

    SoundLevelMeterPluginCreator* creator = PluginLoader::Get().
            createPluginCreator<SoundLevelMeterPluginCreator>(name);
    //m_plugin.reset (creator->createFilter (name, channels, size, fs));

    QProgressDialog progress(tr("Connecting to sound level meter, please wait..."),
                             QString(), 0, 10, dialog);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(1);
    progress.setWindowTitle(tr("APEX 3"));

    QCoreApplication::processEvents();

    SoundLevelMeter*  slm = creator->createSoundLevelMeter(name);
	if (!slm)
		throw ApexStringException(tr("Could not load sound level meter plugin"));

    return slm;
}

bool AutoCalibration::autoCalibrateSingle(bool callback, const QString &parameter)
{
    this->callback = callback;
    this->currentParameter = &parameter;
	clipped = false;

	if(!callback){
		muteAllParameters();
		calibrationIO->setParameter(parameter, outputParameter(parameter));
		calibrationIO->startOutput();
	}
	else
		emit startOutput();

    if(dialog)
		dialog->setStatus("Starting stimulus");

    if(!SLM->startMeasurement(measurementType(slmData.measurementType()),
        frequencyWeightingType(slmData.frequencyWeightingType()),
        timeWeightingType(slmData.timeWeightingType()),
             slmData.valueByType("percentile").toDouble())){
		error = SLM->errorString();
		return false;
    }
    timerValue = slmData.valueByType("time").toInt();
    timer->start(timerValue*1000);
    if(dialog)
        intervalTimer->start(990);

    if(dialog)
		dialog->setStatus(tr(QString::fromLatin1("Starting measurement for %1 seconds")
        .arg(slmData.valueByType("time").toInt())));
    return true;
}

int AutoCalibration::measurementType(QString type)
{
    if(type == "RMS")
        return SoundLevelMeter::SPL_Measurement;
    else if (type == "PEAK")
        return SoundLevelMeter::pkSPL_Measurement;
    else
        Q_ASSERT(0 && "unknown measurement type");

    return -1;
}

int AutoCalibration::frequencyWeightingType(QString type)
{
    if ( type == "Z" )
        return SoundLevelMeter::Z_Weighting;
    if ( type == "A" )
        return SoundLevelMeter::A_Weighting;
    if ( type == "C" )
        return SoundLevelMeter::C_Weighting;

    qFatal("unknown frequency weighting type");
    return -1;
}

int AutoCalibration::timeWeightingType(QString type)
{
    if (type == "S")
        return SoundLevelMeter::SlowWeighting;
    if (type == "F")
        return SoundLevelMeter::FastWeighting;
    if (type == "I")
        return SoundLevelMeter::ImpulseWeighting;

    qFatal("unknown time weighting type");

    return -1;
}

void AutoCalibration::afterMeasurement()
{
    intervalTimer->stop();
    timer->stop();
    SLM->stopMeasurement();
	if(calibrationIO.isNull())
		emit stopOutput();
	else
		calibrationIO->stopOutput();
    if(clipped){
        error = tr("Invalid measurement - signal has clipped");
        if(callback)
            emit measurement_finished(false); //return to calibrator
        else
            calibratingAll(false, false);
    }
    else if(!SLM->measure()){
        error = SLM->errorString();
        if(callback)
		    emit measurement_finished(false); //return to calibrator
        else
		    calibratingAll(false, false);
    }
    else {
        if(dialog)
		    dialog->setStatus("Retrieving measurement");
            measurementResult = SLM->result();
        if(callback)
		    emit measurement_finished(true); //return to calibrator
        else
		    calibratingAll(false, true);
    }
}


void AutoCalibration::setParameters()
{
    CalibrationDatabase database;
    parameters.clear();
    QMapIterator<QString, data::CalibrationParameterData> i(data.parameters());
    while (i.hasNext()){
        i.next();
        if(database.isCalibrated(hardwareSetup, data.calibrationProfile(), i.key())){
            parameters.append(QStringList() << i.key()
				    << tr(QString::fromLatin1("%1 (%2)").arg(outputParameter(i.key()))
				        .arg(targetAmplitude(i.key())))
                    << (results.contains(i.key()) ?
						tr(QString::fromLatin1("%1 (%2)").arg(results[i.key()].outputParameter))
						.arg(results[i.key()].targetAmplitude) :
						tr(QString::fromLatin1("- (%1)")
					    .arg(data.parameters().begin().value()
						.defaultTargetAmplitude())))
					<< tr("-"));
        }
        else {
            parameters.append(QStringList() << i.key()
                << "-" << (results.contains(i.key()) ?
				tr(QString::fromLatin1("%1 (%2)").arg(results[i.key()].outputParameter)
				.arg(results[i.key()].targetAmplitude)) : tr(QString::fromLatin1("- (%1)")
				.arg(data.parameters().begin().value().defaultTargetAmplitude())))
				<< tr("-"));
        }
    }
}



void AutoCalibration::setSlmParameters()
{
    QStringList slmParameters;
    //type - frequency_weighting - time_weighting - percentile - time

    slmParameters << QString::number(measurementType(slmData.measurementType()))
    << QString::number(frequencyWeightingType(slmData.frequencyWeightingType()))
    << QString::number(timeWeightingType(slmData.timeWeightingType()))
    << slmData.valueByType("percentile").toString()
    << slmData.valueByType("time").toString();
    dialog->setSlmParameters(slmParameters);
}

void AutoCalibration::calibratingAll(bool firstCall, bool succes)
{
    static int iteration = 0;
    static QString parameter;
	qDebug("iteration: %d", iteration);
    if(firstCall){
        selectedParameters = dialog->getSelectedParametersToCalibrate();
        if(selectedParameters.size() < 1){
            dialog->setStatus("Select the parameters to calibrate");
            return;
        }
        calibrationProgress = 0;
        dialog->newMeasurement();
        dialog->setCalibrating(true);
        iteration = 0;
        parameter = selectedParameters.at(iteration);
        dialog->setProgressBar(0, selectedParameters.size() * slmData.valueByType("time").toInt(), 0);
    }
    if(!succes){
		dialog->setStatus(tr(QString::fromLatin1("<span style=\"color:#fff;background-color:#f00\">%1</span>")
            .arg(error)));
		qDebug("Error on calibrating parameter %s:%s",
            qPrintable(parameter),qPrintable(error));
        ++iteration;
        dialog->setClipped(parameter);
		calibrationProgress = iteration * slmData.valueByType("time").toInt();
		dialog->setProgressBar(0,selectedParameters.size() * slmData.valueByType("time").toInt(),
			calibrationProgress);

    }
    else if(!firstCall){
        qDebug("Succesfully calibrated parameter %s",qPrintable(parameter));
        const double targetAmpl = dialog->targetAmplitude().toDouble();
        const double currentOutputParameter = outputParameter(parameter);
        const double currentAmplitude = measurementResult;

        const double newOutputParameter = qBound(
            data.parameters()[parameter].minimumParameter(),
            currentOutputParameter + targetAmpl - currentAmplitude,
            data.parameters()[parameter].maximumParameter());

        if(results.contains(parameter)){
            results[parameter].targetAmplitude = targetAmpl;
            results[parameter].outputParameter = newOutputParameter;
        }
        else
            results.insert(parameter, CalibratedParameter(
                targetAmplitude(parameter), newOutputParameter));


		if (isInCalibrationDatabase (parameter)){
			double difference = CalibrationDatabase().targetAmplitude (hardwareSetup,
						data.calibrationProfile(), parameter) -
						CalibrationDatabase().outputParameter (hardwareSetup,
						data.calibrationProfile(), parameter) -
						(targetAmpl - outputParameter(parameter));
			dialog->updateParameter(parameter, tr(QString::fromLatin1("%1 (%2)")
				.arg(outputParameter(parameter)).arg(targetAmpl)),
				QString::number(difference));
		}
		else
			dialog->updateParameter(parameter, tr(QString::fromLatin1("%1 (%2)")
				.arg(outputParameter(parameter)).arg(targetAmpl)));
        ++iteration;
    }

    if(iteration > selectedParameters.size() - 1){
        dialog->setStatus("Calibration finished");
        dialog->setCalibrating(false);
        dialog->setCurrentParameterCalibrated(QString());
        dialog->setProgressBar(0, 1, 1);
        return;
    }

    parameter = selectedParameters.at(iteration);
    dialog->setCurrentParameterCalibrated(parameter);
    autoCalibrateSingle(false, parameter);
}

void AutoCalibration::calibratingAllParameters()
{
    calibratingAll(true);
}

void AutoCalibration::slmParameterChanged(const QString &parameter, const QString &value)
{
    slmData.setValueByType(parameter, value);
}

void AutoCalibration::clippingOccured(bool clipping)
{
	if(clipping){
		clipped = clipping;
		if(dialog){
			dialog->setClipped(*currentParameter);
			afterMeasurement();			//Stop the measurement when clipped
		}
	}
}


void AutoCalibration::hardwareSetupChanged (const QString &value)
{
    qDebug("hardwareSetupChanged(%s)",qPrintable(value));
    discardCalibrationResults();

    hardwareSetup = value;
    setParameters();
    dialog->setParameterList(parameters);

    if (dialog->hardwareSetup().isEmpty())
        dialog->disable();
    else
        dialog->enable();
}

void AutoCalibration::discardCalibrationResults()
{
    results.clear();
}

void AutoCalibration::manageHardwareSetups()
{
    CalibrationSetupsDialog setupsDialog (dialog);
    setupsDialog.exec();

    dialog->setHardwareSetups (CalibrationDatabase().hardwareSetups());
}

void AutoCalibration::setHardwareSetup(const QString &hardwareSetup)
{
    this->hardwareSetup = hardwareSetup;
}

void AutoCalibration::stopCalibrating()
{
    timer->stop();
    SLM->stopMeasurement();
    if(dialog){
		intervalTimer->stop();
		calibrationIO->stopOutput();
		dialog->setCalibrating(false);
		dialog->setStatus("Measurement stopped by user");
		dialog->setProgressBar(0, 0, 0);
		dialog->setCurrentParameterCalibrated(QString());

	}
	else {
		error = tr("Measurement stopped by user");
        emit measurement_finished(false); //return to calibrator
	}
}

void AutoCalibration::mergeCalibrationResults()
{
    // Fill the calibration database, prepare the calibration result
    CalibrationDatabase database;
    const QString profile = data.calibrationProfile();
    selectedParameters = dialog->getSelectedParametersToSave();

    if(selectedParameters.size() < 1){
        dialog->setStatus(tr("<span style=\"color:#fff;background-color:#f00\">No parameters selected</span>"));
        return;
    }

    Q_FOREACH (const QString &name, selectedParameters){
        if (isNewlyCalibrated (name)){
            const CalibratedParameter &parameter = results[name];
            database.calibrate (hardwareSetup, profile, name,
                                parameter.targetAmplitude, parameter.outputParameter);
        }
        else if (isInCalibrationDatabase (name)){
            results.insert (name, CalibratedParameter
                            (database.targetAmplitude (hardwareSetup, profile, name),
                             database.outputParameter (hardwareSetup, profile, name)));
        }
        else {
            dialog->setStatus(tr(
                "<span style=\"color:#fff;background-color:#f00\">Failed to save an uncalibrated parameter.</span>"));
        }
    }
}

bool AutoCalibration::isInCalibrationDatabase (const QString &name) const
{
    return CalibrationDatabase().isCalibrated (hardwareSetup,
            data.calibrationProfile(), name);
}

bool AutoCalibration::isNewlyCalibrated (const QString &name) const
{
    return results.contains (name);
}

double AutoCalibration::outputParameter (const QString &name) const
{
    if (isNewlyCalibrated (name))
        return results.value (name).outputParameter;

    if (isInCalibrationDatabase (name))
        return CalibrationDatabase().outputParameter (hardwareSetup,
                data.calibrationProfile(), name);

    return data.parameters().value (name).defaultParameter();
}

double AutoCalibration::targetAmplitude (const QString &name) const
{
    if (isNewlyCalibrated (name))
        return results.value (name).targetAmplitude;

    if (isInCalibrationDatabase (name))
        return CalibrationDatabase().targetAmplitude (hardwareSetup,
                data.calibrationProfile(), name);

    return data.parameters().value (name).defaultTargetAmplitude();
}

void AutoCalibration::timerInterval()
{
	dialog->setStatus(tr(QString::fromLatin1("Waiting for measurement (%1s remaining)").arg(--timerValue)));
    dialog->setProgressBar(0, 0, ++calibrationProgress);
}

void AutoCalibration::advancedCalibrationAmplitudeUnchecked()
{
	dialog->setTargetAmplitude(QString::number(data.parameters().begin().value().defaultTargetAmplitude()));
}

void AutoCalibration::muteAllParameters()
{
    QMapIterator<QString, data::CalibrationParameterData> i(data.parameters());
    while (i.hasNext()){
        i.next();
        calibrationIO->setParameter(i.key(), i.value().muteParameter());
    }
}

void AutoCalibration::beforeCancel()
{
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

void AutoCalibration::goCalibrateManually()
{
	dialog->finalize(false);
	emit calibrateManually();
}

}//namespace APEX
