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
 
#include "bufferedreader.h"
#include "bufferedreaderthread.h"
#include "utils/checks.h"
#include "utils/tracer.h"
#include "callback/callback.h"
#include "core/containers/circularlogic.h"

using appcore::Lock;
using appcore::UnLock;
using namespace streamapp;

BufferedReader::mt_WriteOp::mt_WriteOp( PositionableAudioFormatReader* a_pReader, const appcore::CriticalSection& ac_Lock ) :
  m_pReader( a_pReader ),
  mc_Lock( ac_Lock )
{
}

bool BufferedReader::mt_WriteOp::operator () ( int** a_pDest, const unsigned ac_nSamples )
{
  const appcore::Lock L( mc_Lock );
  m_pReader->Read( (void**) a_pDest, ac_nSamples );
  return m_pReader->mf_lSamplesLeft() != 0;
}


/************************************************************************************************************/


BufferedReader::BufferedReader( PositionableAudioFormatReader* a_pParent,
                                const unsigned ac_nBufferSize,
                                const bool ac_bDeleteParent,
                                const bool ac_bRegisterWithThread ) :
  mc_nBufferSize( ac_nBufferSize ),
  mc_bDeleteParent( ac_bDeleteParent ),
  m_pParent( a_pParent ),
  mv_nPrevLeft( a_pParent->mf_lSamplesLeft() ),
  m_Buffer( a_pParent->mf_nChannels(), ac_nBufferSize ),
  m_pCallback( 0 )
{
  m_pFifo = new mt_Fifo( m_Buffer.mf_pGetArray(), ac_nBufferSize, a_pParent->mf_nChannels() );
  m_pBufferFiller = new mt_WriteOp( a_pParent, mc_BigLock );
  if( ac_bRegisterWithThread )
    theBufferedReaderThread::sf_pInstance()->mp_AddReader( this );
}

BufferedReader::~BufferedReader()
{
  theBufferedReaderThread::sf_pInstance()->mp_RemoveReader( this );
  delete m_pFifo;
  delete m_pBufferFiller;
  if( mc_bDeleteParent )
    delete m_pParent;
}

unsigned long BufferedReader::mp_FillBuffer()
{
  const Lock L( mc_SmallLock );

  const unsigned nMaxWrite = m_pFifo->mf_nGetMaxWrite();
  mv_nPrevLeft = m_pParent->mf_lSamplesLeft();
  const unsigned nCanWrite = m_pParent->mf_bIsLooping() ? nMaxWrite : mv_nPrevLeft; //if we're looping, just fill the whole
  if( ( nMaxWrite == 0 ) ||
      ( nCanWrite == 0 ) ||
      ( m_pFifo->mf_nGetMaxRead() > ( 2 * mc_nBufferSize ) / 3 ) )
    return 0;

  const UnLock UL( mc_SmallLock );
  const Lock BL( mc_BigLock );

  DBG( "BufferdReader: got filled" );

  size_type nWrittem;
  m_pFifo->mp_Write( *m_pBufferFiller, nWrittem, s_min( nMaxWrite, nCanWrite ) );
  return nWrittem;
}

unsigned long BufferedReader::Read( void** a_pBuf, const unsigned ac_nSamples )
{
  Q_ASSERT( ac_nSamples < mc_nBufferSize ); //else it doesn't make any sense using a buffer

  const Lock L( mc_SmallLock );

  const size_type lParentLeft = m_pParent->mf_lSamplesLeft();
  const size_type lFifoLeft = m_pFifo->mf_nGetMaxRead();

  size_type nCanRead = 0;

  if( lParentLeft )
  {
    nCanRead = ac_nSamples;
    if( lFifoLeft < nCanRead )
    {
      if( m_pCallback ) {
        m_pCallback->mf_Callback();
      } else {
        DBG( "buffer underrun" );
      }
      nCanRead = 0; //total loss
    }
  }
  else
  {
    nCanRead = s_min( lFifoLeft, ac_nSamples );
  }

  size_type nRead = 0;
  if( nCanRead )
    m_pFifo->mp_Read( (int**) a_pBuf, nRead, nCanRead );
  return nRead;
}

unsigned long BufferedReader::mf_nGetNumInBuffer()
{
  return m_pFifo->mf_nGetMaxRead();
}

void BufferedReader::mf_WaitUntilFilled( const unsigned long ac_lHowMuch )
{
  while( m_pFifo->mf_nGetMaxRead() < ac_lHowMuch )
    appcore::IThread::sf_Sleep( 5 );
}

unsigned long BufferedReader::mf_lCurrentPosition() const
{
  return m_pParent->mf_lTotalSamples() - mf_lSamplesLeft();
}

unsigned long BufferedReader::mf_lSamplesLeft() const
{
  if( mc_BigLock.mf_bTryEnter() )
  {
    const unsigned long lLeft = m_pParent->mf_lSamplesLeft();
    const unsigned long lRet  =  lLeft ? lLeft : m_pFifo->mf_nGetMaxRead();
    mc_BigLock.mf_Leave();
    return lRet;
  }
  else
  {
    DBG( "BufferedReader: didn't get BigLock " );
    const Lock L( mc_SmallLock );
    return mv_nPrevLeft;
  }
}

void BufferedReader::mp_SeekPosition( const unsigned long ac_lPosition )
{
  if( mc_BigLock.mf_bTryEnter() )
  {
    m_pFifo->mp_SetEmpty();
    m_pParent->mp_SeekPosition( ac_lPosition );
    mc_BigLock.mf_Leave();
  }
  else
  {
    const Lock L( mc_SmallLock );
    m_pFifo->mp_SetEmpty();
    m_pParent->mp_SeekPosition( ac_lPosition );
  }
}
