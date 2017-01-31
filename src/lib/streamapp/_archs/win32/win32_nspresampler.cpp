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
 
#include "win32_nspresampler.h"
#include "win32_nspfirfilter.h"
#include "resamplers/firtapstool.h"
#include "streamutils.h"
#include <math.h>
#include <string.h>

using namespace streamapp;

NSPResampler::NSPResampler( const unsigned ac_nChan,
                            const unsigned ac_nBufSize,
                            const unsigned ac_nDownFactor,
                            const unsigned ac_nFirTaps ) :
  IResampler( ac_nChan, ac_nBufSize, ac_nDownFactor, ac_nFirTaps ),
    m_UpResult       ( ac_nChan, ac_nBufSize   , true  ),
    m_DownResult     ( ac_nChan, ac_nBufSize / ac_nDownFactor  , true  ),
    m_FirResult      ( ac_nChan, false ),
    m_SplitResult    ( ac_nChan, false ),
    m_FirTaps        ( ac_nChan, false ),
    m_FirUpStates    ( ac_nChan, ac_nDownFactor, false ),
    m_FirDownStates  ( ac_nChan, ac_nDownFactor, false )

{
   m_fInitResampler( ac_nFirTaps );
}

NSPResampler::~NSPResampler( )
{
  for( unsigned i = 0 ; i < mc_nChan ; ++i )
  {
    delete m_FirTaps.mf_pGetArray()[ i ];
    delete m_FirResult.mf_pGetArray()[ i ];
    delete m_SplitResult.mf_pGetArray()[ i ];
  }
}

void NSPResampler::m_fInitResampler( const unsigned nTaps )
{
    //get taps
  assert( nTaps % mc_nDownFactor == 0 );
  const unsigned nSplitTaps = nTaps / mc_nDownFactor;

  CFirTapsTool myTool( mc_nDownFactor, nTaps );
  const ArrayAccess<StreamType>& c_aTaps = myTool.mf_dfDesign_Aaf( );

    //set all channels to use same taps.
  const StreamBuf c_aAllTaps( mc_nDownFactor, nTaps, true );
  for( unsigned i = 0 ; i < mc_nDownFactor ; ++i )
  {
    memcpy( c_aAllTaps.mf_pGetArray()[ i ], c_aTaps.mf_pGetArray(), nTaps * sizeof( StreamType ) ); //was hl instead of firlen
  }

  for( unsigned i = 0 ; i < mc_nChan ; ++i )
  {
    m_FirTaps.mp_Set     ( i, new StreamBuf( mc_nDownFactor, nSplitTaps ) );
    m_FirResult.mp_Set   ( i, new StreamBuf( mc_nDownFactor, mc_nSize, true ) );//used for both up and down so take biggest size
    m_SplitResult.mp_Set ( i, new StreamBuf( mc_nDownFactor, mc_nSize / mc_nDownFactor, true ) );

    for( unsigned j = 0 ; j < mc_nDownFactor ; ++j )
    {
      for( unsigned k = 0 ; k < nSplitTaps ; ++k )
      {
        m_FirTaps.mf_Get( i )->mp_Set( j , k , c_aAllTaps( i , k * mc_nDownFactor + j  ) );
      }

      nspdFirInit ( m_FirTaps.mf_Get( i )->mf_pGetArray()[ j ],
                    nSplitTaps,
                    0    ,
                    &m_FirUpStates( i, j )   );

      nspdFirInit ( m_FirTaps.mf_Get( i )->mf_pGetArray()[ j ],
                    nSplitTaps,
                    0    ,
                    &m_FirDownStates( i, j ) );
    }
  }
}

const Stream& NSPResampler::mf_UpSample     ( const Stream& ac_InputStr )
{
  StreamType ** pFirIn  = ac_InputStr.mf_pGetArray();

  for( unsigned j = 0 ; j < mc_nChan ; ++j )
  {
    for( unsigned i = 0 ; i < mc_nDownFactor ; ++i )
    {
      nspdbFir( &m_FirUpStates( j , i ), pFirIn[ j ], m_FirResult.mf_Get( j )->mf_pGetArray()[ i ], mc_nSize / mc_nDownFactor );
    }
  }

    //upsampling
   for( unsigned i = 0 ; i < mc_nChan ; ++i )
  {
    for( unsigned j = 0 ; j < mc_nSize/mc_nDownFactor ; ++j )
    {
      for( unsigned k = 0 ; k < mc_nDownFactor ; ++k )
      {
        m_UpResult.mp_Set( i, mc_nDownFactor*j + k, mc_nDownFactor * m_FirResult.mf_Get( i )->operator ()( k, j ) );
      }
    }
  }

  return m_UpResult;
}

const Stream& NSPResampler::mf_DownSample   ( const Stream& ac_InputStr )
{
  const unsigned nToDo = mc_nSize / mc_nDownFactor;
  const unsigned nDMin = mc_nDownFactor - 1;

    //split channels in 4 parts
  for( unsigned i = 0 ; i < mc_nChan ; ++i )
  {
    for( unsigned j = 0 ; j < nToDo ; ++j )
    {
      for( unsigned k = 0 ; k < mc_nDownFactor ; ++k )
      {
        m_SplitResult.mf_Get( i )->mp_Set( k, j, ac_InputStr( i, mc_nDownFactor * j + nDMin - k ) );
      }
    }
  }

    //apply split anti-alias filter to 4 parts
  for( unsigned j = 0 ; j < mc_nChan ; ++j )
  {
    for( unsigned i = 0 ; i < mc_nDownFactor ; ++i )
    {
      nspdbFir( &m_FirDownStates( j, i ), m_SplitResult.mf_Get( j )->mf_pGetArray()[ i ], m_FirResult.mf_Get( j )->mf_pGetArray()[ i ], nToDo );
    }
  }

    //sum 4 parts::downsampling
  for( unsigned i = 0 ; i < mc_nChan ; ++i )
  {
    for( unsigned j = 0 ; j < nToDo ; ++j )
    {
      double dSum = 0;
      for( unsigned k = 0 ; k < mc_nDownFactor ; ++k )
      {
        dSum += m_FirResult.mf_Get( i )->mf_Get( k, j );
      }
      m_DownResult.mp_Set( i, j, dSum );
    }
  }

  return m_DownResult;
}

NSPResamplerNorm::NSPResamplerNorm( const unsigned ac_nChan,
                                    const unsigned ac_nBufSize,
                                    const unsigned ac_nDownFactor,
                                    const unsigned ac_nFirTaps ) :
    IResampler( ac_nChan, ac_nBufSize, ac_nDownFactor, ac_nFirTaps ),
  m_DownResult( ac_nChan, ac_nBufSize / ac_nDownFactor, true  ),
  m_AntiAlias ( new NspFirFilter( ac_nChan, ac_nBufSize, ac_nFirTaps ) )
{
  assert( ac_nBufSize % ac_nDownFactor == 0 );
  m_fInitResampler( ac_nFirTaps );
}

NSPResamplerNorm::~NSPResamplerNorm()
{
  delete m_AntiAlias;
}

void NSPResamplerNorm::m_fInitResampler( const unsigned ac_nTaps )
{
  CFirTapsTool myTool( IResampler::mc_nDownFactor, ac_nTaps );
  const ArrayAccess<double>& c_aTaps = myTool.mf_dfDesign_Aaf( );

  PtrStream AllTaps( ac_nTaps, IResampler::mc_nChan );
  for( unsigned i = 0 ; i < IResampler::mc_nChan ; ++i )
    AllTaps.SetChannel( i, c_aTaps.mf_pGetArray() );

  m_AntiAlias->mp_SetFilterTaps( AllTaps.Read() );
}

const Stream& NSPResamplerNorm::mf_DownSample( const Stream &ac_InputStr )
{
  assert( ac_InputStr.mf_nGetChannelCount() == IResampler::mc_nChan );
  assert( ac_InputStr.mf_nGetBufferSize() == IResampler::mc_nSize );

  const Stream& cFiltered = m_AntiAlias->mf_DoProcessing( ac_InputStr );
  const unsigned nIters = IResampler::mf_nGetOutputBufferSize();

   //downsample
  for( unsigned i = 0 ; i < mc_nChan ; ++i )
  {
    for( unsigned j = 0 ; j < nIters ; ++j )
    {
      m_DownResult.mp_Set( i, j, cFiltered( i, j * mc_nDownFactor ) );
    }
  }

  return m_DownResult;
}
