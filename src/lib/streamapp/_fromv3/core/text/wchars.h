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
  *      -- v3tov2 plugin --
  * Do not modify this file by hand,
  * your changes will be overwritten.
  * <^>
  * [ prog ] apex
  * [ file ] core/text/wchars.h
  * [ fmod ] String
  * <^>
  */

  /**
    * @file wchars.h
    * This file defines some functions for use with unicode characters.
    * Atm unicode is avoided in streamapp, there's no reason to use twice
    * the bytes needed to do exactly the same, unless we'd be using Chinese text or so.
    */

#ifndef __STR_WCHARS_H__
#define __STR_WCHARS_H__

#include "../_main.h"

S_NAMESPACE_BEGIN

namespace wchars
{

#ifdef wchar_t
  typedef wchar_t s_wchar;
#else
  typedef unsigned short s_wchar;
#endif

    /**
      * Check if a unicode string starts with the same
      * characters as an ansi string.
      * @param ac_pwsSearch unicode string to check
      * @param ac_psFind ansi string to compare with
      * @param a_nLen length of ac_psFind, will be set if zero.
      * @return true if ac_pwsSearch starts with the same characters as ac_psFind
      */
  s_dll bool gf_bStringStartsWith( const s_wchar* ac_pwsSearch, const char* ac_psFind, size_t& a_nLen ) s_no_throw;

    /**
      * Get the number of characters in a unicode string.
      * @return the number
      */
  s_dll size_t gf_Length( const s_wchar* ac_pwsString ) s_no_throw;

    /**
      * Copy ( ac_nStartOffset - ac_nStopOffset ) characters
      * from a_pwsStringStart + ac_nOffset to a_pwsStringStart,
      * and null-terminate the string.
      * @param a_pwsStringStart pointer to the unicode string
      * @param ac_nStartOffset offset to start copying from
      * @param ac_nStopOffset offset to stop at
      */
  s_dll void gf_InStringCopy( s_wchar* a_pwsStringStart, const size_t ac_nStartOffset, const size_t ac_nStopOffset ) s_no_throw;

    /**
      * Convert from wchar to a String.
      * @param ac_pwsSrc source string
      * @param a_sDest destination
      * @return false if source contains chars that cannot be represented

  s_dll bool gf_bWCharToChar( const s_wchar* ac_pwsSrc, String& a_sDest ) s_no_throw;*/

    /**
      * Convert from wchar to plain char.
      * @param ac_pwsSrc source string
      * @param a_sDest destination
      * @return false if source contains chars that cannot be represented
      */
  s_dll bool gf_bWCharToChar( const s_wchar* ac_pwsSrc, char*& a_psDest ) s_no_throw;

    /**
      * Convert plain char to wchar.
      * Note pointer passed in must be zero,
      * and must be deleted afterwards.
      * @param ac_psSrc source string
      * @param a_pwsDest destination, must be deleted!
      * @return false if source contains chars that cannot be represented
      */
  s_dll bool gf_bCharToWChar( const char* ac_psSrc, s_wchar*& a_pwsDest ) s_no_throw;

}

S_NAMESPACE_END

#endif //#ifndef __STR_WCHARS_H__
