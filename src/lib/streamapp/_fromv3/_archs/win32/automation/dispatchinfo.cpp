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
 
#include "dispatchinfo.h"
#include "idispatch.h"
#include <ocidl.h>

S_NAMESPACE_BEGIN

using namespace win32::automation;

tDispatchInfo::tDispatchInfo() s_no_throw :
  m_DispID( 0 ),
  m_sName( 0 ),
  m_wFlag( 0 ),
  m_nVfOffset( 0 ),
  m_CallConv( CC_STDCALL ),
  m_OutputType( VT_EMPTY ),
  m_nParams( 0 ),
  m_pParamTypes( 0 )
{
}

tDispatchInfo::tDispatchInfo( const tDispatchInfo& r ) s_no_throw :
  m_DispID( r.m_DispID ),
  m_sName( ::SysAllocString( r.m_sName ) ),
  m_wFlag( r.m_wFlag ),
  m_nVfOffset( r.m_nVfOffset ),
  m_CallConv( r.m_CallConv ),
  m_OutputType( r.m_OutputType ),
  m_nParams( r.m_nParams ),
  m_pParamTypes( new WORD[ r.m_nParams ] )
{
  for( int i = 0 ; i < r.m_nParams ; ++i )
    m_pParamTypes[ i ] = r.m_pParamTypes[ i ];
}

tDispatchInfo& tDispatchInfo::operator = ( const tDispatchInfo& r ) s_no_throw
{
  if( &r != this )
  {
    if( m_pParamTypes )
      delete [] m_pParamTypes;
    if( m_sName )
      ::SysFreeString( m_sName );

    m_DispID = r.m_DispID;
    m_sName = ::SysAllocString( r.m_sName );
    m_wFlag = r.m_wFlag;
    m_nVfOffset = r.m_nVfOffset;
    m_CallConv = r.m_CallConv;
    m_OutputType = r.m_OutputType;
    m_nParams = r.m_nParams;
    m_pParamTypes = new WORD[ r.m_nParams ];
  }
  return *this;
}

tDispatchInfo::~tDispatchInfo() s_no_throw
{
  if( m_pParamTypes )
    delete [] m_pParamTypes;
  if( m_sName )
    ::SysFreeString( m_sName );
}

HRESULT DispatchInfoBuilder::smf_hBuildInfo( IDispatch* a_pDispatch, mt_Dispatchinfos& a_Array ) s_no_throw
{
  HRESULT hRet = S_OK;

    //get typeinfo
  tGetIUnknownWrapper< ITypeInfo* >::type pTypeInfo;
  hRet = a_pDispatch->GetTypeInfo( 0 ,LOCALE_SYSTEM_DEFAULT, &pTypeInfo() );
  if( hRet != S_OK || pTypeInfo() == 0 )
    return hRet;

    //get typeattr
  TYPEATTR* pTypeAttr;
  hRet = pTypeInfo->GetTypeAttr( &pTypeAttr );
  if( hRet != S_OK )
    return hRet;

  if( pTypeAttr->typekind != TKIND_DISPATCH && pTypeAttr->typekind != TKIND_COCLASS )
  {
    pTypeInfo->ReleaseTypeAttr( pTypeAttr );
    return E_UNEXPECTED;
  }

    //if COCLASS, look further
  if( pTypeAttr->typekind == TKIND_COCLASS )
  {
    int nFlags;
    HREFTYPE hRefType;
    tGetIUnknownWrapper< ITypeInfo* >::type pTempInfo;
    TYPEATTR* pTempAttr = 0;
    for( WORD i = 0 ; i < pTypeAttr->cImplTypes ; ++i )
    {
      if( pTypeInfo->GetImplTypeFlags( i, &nFlags ) == S_OK && ( nFlags & IMPLTYPEFLAG_FDEFAULT ) )
      {
        hRet = pTypeInfo->GetRefTypeOfImplType( i, &hRefType );
        if( hRet == S_OK )
          hRet = pTypeInfo->GetRefTypeInfo( hRefType, &pTempInfo() );
        if( hRet == S_OK )
        {
          hRet = pTempInfo->GetTypeAttr( &pTempAttr );
          if( hRet != S_OK )
          {
            pTempInfo.mp_Close();
            break;
          }
        }
        else break;
      }
    }
    pTypeInfo->ReleaseTypeAttr( pTypeAttr );
    pTypeInfo.mp_Close();
    if( pTempAttr == 0 )
    {
      return hRet == S_OK ? S_FALSE : hRet;
    }
    else
    {
      pTypeInfo = pTempInfo;
      pTypeAttr = pTempAttr;
    }
  }

    //start filling info
  WORD m_nMethodCount = pTypeAttr->cFuncs;
  WORD m_nVarCount = pTypeAttr->cVars;
  WORD nItems = m_nMethodCount + 2 * m_nVarCount;
  a_Array.reserve( nItems );

    //for methods
  for( WORD i = 0 ; i < m_nMethodCount ; ++i )
  {
    FUNCDESC* pFuncDesc;
    hRet = pTypeInfo->GetFuncDesc( i, &pFuncDesc );
    if( hRet != S_OK )
    {
      pTypeInfo->ReleaseTypeAttr( pTypeAttr );
      a_Array.clear();
      return hRet;
    }

    tDispatchInfo& cur( a_Array[ i ] );

      //ID
    unsigned nCount;
    cur.m_DispID = pFuncDesc->memid;
    hRet = pTypeInfo->GetNames( cur.m_DispID ,&cur.m_sName, 1, &nCount );
    if( hRet != S_OK )
    {
      pTypeInfo->ReleaseFuncDesc( pFuncDesc );
      pTypeInfo->ReleaseTypeAttr( pTypeAttr );
      a_Array.clear();
      return hRet;
    }

      //flags
    switch( pFuncDesc->invkind )
    {
      case INVOKE_PROPERTYGET: cur.m_wFlag = DISPATCH_PROPERTYGET; break;
      case INVOKE_PROPERTYPUT: cur.m_wFlag = DISPATCH_PROPERTYPUT; break;
      case INVOKE_PROPERTYPUTREF: cur.m_wFlag = DISPATCH_PROPERTYPUTREF; break;
      case INVOKE_FUNC: cur.m_wFlag = DISPATCH_METHOD; break;
      default: break;
    }

      //rest
    cur.m_nVfOffset = pFuncDesc->oVft;
    cur.m_CallConv = pFuncDesc->callconv;
    cur.m_OutputType = pFuncDesc->elemdescFunc.tdesc.vt;
    if( cur.m_OutputType == VT_VOID || cur.m_OutputType == VT_NULL )
      cur.m_OutputType = VT_EMPTY;

      //params
    cur.m_nParams = pFuncDesc->cParams;
    cur.m_pParamTypes = new WORD[ cur.m_nParams ];
    for( int j = 0 ; j < cur.m_nParams ; ++j )
    {
      if( pFuncDesc->lprgelemdescParam[ j ].tdesc.vt == VT_SAFEARRAY )
      {
        cur.m_pParamTypes[ j ] = (WORD) ( pFuncDesc->lprgelemdescParam[ j ].tdesc.lptdesc->vt | VT_ARRAY );
      }
      else if( pFuncDesc->lprgelemdescParam[ j ].tdesc.vt == VT_PTR )
      {
        cur.m_pParamTypes[ j ] = (WORD) ( pFuncDesc->lprgelemdescParam[ j ].tdesc.lptdesc->vt | VT_BYREF );
      }
      else
      {
        cur.m_pParamTypes[ j ] = pFuncDesc->lprgelemdescParam[ j ].tdesc.vt;
      }
    }
    pTypeInfo->ReleaseFuncDesc( pFuncDesc );
  }

    //for variables
  for( WORD i = m_nMethodCount ; i < m_nMethodCount + m_nVarCount ; ++i )
  {
    VARDESC* pVarDesc;
    hRet = pTypeInfo->GetVarDesc( i - m_nMethodCount, &pVarDesc );
    if( hRet != S_OK )
    {
      pTypeInfo->ReleaseTypeAttr( pTypeAttr );
      a_Array.clear();
      return hRet;
    }

    tDispatchInfo& cur( a_Array[ i + m_nVarCount ] );
    tDispatchInfo& curP( a_Array[ i ] );

    cur.m_DispID = pVarDesc->memid;
    curP.m_DispID = cur.m_DispID;


    unsigned int nCount;
    hRet = pTypeInfo->GetNames( cur.m_DispID, &curP.m_sName, 1, &nCount );
    if( hRet != S_OK )
    {
      pTypeInfo->ReleaseVarDesc( pVarDesc );
      pTypeInfo->ReleaseTypeAttr( pTypeAttr );
      a_Array.clear();
      return hRet;
    }
    cur.m_sName = ::SysAllocString( curP.m_sName );


    switch( pVarDesc->varkind )
    {
    case VAR_DISPATCH:
      curP.m_wFlag = DISPATCH_PROPERTYGET;
      cur.m_wFlag = DISPATCH_PROPERTYPUT;
      curP.m_OutputType = pVarDesc->elemdescVar.tdesc.vt;
      cur.m_OutputType = VT_EMPTY;
      cur.m_nParams = 1;
      cur.m_pParamTypes = new WORD[ 1 ];
      cur.m_pParamTypes[ 0 ] = curP.m_OutputType;
      break;
    default:
      curP.m_wFlag = 0;
      cur.m_wFlag = 0;
      break;
    }
    pTypeInfo->ReleaseVarDesc( pVarDesc );
  }

  pTypeInfo->ReleaseTypeAttr( pTypeAttr );
  return hRet;
}

HRESULT DispatchInfoBuilder::smf_hGetConnectionPointIDs( IDispatch* a_pDispatch, mt_CLSIDs& a_Array )
{
    //get container
  tGetIUnknownWrapper< IConnectionPointContainer* >::type pCPContainer;
  HRESULT hRet = a_pDispatch->QueryInterface( IID_IConnectionPointContainer, (void**)&pCPContainer() );
  if( hRet != S_OK )
    return hRet;

    //get enumerator
  tGetIUnknownWrapper< IEnumConnectionPoints* >::type pEnumCPoints;
  hRet = pCPContainer->EnumConnectionPoints( &pEnumCPoints() );
  if( hRet != S_OK )
    return hRet;

  ULONG nAvail;
  CLSID add;
  for( ; ; )
  {
    LPCONNECTIONPOINT lpCPT;
    nAvail = 0;
    hRet = pEnumCPoints->Next( 1, &lpCPT, &nAvail );
    if( hRet == S_OK && nAvail == 1 )
    {
      hRet = lpCPT->GetConnectionInterface( &add );
      lpCPT->Release();
      a_Array.push_back( add );
    }
    else
    {
      break;
    }
  }
  return S_OK;
}

S_NAMESPACE_END
