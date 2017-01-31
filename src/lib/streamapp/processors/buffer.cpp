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

#include "buffer.h"

#include <QtGlobal>

using namespace streamapp;

BasicBuffer::BasicBuffer( const unsigned ac_nChan, const unsigned ac_nInputSize, const unsigned ac_nBuffers ) :
  IStreamProcessor(),
  mc_nBuffers( ac_nBuffers ),
  mc_nInputSize( ac_nInputSize ),
  mc_nOutputSize( mc_nBuffers * mc_nInputSize ),
  mc_nChannels( ac_nChan ),
  m_nCount( 0 ),
  m_pResult( 0 ),
  m_pCallback( 0 )
{
  Q_ASSERT( ac_nBuffers );
}

BasicBuffer::~BasicBuffer()
{}

const Stream& BasicBuffer::mf_DoProcessing( const Stream& ac_StrToProc )
{
  Q_ASSERT( ac_StrToProc.mf_nGetChannelCount() >= mf_nGetNumInputChannels() );
  Q_ASSERT( ac_StrToProc.mf_nGetBufferSize() >= mf_nGetBufferSize() );

    //we don't necessary have to do something
  if( m_pResult )
  {
      //get samples
    const unsigned nCurOffset = m_nCount * mf_nGetBufferSize();
    for( unsigned j = 0 ; j < mf_nGetNumInputChannels() ; ++j )
    {
      for( unsigned i = 0 ; i < mf_nGetBufferSize() ; ++ i )
        m_pResult->mp_Set( j, i + nCurOffset, ac_StrToProc( j, i ) );
    }

      //buffer full
    if( ++m_nCount == mc_nBuffers )
    {
      m_nCount = 0;
      if( m_pCallback )
        m_pCallback->mf_DoProcessing( *m_pResult );
    }
  }

  return ac_StrToProc;
}

void BasicBuffer::mp_SetBufferToFill( Stream* ac_Buf )
{
  Q_ASSERT( ac_Buf->mf_nGetChannelCount() >= mf_nGetNumInputChannels() );
  Q_ASSERT( ac_Buf->mf_nGetBufferSize() >= mc_nOutputSize );
  m_pResult = ac_Buf;
}

void BasicBuffer::mp_SetCallback( IStreamProcessor* const ac_pCallbackProc )
{
  m_pCallback = ac_pCallbackProc;
}
