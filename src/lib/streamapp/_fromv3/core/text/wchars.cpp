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
  * [ file ] core/text/wchars.cpp
  * [ fmod ] default
  * <^>
  */

#include "wchars.h"
#include <cstring>

S_NAMESPACE_BEGIN

bool wchars::gf_bStringStartsWith(const s_wchar *ac_pwsSearch,
                                  const char *ac_psFind,
                                  size_t &a_nLen) s_no_throw
{
    if (!a_nLen)
        a_nLen = ::strlen(ac_psFind);
    const char *c_psSearch16 = (const char *)ac_pwsSearch;
    for (size_t i = 0; i < a_nLen; ++i) {
        if (*c_psSearch16 != ac_psFind[i])
            return false;
        c_psSearch16 += 2;
    }
    return true;
}

size_t wchars::gf_Length(const s_wchar *ac_pwsString) s_no_throw
{
    return ::wcslen(ac_pwsString);
}

void wchars::gf_InStringCopy(s_wchar *a_pwsStringStart,
                             const size_t ac_nStartOffset,
                             const size_t ac_nStopOffset) s_no_throw
{
    unsigned short *pString16 = (unsigned short *)a_pwsStringStart;
    for (size_t i = ac_nStartOffset; i < ac_nStopOffset; ++i)
        *pString16++ = a_pwsStringStart[i];
    *pString16 = 0;
}

/*bool wchars::gf_bWCharToChar( const s_wchar* ac_pwsSrc, String& a_sDest )
s_no_throw
{
  const size_t nLen = gf_Length( ac_pwsSrc );
  a_sDest.mp_Reserve( nLen );
  char* pDst = a_sDest.mp_pData();
  for( size_t i = 0; i < nLen ; ++i )
  {
    const s_wchar cur( ac_pwsSrc[ i ] );
    if( cur < 0xFF )
    {
      pDst[ i ] = (char) cur;
    }
    else
    {
      return false;
    }
  }
  pDst[ nLen ] = 0;
  return true;
}*/

bool wchars::gf_bWCharToChar(const s_wchar *ac_pwsSrc,
                             char *&a_psDest) s_no_throw
{
    const size_t nLen = gf_Length(ac_pwsSrc);
    a_psDest = new char[nLen + 1];
    for (size_t i = 0; i < nLen; ++i) {
        const s_wchar cur(ac_pwsSrc[i]);
        if (cur < 0xFF) {
            a_psDest[i] = (char)cur;
        } else {
            delete a_psDest;
            a_psDest = 0;
            return false;
        }
    }
    a_psDest[nLen] = 0;
    return true;
}

bool wchars::gf_bCharToWChar(const char *ac_sSrc,
                             s_wchar *&a_pwsDest) s_no_throw
{
    if (a_pwsDest)
        return false;
    const size_t nLen = ::strlen(ac_sSrc);
    a_pwsDest = new s_wchar[nLen + 1];
    for (size_t i = 0; i < nLen; ++i)
        a_pwsDest[i] = ac_sSrc[i];
    a_pwsDest[nLen] = 0;
    return true;
}

S_NAMESPACE_END
