/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_CALIBRATION_SOUNDLEVELMETER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_CALIBRATION_SOUNDLEVELMETER_H_

#include <QString>
#include <QStringList>
#include <QtPlugin>

/**
 * Interface to sound level meters. Modelled based on B&K meters.
 *
 * A typical measurement works as follows:
 *
 * void measure {
 *  SoundLevelMeter slm;
 *
 *  slm.connect();
 *  slm.setTransducer("4196");
 *  slm.startMeasurement(SPL_Measurement,
 *                      A_Weighting,
 *                      FastWeighting );
 *
 *   ... wait a few seconds ...
 *
 *  if ( measure() ) {
 *      print ( result() );
 *  } else {
 *      print ( "Error: " + slm.error() );
 * }
 *
 *  slm.disconnect();
 *
 * }
 *
 * Type, frequency and time must be specified when starting a measurement.
 * Using some SLMs it is however possible to get other combinations for
 * the same measurement. In that case, use the measure(int,int,int) method.
 *
 * Most methods return true on success and false on error. If false,
 * you can get an error message using the error() method.
 *
 */
class SoundLevelMeter
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~SoundLevelMeter()
    {
    }

    virtual bool connect() = 0;
    virtual bool disconnect() = 0;

    virtual QString errorString() const = 0;

    virtual QStringList transducers() const = 0;
    virtual bool setTransducer(const QString &name) = 0;

    virtual QStringList instruments() const = 0;
    virtual bool setInstrument(const QString &name) = 0;

    virtual bool startMeasurement(int type_measurement, int frequency_weighting,
                                  int time_weighting,
                                  double percentile = 0) = 0;
    virtual bool stopMeasurement() = 0;

    virtual bool measure(int type_measurement, int frequency_weighting,
                         int time_weighting) = 0;
    virtual bool measure(const QString &what) = 0;
    virtual bool measure() = 0;

    virtual double result() const = 0;

    enum type_measurement { SPL_Measurement = 0, pkSPL_Measurement = 1 };
    enum frequency_weighting {
        A_Weighting = 0,
        C_Weighting = 1,
        Z_Weighting = 2
    };
    enum time_weighting {
        FastWeighting = 0,
        SlowWeighting = 1,
        ImpulseWeighting = 2
    };
};

class SoundLevelMeterPluginCreator
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~SoundLevelMeterPluginCreator()
    {
    }

    /** Returns the available soundlevelmeters in the library. A soundlevelmeter
     * can be
     * instantiated with #create().
     *
     * @return list of all available soundlevelmeter plugins
     */
    virtual QStringList availablePlugins() const = 0;

    /** Creates a soundlevelmeter. The caller takes ownership of the created
     * instance and
     * must free it after use. If it is not possible to create a filter
     * instance, returns @c NULL.
     */
    virtual SoundLevelMeter *
    createSoundLevelMeter(const QString &name) const = 0;
};

Q_DECLARE_INTERFACE(SoundLevelMeterPluginCreator,
                    "be.exporl.apex.soundlevelmeterplugin/1.0")

#endif
