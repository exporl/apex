/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef _APEX_SRC_LIB_APEXSPIN_SPINEXPERIMENTCONSTANTS_H_
#define _APEX_SRC_LIB_APEXSPIN_SPINEXPERIMENTCONSTANTS_H_

#include <QString>

namespace spin
{
namespace constants
{

//id prefixes
const QString PREFIX_DATABLOCK              = "datablock";
const QString PREFIX_STIMULUS               = "stimulus";
const QString PREFIX_TRIAL                  = "trial";
const QString PREFIX_GAIN                   = "gain";

//silent datablock
const double  SILENT_DATABLOCK_LENGTH       = 1000.0;
const QString SILENT_DATABLOCK_ID           = PREFIX_DATABLOCK + "_silence";
const QString SILENT_DATABLOCK_FILE         = "silence:" + QString::number(
                                                    SILENT_DATABLOCK_LENGTH);

//noise datablock
const QString NOISE_DATABLOCK_ID            = PREFIX_DATABLOCK + "_noise";

//parameters
const int     PARAM_PRESENTATIONS           = 1;
const int     PARAM_SKIP                    = 0;
const int     PARAM_NB_CHOICES              = 1;
const QString PARAM_DEF_STD                 = QString();
const bool    PARAM_INPUT_DURING_STIMULUS   = false;
const int     PARAM_N_UP                    = 1;
const int     PARAM_N_DOWN                  = 1;
const double  PARAM_START_VALUE             = 0.0;

//answer
const QString ANSWER_EXPERIMENTER_SCREEN    = "correct";

//screen
const QString SCREEN_SUBJECT                = "subject_screen";
const QString SCREEN_EXPERIMENTER_QUIET     = "experimenter_screen_quiet";
const QString SCREEN_EXPERIMENTER_NOISE     = "experimenter_screen_noise";

//device
const QString DEVICE_ID                     = "soundcard";
const QString DEVICE_GAIN_ID                = "cardgain";
const double  DEVICE_GAIN                   = 0.0;

//filter
const QString FILTER_TYPE_NOISE             = "dataloop";
const QString FILTER_TYPE_SPEECH            = "amplifier";
const QString FILTER_GAIN_NOISE             = "noisegain";
const QString FILTER_GAIN_SPEECH            = "speechgain";

//connections
const QString CONN_FROM_ALL_ID              = "_ALL_";

//calibration
const QString CALIBRATION_STIMULUS          = "calibrationstimulus";
const QString NOISE_STIMULUS                = "noisestimulus";
const QString CALIBRATION_PROFILE           = "spin";
const QString CALIBRATION_FIXED_PARAM       = "calibration";
const double  CALIBRATION_MUTE              = -150.0;
const double  CALIBRATION_MIN               = -50.0;
const double  CALIBRATION_MAX               = 10.0;
const double  CALIBRATION_DEFAULT           = 0.0;

//result
const QString HTML_PAGE                     = "apex:resultsviewer.html";
const QString JAVASCRIPT_SCRIPT             = "apexresults.js";

//general
const bool    EXIT_AFTER                    = true;


}
}
#endif
