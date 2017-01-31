/******************************************************************************
 * Copyright (C) 2014  Jonas Vanthornhout <jonasvanthornhout+apex@gmail.com>  *
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
#ifndef _EXPORL_SRC_LIB_APEXCALADMIN_CALIBRATIONDATABASE_H_
#define _EXPORL_SRC_LIB_APEXCALADMIN_CALIBRATIONDATABASE_H_

#include "apextools/global.h"

#include "hardwaresetup.h"

#include <QCoreApplication>
#include <QSettings>

/*!
 * \brief The CalibrationDatabase class
 * This class offers access to the \ref HardwareSetup of calibrations (both local and global).
 * Once one has a \ref HardwareSetup it is possible to make them global or local.
 */
class CALIBRATIONADMIN_EXPORT CalibrationDatabase {
    Q_DECLARE_TR_FUNCTIONS(CalibrationDatabase)
public:
    explicit CalibrationDatabase();

    HardwareSetups hardwareSetups() const;
    HardwareSetups localHardwareSetups() const;
    HardwareSetups globalHardwareSetups() const;

    /*!
     * \brief Get the \ref HardwareSetup with the given name.
     * \throw If no \ref HardwareSetup with the given name is found, a \ref std::range_error is thrown.
     */
    HardwareSetup setup(const QString& hardwareSetupName) const;

    /*!
     * \brief Make the given \ref HardwareSetup local.
     * \throw If the given \ref HardwareSetup is already local then a \ref std::domain_error is thrown.
     */
    void makeLocal(HardwareSetup hardwareSetup);

    /*!
     * \brief Make the given \ref HardwareSetup global.
     * \throw If the given \ref HardwareSetup is already global then a \ref std::domain_error is thrown.
     */
    void makeGlobal(HardwareSetup hardwareSetup);

    /*!
     * \brief Checks if the \ref CalibrationDatabase can write to both local and global settings.
     * \return True if it is possible to write to local and global settings, false otherwise.
     */
    bool isWritable() const;
private:
    QSettings* localSettings;
    QSettings* globalSettings;

    bool exists(QString hardwareSetupName) const;

    /*!
     * \brief Get the names of the \ref HardwareSetup "HardwareSetups" from a \ref QSetting.
     * \param settings The \ref QSetting where the names should be fetched from.
     * This QSetting has either UserScope or SystemScope (see: http://qt-project.org/doc/qt-5/qsettings.html#Scope-enum).
     * \return A \ref QStringList with the names of the \ref HardwareSetup "HardwareSetups" present in the given \ref QSetting.
     */
    QStringList hardwareSetupNames(QSettings* settings) const;

    /*!
     * \brief Convert \ref HardwareSetups to a \ref QStringList.
     * \param names The names of the \ref HardwareSetup "HardwareSetups" to convert.
     * \param isLocal Indicates if the \ref HardwareSetup "HardwareSetups" will be local or not.
     * \return \ref HardwareSetups constructed from the given names and the given flag.
     */
    HardwareSetups QStringList2HardwareSetups(const QStringList& names, bool isLocal) const;

    /*!
     * \brief Move the \ref HardwareSetup with the given name to another QSetting (i.e. make it global or local).
     * \param from The \ref QSetting where the \ref HardwareSetup with the given name is currently located.
     * This QSetting has either UserScope or SystemScope (see: http://qt-project.org/doc/qt-5/qsettings.html#Scope-enum).
     * \param to The \ref QSetting where the \ref HardwareSetup with the given name has to be moved to.
     * This QSetting has either UserScope or SystemScope (see: http://qt-project.org/doc/qt-5/qsettings.html#Scope-enum).
     * \throw Throws a \ref std::domain_error if it is not possible to write the settings (i.e. at least one of the two settings is not writable).
     */
    void move(const QString& hardwareSetupName, QSettings* from, QSettings* to);
};

#endif // _EXPORL_SRC_LIB_APEXCALADMIN_CALIBRATIONDATABASE_H_
