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
    * @file safearray.h
    * some SAFEARRAY helpers
    */

#ifndef __STR_SAFEARRAY_H__
#define __STR_SAFEARRAY_H__

#include "win32_headers.h"
#include "core/text/wchars.h"

S_NAMESPACE_BEGIN

namespace win32
{
  namespace automation
  {

    template< class tStringArray >
    bool gf_bSafeArrayToStringArray( const VARIANT& ac_Array, tStringArray& a_Dest )
    {
      a_Dest.clear();
      SAFEARRAY& p( *ac_Array.parray );
        //check array is really a string array of a format we handle
      if( p.cbElements != 4 || ac_Array.vt != ( VT_ARRAY | VT_BSTR ) )
        return false;
      OLECHAR** pElems = (OLECHAR**) p.pvData;
      for( unsigned i = 0 ; i < p.rgsabound[ 0 ].cElements ; ++i )
      {
        char* pText = 0;
        wchars::gf_bWCharToChar( pElems[ i ], pText );
        a_Dest.push_back( pText );
        delete pText;
      }
      return true;
    }

  }
}

S_NAMESPACE_END

#endif //#ifndef __STR_SAFEARRAY_H__
