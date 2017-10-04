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
  * [ file ] _archs/win32/automation/clsid.cpp
  * [ fmod ] String
  * <^>
  */

#include "../../../core/text/wchars.h"

#include "clsid.h"

S_NAMESPACE_BEGIN

/*bool win32::automation::gf_bCLSIDToProgID( const CLSID& ac_ID, String&
a_sProgID )// s_no_throw
{
  LPOLESTR p;
  if( ProgIDFromCLSID( ac_ID, &p ) == S_OK )
    return wchars::gf_bWCharToChar( p, a_sProgID );
  return false;
}*/

/*bool win32::automation::gf_bProgIDToCLSID( const char* ac_psProgID, CLSID&
a_ID ) s_no_throw
{
  LPOLESTR p = 0;
  if( wchars::gf_bCharToWChar( ac_psProgID, p ) )
  {
    bool bRet = ( CLSIDFromProgID( p, &a_ID ) == S_OK );
    delete p;
    return bRet;
  }
  return false;
}*/

S_NAMESPACE_END
