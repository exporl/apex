/******************************************************************************
 * Copyright (C) 2007 Pieter Verlinde                                         *
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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_CALIBRATION_AUTOCALIBRATION_H_
#define _EXPORL_SRC_LIB_APEXMAIN_CALIBRATION_AUTOCALIBRATION_H_

#include "apexdata/calibration/calibrationdata.h"
#include "apexdata/calibration/soundlevelmeterdata.h"

#include "gui/autocalibrationdialog.h"
#include "gui/mainwindow.h"

#include "runner/experimentrundelegate.h"

#include "services/mainconfigfileparser.h"

#include "calibrationio.h"
#include "soundlevelmeter.h"

#include <QMap>
#include <QPluginLoader>
#include <QTimer>

namespace apex
{
using namespace data;

class CalibratedParameter;

/**
 * Apex autocalibration
 * Used to calibrate a single parameter
 */
class AutoCalibration : public QObject
{
    Q_OBJECT
public:
        /*
         * Constructor
         * @param SoundLevelMeterData The information about the soundlevelmeter and the measurement settings
         */
    AutoCalibration(ExperimentRunDelegate *runDelegate, const CalibrationData &data,
        const QString &hardwareSetup);

        /*
         * Destructor
         */
    ~AutoCalibration();
        /*
         * Start the measurement according to the SoundLevelMeterData
         */
    bool autoCalibrateSingle(bool callback, const QString &parameter);
        /*
         * Retrieve the result of the measurement
         */
    double result() const{
        return measurementResult;
    };
        /*
         * This functions shows the AutoCalibrateDialog
         */
    bool autoCalibrateAll();

        /*
         * Fills in the parameters to pass to the treeview.
         */
    void setParameters();
        /*
         * Fills in the soundlevelmeterparemeters for the advanced view
         */
    void setSlmParameters();
        /*
         * Returns the latest error.
         */
    const QString& errorString() const
    {
        return error;
    };

        /*
         * Sets the hardwaresetup for autocalibratesingle
         */
    void setHardwareSetup(const QString &hardwareSetup);

public Q_SLOTS:
        /*
         * Start the auto calibrate all parameters
         */
    void calibratingAll(bool firstCall, bool succes = true);

        void calibratingAllParameters();
        /*
         * Slot get's called when signal clipping occurs
         */
    void clippingOccured(bool);
private Q_SLOTS:
        /**
         * The callbackfunction for the timer too make sure the soundlevelmeter
         * made his measurement.
         */
    void afterMeasurement();
        /**
         *  The callbackfunction for the timer wich gives feedback to the users
         *  at certain intervals.
         */
    void timerInterval();
        /**
         * Gets fired when the users changes one of the slm parameters
         *
         * @param parameter The parameter the user changed.
         * @param value The new value for that parameter.
         */
    void slmParameterChanged(const QString &parameter, const QString &value);
        /**
         * Is fired when the hardware setup is changed.
         *
         * @param value The selected string value in the combobox.
         */
    void hardwareSetupChanged (const QString &value);
        /**
         * Show the manage hardware setup dialog.
         */
    void manageHardwareSetups();
        /**
         * Stops the auto calibration
         */
    void stopCalibrating();
        /**
         * Persists the calibration results to the database
         */
    void mergeCalibrationResults();
        /**
         * Resets the calibration amplitude textfield to
         * its default value when the users unchecks the
         * advanced calibration setting.
         */
    void advancedCalibrationAmplitudeUnchecked();
        /*
         * When the user presses cancel
         */
        void beforeCancel();

        /*
         * Go to manual calibration
         */
        void goCalibrateManually();

Q_SIGNALS:
        /**
         * Callbackfunction to the calibrator.
         */
    void measurement_finished(bool succes);
        /**
         * Signal to start the stimilus output in calibrator.
         */
    void startOutput();
        /*
         * Signal to stop the stimilus output in calibrator
         */
    void stopOutput();
        /*
         * Sets a parameter to the calibrationIO
         */
    void setParameter (const QString &name, double value);
        /*
         * Go back to the manual calibrationdialog
         */
    void calibrateManually();
private:
    void discardCalibrationResults();
    SoundLevelMeter* loadSoundLevelMeterPlugin(const QString &name);
    bool isNewlyCalibrated(const QString &parameter) const;
    bool isInCalibrationDatabase(const QString &parameter) const;
    double outputParameter(const QString &name) const;
    double targetAmplitude (const QString &name) const;
    void muteAllParameters();
    int measurementType(QString type);
    int frequencyWeightingType(QString type);
    int timeWeightingType(QString type);

private:
    ExperimentRunDelegate *runDelegate;
    QPointer<CalibrationIO> calibrationIO;
    QPointer<QTimer> timer;
    QPointer<QTimer> intervalTimer;
    QScopedPointer<SoundLevelMeter> SLM;
    SoundLevelMeterData slmData;
    const CalibrationData &data;
    double measurementResult;
    bool callback;
    bool clipped;
    int timerValue;
    int calibrationProgress;
    const QString *currentParameter;
    QPointer<AutoCalibrationDialog> dialog;
    QList<QStringList> parameters;
    QStringList selectedParameters;

    QMap<QString, CalibratedParameter> results;

    QString error;

public:
        QString hardwareSetup;

};

} // namespace apex

#endif
