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
 * Calibration setup management dialog declaration.
 */

#ifndef _EXPORL_SRC_LIB_APEXMAIN_GUI_CALIBRATIONSETUPSDIALOG_H_
#define _EXPORL_SRC_LIB_APEXMAIN_GUI_CALIBRATIONSETUPSDIALOG_H_

#include <QDialog>

#include <memory>

namespace apex
{

class CalibrationSetupsDialogPrivate;

/** Dialog for the management of the available calibration setups.
 */
class CalibrationSetupsDialog : public QDialog
{
public:
    /** Creates a new calibration setup management dialog. You still need to
     * call QWidget#show() to make it visible. On successful completion, the
     * dialog will perform the requested modifications to the available
     * calibration setups.
     *
     * @param parent the parent QWidget
     *
     * @see CalibrationDatabase
     */
    CalibrationSetupsDialog(QWidget *parent = NULL);

    /** Destroys the calibration setups dialog. */
    ~CalibrationSetupsDialog();

protected:
    const QScopedPointer<CalibrationSetupsDialogPrivate> d;
};

} // namespace apex

#endif
