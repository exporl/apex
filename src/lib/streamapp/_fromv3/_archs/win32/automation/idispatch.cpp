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
  * [ file ] _archs/win32/automation/idispatch.cpp
  * [ fmod ] default
  * <^>
  */

#include "idispatch.h"
#include "clsid.h"

S_NAMESPACE_BEGIN

using namespace win32::automation;

IDispatchInstance::IDispatchInstance()
{
}

IDispatchInstance::IDispatchInstance( IDispatch* a_pObj ) :
  parent( a_pObj )
{
}

IDispatchInstance::~IDispatchInstance()
{
}

HRESULT IDispatchInstance::mp_bCreateObject( const CLSID& ac_tID ) s_no_throw
{
  IUnknownInstance pUnknown;  //IUnknown of automation object

    //Create an instance of the automation object
  HRESULT m_hResult = CoCreateInstance( ac_tID, 0, CLSCTX_SERVER, IID_IUnknown, (void**) &pUnknown.mf_hGetHandle() );
  if( FAILED( m_hResult ) )
    goto error;

    //get the IDispatch interface
  m_hResult = pUnknown->QueryInterface( IID_IDispatch,(void**) &this->mf_hGetHandle() );
  if( FAILED( m_hResult ) )
    goto error;

  return true;

error:
  mp_Close();
  return m_hResult;
}

HRESULT IDispatchInstance::mp_bCreateObject( const char* ac_psName ) s_no_throw
{
  CLSID id;
  gf_bProgIDToCLSID( ac_psName, id );
  return mp_bCreateObject( id );
}

S_NAMESPACE_END
