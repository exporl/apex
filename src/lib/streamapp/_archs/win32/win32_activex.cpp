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
 
#include "win32_activex.h"
#include "utils/stringexception.h"
#include <objbase.h>

using namespace appcore;

GetActiveXObject::GetActiveXObject()
{
  if( CoInitialize( 0 ) != S_OK )
    throw utils::StringException( "CoInitialize failed!!" );
}

GetActiveXObject::~GetActiveXObject()
{
  CoUninitialize();
}

void GetActiveXObject::mf_pGetObject( REFCLSID a_FactoryID, REFCLSID a_ClassID, void** a_pTheObject )
{
  HRESULT hr;
  IClassFactory* pCF;

  hr = CoGetClassObject( a_FactoryID, CLSCTX_INPROC_SERVER, 0, IID_IClassFactory, (LPVOID*) &pCF );
  if( hr )
    throw utils::StringException( "Can't get IClassFactory; CoGetClassObject error" );
  else
  {
    hr = pCF->CreateInstance( 0, a_ClassID, (LPVOID*) a_pTheObject );
    pCF->Release();
    if( hr )
      throw utils::StringException( "Can't create the COM object; CreateInstance error" );
  }
}
