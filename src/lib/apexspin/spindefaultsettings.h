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

#ifndef _EXPORL_SRC_LIB_APEXSPIN_SPINDEFAULTSETTINGS_H_
#define _EXPORL_SRC_LIB_APEXSPIN_SPINDEFAULTSETTINGS_H_

namespace spin
{
namespace data
{
/**
 * This namespace holds all default settings for the SPIN dialog.
 * If defaults need to be changed, just change this file.
 */
namespace defaults
{
//procedure
const bool      REPEAT_FIRST_UNTIL_CORRECT  = true;
const double    INITIAL_STEPSIZE            = 2.0;

//options
const double    TIME_BEFORE_FIRST_STIMULUS  = 0;
const bool      REINFORCEMENT               = true;
const bool      SHOW_RESULT_EXPERIMENTER    = true;
const bool      SHOW_RESULT_SUBJECT         = false;
const unsigned  NB_RESPONSES_THAT_COUNT     = 6;
}//ns defaults
}//ns data
}//ns spin

#endif
