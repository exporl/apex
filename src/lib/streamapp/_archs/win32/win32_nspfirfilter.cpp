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
 
#include "win32_nspfirfilter.h"

#define nsp_UsesFir
#include "nsp.h"
#include "nspfirh.h"

using namespace streamapp;

NspFirFilter::NspFirFilter( const unsigned ac_nChannels, const unsigned ac_nBufferSize, const unsigned ac_nTaps ) :
  m_FirState( ac_nChannels ),
  m_FirTaps( ac_nChannels, ac_nTaps ),
  m_FirResult( ac_nChannels, ac_nBufferSize )
{
}

NspFirFilter::~NspFirFilter()
{
  const unsigned nChannels = mf_nGetNumInputChannels();
  NSPFirState* p = m_FirState.mf_pGetArray();
  
  for( unsigned i = 0 ; i < nChannels ; ++i )
    nspFirFree( &p[ i ] );
}

void NspFirFilter::mp_SetFilterTaps( const Stream& ac_Taps )
{
  const unsigned nChannels = mf_nGetNumInputChannels();
  const unsigned nTaps = mf_nGetNumFirTaps();
  m_FirTaps.mp_CopyFrom( ac_Taps );
  
  NSPFirState* p = m_FirState.mf_pGetArray();
  for( unsigned i = 0 ; i < nChannels ; ++i )
    nspdFirInit( m_FirTaps.mf_pGetArray()[ i ], nTaps, 0, &p[ i ] );
}

const Stream& NspFirFilter::mf_DoProcessing( const Stream& ac_Input )
{
  const unsigned nChannels = mf_nGetNumInputChannels();
  const unsigned nBuffSize = mf_nGetBufferSize();
  StreamType** pSrc = ac_Input.mf_pGetArray();
  StreamType** pDst = m_FirResult.mf_pGetArray();
  
  NSPFirState* p = m_FirState.mf_pGetArray();
  for( unsigned i = 0 ; i < nChannels ; ++i )
    nspdbFir( &p[ i ], pSrc[ i ], pDst[ i ], nBuffSize );

  return m_FirResult; 
}

void NspFirFilter::mf_DoProcessReplacing( Stream& a_Input )
{
  const unsigned nChannels = mf_nGetNumInputChannels();
  const unsigned nBuffSize = mf_nGetNumInputChannels();
  StreamType** pSrc = a_Input.mf_pGetArray();
  StreamType** pDst = pSrc;
  
  NSPFirState* p = m_FirState.mf_pGetArray();
  for( unsigned i = 0 ; i < nChannels ; ++i )
    nspdbFir( &p[ i ], pSrc[ i ], pDst[ i ], nBuffSize );
} 
