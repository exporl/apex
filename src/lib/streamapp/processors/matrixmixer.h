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

#ifndef __MMIXER_H__
#define __MMIXER_H__

#include "stream.h"
#include "typedefs.h"
#include "streamappdefines.h"
#include "utils/dataconversion.h"

using namespace dataconversion;

namespace streamapp
{

  namespace
  {

      /**
        * Manages memory for MatrixMixer's result buffer.
        */
    INLINE const StreamBuf* f_ManageMem(  const StreamBuf* a_pTarget,
                                          const unsigned  ac_nChan,
                                          const unsigned  ac_nSize    )
    {
      if( !a_pTarget )                                          //first time new
        return( new StreamBuf( ac_nChan , ac_nSize , true ) );
      else
      {
        if( a_pTarget->mf_nGetChannelCount() != ac_nChan )      //new if different nChan
        {
          delete a_pTarget;
          return( new StreamBuf( ac_nChan , ac_nSize , true ) );
        }
        else                                                    //zeroize if ok
        {
          for( unsigned i = 0 ; i < ac_nChan ; ++i )
          {
            memset( a_pTarget->mf_pGetArray()[ i ], 0 , ac_nSize * sizeof( GainType ) );
          }
          return a_pTarget;
        }
      }
    }

  }

      /**
        * MatrixMixer
        *   software implementation of a matrixmixer.
        *   The mixer can mix any inputchannel to any output channel.
        *   Eg to mix input 0 to both outputs 1 and 3 at -3dB:
        *   @code
        *   mp_SetMatrixNode( 0, 1, -3.0 );
        *   mp_SetMatrixNode( 0, 3, -3.0 );
        *   @endcode
        *
        *   The mixer contains some optimizations, eg it doesn't calculate
        *   nodes that are set to -150dB or lower and it doesn't call the
        *   multiply function if gain is 0dB.
        *
        *   TODO also derive from IMatrixMixer
        *
        *   After construction, the nodes are initialized this way:
            @verbatim
             Out  0   1   2  ... 63
           In

            0     1   0   0   0   0

            1     0   1   0   0   0

            2     0   0   1   0   0

            ...   0   0   0   1   0

            63    0   0   0   0   1
            @endverbatim
        *
        ****************************************************************** */
  class MatrixMixer : public IStreamProcessor
  {
  public:
      /**
        * Constructor.
        * Initializes all nodes.
        * @param ac_nBufSize the buffersize that will be used
        * @param ac_nChan the maximum number of channels to use.
        */
    MatrixMixer( const unsigned ac_nBufSize,
                 const unsigned ac_nChan = 64 ) :
      m_Matrix ( ac_nChan, ac_nChan ),
      m_pResult( 0 ),
      m_nIChan ( 0 ),
      m_nOChan ( 0 ),
      mc_nSize( ac_nBufSize )
    {
    }

      /**
        * Destructor.
        */
    virtual ~MatrixMixer()
    {
      delete m_pResult;
    }

      /**
        * Set a single node.
        * @param ac_dGaindB the gain in dB.
        * @param ac_nIChan the row
        * @param ac_nIChan the column
        */
    void mp_SetMatrixNode( const unsigned ac_nIChan , const unsigned ac_nOChan , const GainType ac_dGaindB )
    {
      m_Matrix.mp_Set( ac_nIChan , ac_nOChan , gf_dBtoLinear( ac_dGaindB ) );
      if( ac_nIChan >= m_nIChan )
        m_nIChan = ac_nIChan + 1;
      if( ac_nOChan >= m_nOChan )
        m_nOChan = ac_nOChan + 1;
    }

      /**
        * Set an entire row to one value.
        * @param ac_nRow the row to set
        * @param ac_dGainInDb the gain in dB
        */
    void mp_SetMatrixRow( const unsigned ac_nRow, const GainType ac_dGainInDb )
    {
      for( unsigned i = 0 ; i < m_nOChan ; ++i )
        mp_SetMatrixNode( ac_nRow, i, ac_dGainInDb );
    }

      /**
        * Set every node to the same value.
        * @param ac_dGaindB the gain in dB.
        */
    void mp_SetAllNodes( const GainType ac_dGaindB )
    {
      const GainType c_dLin = gf_dBtoLinear( ac_dGaindB );
      m_Matrix.mp_SetAll( c_dLin );
    }

      /**
        * Set the number of output channels.
        * If set to 0, the maximum set in the constructor is used.
        * @param ac_nChan the number.
        */
    void mp_SetNumOuts( const unsigned ac_nChan )
    {
      assert( ac_nChan <= m_Matrix.mf_nGetChannelCount() );
      m_nOChan = ac_nChan;
    }

      /**
        * Set the number of input channels.
        * @param ac_nChan the number.
        * @see mp_SetNumOuts()
        */
    void mp_SetNumIns( const unsigned ac_nChan )
    {
      assert( ac_nChan <= m_Matrix.mf_nGetChannelCount() );
      m_nIChan = ac_nChan;
    }

      /**
        * Get a node's value.
        * @param ac_nIChan the input channel
        * @param ac_nOChan the output channel
        * @return the value on node ( ac_nIChan, ac_nOChan )
        */
    GainType mf_dGetMatrixNode( const unsigned ac_nIChan , const unsigned ac_nOChan ) const
    {
      return gf_LinearTodB( m_Matrix( ac_nIChan , ac_nOChan ) );
    }

      /**
        * Copy values from another mixer.
        * @param ac_MatrixToCopy the other mixer
        */
    void mp_CopyMatrixFrom( const MatrixMixer& ac_MatrixToCopy )
    {
      const unsigned nIChanToCopy = s_min( ac_MatrixToCopy.mf_nGetNumInputChannels(), m_nOChan );
      const unsigned nOChanToCopy = s_min( ac_MatrixToCopy.mf_nGetNumOutputChannels(), m_nIChan );
      for( unsigned i = 0 ; i < nIChanToCopy ; ++i )
        for( unsigned j = 0 ; j < nOChanToCopy ; ++j )
          mp_SetMatrixNode( i, j, ac_MatrixToCopy.mf_dGetMatrixNode( i, j ) );
      m_nIChan = nIChanToCopy;
      m_nOChan = nOChanToCopy;
    }

      /**
        * Print the current matrix nodes to a vector of strings.
        * @return the vector of strings
        */
    tStringVector mf_PrintMatrix() const;

      /**
        * Implementation of the IStreamProcessor method.
        */
    INLINE unsigned mf_nGetBufferSize() const
    {
      return mc_nSize;
    }

      /**
        * Implementation of the IStreamProcessor method.
        * This returns the channel number set by other methods.
        * Eg after initializing, this returns 0, after mp_SetNumIns( 4 ),
        * this returns 4, and after mp_SetMatrixNode( 5, 0, 0.0 ) this
        * will return 5.
        */
    INLINE unsigned mf_nGetNumOutputChannels() const
    {
      return m_nOChan;
    }

      /**
        * Implementation of the IStreamProcessor method.
        * This returns the largest channel number set by mp_SetNumIns().
        * @see mf_nGetNumOutputChannels()
        */
    INLINE unsigned mf_nGetNumInputChannels() const
    {
      return m_nIChan;
    }

      /**
        * Implementation of the IStreamProcessor method.
        */
    const Stream& mf_DoProcessing( const Stream& ac_dInput )
    {
      const unsigned nIChan = s_min( ac_dInput.mf_nGetChannelCount(), m_nIChan );
      const unsigned nSize  = ac_dInput.mf_nGetBufferSize();
      const unsigned nOChan = m_nOChan;

      m_pResult = f_ManageMem( m_pResult, nOChan, nSize );
      StreamType** dst = m_pResult->mf_pGetArray();
      StreamType** src = ac_dInput.mf_pGetArray();

      for( unsigned i = 0 ; i < nOChan ; ++i )
      {
        for( unsigned k = 0 ; k < nIChan ; ++k )
        {
          const GainType dGain = m_Matrix( k , i );
          if( dGain == 1.0 )
          {
            for( unsigned j = 0 ; j < nSize ; ++j )
              dst[ i ][ j ] += src[ k ][ j ];
          }
          else if( dGain != 0.0 )
          {
            for( unsigned j = 0 ; j < nSize ; ++j )
              dst[ i ][ j ] += src[ k ][ j ] * dGain;
          }
        }
      }
      return *m_pResult;
    }

  private:
    MatrixStorage<GainType> m_Matrix;
    const StreamBuf*        m_pResult;
    unsigned                m_nIChan;
    unsigned                m_nOChan;
    const unsigned          mc_nSize;

    MatrixMixer( const MatrixMixer& );
    MatrixMixer& operator = ( const MatrixMixer& );
  };

}

#endif //#ifndef __MMIXER_H__
