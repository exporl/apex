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

#include "streamapp/defines.h"

#include "streamapp/processors/matrixmixer.h"

#include "streamapp/stream.h"

#include "streamapp/utils/stringutils.h"

#include "mixercallback.h"

using namespace apex;
using namespace apex::stimulus;
using namespace streamapp;

IApexMixerCallback::IApexMixerCallback( const unsigned ac_nMaxNumInputs, const unsigned ac_nNumOutputs, const unsigned ac_nBufferSize ) :
    ConnectItem( ac_nBufferSize, ac_nMaxNumInputs ),
  mv_dGain( 0.0 ),
  mc_pMixer( new MatrixMixer( ac_nBufferSize, ac_nMaxNumInputs ) )
{
  mc_pMixer->mp_SetNumIns( ac_nMaxNumInputs );
  mc_pMixer->mp_SetNumOuts( ac_nNumOutputs );
  mc_pMixer->mp_SetAllNodes( sc_dMixerMin );
}

IApexMixerCallback::~IApexMixerCallback()
{
  delete mc_pMixer;
}

void IApexMixerCallback::mp_AddInput( StreamCallback* const ac_pItem )
{
  const unsigned nIChan = ac_pItem->mf_nGetNumChannels();
  const unsigned nOChan = mc_pMixer->mf_nGetNumOutputChannels();
  unsigned curOChan = 0;
  for( unsigned i = 0 ; i < nIChan ; ++i )
  {
    IApexMixerCallback::mp_AddInput( ac_pItem, i, curOChan );
    if( ++curOChan == nOChan )
      curOChan = 0;
  }
}

void IApexMixerCallback::mp_AddInput( StreamCallback* const ac_pItem, const unsigned ac_nItemOutChannel )
{
  IApexMixerCallback::mp_AddInput( ac_pItem, ac_nItemOutChannel, 0 );
}

void IApexMixerCallback::mp_AddInput( StreamCallback* const ac_pItem, const unsigned ac_nItemOutChannel, const unsigned ac_nThisInChannel )
{
  int nFree = ConnectItem::mf_nGetFirstFreeChannel();
  if( nFree != -1 )
  {
      //clear row and add current
    mc_pMixer->mp_SetMatrixRow( nFree, sc_dMixerMin );
    mc_pMixer->mp_SetMatrixNode( (unsigned) nFree, ac_nThisInChannel, mv_dGain );
  #ifdef PRINTCONNECTIONS
    PrintVector( mc_pMixer->mf_PrintMatrix() );
  #endif
      //add connection
    ConnectItem::mp_AddInput( ac_pItem, ac_nItemOutChannel, (unsigned) nFree );
  }
  else
    Q_ASSERT( 0 && "check connections!!" );
}

void IApexMixerCallback::mp_RemoveInput( const unsigned ac_nThisInChannel )
{
  ConnectItem::mp_RemoveInput( ac_nThisInChannel );
  mc_pMixer->mp_SetMatrixRow( ac_nThisInChannel, sc_dMixerMin );
#ifdef PRINTCONNECTIONS
  PrintVector( mc_pMixer->mf_PrintMatrix() );
#endif
}

void IApexMixerCallback::mp_RemoveInput( StreamCallback* const ac_pItem, const unsigned ac_nItemOutChannel, const unsigned ac_nThisInChannel )
{
  int nIndex = sf_nFindInArray( ac_pItem, m_Inputs );
  while( nIndex != -1 )
  {
      //[ stijn 28/02/2009 ] when device parameters are not reset before loading a stimulus,
      //all gains in the matrix stay where they are; this value is however not necessarily the same as mv_dGain:
      //gain only gets set when using mp_SetGain( dGain ), not when using mp_SetGain( dGain, nChannel ).
      //So the line mc_pMixer->mf_dGetMatrixNode( (unsigned) nIndex, ac_nThisInChannel ) == mv_dGain might return false,
      //event though the gain at that position was set earlier. The purpose of the line is only to figure out if it is in use,
      //so it's ok if we just check if mc_pMixer->mf_dGetMatrixNode( (unsigned) nIndex, ac_nThisInChannel ) != sc_dMixerMin :
      //if that is the case, the position in the mixer is in use, else it's not.
    //if( mc_pMixer->mf_dGetMatrixNode( (unsigned) nIndex, ac_nThisInChannel ) == mv_dGain )
    if( mc_pMixer->mf_dGetMatrixNode( (unsigned) nIndex, ac_nThisInChannel ) != sc_dMixerMin )
    {
      if( ac_nItemOutChannel == (unsigned) m_InputChans( nIndex ) )
      {
        mp_RemoveInput( nIndex );
        break;
      }
    }
    nIndex = sf_nFindInArray( ac_pItem, m_Inputs, nIndex );
  }
}

void IApexMixerCallback::mp_SetGain( const double ac_dGainIndB )
{
  const unsigned nIChan = mc_pMixer->mf_nGetNumInputChannels();
  const unsigned nOChan = mc_pMixer->mf_nGetNumOutputChannels();

  for( unsigned i = 0 ; i < nIChan ; ++i )
  {
    for( unsigned j = 0 ; j < nOChan ; ++j )
    {
      if( mc_pMixer->mf_dGetMatrixNode( i, j ) != sc_dMixerMin )
        mc_pMixer->mp_SetMatrixNode( i, j, ac_dGainIndB );
    }
  }

#ifdef PRINTCONNECTIONS
  PrintVector( mc_pMixer->mf_PrintMatrix() );
#endif

  mv_dGain = ac_dGainIndB;
}

void IApexMixerCallback::mp_SetGain( const double ac_dGainIndB, const unsigned ac_nChannel )
{
  const unsigned nIChan = mc_pMixer->mf_nGetNumInputChannels();

  for( unsigned i = 0 ; i < nIChan ; ++i )
  {
    if( mc_pMixer->mf_dGetMatrixNode( i, ac_nChannel ) != sc_dMixerMin ) {
      mc_pMixer->mp_SetMatrixNode( i, ac_nChannel, ac_dGainIndB );
    } else {
        //qCDebug(APEX_RS, "Not setting channel to %f dB, it is muted", ac_dGainIndB);
    }
  }
#ifdef PRINTCONNECTIONS
  PrintVector( mc_pMixer->mf_PrintMatrix() );
#endif
}

void IApexMixerCallback::mf_Callback()
{
  if( mf_bResultLocked() )
    return;
  ConnectItem::mf_CallbackFunc();
  IApexMixerCallback::mf_CallbackFunc();
  mp_LockResult();
}

void IApexMixerCallback::mf_CallbackFunc()
{
  ConnectItem::mc_pResult = &mc_pMixer->mf_DoProcessing( *ConnectItem::mc_pResult );
}

/**********************************************************************************************************************/

ApexOutputCallback::ApexOutputCallback( OutputStream* const ac_pOutput, const unsigned ac_nMaxNumInputs, const bool ac_bDeleteOutput ) :
    IApexMixerCallback( ac_nMaxNumInputs, ac_pOutput->mf_nGetNumChannels(), ac_pOutput->mf_nGetBufferSize() ),
  mc_bDeleteOutput( ac_bDeleteOutput ),
  mc_pOutput( ac_pOutput )
{
}

ApexOutputCallback::~ApexOutputCallback()
{
  if( mc_bDeleteOutput )
    delete mc_pOutput;
}

void ApexOutputCallback::mf_Callback()
{
  if( mf_bResultLocked() )
    return;
  IApexMixerCallback::mf_Callback();
  mf_CallbackFunc();
  mp_LockResult();
}

void ApexOutputCallback::mf_CallbackFunc()
{
  mc_pOutput->Write( *ConnectItem::mc_pResult );
}

/**********************************************************************************************************************/

ApexProcessorCallback::ApexProcessorCallback( IStreamProcessor* const ac_pProc, const unsigned ac_nMaxNumInputs ) :
    IApexMixerCallback( ac_nMaxNumInputs, ac_pProc->mf_nGetNumInputChannels(), ac_pProc->mf_nGetBufferSize() ),
  mc_pProc( ac_pProc )
{
}

ApexProcessorCallback::~ApexProcessorCallback()
{
  delete mc_pProc;
}

void ApexProcessorCallback::mf_Callback()
{
  if( mf_bResultLocked() )
    return;
  IApexMixerCallback::mf_Callback();
  mf_CallbackFunc();
  mp_LockResult();
}

void ApexProcessorCallback::mf_CallbackFunc()
{
  ConnectItem::mc_pResult = &mc_pProc->mf_DoProcessing( *ConnectItem::mc_pResult );
}
