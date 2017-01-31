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
 
#include "socketfunc.h"
#include <math.h>
#include <stdio.h>

using namespace network;

namespace
{

  const int nAsciiConv = 48;

  unsigned gf_nParseSize( const char* ac_pBuf, unsigned ac_nBytes )
  {
    unsigned nRet = 0;
    unsigned nCnt = 0;
    for( unsigned i = ac_nBytes ; i > 0 ; --i )
    {
      nRet += (unsigned) ( powf( 10.f, (float) nCnt ) ) * ( ( (unsigned) ac_pBuf[ i - 1 ] ) - nAsciiConv );
      nCnt++;
    }
    return nRet;
  }
}

CommSocket::CommSocket( ISocket* const ac_pSocket, const bool ac_bDisconnectOnKill ) :
  m_pSocket( ac_pSocket ),
  mc_bDisconnect( ac_bDisconnectOnKill ),
  mv_nMagicNumber( 0 )
{
}

CommSocket::~CommSocket()
{
  if( mc_bDisconnect )
    m_pSocket->mf_bClose();
}

CommSocket::mt_eStatus CommSocket::mf_eReadFromSocket( RawMemory& a_Destination )
{
  if( m_pSocket->mf_bIsOpen() )
  {
    int nBytesToReceive = 10;
    if( mv_nMagicNumber )
      nBytesToReceive += 10;

    if( m_pSocket->mf_nRead( m_pRecvBuffer, nBytesToReceive ) != nBytesToReceive )
      return mp_eReturnWithError( mc_eSockErrR );

    if( mv_nMagicNumber && (unsigned)( gf_nParseSize( m_pRecvBuffer, 10 ) ) != mv_nMagicNumber )
      return mp_eReturnWithError( mc_eMagicErr );

    if( mv_nMagicNumber )
      nBytesToReceive = gf_nParseSize( m_pRecvBuffer + 10, 10 );
    else
      nBytesToReceive = gf_nParseSize( m_pRecvBuffer, 10 );

    a_Destination.mp_Resize( (unsigned) nBytesToReceive );

    if( m_pSocket->mf_nRead( a_Destination.mf_pGetMemory(), nBytesToReceive ) != nBytesToReceive )
      return mp_eReturnWithError( mc_eSockErrR );

    return mc_eOk;
  }
  return mc_eSockNoC;
}

CommSocket::mt_eStatus CommSocket::mf_eWriteToSocket( const RawMemoryAccess& ac_Source, const unsigned ac_nBytesToSend )
{
  if( m_pSocket->mf_bIsOpen() )
  {
    s_assert( ac_nBytesToSend <= ac_Source.mf_nGetSize() );

    int nBytesToSend = 10;
    if( mv_nMagicNumber )
    {
      nBytesToSend += 10;
      sprintf( m_pRecvBuffer, "%010u", mv_nMagicNumber );
      sprintf( m_pRecvBuffer + 10, "%010u", ac_nBytesToSend );
    }
    else
    {
      sprintf( m_pRecvBuffer, "%010u", ac_nBytesToSend );
    }

    if( m_pSocket->mf_nWrite( m_pRecvBuffer, nBytesToSend ) != nBytesToSend )
      return mp_eReturnWithError( mc_eSockErrW );

    nBytesToSend = ac_nBytesToSend;

    if( m_pSocket->mf_nWrite( ac_Source.mf_pGetMemory(), nBytesToSend ) != nBytesToSend )
      return mp_eReturnWithError( mc_eSockErrW );

    return mc_eOk;
  }
  return mc_eSockNoC;
}

CommSocket::mt_eStatus CommSocket::mf_eWriteToSocket( const RawMemoryAccess& ac_Source )
{
  return mf_eWriteToSocket( ac_Source, ac_Source.mf_nGetSize() );
}

CommSocket::mt_eStatus CommSocket::mp_eReturnWithError( mt_eStatus a_Err )
{
  if( mc_bDisconnect )
    m_pSocket->mf_bClose();
  return a_Err;
}
