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
  * [ file ] _archs/win32/automation/olecontroller.cpp
  * [ fmod ] default
  * <^>
  */

#include "olecontroller.h"
#include "olecontrollerhlp.h"
#include "core/text/wchars.h"

S_NAMESPACE_BEGIN

using namespace win32::automation;

#define s_set_err( res ) m_hResult = ResultFromScode( res );

#define s_check_arg( expr, res )     \
  if( expr )                          \
  {                                    \
    s_set_err( res );                   \
    return false;                        \
  }

#define s_check_argg( expr, res, g ) \
  if( expr )                          \
  {                                    \
    s_set_err( res );                   \
    goto g;                              \
  }

OleController::OleController() :
  m_hResult( NOERROR )
{
}

OleController::OleController( IDispatch* a_pObject ) s_no_throw :
  m_hResult( NOERROR ),
  m_pDispatch( a_pObject )
{
}

OleController::~OleController() s_no_throw
{
}

void OleController::mp_DestroyObject() s_no_throw
{
  m_pDispatch.mp_Close();
}

bool OleController::mp_bCreateObject( const CLSID& ac_tID ) s_no_throw
{
  m_hResult = m_pDispatch.mp_bCreateObject( ac_tID );
  return FAILED( m_hResult ) ? false : true;
}

bool OleController::mp_bCreateObject( const char* ac_psName ) s_no_throw
{
  m_hResult = m_pDispatch.mp_bCreateObject( ac_psName );
  return FAILED( m_hResult ) ? false : true;
}

DISPID OleController::mp_GetDispatchID( const char* ac_psName ) s_no_throw
{
  if( !m_pDispatch )
    return 0;

  DISPID id;

  LPOLESTR OleName = 0;
  wchars::gf_bCharToWChar( ac_psName, OleName );
  m_hResult = m_pDispatch->GetIDsOfNames( IID_NULL, &OleName, 1, LOCALE_USER_DEFAULT, &id );
  delete OleName;
  if( FAILED( m_hResult ) )
    return 0;
  return id;
}


bool OleController::mp_bGetProperty( const DISPID ac_ID, const VARTYPE ac_eType, void* a_pRet ) s_no_throw
{
  return mp_bInvoke( DISPATCH_PROPERTYGET, ac_ID, ac_eType, a_pRet, 0, 0, 0 );
}

bool OleController::mp_bGetProperty( const DISPID ac_ID, const char* ac_cPropertyType, void* a_pRet ) s_no_throw
{
  VARTYPE vt = VT_EMPTY;
  gf_psGetNextVarType( ac_cPropertyType, vt );
  if( vt != VT_EMPTY )
    return mp_bGetProperty( ac_ID, vt, a_pRet );
  s_set_err( E_INVALIDARG )
  return false;
}


bool OleController::mp_bSetProperty( const char* ac_psName, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_pParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_PROPERTYPUT, mp_GetDispatchID( ac_psName ), VT_EMPTY, 0,
                                ac_nParams, ac_pParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}

bool OleController::mp_bSetProperty( const char* ac_psName, const char* ac_psParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_psParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_PROPERTYPUT, mp_GetDispatchID( ac_psName ), VT_EMPTY, 0,
                                ac_psParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}

bool OleController::mp_bSetProperty( const DISPID ac_ID, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_pParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_PROPERTYPUT, ac_ID, VT_EMPTY, 0,
                                ac_nParams, ac_pParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}

bool OleController::mp_bSetProperty( const DISPID ac_ID, const char* ac_psParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_psParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_PROPERTYPUT, ac_ID, VT_EMPTY, 0,
                                ac_psParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}


bool OleController::mp_bSetPropertyRef( const char* ac_psName, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_pParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_PROPERTYPUTREF, mp_GetDispatchID( ac_psName ), VT_EMPTY, 0,
                                ac_nParams, ac_pParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}

bool OleController::mp_bSetPropertyRef( const char* ac_psName, const char* ac_psParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_psParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_PROPERTYPUTREF, mp_GetDispatchID( ac_psName ), VT_EMPTY, 0,
                                ac_psParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}

bool OleController::mp_bSetPropertyRef( const DISPID ac_ID, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_pParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_PROPERTYPUTREF, ac_ID, VT_EMPTY, 0,
                                ac_nParams, ac_pParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}

bool OleController::mp_bSetPropertyRef( const DISPID ac_ID, const char* ac_psParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_psParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_PROPERTYPUTREF, ac_ID, VT_EMPTY, 0,
                                ac_psParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}


bool OleController::mp_bInvokeMethod( const DISPID ac_ID ) s_no_throw
{
  return mp_bInvoke( DISPATCH_METHOD, ac_ID, VT_EMPTY, 0, 0, 0, 0 );
}

bool OleController::mp_bInvokeMethod( const char* ac_psName, const char* ac_psParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_psParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_METHOD, mp_GetDispatchID( ac_psName ), VT_EMPTY, 0,
                                ac_psParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}

bool OleController::mp_bInvokeMethod( const char* ac_psName, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_pParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_METHOD, mp_GetDispatchID( ac_psName ), VT_EMPTY, 0,
                                ac_nParams, ac_pParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}

bool OleController::mp_bInvokeMethod( const DISPID ac_ID, const char* ac_psParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_psParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_METHOD, ac_ID, VT_EMPTY, 0,
                                ac_psParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}

bool OleController::mp_bInvokeMethod( const DISPID ac_ID, const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, ... ) s_no_throw
{
  va_list a_ArgList;
  va_start( a_ArgList, ac_pParamInfo );
  const bool bRet = mp_bInvoke( DISPATCH_METHOD, ac_ID, VT_EMPTY, 0,
                                ac_nParams, ac_pParamInfo, a_ArgList );
  va_end( a_ArgList );
  return bRet;
}

bool OleController::mp_bInvoke( WORD a_Flags, DISPID a_nDispatchId, VARTYPE a_vtRet, void* a_pRet,
                                const unsigned ac_nParams, const VARTYPE* ac_pParamInfo, va_list a_ArgList ) s_no_throw
{
  if( !m_pDispatch || !a_nDispatchId )
    return false;
  unsigned nArgs = ac_nParams;
  VARIANTARG* p = mf_pBuildArgList( nArgs, ac_pParamInfo, a_ArgList );
  if( !FAILED( m_hResult ) )
    return mp_bInvoke( a_Flags, a_nDispatchId, a_vtRet, a_pRet, nArgs, p );
  return false;
}

bool OleController::mp_bInvoke( WORD a_Flags, DISPID a_nDispatchId, VARTYPE a_vtRet, void* a_pRet,
                                const char* ac_psParamInfo, va_list a_ArgList ) s_no_throw
{
  if( !m_pDispatch || !a_nDispatchId )
    return false;
  unsigned nArgs = 0;
  VARIANTARG* p = mf_pBuildArgList( nArgs, ac_psParamInfo, a_ArgList );
  if( !FAILED( m_hResult ) )
    return mp_bInvoke( a_Flags, a_nDispatchId, a_vtRet, a_pRet, nArgs, p );
  return false;
}

bool OleController::mp_bInvoke( WORD a_Flags, DISPID a_nDispatchId, VARTYPE a_vtRet, void* a_pRet,
                                unsigned& a_nParams, VARIANTARG* a_pArgList ) s_no_throw
{
    //setup disp params
  DISPPARAMS DispatchParams;
  memset( &DispatchParams, 0, sizeof( DispatchParams ) );
  mf_CheckPropertyPut( a_Flags, DispatchParams, a_nParams );

    //initialize return variant
  VARIANT* pvMyRet = 0;
  VARIANTARG vResult;
  if( a_vtRet != VT_EMPTY )
  {
    VariantInit( &vResult );
    pvMyRet = &vResult;
  }

    //check if all is fine
  if( FAILED( m_hResult ) )
    goto cleanup;

    //make the call
  DispatchParams.cArgs  = a_nParams;
  DispatchParams.rgvarg = a_pArgList;
  try
  {
    m_hResult = m_pDispatch->Invoke( a_nDispatchId, IID_NULL, LOCALE_USER_DEFAULT,
                                     a_Flags, &DispatchParams, pvMyRet, 0, 0 );
  }
  catch( ... )
  {
    m_hResult = E_FAIL;
  }

cleanup:
    //clean up
  mf_CleanUpArgList( a_nParams, a_pArgList );

    //get return value
  if( pvMyRet )
    m_hResult = gf_hConvertVariant( a_vtRet, *pvMyRet, a_pRet );

  return FAILED( m_hResult ) ? false : true;
}

void OleController::mf_CheckPropertyPut( const WORD ac_Flags, DISPPARAMS& a_Params, const unsigned ac_nArgs ) s_no_throw
{
    //property puts have a named argument that represents
    //the value that the property is being assigned.
  static DISPID DispIdNamed = DISPID_PROPERTYPUT;
  if( ac_Flags & ( DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF ) )
  {
    if( ac_nArgs == 0 )
      s_set_err( E_INVALIDARG )
    a_Params.cNamedArgs = 1;
    a_Params.rgdispidNamedArgs = &DispIdNamed;
  }
}

VARIANTARG* OleController::mf_pBuildArgList( unsigned& a_nArgs, const VARTYPE* ac_pParamInfo, va_list a_ArgList ) s_no_throw
{
  VARIANTARG* ArgVector= 0;
  if( a_nArgs )
  {
      //allocate memory for all VARIANTARG parameters
    ArgVector = new VARIANTARG[ a_nArgs ];
    s_check_arg( !ArgVector, E_OUTOFMEMORY )
    memset( ArgVector, 0, sizeof( VARIANTARG ) * a_nArgs );

      //get ready to walk vararg list
    const VARTYPE* pb = ac_pParamInfo;
    VARIANTARG*     p = ArgVector + a_nArgs - 1;  //params go in opposite order!
    for( unsigned i = 0 ; i < a_nArgs ; ++i )
    {
      s_check_argg( p < ArgVector, E_INVALIDARG, cleanup )
      p->vt = *pb;
      if( FAILED( ( m_hResult = gf_hParseArg( *p, a_ArgList ) ) ) )
        goto cleanup;
      --p;
      ++pb;
    }
  }

  return ArgVector;

cleanup:
  mf_CleanUpArgList( a_nArgs, ArgVector );
  return 0;
}

VARIANTARG* OleController::mf_pBuildArgList( unsigned& a_nArgs, const char* ac_psParamInfo, va_list a_ArgList ) s_no_throw
{
  VARIANTARG* ArgVector= 0;
  m_hResult = gf_hCountArgsInFormat( ac_psParamInfo, a_nArgs );
  if( FAILED( m_hResult ) )
    goto cleanup;
  if( a_nArgs )
  {
      //allocate memory for all VARIANTARG parameters
    ArgVector = new VARIANTARG[ a_nArgs ];
    s_check_arg( !ArgVector, E_OUTOFMEMORY )
    memset( ArgVector, 0, sizeof( VARIANTARG ) * a_nArgs );

      //get ready to walk vararg list
    LPCTSTR   psz = ac_psParamInfo;
    VARIANTARG* p = ArgVector + a_nArgs - 1;  //params go in opposite order!

    for( unsigned i = 0 ; i < a_nArgs ; ++i )
    {
      s_check_argg( p < ArgVector, E_INVALIDARG, cleanup )
      psz = gf_psGetNextVarType( psz, p->vt );
      if( FAILED( ( m_hResult = gf_hParseArg( *p, a_ArgList ) ) ) )
        goto cleanup;
      --p;
    }
  }

  return ArgVector;

cleanup:
  mf_CleanUpArgList( a_nArgs, ArgVector );
  return 0;
}

void OleController::mf_CleanUpArgList( unsigned& a_nArgs, VARIANTARG* a_pArgList ) s_no_throw
{
  if( a_nArgs && a_pArgList )
  {
    VARIANTARG* p = a_pArgList;
    while( a_nArgs-- )
    {
      switch( p->vt )
      {
        case VT_BSTR :
          VariantClear( p );
          break;
        default :
          break;
      }
      ++p;
    }
    delete [] a_pArgList;
  }
}

S_NAMESPACE_END
