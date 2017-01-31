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
 
//#ifndef SLM_2250_H
//#define SLM_2250_H

#include <QObject>

#include <math.h>
#include "calibration/soundlevelmeter.h"

class dummy_slmCreator :
    public QObject,
    public SoundLevelMeterPluginCreator
{
    Q_OBJECT
    Q_INTERFACES (SoundLevelMeterPluginCreator)
public:
    virtual QStringList availablePlugins() const;

    virtual SoundLevelMeter *createSoundLevelMeter(const QString &name) const;
};

Q_EXPORT_PLUGIN2 (dummy_slm, dummy_slmCreator)

class dummy_slm : public QObject, public SoundLevelMeter
{
	Q_OBJECT
public:

/**
  * Constructor of dummy slm
  * Does nothing.
  */
	dummy_slm();

/**
  * Desctructor of dummy slm.
  * Does nothing
  */
	~dummy_slm();

/**
  * This method connects to the specific device.
  * Does nothing, returns true.
  */
	bool connect();

/**
  * This method disconnects the device
  * Does nothing, returns true.
  */
	bool disconnect();

/**
  * This method returns the error string. 
  */
	inline const QString errorString() const {return error;};

/**
  * This method returns an array of the supported transducers by the device.
  */
	const QStringList transducers() const;

/**
  * This method sets a specific transducer to the device.
  * The transducer has to be an existing one. 
  * To get the supported transducers, use the transducers() method.
  * @param name : the transducer's name
  */
	bool setTransducer(QString);

/**
  * This method returns an array of connected devices.
  */
	const QStringList instruments() const;

/**
  * This method sets an instrument.
  * Connected instruments can be obtained by calling instruments(). 
  * @param _instrument the name of the instrument you want to use.
  */
    inline bool setInstrument(QString _instrument){this->instrument = _instrument; return true;};

/**
  * This method starts a measurement.
  * @param type_measurement : the measurement type (see SoundLevelMeter::type_measurement for supported measurements)
  * @param frequency_weighting : the frequency weighting (see SoundLevelMeter::frequency_weighting for supported frequency weightings)
  * @param time_weighting : the time weighting (see SoundLevelMeter::time_weighting for supported time weightings)
  * @percentile : not compulsory : value between 0 and 1, get's rounded to 3 digits.
  */
	bool startMeasurement(int type_measurement,int frequency_weighting,int time_weighting,double percentile=0);
	
/**
  * This method stops the measurement.  
  */
	bool stopMeasurement();

/**
  * This method calculates the measured result and stores it in result (call it with result()). 
  * You're still able to change the results you want to obtain. 
  * @param type_measurement : the measurement type (see SoundLevelMeter::type_measurement for supported measurements)
  * @param frequency_weighting : the frequency weighting (see SoundLevelMeter::frequency_weighting for supported frequency weightings)
  * @param time_weighting : the time weighting (see SoundLevelMeter::time_weighting for supported time weightings)
  */
	bool measure(int type_measurement,int frequency_weighting,int time_weighting);
	
/**
  * This method calculates the measured result and stores it in result (call it with result()). 
  * You're still able to change the results you want to obtain. 
  * @param measurement : the measurement specific string (see SLM_2250 documentation)
  */
	bool measure(QString);
	
/**
  * This method calculates the measured result and stores it in result (call it with result()). 
  * The result is calculated according to de defined parameters in the startMeasurement method.
  */
	bool measure();

/**
  * This method returns the measured result after measure has been called. 
  */
	inline double result() const {return ((double)_result)/100;};
private:

/** 
  * NOT WORKING PROPERLY
  * This methods return the current measuring state.
  * 0 = paused; 1 = running; 2 = stopped	
  */
	int measurementState(); //doesn't work properly
	
	int timeWeighting;
	int typeMeasurement;
	int frequencyWeighting;
	QString instrument;
	int _result;
	bool percentile;
	bool measuring;
};

/* SLM_2250Creator CPP*/

QStringList dummy_slmCreator::availablePlugins() const
{
    return QStringList() << "dummy_slm";
}

SoundLevelMeter * dummy_slmCreator::createSoundLevelMeter(const QString &name) const
{
    if (name == "dummy_slm")
        return new dummy_slm();

    return NULL;
}

/*******SLM_2250 CPP*******/

dummy_slm::dummy_slm():measuring(false){}


dummy_slm::~dummy_slm(){}


const QStringList dummy_slm::instruments() const
{
	QStringList _instruments;
	_instruments.push_back("newInstrument");
	return _instruments; 
}




int dummy_slm::measurementState() 
{
	return 0;
}



bool dummy_slm::connect()
{
	return true;
}


bool dummy_slm::disconnect()
{
	return true;
}


const QStringList dummy_slm::transducers() const
{
	QStringList transducers;
	transducers.push_back("newTransducer");
	return transducers;
}


bool dummy_slm::setTransducer(QString /*name*/)
{
	 return true;
}


bool dummy_slm::startMeasurement(int,int,int,double)
{
	measuring = true;
	return true;
}


bool dummy_slm::stopMeasurement()
{
	if(!measuring)
	{
		error = "Measurement should be started first.";
		return false;
	}
	measuring = false;
	measure();
	return true;
}

bool dummy_slm::measure(int type_measurement,int frequency_weighting,int time_weighting)
{
    type_measurement = typeMeasurement;
	frequency_weighting = frequencyWeighting;
	time_weighting = timeWeighting;
	return measure();
}

bool dummy_slm::measure(QString /*measurement*/)
{
	_result = 6840;
	return true;
}


bool dummy_slm::measure()
{
	_result = 6840;
	return true;
}


#include "dummy_slm.moc"

