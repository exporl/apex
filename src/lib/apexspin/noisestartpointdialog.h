/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _APEX_SRC_LIB_APEXSPIN_NOISESTARTPOINTDIALOG_H_
#define _APEX_SRC_LIB_APEXSPIN_NOISESTARTPOINTDIALOG_H_

#include "ui_noisestartpointdlg.h"

namespace spin
{
namespace gui
{

class StartLevelWidget;

class NoiseStartpointDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @param   slw     The widget the reults of this dialog will be sent to.
     * @param   value   The value to be filled in in this dialog.
     *                  If < 0: startpoint will be set to random.
     */
    NoiseStartpointDialog(QWidget *parent, StartLevelWidget *slw, double value);
    ~NoiseStartpointDialog()
    {
    }

private:
    void setupUi(double value);
    void setupConnections(StartLevelWidget *slw);

    Ui::NoiseStartpointDlg widgets;

private slots:

    void accept();

signals:

    void startpointChosen(double startpoint);
};
}
}

#endif
