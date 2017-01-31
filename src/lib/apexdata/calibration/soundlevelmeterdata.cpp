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
 
#include "soundlevelmeterdata.h"

apex::data::SoundLevelMeterData::SoundLevelMeterData()
{
    setValueByType("frequency_weighting", "Z");
    setValueByType("time_weighting", "S");
    setValueByType("type", "RMS");
    setValueByType("percentile", "0");
    setValueByType("time", "0");
}

QString apex::data::SoundLevelMeterData::measurementType() const
{
    QString type = valueByType ( "type" ).toString().toUpper();

    if ( type == "RMS" || type =="PEAK" )
        return type;
    else
    {
        error = "Unsupported measurement type";
        return "";
    }
}
QString apex::data::SoundLevelMeterData::frequencyWeightingType() const
{
    QString fw = valueByType("frequency_weighting").toString().toUpper();

    if (fw == "Z" || fw == "A" || fw == "C")
        return fw;
    else
    {
        error = "Unsupported frequency weighting type";
        return "";
    }
}

QString apex::data::SoundLevelMeterData::timeWeightingType() const
{
    QString tw = valueByType("time_weighting").toString().toUpper();
    
    if (tw == "S" || tw == "F" || tw == "I")
        return tw;
    else
    {
        error = "Unsupported time weighting type";
        return "";
    }
}

bool apex::data::SoundLevelMeterData::containsSupportedData() const
{
	
	if(valueByType("time").toInt() <= 0)
	{
		error = "Time must be an integer greater than zero";
		return false;
	}
	if(!(valueByType("percentile").toDouble() <= 1 && 
		valueByType("percentile").toDouble() >= 0))
	{
		error = "Percentile must be between 0 and 1";
		return false;
	}
	if(measurementType() == "")
		return false;
	if(frequencyWeightingType() == "")
		return false;
	if(timeWeightingType() == "")
		return false;
	return true;
}
const QString& apex::data::SoundLevelMeterData::errorString() const
{
	return error;
}


