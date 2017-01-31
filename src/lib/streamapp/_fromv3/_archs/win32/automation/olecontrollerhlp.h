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
  * [ file ] _archs/win32/automation/olecontrollerhlp.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_OLECONTROLLERHLP_H__
#define __STR_OLECONTROLLERHLP_H__

#include "win32_headers.h"

S_NAMESPACE_BEGIN

namespace win32
{
  namespace automation
  {

      /**
        * Count number of arguments in format string.
        * @param ac_psFormat format string
        * @param a_nArgs set to number of arguments
        * @return NOERROR if ok
        */
    HRESULT gf_hCountArgsInFormat( const char* ac_psFormat, unsigned& a_nArgs ) s_no_throw;

      /**
        * Get next VARTYPE from format string.
        * @param ac_psFormat format string
        * @param a_nArgs set to number of arguments
        * @return NOERROR if ok
        */
    const char* gf_psGetNextVarType( const char* ac_psFormat, VARTYPE& a_tVarType ) s_no_throw;

      /**
        * Get an argument from a va_list.
        * @param a_Arg destination argument, must have it's type set already!
        * @param a_ArgList a va_list
        * @return NOERROR if ok
        */
    HRESULT gf_hParseArg( VARIANTARG& a_Arg, va_list& a_ArgList ) s_no_throw;

      /**
        * Convert a VARIANTARG into a value.
        * @param ac_Type destination type
        * @param a_Source source variant
        * @param a_pDest destination pointer, better matches ac_Type!
        * @return NOERROR if ok
        */
    HRESULT gf_hConvertVariant( const VARTYPE ac_Type, VARIANTARG a_Source, void* a_pDest ) s_no_throw;

  }
}

S_NAMESPACE_END

#endif //#ifndef __STR_OLECONTROLLERHLP_H__
