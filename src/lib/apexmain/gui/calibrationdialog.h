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

/** @file
 * Calibration widget declaration.
 */

#ifndef _APEX_SRC_GUI_CALIBRATIONDIALOG_H_
#define _APEX_SRC_GUI_CALIBRATIONDIALOG_H_

#include <QDialog>

#include <memory>

namespace apex
{

class CalibrationDialogPrivate;

/** Widget for stimulus calibration.
 *
 * It provides an easy interface to the user for the calibration of an
 * output parameter (e.g., a stimulus amplitude in the range between 0 and
 * 1) and the corresponding achieved output amplitude (e.g., a certain sound
 * pressure level).
 *
 * The output parameter can be entered manually into a line edit or selected
 * via a slider.
 *
 * @ingroup calibration
 */
class CalibrationDialog : public QDialog
{
    Q_OBJECT
    /** Available hardware setups. */
    Q_PROPERTY (QStringList hardwareSetups READ hardwareSetups
            WRITE setHardwareSetups)
    /** Currently selected hardware setup. */
    Q_PROPERTY (QString hardwareSetup READ hardwareSetup
            WRITE setHardwareSetup)
    /** Minimum output parameter value. */
    Q_PROPERTY (double minimumOutputParameter READ minimumOutputParameter
            WRITE setMinimumOutputParameter)
    /** Maximum output parameter value. */
    Q_PROPERTY (double maximumOutputParameter READ maximumOutputParameter
            WRITE setMaximumOutputParameter)
    /** Current output parameter value. */
    Q_PROPERTY (QString outputParameter READ outputParameter
            WRITE setOutputParameter)
    /** Target amplitude for calibration. */
    Q_PROPERTY (QString targetAmplitude READ targetAmplitude
            WRITE setTargetAmplitude)
    /** Amplitude measured by the user. */
    Q_PROPERTY (QString measuredAmplitude READ measuredAmplitude
            WRITE setMeasuredAmplitude)

public:
    /** Creates a new calibration dialog. You still need to call
     * QDialog#exec() to make it work.
     *
     * Set a default target amplitude with #setTargetAmplitude() and the
     * output parameter range and value with #setMinimumOutputParameter(),
     * #setMaximumOutputParameter() and #setOutputParameter().
     *
     * Connect to #outputParameterChanged() to be able to react to the
     * adjustments of the output parameter made by the user.
     *
     * @param parent the parent QWidget
     */
    CalibrationDialog (QWidget *parent = NULL);

    /** Destroys the calibration dialog. */
    ~CalibrationDialog();

public Q_SLOTS:
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

    /** Sets the database status that is displayed in the user interface.
     *
     * @param status database status description
     */
    void setDatabaseStatus (const QString &status);

    /** Sets the available stimuli. This determines which stimuli the
     * user can select in the advanced UI. If the currently selected value is
     * not available in the new list, the currently selected stimulus will
     * be reset.
     *
     * @param value available stimuli
     */
    void setStimuli (QStringList value);

    /** Returns the available stimuli.
     *
     * @return available stimuli in the UI.
     */
    QStringList stimuli() const;

    /** Sets the currently selected stimulus. Pass an empty string or
     * an invalid stimulus to reset the current selection. Calling this method
     * will emit #stimulusChanged() for changes.
     *
     * @param value stimulus to select
     */
    void setStimulus (const QString &value);

    /** Returns the currently selected stimulus.
     *
     * @return currently selected stimulus or an empty string if no selection
     * exists
     */
    QString stimulus() const;

    /** Sets whether stimuli should loop. Calling this method will emit
     * #loopingChanged() for changes.
     *
     * @param value true if stimuli should loop, false otherwise
     */
    void setLooping (bool value);

    /** Returns whether stimuli should loop.
     *
     * @return true if stimuli are looping, false otherwise
     * exists
     */
    bool isLooping() const;

    /** Disables and enables the right buttons depending whether a stimulus is
     * currently playing.
     *
     * @param value true if playing, false otherwise
     */
    void setPlaying (bool value);

    /** Disables or enables the ok button depending whether the calibration is
     * considered complete.
     *
     * @param value true if the ok button should be enabled, false otherwise
     */
    void setComplete (bool value);

    /** Sets the available parameters. If the currently selected value is not
     * available in the new list, the currently selected parameter will be
     * reset. This will also reset the muted parameters to the default. The
     * signal #parameterMuted() will only be emitted for the new parameters.
     *
     * @param value available parameters
     */
    void setParameterNames (const QStringList &value);

    /**
     * If true, shows an autocalibrate button
     */
    void setAutoCalibrateEnabled (bool value);

    /** Returns the available parameters.
     *
     * @return available parameters in the UI
     */
    QStringList parameterNames() const;

    /** Sets the muted parameters.
     *
     * @param value muted parameters
     */
    void setMutedParameters (const QStringList &value);

    /** Returns the muted parameters.
     *
     * @return muted parameters in the UI
     */
    QStringList mutedParameters() const;

    /** Sets the currently selected parameter. Pass an empty string or
     * an invalid parameter to reset the current selection. Calling this method
     * will emit #parameterChanged() for changes. This will also reset the muted
     * parameters to the default.
     *
     * @param value parameter to select
     */
    void setParameterName (const QString &value);

    /** Returns the currently selected parameter.
     *
     * @return currently selected parameter or an empty string if no selection
     * exists
     */
    QString parameterName() const;

    /** Sets the minimum allowed value for the output parameter. This will
     * update the slider labels and adjust the slider position. Invalid
     * values or values below this limit may still be entered manually, but
     * #outputParameterChanged() will not be emitted in this case.
     *
     * @param value minimum output parameter value
     */
    void setMinimumOutputParameter (double value);

    /** Returns the current minimum output parameter. See
     * #setMinimumOutputParameter() for details.
     *
     * @return current minimum output parameter
     * @see setMinimumOutputParameter()
     */
    double minimumOutputParameter() const;

    /** Sets the maximum allowed value for the output parameter. This will
     * update the slider labels and adjust the slider position. Invalid
     * values or values above this limit may still be entered manually, but
     * #outputParameterChanged() will not be emitted in this case.
     *
     * @param value maximum output parameter value
     */
    void setMaximumOutputParameter (double value);

    /** Returns the current maximum output parameter. See
     * #setMaximumOutputParameter() for details.
     *
     * @return current maximum output parameter
     * @see setMaximumOutputParameter()
     */
    double maximumOutputParameter() const;

    /** Sets the current output parameter. You can also specify invalid
     * double values here, but then only the line edit and not the slider
     * will be updated.
     *
     * @param value current output parameter value
     */
    void setOutputParameter (const QString &value);

    /** Returns the current output parameter. For invalid double values or
     * values exceeding the limits set by #setMinimumOutputParameter() and
     * #setMaximumOutputParameter(), an empty string is returned.
     *
     * @return current output parameter or an empty string for invalid
     * values
     */
    QString outputParameter() const;

    /** Sets the target amplitude. You can specify invalid
     * double values here, but then a subsequent call to #targetAmplitude()
     * will return an empty string.
     *
     * @param amplitude target amplitude
     */
    void setTargetAmplitude (const QString &amplitude);

    /** Returns the target amplitude. For invalid double values, an empty
     * string is returned.
     *
     * @return target amplitude or an empty string for invalid values
     */
    QString targetAmplitude() const;

    /** Sets the measured amplitude. You can specify invalid double values
     * here, but then a subsequent call to #measuredAmplitude() will return
     * an empty string and the user can not request an automatic output
     * parameter adjustment.
     *
     * @param amplitude measured amplitude
     * @see #correctOutputParameter()
     */
    void setMeasuredAmplitude (const QString &amplitude);

    /** Returns the measured amplitude as entered by the user. For invalid
     * double values, an empty string is returned.
     *
     * @return measured output amplitude as entered by the user or an empty
     * string for invalid values
     */
    QString measuredAmplitude() const;

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

    /** Shows the advanced parts of the interface and allows manual setting
     * of the stimulus level.
     *
     * @param advanced true if the advanced interface should be shown, false
     * otherwise
     */
    void showAdvanced (bool advanced);

    /** Sets the unit that is used to display measured and target amplitude.
     *
     * @param unit unit to use
     */
    void setUnit (const QString &unit);

    /** Returns the unit that is used to display measured and target amplitude.
     *
     * @return unit that is used
     */
    QString unit() const;

    /** Sets whether a clipping warning is displayed.
     *
     * @param value true if a clipping warning should be displayed, false
     * otherwise
     */
    void setClipping (bool value);

	/**
	 * Close the window by calling the accept/reject slot
	 *
	 * @param save save the measured results
	 */
	void finalize(bool save);

Q_SIGNALS:
    /** Emitted when the user (or program) changes the hardware setup. The
     * passed \c value can only be one of the entries in the string list
     * set by #setHardwareSetups() or an empty string.
     *
     * @param value new hardware setup or an empty string if no setup is
     * selected
     */
    void hardwareSetupChanged (const QString &value);

    /** Emitted when the user (or program) changes the stimulus. The
     * passed \c value can only be one of the entries in the string list
     * set by #setStimuli() or an empty string.
     *
     * @param value new stimulus or an empty string if no stimulus is
     * selected
     */
    void stimulusChanged (const QString &value);

    /** Emitted when the user (or program) changes whether stimuli should loop.
     *
     * @param value true if stimuli are looping
     */
    void loopingChanged (bool value);

    /** Emitted when the user (or program) changes the muted state of a
     * parameter.
     *
     * @param name parameter name
     * @param muted true if the parameter is muted, false otherwise
     */
    void mutingChanged (const QString &name, bool muted);

    /** Emitted when the user (or program) changes the parameter. The
     * passed \c value can only be one of the entries in the string list
     * set by #setParameters() or an empty string.
     *
     * @param value new parameter or an empty string if no parameter is
     * selected
     */
    void parameterNameChanged (const QString &value);

    /** Emitted when the user requests to manage the hardware setups. You
     * can query and set the hardware setups with #hardwareSetups() and
     * #setHardwareSetups().
     */
    void manageHardwareSetups();

    /** Emitted when the user (or program) changes the target amplitude. For
     * invalid values, an empty string is passed as the parameter.
     *
     * @param value new target amplitude value or an empty string if the new
     * target amplitude is invalid
     */
    void targetAmplitudeChanged (const QString &value);

    /** Emitted when the user (or program) changes the output parameter. For
     * invalid values or values not in the range specified with
     * #setMinimumOutputParameter() and #setMaximumOutputParameter(), an
     * empty string is passed as the parameter.
     *
     * @param value new output parameter value or an empty string if the new
     * output parameter is invalid
     */
    void outputParameterChanged (const QString &value);

    /** Emitted when the user requests to automatically adjust the output
     * parameter. The measured output amplitude for the current output
     * parameter value is provided in \p amplitude. You should retrieve the
     * current output parameter value with #outputParameter() and adjust it
     * to cope for the difference between the measured and target output
     * amplitude. Then update the UI with the new output parameter by calling
     * #setOutputParameter(). You should limit the amplitude to the range given
     * by #minimumOutputParameter() and #maximumOutputParameter(), otherwise
     * further calls to correctOutputParameter will fail because the current
     * output amplitude can not be retrieved.
     *
     * @param amplitude measured output amplitude as entered by the user
     */
    void correctOutputParameter (double amplitude);

    /** Emitted when the user requests to start the sound output.
     */
    void startOutput();

    /** Emitted when the user requests to stop the sound output.
     */
    void stopOutput();

    /** Emitted when the user requests to save the calibration. (Apply button)
     */
    void saveCalibration();

    /** Emitted when the user clicks the autoCalibrateAll button
     */
    void autoCalibrateAll();

    /** Emitted when the user clicks the autoCalibrate button for a stimulus
     */
    void autoCalibrateSingle(const QString& parameter);

	/** Emitted when the user presses cancel
	 */
	void beforeCancel();

private:
    std::auto_ptr<CalibrationDialogPrivate> d;

    friend class CalibrationDialogPrivate;
};

} // namespace apex

#endif
