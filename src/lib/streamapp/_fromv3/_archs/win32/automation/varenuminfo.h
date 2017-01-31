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
  * [ file ] _archs/win32/automation/varenuminfo.h
  * [ fmod ] String
  * <^>
  */

#ifndef __STR_VARENUMINFO_H__
#define __STR_VARENUMINFO_H__

#include "win32_headers.h"
#include "core/mpl/macros.h"

S_NAMESPACE_BEGIN

namespace win32
{
  namespace automation
  {

      /**
        * tVarEnumType
        *   maps a type to it's VARTYPE.
        *   Only for types that don't conflict..
        **************************************** */
    s_static_integral_const_decl( tVarEnumType, tType           , VT_EMPTY      , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, short           , VT_I2         , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, int             , VT_I4         , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, float           , VT_R4         , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, double          , VT_R8         , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, LPOLESTR        , VT_BSTR       , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, IDispatch*      , VT_DISPATCH   , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, SCODE           , VT_ERROR      , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, VARIANT*        , VT_VARIANT    , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, IUnknown*       , VT_UNKNOWN    , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, char            , VT_I1         , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, unsigned char   , VT_UI1        , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, unsigned short  , VT_UI2        , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, unsigned int    , VT_UI4        , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, s_int64         , VT_I8         , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, s_uint64        , VT_UI8        , VARTYPE )
    s_static_integral_const_spec( tVarEnumType, char*           , VT_LPSTR      , VARTYPE )

  }
}

S_NAMESPACE_END

#endif //#ifndef __STR_VARENUMINFO_H__
