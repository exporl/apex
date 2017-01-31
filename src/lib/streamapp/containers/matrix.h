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

#ifndef __STRMATRIX_H__
#define __STRMATRIX_H__

#include "array.h"

#include <QtGlobal>

namespace streamapp
{
    /**
      * MatrixAccess
      *   encapsulates access to a two-dimensional array of tTypes.
      *   is just like an array, but contains bound-checking when compiled
      *   in debug mode. This is an invalueable tool when creating an application,
      *   it is way to easy to index something the wrong way, which leads to
      *   exploring memory that's not yours, aka crashing behaviour.
      *   Compiled in release mode, the code should be as fast as a normal
      *   array access, at least when your compiler supports inilining of functions.
      *   @see ArrayAccess
      ****************************************************************************** */
  template<class tType>
  class MatrixAccess
  {
  public:
      /**
        * Default Constructor.
        * Use only if you know what you're doing, since
        * it initializes all members to 0.
        */
    MatrixAccess() :
        //Noisy( "ChannelSamplesAccess" ),
      mc_pSamplesArray( 0 ),
      mc_nChannelCount( 0 ),
      mc_nBufferSize  ( 0 )
    {
    }

      /**
        * Constructor.
        * @param ac_pBuffer pointer to the memory to access
        * @param ac_nChannelCount the number of channels (==rows) pointed to
        * @param ac_nBufferSize the number of tTypes pointed to in each channel
        */
    MatrixAccess( tType  ** const   ac_pBuffer,
                  const unsigned    ac_nChannelCount,
                  const unsigned    ac_nBufferSize ) :
        //Noisy( "ChannelSamplesAccess" ),
      mc_pSamplesArray( ac_pBuffer ),
      mc_nChannelCount( ac_nChannelCount ),
      mc_nBufferSize  ( ac_nBufferSize )
    {
    }

      /**
        * Copy Constructor.
        * Make sure not to delete the memory pointed to by ac_ToCopy
        * for the lifetime of this object.
        * @param ac_ToCopy the object to copy
        */
    MatrixAccess( const MatrixAccess& ac_ToCopy ) :
        //Noisy( "ChannelSamplesAccess" ),
      mc_pSamplesArray( ac_ToCopy.mf_pGetArray() ),
      mc_nChannelCount( ac_ToCopy.mf_nGetChannelCount() ),
      mc_nBufferSize  ( ac_ToCopy.mf_nGetBufferSize() )
    {
    }

      /**
        * Assignment Operator.
        * Copies all values.
        * Make sure not to assign to an empty MatrixAccess.
        * @param ac_ToCopy the object to copy
        */
    MatrixAccess& operator = ( const MatrixAccess& ac_ToCopy )
    {
      if( this == &ac_ToCopy )
        return *this;
      Q_ASSERT( mc_pSamplesArray );
      mp_CopyFrom( ac_ToCopy );
      return *this;
    }

      /**
        * Destructor.
        * Never deletes anything.
        */
    virtual ~MatrixAccess()
    {
    }

      /**
        * Set a value
        * @param ac_nChannel the row index
        * @param ac_nPos the column index
        * @param ac_dfVal the new value
        */
    INLINE void  mp_Set(  const unsigned  ac_nChannel,
                          const unsigned  ac_nPos,
                          const tType     ac_dfVal )
    {
      Q_ASSERT( ac_nChannel < mc_nChannelCount );
      Q_ASSERT( ac_nPos     < mc_nBufferSize   );

      mc_pSamplesArray[ac_nChannel][ac_nPos] = ac_dfVal;
    }

      /**
        * Get an item.
        * @param ac_nChannel the row index
        * @param ac_nPos the column index
        * @return the value
        * @see mf_Get()
        */
    INLINE tType mf_Get( const unsigned  ac_nChannel, const unsigned  ac_nPos )
    {
      Q_ASSERT( ac_nChannel < mc_nChannelCount );
      Q_ASSERT( ac_nPos     < mc_nBufferSize   );
      return mc_pSamplesArray[ac_nChannel][ac_nPos];
    }

      /**
        * Get a const reference to an item.
        * @param ac_nChannel the row index
        * @param ac_nPos the column index
        * @return the value
        */
    INLINE const tType& operator() ( const unsigned  ac_nChannel, const unsigned  ac_nPos ) const
    {
      Q_ASSERT( ac_nChannel < mc_nChannelCount );
      Q_ASSERT( ac_nPos     < mc_nBufferSize   );
      return mc_pSamplesArray[ac_nChannel][ac_nPos];
    }

      /**
        * Get a reference to an item.
        * @code
        * tType& item = matrix( 0, 0 );
        * item = value;
        * @endcode
        * @param ac_nChannel the row index
        * @param ac_nPos the column index
        * @return the value
        */
    INLINE tType& operator()( const unsigned ac_nChannel, const unsigned ac_nPos )
    {
      Q_ASSERT( ac_nChannel < mc_nChannelCount );
      Q_ASSERT( ac_nPos     < mc_nBufferSize   );
      return mc_pSamplesArray[ac_nChannel][ac_nPos];
    }

      /**
        * Get the number of items per row.
        * @return the number
        */
    INLINE const unsigned& mf_nGetBufferSize() const
    {
      return mc_nBufferSize;
    }

      /**
        * Get the number of channels (aka rows).
        * @return
        */
    INLINE const unsigned& mf_nGetChannelCount() const
    {
      return mc_nChannelCount;
    }

      /**
        * Copy all items from another MatrixAccess.
        * The array to copy from must be at least of equal size.
        * @param ac_ToCopyFrom the object to copy from
        */
    INLINE void mp_CopyFrom( const MatrixAccess<tType>& ac_ToCopyFrom )
    {
      Q_ASSERT( ac_ToCopyFrom.mf_nGetChannelCount() >= mc_nChannelCount );
      Q_ASSERT( ac_ToCopyFrom.mf_nGetBufferSize() >= mc_nBufferSize );
      tType** src = ac_ToCopyFrom.mf_pGetArray();
      for( unsigned i = 0 ; i < mc_nChannelCount ; ++i )
        for( unsigned j = 0 ; j < mc_nBufferSize ; ++j )
          mc_pSamplesArray[ i ][ j ] = src[ i ][ j ];
    }

      /**
        * Copy some items from another MatrixAccess.
        * @param ac_ToCopyFrom the object to copy from
        * @param ac_nSourceStart the offset in ac_ToCopyFrom to copy from
        * @param ac_nThisStart the offset in this to start at
        * @param ac_nSamples the number of samples to copy
        */
    INLINE void mp_CopyFrom( const MatrixAccess<tType>& ac_ToCopyFrom, const unsigned ac_nSourceStart, const unsigned ac_nThisStart, const unsigned ac_nSamples )
    {
      Q_ASSERT( ac_ToCopyFrom.mf_nGetChannelCount() >= mc_nChannelCount );
      Q_ASSERT( ac_ToCopyFrom.mf_nGetBufferSize() >= ac_nSourceStart + ac_nSamples );
      Q_ASSERT( mc_nBufferSize >= ac_nThisStart + ac_nSamples );
      tType** src = ac_ToCopyFrom.mf_pGetArray();
      for( unsigned i = 0 ; i < mc_nChannelCount ; ++i )
        for( unsigned j = 0 ; j < ac_nSamples ; ++j )
          mc_pSamplesArray[ i ][ j + ac_nThisStart ] = src[ i ][ j + ac_nSourceStart ];
    }

      /**
        * Fill the memory with the specified value.
        * @param ac_tVal the new value
        */
    INLINE void mp_Clear( const int ac_tVal = 0 )
    {
      for( unsigned i = 0 ; i < mf_nGetChannelCount() ; ++i )
        memset( mf_pGetArray()[ i ], ac_tVal, mf_nGetBufferSize() * sizeof( tType ) );
    }

      /**
        * Fill the memory with the specified value.
        * @param ac_tVal the new value
        */
    INLINE void mp_ClearRegion( const unsigned ac_nStart, const unsigned ac_nLength, const int ac_tVal = 0 )
    {
      Q_ASSERT( ac_nLength + ac_nStart <= mc_nBufferSize );
      for( unsigned i = 0 ; i < mf_nGetChannelCount() ; ++i )
        memset( mf_pGetArray()[ i ] + ac_nStart, ac_tVal, ac_nLength * sizeof( tType ) );
    }

      /**
        * Fill the memory of one row with the specified value.
        * @param ac_nChannel the row index
        * @param ac_tVal the new value
        */
    INLINE void mp_Clear( const unsigned ac_nChannel, const int ac_tVal = 0 )
    {
      Q_ASSERT( ac_nChannel < mf_nGetChannelCount() );
      memset( mf_pGetArray()[ ac_nChannel ], ac_tVal, mf_nGetBufferSize() * sizeof( tType ) );
    }

      /**
        * Set all items to the same value.
        * @param ac_tValue the new value
        */
    void mp_SetAll( const tType ac_tValue )
    {
      for( unsigned i = 0 ; i < mc_nChannelCount ; ++i )
        for( unsigned j = 0 ; j < mc_nBufferSize ; ++j )
          mc_pSamplesArray[ i ][ j ] = ac_tValue;
    }

      /**
        * Set all items of one channel to the same value.
        * @param ac_tValue the new value
        */
    void mp_SetAll( const unsigned ac_nChannel, const tType ac_tValue )
    {
      Q_ASSERT( ac_tValue < mc_nChannelCount );
      for( unsigned j = 0 ; j < mc_nBufferSize ; ++j )
        mc_pSamplesArray[ ac_nChannel ][ j ] = ac_tValue;
    }

      /**
        * Returns the array as tType* to pass onto third party libs.
        * Use extreme care when using this method since you have no control over the access anymore.
        * @return the array pointer
        */
    INLINE tType** mf_pGetArray() const
    {
      return mc_pSamplesArray;
    }

      /**
        * Get an ArrayAccess to a single channel.
        * @param ac_nChannel the channel
        * @return an ArrayAccess for mf_pGetArray()[ ac_nChannel ] with mf_nGetBufferSize() length
        * @note no INLINE statement since compilers normally cannot inline the call because of returning a stack object
        */
    ArrayAccess<tType> mf_pGetChannel( const unsigned ac_nChannel ) const
    {
      Q_ASSERT( ac_nChannel < mc_nChannelCount );
      return ArrayAccess<tType>( mc_pSamplesArray[ ac_nChannel ], mf_nGetBufferSize() );
    }

      /**
        * Sets the specified channel to point to other memory.
        * Use only if you know what you're doing, this changes the memory pointed to.
        * @param ac_nChannel the row index
        * @param ac_Channel the ArrayAccess to point to
        */
    INLINE void mf_pSetChannel( const unsigned  ac_nChannel, const ArrayAccess<tType>& ac_Channel ) const
    {
      Q_ASSERT( ac_nChannel < mc_nChannelCount );
      mc_pSamplesArray[ ac_nChannel ] = ac_Channel.mf_pGetArray();
    }

  protected :
    tType** const mc_pSamplesArray;

  private:
    const unsigned mc_nChannelCount;
    const unsigned mc_nBufferSize;
  };

    /**
      * Allocate array memory.
      * @param ac_nChannelCount the number of rows
      * @param ac_nBufferSize the number of tTypes per row
      * @param ac_bInitMem if true, clears the memory (sets to 0)
      * @return pointer to the allocated memory.
      */
  template<class tType>
  tType** f_InitMem(  const unsigned ac_nChannelCount,
                      const unsigned ac_nBufferSize,
                      const bool     ac_bInitMem )
  {
    tType  ** const c_pArray = new tType*[ac_nChannelCount];

    for( unsigned i = 0 ; i < ac_nChannelCount ; ++i )
    {
      c_pArray[i] = new tType[ac_nBufferSize];
      if( ac_bInitMem  )
        memset( c_pArray[i], 0, sizeof(tType)*ac_nBufferSize );
    }

    return c_pArray;
  }

    /**
      * MatrixStorage
      *   MatrixAccess implementation providing memory allocation for the array.
      *   @see MatrixAccess
      ************************************************************************** */
  template<class tType>
  class MatrixStorage : public MatrixAccess<tType>//, public Noisy
  {
  public:
      /**
        * Constructor.
        * Allocates memory for the MatrixAccess.
        * @param ac_nChannelCount the number of rows
        * @param ac_nBufferSize the number of columns
        * @param ac_bInitMem if true, sets all values to zero
        */
    MatrixStorage(  const unsigned ac_nChannelCount,
                    const unsigned ac_nBufferSize,
                    const bool     ac_bInitMem = false ) :
        MatrixAccess<tType>( f_InitMem<tType>( ac_nChannelCount, ac_nBufferSize, ac_bInitMem ), ac_nChannelCount, ac_nBufferSize )
      //, Noisy( "Buffer : " + toString( ac_nBufferSize ) + ", " + toString( ac_nChannelCount ) )
    {
    }


        /**
          * Copy Constructor.
          * Allocates required memory and copies all values to it.
          * @param ac_Rh the object to copy all samples from.
          */
    MatrixStorage( const MatrixStorage& ac_Rh ) :
      MatrixAccess<tType>(  f_InitMem<tType>( ac_Rh.mf_nGetChannelCount(), ac_Rh.mf_nGetBufferSize(), false ),
                            ac_Rh.mf_nGetChannelCount(),
                            ac_Rh.mf_nGetBufferSize() )
      //,Noisy( "Buffer : " + toString( ac_Rh.mf_nGetBufferSize() ) + ", " + toString( ac_Rh.mf_nGetChannelCount() ) )
    {
      MatrixAccess<tType>::mp_CopyFrom( ac_Rh );
    }

      /**
        * Destructor.
        * Deletes allocated memory
        */
    ~MatrixStorage()
    {
      for( unsigned i = 0 ; i < MatrixAccess<tType>::mf_nGetChannelCount() ; ++i )
        delete [] MatrixAccess<tType>::mc_pSamplesArray[ i ];
      delete [] MatrixAccess<tType>::mc_pSamplesArray;
    }

  private:
    MatrixStorage& operator = ( const MatrixStorage& );
  };


    /**
      * Allocates pointer memory for ExternalMatrixAccess.
      * @param ac_nChannels the number of pointers
      * @return pointer to the allocated memory
      * @see ExternalMatrixAccess
      */
  template<class tType>
  tType** f_InitPtrMem( const unsigned ac_nChannels )
  {
    tType** pRet = new tType*[ ac_nChannels ];
    return pRet;
  }

    /**
      * ExternalMatrixAccess
      *   use this for working with MatrixAccess when memory for just the pointers is needed.
      *   This can be used when the memory itself (the row memory) is elsewhere,
      *   and when the array will be setup by mf_pSetChannel().
      *   Use only if you know what you're doing!!
      *************************************************************************************** */
  template<class tType>
  class ExternalMatrixAccess : public MatrixAccess<tType>
  {
  public:
      /**
        * Constructor.
        * Allocates memory for ac_nChannels pointers.
        * @param ac_nChannels the number of rows
        * @param ac_nSamples the number of columns
        */
    ExternalMatrixAccess( const unsigned ac_nChannels, const unsigned ac_nSamples ) :
      MatrixAccess<tType>( f_InitPtrMem<tType>( ac_nChannels ), ac_nChannels, ac_nSamples )
    {
    }

      /**
        * Destructor.
        */
    ~ExternalMatrixAccess()
    {
      for( unsigned i = 0 ; i < MatrixAccess<tType>::mf_nGetChannelCount() ; ++i )
        MatrixAccess<tType>::mc_pSamplesArray[ i ] = 0;
      delete [] MatrixAccess<tType>::mc_pSamplesArray;
    }

  private:
    ExternalMatrixAccess( const ExternalMatrixAccess& );
    ExternalMatrixAccess& operator = ( const ExternalMatrixAccess& );
  };

    /**
      * Swap all the row data with data from the row next to it.
      * Only works for matrices with an even number of rows.
      * @param ac_toReverse the arry to act on
      */
  template<class tType>
  void fSwapTwoByTwo( MatrixAccess<tType>& ac_toReverse )
  {
    const unsigned nChan = ac_toReverse.mf_nGetChannelCount();
    Q_ASSERT( nChan % 2 == 0 ); //even for simplicity
    for( unsigned i = 0 ; i < nChan ; i = i + 2 )
    {
      ArrayAccess<tType> temp = ac_toReverse.mf_pGetChannel( i );
      ac_toReverse.mf_pSetChannel( i, ac_toReverse.mf_pGetChannel( i + 1 ) );
      ac_toReverse.mf_pSetChannel( i + 1, temp );
    }
  }

}

#endif //#ifndef __STRMATRIX_H__
