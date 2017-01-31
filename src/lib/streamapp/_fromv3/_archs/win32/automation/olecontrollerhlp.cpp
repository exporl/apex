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
  * [ file ] _archs/win32/automation/olecontrollerhlp.cpp
  * [ fmod ] String
  * <^>
  */

#include "olecontrollerhlp.h"
#include "olecontroller.h"
#include "core/text/wchars.h"
#include <string>

S_NAMESPACE_BEGIN

HRESULT win32::automation::gf_hCountArgsInFormat( const char* ac_psFormat, unsigned& a_nArgs ) s_no_throw
{
  a_nArgs = 0;

  if( ac_psFormat == 0 )
    return NOERROR;

  while( *ac_psFormat )
  {
    if( *ac_psFormat == '&' )
      ac_psFormat++;

    switch( *ac_psFormat )
    {
      case 'b':
      case 'i':
      case 'I':
      case 'r':
      case 'R':
      case 'c':
      case 's':
      case 'S':
      case 'a':
      case '3':
      case 'e':
      case 'd':
      case 'v':
      case 'D':
      case 'U':
        ++a_nArgs;
        ac_psFormat++;
        break;
      case '\0':
      default:
        return ResultFromScode( E_INVALIDARG );
    }
  }
  return NOERROR;
}

LPCTSTR win32::automation::gf_psGetNextVarType( const char* ac_psFormat, VARTYPE& a_tVarType ) s_no_throw
{
  a_tVarType = 0;
  if( *ac_psFormat == '&' )
  {
    a_tVarType = VT_BYREF;
    ac_psFormat++;
    if( !*ac_psFormat )
      return 0;
  }
  switch(*ac_psFormat)
  {
    case 'b':
      a_tVarType |= VT_BOOL;
      break;
    case 'i':
      a_tVarType |= VT_I2;
      break;
    case 'I':
      a_tVarType |= VT_I4;
      break;
    case 'r':
      a_tVarType |= VT_R4;
      break;
    case 'R':
      a_tVarType |= VT_R8;
      break;
    case 'c':
      a_tVarType |= VT_CY;
      break;
    case 'a':
      a_tVarType |= OleController::VT_BSTRA;
      break;
    case 's':
      a_tVarType |= OleController::VT_BSTRS;
      break;
    case '3':
      a_tVarType |= OleController::VT_BSTR3;
      break;
    case 'S':
      a_tVarType |= VT_BSTR;
      break;
    case 'e':
      a_tVarType |= VT_ERROR;
      break;
    case 'd':
      a_tVarType |= VT_DATE;
      break;
    case 'v':
      a_tVarType |= VT_VARIANT;
      break;
    case 'U':
      a_tVarType |= VT_UNKNOWN;
      break;
    case 'D':
      a_tVarType |= VT_DISPATCH;
      break;
    case '\0':
       return 0;     //end of Format string
    default:
      return 0;
  }
  return ++ac_psFormat;
}


HRESULT win32::automation::gf_hParseArg( VARIANTARG& a_Arg, va_list& a_ArgList ) s_no_throw
{
  switch( a_Arg.vt )
  {
    case VT_I2 :
              V_I2( &a_Arg ) = va_arg( a_ArgList, short );
              break;
    case VT_I4 :
              V_I4( &a_Arg ) = va_arg( a_ArgList, long );
              break;
    case VT_R4 :
              V_R4( &a_Arg ) = va_arg( a_ArgList, float );
              break;
    case VT_DATE :
    case VT_R8 :
              V_R8( &a_Arg ) = va_arg( a_ArgList, double );
              break;
    case VT_CY :
              V_CY( &a_Arg ) = va_arg( a_ArgList, CY );
              break;
    case VT_BSTR :
              V_BSTR( &a_Arg ) = ::SysAllocString( va_arg( a_ArgList, OLECHAR* ) );
              if( !a_Arg.bstrVal )
                return E_OUTOFMEMORY;
              break;
    case OleController::VT_BSTRA :
              {
                OLECHAR* pDst = 0;
                const char* pSrc = va_arg( a_ArgList, char* );
                wchars::gf_bCharToWChar( pSrc, pDst );
                V_BSTR( &a_Arg ) = ::SysAllocString( pDst );
                a_Arg.vt = VT_BSTR;
                delete pDst;
              }
              if( !a_Arg.bstrVal )
                return E_OUTOFMEMORY;
              break;
    /*case OleController::VT_BSTR3 :
              {
                OLECHAR* pDst = 0;
                const char* pSrc = va_arg( a_ArgList, String* )->mf_pData();
                wchars::gf_bCharToWChar( pSrc, pDst );
                V_BSTR( &a_Arg ) = ::SysAllocString( pDst );
                a_Arg.vt = VT_BSTR;
                delete pDst;
              }
              if( !a_Arg.bstrVal )
                return E_OUTOFMEMORY;
              break;*/
    case OleController::VT_BSTRS :
              {
                OLECHAR* pDst = 0;
                const char* pSrc = va_arg( a_ArgList, std::string* )->c_str();
                wchars::gf_bCharToWChar( pSrc, pDst );
                V_BSTR( &a_Arg ) = ::SysAllocString( pDst );
                a_Arg.vt = VT_BSTR;
                delete pDst;
              }
              if( !a_Arg.bstrVal )
                return E_OUTOFMEMORY;
              break;
    case VT_DISPATCH :
              V_DISPATCH( &a_Arg ) = va_arg( a_ArgList, LPDISPATCH );
              break;
    case VT_ERROR :
              V_ERROR( &a_Arg ) = va_arg( a_ArgList, SCODE );
              break;
    case VT_BOOL :
              V_BOOL( &a_Arg ) = va_arg( a_ArgList, BOOL ) ? -1 : 0;
              break;
    case VT_VARIANT :
              a_Arg = va_arg( a_ArgList, VARIANTARG );
              break;
    case VT_UNKNOWN :
              V_UNKNOWN( &a_Arg ) = va_arg( a_ArgList, LPUNKNOWN );
              break;
    case VT_I2 | VT_BYREF :
              V_I2REF( &a_Arg ) = va_arg( a_ArgList, short* );
              break;
    case VT_I4 | VT_BYREF :
              V_I4REF( &a_Arg ) = va_arg( a_ArgList, long* );
              break;
    case VT_R4 | VT_BYREF :
              V_R4REF( &a_Arg ) = va_arg( a_ArgList, float* );
              break;
    case VT_R8 | VT_BYREF :
              V_R8REF( &a_Arg ) = va_arg( a_ArgList, double* );
              break;
    case VT_DATE | VT_BYREF :
              V_DATEREF( &a_Arg ) = va_arg( a_ArgList, DATE* );
              break;
    case VT_CY | VT_BYREF :
              V_CYREF( &a_Arg ) = va_arg( a_ArgList, CY* );
              break;
    case VT_BSTR | VT_BYREF :
              V_BSTRREF( &a_Arg ) = va_arg( a_ArgList, BSTR* );
              break;
    case OleController::VT_BSTRA | VT_BYREF :
              return E_INVALIDARG;
    case VT_DISPATCH | VT_BYREF :
              V_DISPATCHREF( &a_Arg ) = va_arg( a_ArgList, LPDISPATCH* );
              break;
    case VT_ERROR | VT_BYREF :
              V_ERRORREF( &a_Arg ) = va_arg( a_ArgList, SCODE* );
              break;
    case VT_BOOL | VT_BYREF :
              {
                BOOL* pBool = va_arg( a_ArgList, BOOL* );
                *pBool = 0;
                V_BOOLREF( &a_Arg ) =( VARIANT_BOOL*) pBool;
              }
              break;
    case VT_VARIANT | VT_BYREF :
              V_VARIANTREF( &a_Arg ) = va_arg( a_ArgList, VARIANTARG* );
              break;
    case VT_UNKNOWN | VT_BYREF :
              V_UNKNOWNREF( &a_Arg ) = va_arg( a_ArgList, LPUNKNOWN* );
              break;
    default :
              return E_INVALIDARG;

  }
  return NOERROR;
}


HRESULT win32::automation::gf_hConvertVariant( const VARTYPE ac_Type, VARIANTARG a_Source, void* a_pDest ) s_no_throw
{
  if( ac_Type != VT_EMPTY )
  {
      //convert return value
    VARTYPE t = ( ac_Type == OleController::VT_BSTRS || ac_Type == OleController::VT_BSTR3 ) ? (VARTYPE) VT_BSTR : ac_Type;
    if( t != VT_VARIANT )
    {
      HRESULT m_hResult = VariantChangeType( &a_Source, &a_Source, 0, t );
      if( FAILED( m_hResult ) || t != a_Source.vt )
        return m_hResult;
    }

      //copy val
    switch( ac_Type )
    {
    case VT_UI1:
      *(BYTE*) a_pDest = a_Source.bVal;
      break;
    case VT_UI2:
      *(USHORT*) a_pDest = a_Source.uiVal;
      break;
    case VT_UI4:
      *(ULONG*) a_pDest = a_Source.ulVal;
      break;
    case VT_UI8:
      *(ULONGLONG*) a_pDest = a_Source.ullVal;
      break;
    case VT_I1:
      *(char*) a_pDest = a_Source.cVal;
      break;
    case VT_I2:
      *(short*) a_pDest = a_Source.iVal;
      break;
    case VT_I4:
      *(long*) a_pDest = a_Source.lVal;
      break;
    case VT_I8:
      *(LONGLONG*) a_pDest = a_Source.llVal;
      break;
    case VT_R4:
      *(float*) a_pDest = a_Source.fltVal;
      break;
    case VT_R8:
      *(double*) a_pDest = a_Source.dblVal;
      break;
    case VT_DATE:
      *(double*) a_pDest = *(double*)&a_Source.date;
      break;
    case VT_CY:
      *(CY*) a_pDest = a_Source.cyVal;
      break;
    /*case OleController::VT_BSTR3:
        wchars::gf_bWCharToChar( a_Source.bstrVal, *( (String*) a_pDest ) );
        SysFreeString( a_Source.bstrVal );
        break;*/
    case OleController::VT_BSTRS:
      {
        char* p = 0;
        if( wchars::gf_bWCharToChar( a_Source.bstrVal, p ) )
        {
          *( (std::string*) a_pDest ) = p;
          delete p;
          SysFreeString( a_Source.bstrVal );
          return E_INVALIDARG;
        }
        SysFreeString( a_Source.bstrVal );
        break;
      }
    case VT_DISPATCH:
      *(LPDISPATCH*) a_pDest = a_Source.pdispVal;
      break;
    case VT_ERROR:
      *(SCODE*) a_pDest = a_Source.scode;
      break;
    case VT_BOOL:
      *(BOOL*) a_pDest = ( V_BOOL( &a_Source ) != 0 );
      break;
    case VT_VARIANT:
      *(VARIANT*) a_pDest = a_Source;
      break;
    case VT_UNKNOWN:
      *(LPUNKNOWN*) a_pDest = a_Source.punkVal;
      break;

    case VT_BSTR:
    case OleController::VT_BSTRA:
      return E_NOTIMPL;
    default:
      return E_INVALIDARG;
    }
  }
  return NOERROR;
}

S_NAMESPACE_END
