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

#ifndef _APEX_SRC_LIB_APEXSPIN_STARTLEVELWIDGET_H_
#define _APEX_SRC_LIB_APEXSPIN_STARTLEVELWIDGET_H_

#include "spinusersettings.h"

#include <QDoubleValidator>
#include <QWidget>

namespace Ui
{
class StartLevelWidget;
}

namespace spin
{
namespace gui
{

class StartLevelWidget : public QWidget
{
    Q_OBJECT

public:
    StartLevelWidget(QWidget *parent);
    virtual ~StartLevelWidget();

    void showAngle(uint angle);
    bool hasSpeechLevel() const;
    bool hasNoiseLevel() const;
    bool hasLevels() const;

    /**
     * Returns the filled in value of the speech line edit.
     * Will assert if hasSpeechLevel() returns false.
     */
    double speechLevel() const;

    /**
     * Returns the filled in value of the noise line edit.
     * Will assert if hasNoiseLevel() returns false.
     */
    double noiseLevel() const;

    /**
     * Returns the speaker levels filled in in this widget.
     * If there are none filled in, hasLevels will be set to false.
     */
    data::SpeakerLevels speakerLevels(bool *hasLevels) const;

    /**
     * Fills the widget with the given speaker levels.
     */
    void setSpeakerLevels(const data::SpeakerLevels &levels);

    /**
     * Checks whether all obliged fields are filled in, if not, w will
     * be set to the first found widget that was not filled in.
     */
    bool hasAllObligedFields(QWidget **w);

public slots:

    /**
     * Clears all widgets:
     *  * Unchecks QCheckBoxes
     *  * Clears QLineEdits
     */
    void clear();

    /**
     * Shows/hides the widgets conecerning oncorrelated noises
     */
    void setUncorrelatedNoisesVisible(bool visible);

    /**
     * Sets the noise startlevel if the checkbox is checked.
     */
    void setNoiseStartlevel(double level);

    /**
     * Sets the speech startlevel if the checkbox is checked.
     */
    void setSpeechStartlevel(double level);

    /**
     * Returns the start point of the noise that has been set in this
     * widget. Will return spin::data::RANDOM if random;
     */
    double noiseStartPoint() const;

signals:

    /**
     * Emitted when something has changed in one of the widgets.
     */
    void contentsChanged();

    void speechlevelChanged(double level);
    void noiselevelChanged(double level);

    /**
     * Emitted when the user checks/unchecks the speech checkbox.
     */
    void speechUsedChanged(bool used);

    /**
     * Emitted when the user checks/unchecks the noise checkbox.
     */
    void noiseUsedChanged(bool used);

private slots:

    void updateNoiseStartPoint(double to);
    void showNoiseStartPointDialog();
    void noiselevelChanged(QString level);
    void speechlevelChanged(QString level);

    void printChange(QString to);

private:
    void setupUi();
    void setupConnections();
    void setupValidators();

    Ui::StartLevelWidget *widgets;
    QDoubleValidator dBVal;

    double noiseJump;

    enum { RANDOM = data::RANDOM };
};
}
}

#endif
