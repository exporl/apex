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
 
#include "win32_getmacaddress.h"
#include "win32_headers.h"
#include <assert.h>
#include <Iphlpapi.h>

namespace
{
  const unsigned nTempBufSize = 128; //this should be big enough..
}

std::string win32net::gf_sPrintMACaddress( const unsigned char a_cMACData[ 8 ] )
{
  static char tmpbuf[ nTempBufSize ];

    //print in the same way eg ipconfig does
  sprintf( tmpbuf, "%02x-%02x-%02x-%02x-%02x-%02x",
            a_cMACData[ 0 ],
            a_cMACData[ 1 ],
            a_cMACData[ 2 ],
            a_cMACData[ 3 ],
            a_cMACData[ 4 ],
            a_cMACData[ 5 ] );

  return std::string( tmpbuf );
}

streamapp::tStringVector win32net::gf_saGetMACaddress()
{
  IP_ADAPTER_INFO AdapterInfo[ 16 ];        //Allocate information for up to 16 NICs
  DWORD dwBufLen = sizeof(AdapterInfo);
  DWORD dwStatus = GetAdaptersInfo( AdapterInfo, &dwBufLen );
  assert( dwStatus == ERROR_SUCCESS );

  streamapp::tStringVector ret;
  PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
  do
  {
    ret.push_back( gf_sPrintMACaddress( pAdapterInfo->Address ) );
    pAdapterInfo = pAdapterInfo->Next;
  }
  while( pAdapterInfo );
  return ret;
}
