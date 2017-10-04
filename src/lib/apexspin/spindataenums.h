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

#ifndef _APEX_SRC_LIB_APEXSPIN_SPINDATAENUMS_H_
#define _APEX_SRC_LIB_APEXSPIN_SPINDATAENUMS_H_

namespace spin
{
namespace data
{

enum SpeakerType { NO_TYPE, FREE_FIELD, HEADPHONE };
enum ProcedureType { INVALID_PROCEDURE, CONSTANT, ADAPTIVE };
enum Order { ORDER_INVALID, ORDER_RANDOM, ORDER_SEQUENTIAL };
enum Material { NO_MATERIAL, SPEECH, NOISE };
enum Person { INVALID_PERSON, EXPERIMENTER, SUBJECT };
enum SoundCard { DefaultSoundcard, RmeMultiface, RmeFirefaceUc, LynxOne };
enum { RANDOM = -1 };

} // ns data
} // ns spin

#endif
