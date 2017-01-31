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

#include "rmstester.h"
#include "streamappfactory.h"
#include "audiosamplebuffer.h"
#include "soundcard/soundcard.h"
#include "processors/processor.h"
#include "callback/streamappcallbacks.h"
using namespace streamapp;


RmsProc::RmsProc( const unsigned ac_nChan, const unsigned ac_nSize ) :
    StreamProcessor( ac_nChan, ac_nSize ),
  m_Rms( ac_nChan, true ),
  mv_dCount( 0.0 ),
  mv_bReset( true )
{
}

RmsProc::~RmsProc()
{
}

void RmsProc::mp_Reset()
{
  mv_bReset = true;
  mv_dCount = 0.0;
}

const Stream& RmsProc::mf_DoProcessing( const Stream& ac_ToCheck )
{
  AudioSampleBuffer calc( ac_ToCheck );
  for( unsigned i = 0 ; i < mc_nChan ; ++i )
  {
    StreamType dRms;
    calc.mf_dCalculateRMS( i, dRms );
    if( mv_bReset )
      m_Rms.mp_Set( i, dRms );
    else
    {
      const StreamType dPrev = m_Rms( i ) * ( mv_dCount - 1.0 );
      const StreamType dNewv = ( dRms + dPrev ) / mv_dCount;
      m_Rms.mp_Set( i, dNewv );
    }
  }
  return ac_ToCheck;
}

RmsTester::RmsTester( ISoundCard* a_pSoundCard ) :
  m_pCard( a_pSoundCard ),
  m_pProc( new IOProcessor( true ) ),
  m_pCallback( new Channel( true ) ),
  m_pRMS( new RmsProc( a_pSoundCard->mf_nGetIChan(), a_pSoundCard->mf_nGetBufferSize() ) )
{
  m_pCallback->mp_SetSource( StreamAppFactory::sf_pInstance()->mf_pSoundCardIn( *a_pSoundCard ) );
  m_pCallback->mp_SetSink( StreamAppFactory::sf_pInstance()->mf_pSoundCardOut( *a_pSoundCard ) );
  m_pCallback->mp_SetProcessor( m_pProc );
  m_pProc->mp_ReplaceProcessor( m_pRMS );
}

RmsTester::~RmsTester()
{
  delete m_pCallback;
}

void RmsTester::mp_SetSource( InputStream* a_pStream )
{
  m_pProc->mp_ReplaceOutput( a_pStream );
}

double RmsTester::mf_dGetRecordedRMS( const unsigned ac_nChannel ) const
{
  return m_pRMS->mf_GetLevels()( ac_nChannel );
}

void RmsTester::mp_Start()
{
  m_pRMS->mp_Reset();
  m_pCard->mp_bStart( *m_pCallback );
}

void RmsTester::mp_Stop()
{
  m_pCard->mp_bStop();
}
