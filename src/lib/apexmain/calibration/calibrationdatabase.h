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
 * Calibration database declaration.
 */

#ifndef _APEX_SRC_CALIBRATION_CALIBRATIONDATABASE_H_
#define _APEX_SRC_CALIBRATION_CALIBRATIONDATABASE_H_

#include <QString>

#include "global.h"

namespace apex
{

/** Calibration database. Associates a hardware setup, calibration profile and
 * parameter name with a target amplitude and a calibrated output parameter.
 *
 * Each calibration database entry is characterized by the following
 * parameters:
 * - hardware setup: name for a specific configuration of the hardware
 *   setup, e.g., for different sound cards, with and without insert phones
 *   etc.
 * - calibration profile: experiment setup that is calibrated, e.g., speech in
 *   noise
 * - parameter name: parameter of the experiment that is calibrated, e.g., gain
 */
class APEX_EXPORT CalibrationDatabase
{
public:
    /** Creates a new instance. Because the calibration database is saved
     * with QSettings, you need to call
     * QCoreApplication#setApplicationName(),
     * QCoreApplication#setOrganizationName() and
     * QCoreApplication#setOrganizationDomain() beforehand.
     */
    CalibrationDatabase();

    /** Destroys the instance. */
    ~CalibrationDatabase();

    /** Returns all available hardware setups.
     * This list may be changed with #addHardwareSetup() and
     * #removeHardwareSetup().
     *
     * @return list with all available hardware setups for which calibration
     * data exists
     *
     * @see #calibrationProfiles(), #parameterNames()
     */
    QStringList hardwareSetups() const;

    /** Returns all available calibration profiles for a certain hardware setup.
     * This list is changed implicitly if #calibrate() or #remove() is called.
     *
     * @param setup hardware setup
     * @return list with all calibration profiles for which calibration results
     * exist in the database
     *
     * @see #hardwareSetups(), #parameterNames()
     */
    QStringList calibrationProfiles (const QString &setup) const;

    /** Returns all available parameters for a certain hardware setup and
     * calibration profile. This list is changed implicitly if #calibrate() or
     * #remove() is called.
     *
     * @param setup hardware setup
     * @param profile calibration profile
     * @return list with all parameters for which calibration results exist in
     * the database
     *
     * @see #hardwareSetups(), #calibrationProfiles()
     */
    QStringList parameterNames (const QString &setup,
            const QString &profile) const;

    /** Returns the total number of all calibration results for the given
     * hardware setup.
     *
     * @param setup hardware setup @return total number of all calibration
     * results for the given hardware setup
     */
    unsigned calibrationCount (const QString &setup) const;

    /** Adds a new hardware setup to the calibration database. You do not
     * need to call this method if you just want to add a new calibration
     * for an unknown hardware setup, #calibrate() will take care to update
     * the list of available hardware setups itself. If the setup already
     * exists in the database, nothing is added.
     *
     * @param setup new hardware setup to add
     */
    void addHardwareSetup (const QString &setup);

    /** Removes a hardware setup and all its calibrations from the database.
     * Take care with calling this method, it also removes all calibrations
     * that are associated with this hardware setup! If the hardware setup
     * does not exist in the database, nothing happens.
     *
     * @param setup hardware setup to remove
     */
    void removeHardwareSetup (const QString &setup);

    /** Renames a hardware setup. If the new setup already exists,
     * calibrations with the same calibration profile and parameter will be
     * overwritten. If the hardware setup does not exist in the database, an
     * empty hardware setup with the new name will be created.
     *
     * @param setup hardware setup to rename
     * @param newName new name for the hardware setup
     */
    void renameHardwareSetup (const QString &setup, const QString &newName);

    /** Exports the given hardware setup to an XML file.
     *
     * @param setup hardware setup to export
     * @param fileName file name of the XML file
     *
     * @throws std::exception on IO errors
     */
    void exportHardwareSetup (const QString &setup,
            const QString &fileName) const;

    /** Imports the given hardware setup from an XML file. Already existing
     * setups will be overwritten. Before an exception is thrown, a
     * partially imported setup will be deleted. The functions returns the
     * name of the imported setup on success.
     *
     * @param fileName file name of the XML file
     * @return name of the imported hardware setup
     *
     * @throws std::exception on IO and XML errors
     */
    QString importHardwareSetup (const QString &fileName);

    /** Returns whether a certain combination of hardware setup, calibration
     * profile and parameter name has already been calibrated. You can then use
     * #target() and #parameter() to obtain further information about the
     * calibration.
     *
     * Do not use slashes in the @a setup, @a profile and @a name arguments,
     * they will be replaced by underscores.
     *
     * @param setup hardware setup for the calibration
     * @param profile calibration profile
     * @param name parameter name
     * @return true if this combination of hardware setup, calibration profile
     * and parameter name has already been calibrated, false otherwise
     */
    bool isCalibrated (const QString &setup, const QString &profile,
            const QString &name) const;

    /** Returns the target amplitude for a calibration. You have to ensure
     * that a calibration for the given combination of hardware setup,
     * calibration profile and parameter name really exists, otherwise the
     * result will be undefined.
     *
     * Do not use slashes in the @a setup, @a profile and @a name arguments,
     * they will be replaced by underscores.
     *
     * @param setup hardware setup for the calibration
     * @param profile calibration profile
     * @param name parameter name
     * @return target amplitude of the calibration for the given combination
     * of hardware setup, calibration profile and parameter name, the result is
     * undefined if no such calibration exists
     */
    double targetAmplitude (const QString &setup, const QString &profile,
            const QString &name) const;

    /** Returns the output parameter for a calibration. You have to ensure
     * that a calibration for the given combination of hardware setup,
     * calibration profile and parameter name really exists, otherwise the
     * result will be undefined.
     *
     * Do not use slashes in the @a setup, @a profile and @a name arguments,
     * they will be replaced by underscores.
     *
     * @param setup hardware setup for the calibration
     * @param profile calibration profile
     * @param name parameter name
     * @return output parameter of the calibration for the given combination
     * of hardware setup, calibration profile and parameter name, the result is
     * undefined if no such calibration exists
     */
    double outputParameter (const QString &setup, const QString &profile,
            const QString &name) const;

    /** Stores a calibration result in the database. This method associates
     * a combination of hardware setup, calibration profile and parameter name
     * with the given target amplitude and calibrated output parameter. If such
     * a calibration already exists, it will be overwritten.
     *
     * Do not use slashes in the @a setup, @a profile and @a name arguments,
     * they will be replaced by underscores.
     *
     * @param setup hardware setup for the calibration
     * @param profile calibration profile
     * @param name parameter name
     * @param target target amplitude of the calibration
     * @param parameter output parameter of the calibration
     */
    void calibrate (const QString &setup, const QString &profile,
            const QString &name, double target, double parameter);

    /** Removes a calibration result from the database. Nothing will happen
     * if the calibration does not exist.
     *
     * @param setup hardware setup for the calibration
     * @param profile calibration profile
     * @param name parameter name
     */
    void remove (const QString &setup, const QString &profile,
            const QString &name);

    /** Returns the currently selected hardware setup for the UI that was set
     * with #setCurrentHardwareSetup().
     *
     * @return currently selected hardware setup
     */
    QString currentHardwareSetup() const;

    /** Sets the currently selected hardware setup for the UI.
     *
     * @param setup currently selected hardware setup
     */
    void setCurrentHardwareSetup (const QString &setup);
};

} // namespace apex

#endif

