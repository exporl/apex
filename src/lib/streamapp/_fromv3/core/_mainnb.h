/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

/**
  *      -- v3tov2 plugin --
  * Do not modify this file by hand,
  * your changes will be overwritten.
  * <^>
  * [ prog ] apex
  * [ file ] core/_mainnb.h
  * [ fmod ] default
  * <^>
  */

/**
  * @file _mainnb.h
  * Include file for the basic headers (string, memory, tracer).
  * Prevents the files from including themselves, which is no real problem an
 * sich,
  * but it leads to eg string.h not being able to get the definitions in
 * memory.h.
  */

#ifndef __STR_MAINNB_H__
#define __STR_MAINNB_H__

#define S_NOBASIC_INCLUDE
#include "_main.h"
#undef S_NOBASIC_INCLUDE

#endif //#ifndef __STR_MAINNB_H__
