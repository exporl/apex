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

#include "../callback/eofcheckcallbacks.h"

#include "../containers/dynstrlist.h"

#include "../multistream.h"

#include "../soundcard/soundcard.h"

#include "../streamappfactory.h"
#include "../streamappfactory.h"
#include "../streamutils.h"

#include "../utils/stringutils.h"

#include "latencytester.h"

#include <QtGlobal>

using namespace appcore;
using namespace streamapp;

namespace
{
  StreamAppFactory* const sc_pFac( StreamAppFactory::sf_pInstance() );

  const std::string sc_sSoundIn( "soundin" );
  const std::string sc_sSoundOut( "soundout" );
  const std::string sc_sFileIn( "filein" );
  const std::string sc_sFileOut( "fileout" );
}

LatencyTester::LatencyTester( const unsigned ac_nBufferSize, const unsigned ac_nChannels, const unsigned long ac_nSamplerate, const gt_eDeviceType ac_eType ) :
  mv_sOutputDir( "" ),
  mc_pCard( sc_pFac->mf_pSoundCard( ac_eType, sc_sDefault.toStdString(), ac_nChannels, ac_nChannels, ac_nSamplerate, ac_nBufferSize ) ),
  m_ConnMan(),
  m_pSoundInput( 0 ),
  m_pSoundOutput( 0 ),
  m_pInput( 0 ),
  m_pOutputs( new MultiOutputStream< true >() ),
  m_pCallback( new EofCheckWaitCallback() )
{
    //setup soundcard io
  m_pSoundInput = sc_pFac->mf_pSoundCardIn( *mc_pCard );
  m_pSoundOutput = sc_pFac->mf_pSoundCardOut( *mc_pCard );
  m_ConnMan.mp_RegisterItem( sc_sSoundIn, m_pSoundInput );
  m_ConnMan.mp_RegisterItem( sc_sSoundOut, m_pSoundOutput );

    //setup callback mechanism
  m_pCallback->mp_RegisterMasterCallback( m_ConnMan );
}

LatencyTester::~LatencyTester()
{
  delete m_pSoundInput;
  delete m_pSoundOutput;
  delete mc_pCard;
  delete m_pOutputs;
  delete m_pCallback;
}

void LatencyTester::mp_SetDiskIO( const std::string& ac_sInputFile, const std::string& ac_sOutputDir )
{
  m_pInput = sc_pFac->mf_pReaderStream( ac_sInputFile, mc_pCard->mf_nGetBufferSize() );
  m_pCallback->mp_GetEofCheck()->mp_AddInputToCheck( *m_pInput );
  m_ConnMan.mp_RegisterItem( sc_sFileIn, m_pInput );
  mv_sOutputDir = ac_sOutputDir;
}

void LatencyTester::mp_SetChannels( const tUnsignedVector& ac_PlayChannelNumbers, const tUnsignedVector& ac_RecordChannelNumbers )
{
  const tUnsignedVector::size_type nPlay = ac_PlayChannelNumbers.size();
  const tUnsignedVector::size_type nRec = ac_RecordChannelNumbers.size();

  Q_ASSERT( nRec <= mc_pCard->mf_nGetIChan() );
  Q_ASSERT( nPlay <= mc_pCard->mf_nGetOChan() );

    //setup output first
  for( tUnsignedVector::size_type i = 0 ; i < nRec ; ++i )
    m_pOutputs->mp_AddItem( sc_pFac->mf_pWriterStream( mv_sOutputDir + "channel_" + toString( i ) + ".wav", 1, mc_pCard->mf_lGetSampleRate(), mc_pCard->mf_nGetBufferSize() ) );

    //register it
  m_ConnMan.mp_RegisterItem( sc_sFileOut, m_pOutputs );

    //use it
  for( tUnsignedVector::size_type i = 0 ; i < nRec ; ++i )
    m_ConnMan.mp_Connect( sc_sSoundIn, sc_sFileOut, ac_RecordChannelNumbers[ i ], i );

    //also use input ;-
  for( tUnsignedVector::size_type i = 0 ; i < nPlay ; ++i )
    m_ConnMan.mp_Connect( sc_sFileIn, sc_sSoundOut, 0, ac_PlayChannelNumbers[ i ] );
}

void LatencyTester::mp_GaanMetDieBanaan()
{
  mc_pCard->mp_bStart( *m_pCallback );
  m_pCallback->mf_GetWaitObject().mf_eWaitForSignal();
  mc_pCard->mp_bStop();
}
