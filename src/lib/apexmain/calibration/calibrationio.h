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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_CALIBRATION_CALIBRATIONIO_H_
#define _EXPORL_SRC_LIB_APEXMAIN_CALIBRATION_CALIBRATIONIO_H_

#include <QObject>

#include <memory>

namespace apex
{

class ExperimentRunDelegate;

namespace stimulus
{
class StimulusOutput;
}

class CalibrationIOPrivate;

/**
 * Do IO related things for the calibrator, like loading a stimulus and
 * setting a parameter
 *
 *  TODO CALIB notification is done with non-transient messageboxes
 */
class CalibrationIO : public QObject
{
    Q_OBJECT
public:
    // window is not used for ownership
    CalibrationIO(ExperimentRunDelegate *runDelegate, bool checkClipping);
    ~CalibrationIO();

public Q_SLOTS:
    // Sets the next stimulus. If already playing, the stimulus is changed and *
    // the output restarted
    void setStimulus(const QString &name);

    void startOutput();
    void stopOutput();

    // Sets a parameter to the given value. Has immediate effect.
    void setParameter(const QString &name, double value);
    void queueParameter(const QString &name, double value);

    // If true, loop the current stimulus infinitely, else stop after one
    // presentation.
    void setLooping(bool loop);

Q_SIGNALS:
    void playingChanged(bool value);
    void clippingOccured(bool clipping);

private:
    const QScopedPointer<CalibrationIOPrivate> d;
};

} // namespace apex

#endif
