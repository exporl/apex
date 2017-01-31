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

#ifndef _APEX_SRC_CALIBRATION_CALIBRATIONDATA_H_
#define _APEX_SRC_CALIBRATION_CALIBRATIONDATA_H_

#include <QMap>
#include <QSharedDataPointer>

#include "global.h"

class QStringList;

namespace apex
{

namespace data
{

class CalibrationParameterDataPrivate;
class SoundLevelMeterData;

//FIXME why doesn't this inherit SimpleParameters?
class APEXDATA_EXPORT CalibrationParameterData
{
public:
    CalibrationParameterData();
    CalibrationParameterData (double minimumParameter, double maximumParameter,
            double defaultParameter, double muteParameter,
            double defaultTargetAmplitude, double finalTargetAmplitude);
    ~CalibrationParameterData();
    CalibrationParameterData (const CalibrationParameterData &other);
    CalibrationParameterData &operator= (const CalibrationParameterData &other);

    double minimumParameter() const;
    double maximumParameter() const;
    double defaultParameter() const;
    double muteParameter() const;
    double defaultTargetAmplitude() const; //calibration
    double finalTargetAmplitude() const; //target

    void setFinalTargetAmplitude(const double a);

    bool operator==(const CalibrationParameterData& other) const;

private:
    QSharedDataPointer<CalibrationParameterDataPrivate> d;
};

class CalibrationDataPrivate;

/**
 * Data for calibrator obtained after parsing an experiment file.
 */
class APEXDATA_EXPORT CalibrationData
{
public:
    CalibrationData();
    ~CalibrationData();

    void setCalibrationProfile (const QString &value);
    QString calibrationProfile() const;

    void addAvailableStimulus (const QString &value);
    QStringList availableStimuli() const;

    void addParameter (const QString &name, const CalibrationParameterData &value);
    QMap<QString, CalibrationParameterData> parameters() const;

    void setSoundLevelMeterData(data::SoundLevelMeterData* d);
    const SoundLevelMeterData* soundLevelMeterData() const;

    bool operator==(const CalibrationData& other) const;

private:
    CalibrationDataPrivate* d;

    //prevent copying
    CalibrationData(const CalibrationData&);
    CalibrationData& operator=(const CalibrationData&);
};

} // namespace data

} // namespace apex

#endif
