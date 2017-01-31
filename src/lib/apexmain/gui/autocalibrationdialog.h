/******************************************************************************
 * Copyright (C) 2007  Pieter Verlinde                                                                    *
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

/** @file
 * AutoCalibration widget declaration.
 */

#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_AUTOCALIBRATIONDIALOG_H_
#define _EXPORL_SRC_LIB_APEXMAIN_GUI_AUTOCALIBRATIONDIALOG_H_

#include <QDialog>
#include <QList>

#include <memory>

namespace apex
{

class AutoCalibrationDialogPrivate;


/** Widget for auto stimulus calibration.
 *
 *
 */
class AutoCalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    /** Creates a new autocalibration dialog. You still need to call
     * QDialog#exec() to make it work.
     *
     * @param parent the parent QWidget
     */
    AutoCalibrationDialog (QWidget *parent = NULL);

    /**Destroys the calibration dialog.
         */

    ~AutoCalibrationDialog();

        /** Sets the parameter QTreeView
         *
         */
        void setParameterList(const QList<QStringList> &values);

        /** Sets the soundlevelmeter parameter QTreeView
         *
         */
        void setSlmParameters(const QStringList& data);

        /* Sets the progressbar
         *
         */
        void setProgressBar(int min, int max, int pos);
        /* Sets the status message
         *
         */
        void setStatus(const QString& status);
        /**
         * Sets the calibrating status.
         */
        void setCalibrating (bool calibrating);
        /**
         * Selects the current parameter beïng calibrated
         * @param parameter the parameter currently beïng calibrated
         */
        void setCurrentParameterCalibrated(const QString& parameter);

        /**
         * Returns the selected parameters to save (marked checkboxes)
         */
        QStringList getSelectedParametersToSave();
        /**
         * Returns the selected parameters to calibrate
         */
        QStringList getSelectedParametersToCalibrate();

        /**
         * Updates a single parameter in the treewidget
         * @param value the new output value
         * @param difference difference between old & new parameter
         */
        void updateParameter(const QString& parameter, const QString& value, const QString& difference = "-");

        /**
         * Indicates the signal has clipped for the given parameter
         */
        void setClipped(const QString& parameter);

        /**
         * Resets the parameter QTreeWidget
         */
        void newMeasurement();
        /*
         * Returns the target calibration amplitude from
         * the advanced field
         */
        QString targetAmplitude() const;
        /*
         * Sets the target calibration amplitude from
         * the advanced field
         */
        void setTargetAmplitude(const QString&);

        /**
         * Selects all parameters by default in the treewidget
         * to autocalibrate.
         */
        void selectAllParamersByDefault();

        /**
         * Close the window by calling the accept/reject slot
         *
         * @param save save the measured results
         */
        void finalize(bool save);

public Q_SLOTS:

    /** Shows the advanced parts of the interface and allows the user to
     * set specific settings values for the soundlevelmeter.
     *
     * @param advanced true if the advanced interface should be shown, false
     * otherwise
     */
        void showAdvanced (bool advanced);

        /**
         * Enables the textbox where the user can define his own
         * calibration amplitude.
         *
         * @param checkState the state of the chechbox
         */
        void enableUserDefinedCalibrationAmplitude(int checkState);

        /** Sets the available hardware setups. This determines which setups the
     * user can select in the UI. If the currently selected value is not
     * available in the new list, the currently selected hardware setup will
     * be reset.
     *
     * @param value available hardware setups
     */
    void setHardwareSetups (const QStringList &value);

    /** Returns the available hardware setups.
     *
     * @return available hardware setups in the UI.
     */
    QStringList hardwareSetups() const;

    /** Sets the currently selected hardware setup. Pass an empty string or
     * an invalid setup to reset the current selection. Calling this method
     * will emit #hardwareSetupChanged() for changes.
     *
     * @param value hardware setup to select
     */
    void setHardwareSetup (const QString &value);

    /** Returns the currently selected hardware setup.
     *
     * @return currently selected hardware setup or an empty string if no
     * selection exists
     */
    QString hardwareSetup() const;

        /** Reenables elements that were disabled by a call to #disable().
     *
     * @see #disable()
     */
    void enable();

    /** Disables all elements that are not related to the hardware setup
     * selection. You can use this to disable the dialog if no valid
     * hardware setup is selected.
     *
     * @see #enable()
     */
    void disable();


        /**
         * Following slots get called when one of the soundlevelmeter
         * parameters is changed within the advanced window.
         */
        void slmTypeChanged(const QString&);
        void slmFreqWeighChanged(const QString&);
        void slmTimeWeighChanged(const QString&);
        void slmPercChanged(const QString&);
        void slmTimeChanged(const QString&);
Q_SIGNALS:
            /** Emitted when the user (or program) changes the hardware setup. The
     * passed \c value can only be one of the entries in the string list
     * set by #setHardwareSetups() or an empty string.
     *
     * @param value new hardware setup or an empty string if no setup is
     * selected
     */
    void hardwareSetupChanged (const QString &value);
        /** Emitted when the user requests to manage the hardware setups. You
     * can query and set the hardware setups with #hardwareSetups() and
     * #setHardwareSetups().
     */
    void manageHardwareSetups();

        /** Stops the autocalibration
         */
        void stopAutoCalibration();
        /** Start autocalibrating the selected parameters
         */
        void calibrateAllParameters();
        /** Go back to the manual calibration window
         */
        void calibrateManually();

        /** Signal is emitted when one of the soundlevelmeter
         *  parameters has changed.
         *
         *  @param the parameter that has changed
         *  @param the new value
         */
        void slmParameterChanged(const QString&, const QString&);

        /**
         * When the user unchecks the advanced calibration amplitude
         * checkbox, Autocalibration has to set the default value back.
         */
        void advancedCalibrationAmplitudeUnchecked();

        /**
         * User presses cancel
         */
        void beforeCancel();

private:
    QScopedPointer<AutoCalibrationDialogPrivate> d;

    friend class AutoCalibrationDialogPrivate;
};

} // namespace apex

#endif
