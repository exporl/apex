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
 
#include "resamplers/dspresampler.h"

namespace streamapp 
{

  DSPResampler::DSPResampler( const unsigned ac_nChan, 
                              const unsigned ac_nBufSize, 
                              const unsigned ac_nDownFactor,
                              const unsigned ac_nFirTaps ) :    
      CResampler( ac_nChan, ac_nBufSize, ac_nDownFactor, ac_nFirTaps ),
    test( ac_nChan, ac_nBufSize, true ),
    test2( ac_nChan, ac_nBufSize, true ),
    test3( ac_nChan, ac_nBufSize, true )
  {
    int error;
    data = new SRC_DATA();
    m_pState = src_new( SRC_SINC_BEST_QUALITY, 1, &error );
    if( !m_pState )
      src_strerror( error );
  }

  DSPResampler::~DSPResampler( )
  {
    src_delete( m_pState );
  } 

  CChanStrDouble& DSPResampler::mf_UpSample( const CChanStrDouble& ac_InputStr )
  {
    for( unsigned i = 0 ; i < 1 ; ++i ) //limit to mono
    {
      for( unsigned j = 0 ; j < mc_nSize / 2 ; ++j )
      {
        test.mp_SetSample( i, j, (float) ac_InputStr( i, j ) );
      }
    }

    data->data_in = test.mf_pGetBuffers()[ 0 ];
    data->data_out = test2.mf_pGetBuffers()[ 0 ];
    data->src_ratio = 2;
    data->input_frames = mc_nSize / 2;
    data->output_frames = mc_nSize;

    int err = src_process( m_pState, data );
    if( err )
      src_strerror( err );

    if( data->output_frames_gen == mc_nSize )
    {
      for( unsigned i = 0 ; i < 1 ; ++i ) //limit to mono
      {
        for( unsigned j = 0 ; j < mc_nSize ; ++j )
        {
          test3.mp_SetSample( i, j, (double) test2( i, j ) );
        }
      }
    }
    else
    {
      memset( test3.mf_pGetBuffers()[ 0 ], 0, sizeof( double ) * ((mc_nSize)-data->output_frames_gen) );
      for( unsigned i = 0 ; i < 1 ; ++i ) //limit to mono
      {
        for( unsigned j = ((mc_nSize)-data->output_frames_gen) ; j < mc_nSize ; ++j )
        {
          test3.mp_SetSample( i, j, (double) test2( i, j - ((mc_nSize)-data->output_frames_gen) ) );
        }
      }
    }

    return (CChanStrDouble&) test3;
  }

  CChanStrDouble& DSPResampler::mf_DownSample( const CChanStrDouble& ac_InputStr )
  {
    for( unsigned i = 0 ; i < 1 ; ++i ) //limit to mono
    {
      for( unsigned j = 0 ; j < mc_nSize ; ++j )
      {
        test.mp_SetSample( i, j, (float) ac_InputStr( i, j ) );
      }
    }

    data->data_in = test.mf_pGetBuffers()[ 0 ];
    data->data_out = test2.mf_pGetBuffers()[ 0 ];
    data->src_ratio = 0.5;
    data->input_frames = mc_nSize;
    data->output_frames = mc_nSize / 2;

    int err = src_process( m_pState, data );
    if( err )
      src_strerror( err );

    if( data->output_frames_gen == mc_nSize / 2 )
    {
      for( unsigned i = 0 ; i < 1 ; ++i ) //limit to mono
      {
        for( unsigned j = 0 ; j < mc_nSize / 2 ; ++j )
        {
          test3.mp_SetSample( i, j, (double) test2( i, j ) );
        }
      }
    }
    else
    {
      memset( test3.mf_pGetBuffers()[ 0 ], 0, sizeof( double ) * ((mc_nSize/2)-data->output_frames_gen) );
      for( unsigned i = 0 ; i < 1 ; ++i ) //limit to mono
      {
        for( unsigned j = ((mc_nSize/2)-data->output_frames_gen) ; j < mc_nSize / 2 ; ++j )
        {
          test3.mp_SetSample( i, j, (double) test2( i, j - ((mc_nSize/2)-data->output_frames_gen) ) );
        }
      }
    }

    return (CChanStrDouble&) test3;
  }

}

