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

  /**
    * @file typedefs.h
    * Contains common typedefs.
    */
#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <map>
#include <string>
#include <vector>
#include <QString>

namespace streamapp
{

  const QString sc_sDefault( "default" );     //!< "default" string
  const std::string sc_sNull( "" );               //!< null string

    /**
      * std::vector of std::string
      */
  typedef std::vector< std::string >    tStringVector;

    /**
      * std::vector of unsigned
      */
  typedef std::vector< unsigned >       tUnsignedVector;

    /**
      * std::vector of unsigned long
      */
  typedef std::vector< unsigned long >  tUnsignedLongVector;

    /**
      * std::map with usigned as key and bool as value
      */
  typedef std::map< unsigned, bool >        tUnsignedBoolMap;

    /**
      * tUnsignedBoolMap iterator
      */
  typedef tUnsignedBoolMap::iterator        tUnsignedBoolMapIt;

    /**
      * tUnsignedBoolMap const_iterator
      */
  typedef tUnsignedBoolMap::const_iterator  tUnsignedBoolMapCIt;


  //FIXME the stuff below doesn't belong here

  /**
    * Message Types for dialogboxes etc
    */
  typedef enum
  {
    gc_eMessage,
    gc_eWarning,
    gc_eError,
    gc_eCritical,
    gc_eFatal
  } gt_eMsgType;

  const double sc_dMixerMin = -150.0;             //!< minimum value in dB for gains
#define MATLABCOMPATIBLE
#ifdef MATLABCOMPATIBLE
  const short  sc_n12BitMax    = 4096;            //!< 12 bits maximum
  const double sc_d16BitMinMax = 32768.0;         //!< 16 bits maximum
  const double sc_d24BitMinMax = 8388608.0;       //!< 24 bits maximum
  const double sc_d32BitMinMax = 2147483648.0;    //!< 32 bits maximum
  const float  sc_f32BitMinMax = 2147483648.f;
#else
  const short  sc_n12BitMax    = 4095;
  const double sc_d16BitMinMax = 32767.0;
  const double sc_d24BitMinMax = 8388607.0;
  const double sc_d32BitMinMax = 2147483647.0;
  const float  sc_f32BitMinMax = 2147483647.f;
#endif
  const unsigned sc_nInfinite = 0xFFFFFFFF;       //!< "infinite" for 32 bit unsigned
  const unsigned long sc_lInfinite = 0xFFFFFFFF;  //!< "infinite" for 32 bit unsigned long

}

#endif //#ifndef __TYPEDEFS_H__
