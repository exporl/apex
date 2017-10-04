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

#include "apextools/apextools.h"

#include "calibrationdata.h"
#include "soundlevelmeterdata.h"

#include <QStringList>

#include <memory>

namespace apex
{

namespace data
{

class CalibrationParameterDataPrivate : public QSharedData
{
public:
    CalibrationParameterDataPrivate()
        : minimumParameter(0),
          maximumParameter(0),
          defaultParameter(0),
          muteParameter(0),
          defaultTargetAmplitude(0),
          finalTargetAmplitude(0)
    {
    }

    CalibrationParameterDataPrivate(double minimumParameter,
                                    double maximumParameter,
                                    double defaultParameter,
                                    double muteParameter,
                                    double defaultTargetAmplitude,
                                    double finalTargetAmplitude)
        : minimumParameter(minimumParameter),
          maximumParameter(maximumParameter),
          defaultParameter(defaultParameter),
          muteParameter(muteParameter),
          defaultTargetAmplitude(defaultTargetAmplitude),
          finalTargetAmplitude(finalTargetAmplitude)
    {
    }

    double minimumParameter;
    double maximumParameter;
    double defaultParameter;
    double muteParameter;
    double defaultTargetAmplitude;
    double finalTargetAmplitude;
};

class CalibrationDataPrivate
{
public:
    QString calibrationProfile;
    QStringList availableStimuli;
    QMap<QString, CalibrationParameterData> parameters;
    QScopedPointer<SoundLevelMeterData> soundLevelMeterData;
};

// CalibrationParameterData ====================================================

CalibrationParameterData::CalibrationParameterData()
    : d(new CalibrationParameterDataPrivate)
{
}

CalibrationParameterData::CalibrationParameterData(
    double minimumParameter, double maximumParameter, double defaultParameter,
    double muteParameter, double defaultTargetAmplitude,
    double finalTargetAmplitude)
    : d(new CalibrationParameterDataPrivate(
          minimumParameter, maximumParameter, defaultParameter, muteParameter,
          defaultTargetAmplitude, finalTargetAmplitude))
{
}

CalibrationParameterData::~CalibrationParameterData()
{
}

CalibrationParameterData::CalibrationParameterData(
    const CalibrationParameterData &other)
    : d(other.d)
{
}

CalibrationParameterData &CalibrationParameterData::
operator=(const CalibrationParameterData &other)
{
    d = other.d;
    return *this;
}

double CalibrationParameterData::minimumParameter() const
{
    return d->minimumParameter;
}

double CalibrationParameterData::maximumParameter() const
{
    return d->maximumParameter;
}

double CalibrationParameterData::defaultParameter() const
{
    return d->defaultParameter;
}

double CalibrationParameterData::muteParameter() const
{
    return d->muteParameter;
}

double CalibrationParameterData::defaultTargetAmplitude() const
{
    return d->defaultTargetAmplitude;
}

double CalibrationParameterData::finalTargetAmplitude() const
{
    return d->finalTargetAmplitude;
}

void CalibrationParameterData::setFinalTargetAmplitude(const double a)
{
    d->finalTargetAmplitude = a;
}

bool CalibrationParameterData::
operator==(const CalibrationParameterData &other) const
{
    return d->minimumParameter == other.d->minimumParameter &&
           d->maximumParameter == other.d->maximumParameter &&
           d->defaultParameter == other.d->defaultParameter &&
           d->muteParameter == other.d->muteParameter &&
           d->defaultTargetAmplitude == other.d->defaultTargetAmplitude &&
           d->finalTargetAmplitude == other.d->finalTargetAmplitude;
}

// CalibrationData =============================================================

CalibrationData::CalibrationData() : d(new CalibrationDataPrivate)
{
}

CalibrationData::~CalibrationData()
{
    delete d;
}

void CalibrationData::setCalibrationProfile(const QString &value)
{
    d->calibrationProfile = value;
}

QString CalibrationData::calibrationProfile() const
{
    return d->calibrationProfile;
}

void CalibrationData::addAvailableStimulus(const QString &value)
{
    d->availableStimuli.append(value);
}

QStringList CalibrationData::availableStimuli() const
{
    return d->availableStimuli;
}

void CalibrationData::addParameter(const QString &name,
                                   const CalibrationParameterData &value)
{
    d->parameters.insert(name, value);
}

QMap<QString, CalibrationParameterData> CalibrationData::parameters() const
{
    return d->parameters;
}

void CalibrationData::setSoundLevelMeterData(data::SoundLevelMeterData *slmdata)
{
    d->soundLevelMeterData.reset(slmdata);
}

const SoundLevelMeterData *CalibrationData::soundLevelMeterData() const
{
    return d->soundLevelMeterData.data();
}

bool CalibrationData::operator==(const CalibrationData &other) const
{
    return d->calibrationProfile == other.d->calibrationProfile &&
           ApexTools::haveSameContents(d->availableStimuli,
                                       other.d->availableStimuli) &&
           d->parameters == other.d->parameters &&
           (d->soundLevelMeterData.data() ==
                other.d->soundLevelMeterData.data() ||
            *d->soundLevelMeterData == *other.d->soundLevelMeterData);
}

} // namespace data

} // namespace apex
