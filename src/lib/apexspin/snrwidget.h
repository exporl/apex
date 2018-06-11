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

#ifndef _APEX_SRC_LIB_APEXSPIN_SNRWIDGET_H_
#define _APEX_SRC_LIB_APEXSPIN_SNRWIDGET_H_

#include "ui_snrwidget.h"

namespace spin
{
namespace gui
{

class SnrWidget : public QWidget
{
    Q_OBJECT

public:
    SnrWidget(QWidget *parent = 0);

public slots:

    /**
     * Shows the SNR calculation tool.
     */
    void showSnr();

    /**
     * Shows a warning telling that all levels have to be locked together
     * with a button to do just that.
     */
    void showWarning();

    /**
     * Sets the speechlevel. Connect this to a "contents changed" signal
     * of the input field of the speaker levels widget.
     */
    void setSpeechlevel(double level);

    /**
     * Sets the noiselevel. Connect this to a "contents changed" signal
     * of the input field of the speaker levels widget.
     */
    void setNoiselevel(double level);

signals:

    /**
     * Emitted when the "lock levels" button has been clicked.
     */
    void lockButtonClicked();

    /**
     * Emitted when the levels changed.
     */
    void levelsChanged(double speech, double noise);

private:
    void setupUi();
    void setupConnections();

    /**
     * Calculates the SNR given speech and noiselevels.
     */
    double calculateSnr(double speech, double noise);

    /**
     * Calculates the speechlevel given the SNR and noiselevel.
     */
    double calculateSpeechlevel(double snr, double noise);

    /**
     * Calculates the noiselevel given the SNR and speechlevel.
     */
    double calculateNoiselevel(double snr, double speech);

    Ui::SnrWidget widgets;

    enum { SNR, WARNING };

    bool updatingSNR;

private slots:

    void updateSnr();
};

} // ns gui
} // ns spin

#endif
