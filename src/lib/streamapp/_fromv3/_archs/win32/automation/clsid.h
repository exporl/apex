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
  * [ file ] _archs/win32/automation/clsid.h
  * [ fmod ] String
  * <^>
  */

#ifndef __STR_CLSID_H__
#define __STR_CLSID_H__

#include "../../../../_archs/win32/win32_headers.h"

S_NAMESPACE_BEGIN

namespace win32
{
namespace automation
{

/**
  * Get a program ID for a CLSID.
  * @param ac_ID the CLSID
  * @param a_sProgID result
  * @return true if ok, false if not found

s_dll bool gf_bCLSIDToProgID( const CLSID& ac_ID, String& a_sProgID )
s_no_throw;*/

/**
  * Get a CLSID for a program ID.
  * @param ac_sProgID the prog ID
  * @param a_ID result
  * @return true if ok, false if not found
  */
s_dll bool gf_bProgIDToCLSID(const char *ac_psProgID, CLSID &a_ID) s_no_throw;
}
}

S_NAMESPACE_END

#endif //#ifndef __STR_CLSID_H__
