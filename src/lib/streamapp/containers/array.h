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
 
#ifndef __STRARRAY_H__
#define __STRARRAY_H__

#include "defines.h"
#include <string.h>
#include <assert.h>

namespace streamapp
{

    /**
      * ArrayAccess
      *   encapsulates access to a fixed-size array of primitive types.
      *   ArrayAccess wraps an array into a class, extends the functionality,
      *   and contains bound-checking when compiled in debug mode.
      *   @see MatrixAccess for a 2D implementation
      *********************************************************************** */
  template<class tType>
  class ArrayAccess
  {
  public:
      /**
        * Constructor.
        * @param ac_pBuffer pointer to the memory to access
        * @param ac_nBufferSize the number of tTypes pointed to
        */
    ArrayAccess(  tType  * const ac_pBuffer,
                  const unsigned ac_nBufferSize ) :
      mc_pArray       ( ac_pBuffer ),
      mc_nBufferSize  ( ac_nBufferSize )
    {
    }

      /**
        * Copy Constructor.
        * @param ac_Rh the object to copy from
        */
    ArrayAccess( const ArrayAccess& ac_Rh ) :
      mc_pArray( ac_Rh.mf_pGetArray() ),
      mc_nBufferSize( ac_Rh.mf_nGetBufferSize() )
    {
    }

      /**
        * Assignment Operator.
        * Copies values only, not the array pointer.
        * @param ac_prh the other object
        * @return reference to this
        */
    ArrayAccess& operator= ( const ArrayAccess& ac_prh )
    {
      if( this == &ac_prh )
        return *this;
      mp_CopyFrom( ac_prh );
      return *this;
    }

      /**
        * Destructor.
        * Never deletes anything.
        */
    virtual ~ArrayAccess()
    {
    }

      /**
        * Set a value.
        * @param ac_nPos the index
        * @param ac_dfVal the new value
        */
    INLINE void  mp_Set( const unsigned  ac_nPos,
                         const tType     ac_dfVal )
    {
      assert( ac_nPos < mc_nBufferSize );
      mc_pArray[ ac_nPos ] = ac_dfVal;
    }

      /**
        * Get an item.
        * @param ac_nPos the index
        * @return the item
        */
    INLINE tType mf_Get( const unsigned ac_nPos ) const
    {
      assert( ac_nPos < mc_nBufferSize );
      return mc_pArray[ ac_nPos ];
    }

      /**
        * Get a const reference to an item.
        * @param ac_nPos the index
        * @return the item
        */
    INLINE const tType& operator()( const unsigned ac_nPos ) const
    {
      assert( ac_nPos < mc_nBufferSize );
      return mc_pArray[ ac_nPos ];
    }

      /**
        * Get a reference to an item.
        * @param ac_nPos the index
        * @return the item
        */
    INLINE tType& operator()( const unsigned ac_nPos )
    {
      assert( ac_nPos < mc_nBufferSize );
      return mc_pArray[ ac_nPos ];
    }

      /**
        * Get the number of items.
        * @return
        */
    INLINE const unsigned& mf_nGetBufferSize() const
    {
      return mc_nBufferSize;
    }

      /**
        * Copy all items from another ArrayAccess object.
        * @param ac_ToCopy the other object
        */
    INLINE void mp_CopyFrom( const ArrayAccess& ac_ToCopy )
    {
      assert( ac_ToCopy.mf_nGetBufferSize() >= mc_nBufferSize );
      for( unsigned i = 0 ; i < mc_nBufferSize ; ++ i )
        mc_pArray[ i ] = ac_ToCopy( i );
    }

      /**
        * Returns the array as tType* to pass onto third party libs.
        * Use extreme care when using this method since you have no control over the access anymore.
        * @return the array pointer
        */
    INLINE tType* mf_pGetArray( ) const
    {
      return mc_pArray;
    }

  protected :
    tType* const mc_pArray;

  private:
    const unsigned mc_nBufferSize;
  };

    /**
      * Allocate array memory for ArrayStorage.
      * @param ac_nBufferSize the number of tTypes
      * @param ac_bInitMem if true, clears the memory (sets to 0)
      * @return pointer to the allocated memory.
      * @see ArrayStorage
      * TODO use a function pointer so we can use our own heap manager
      * TODO also redefine new and delete
      */
  template<class tType>
  tType* f_InitArrayMem( const unsigned ac_nBufferSize,
                         const bool     ac_bInitMem    )
  {
    if( ac_nBufferSize != 0 )
    {
      tType* const c_pArray = new tType[ ac_nBufferSize ];

      if( ac_bInitMem  )
        memset( c_pArray, 0, sizeof(tType) * ac_nBufferSize );

      return c_pArray;
    }
    return 0;
  }

    /**
      * ArrayStorage
      *   ArrayAccess implementation providing memory allocation for the array.
      *   @see ArrayAccess
      ************************************************************************* */
  template<class tType>
  class ArrayStorage : public ArrayAccess<tType>
  {
  public:
      /**
        * Constructor.
        * @param ac_nBufferSize the number of tTypes to allocate
        * @param ac_bInitMem if true, sets all memory to 0
        */
    ArrayStorage( const unsigned ac_nBufferSize,
                  const bool     ac_bInitMem = false ) :
      ArrayAccess<tType>(  f_InitArrayMem<tType>( ac_nBufferSize, ac_bInitMem ), ac_nBufferSize  )
    {
    }

      /**
        * Copy Constructor.
        * @param ac_Rh the object to copy from
        */
    ArrayStorage( const ArrayStorage& ac_Rh ) :
      ArrayAccess<tType>( f_InitArrayMem<tType>( ac_Rh.mf_nGetBufferSize(), false ), ac_Rh.mf_nGetBufferSize() )
    {
      ArrayAccess<tType>::mp_CopyFrom( ac_Rh );
    }

      /**
        * Destructor.
        * Deletes the allocated memory.
        */
    ~ArrayStorage()
    {
      delete [] ArrayAccess<tType>::mc_pArray;
    }

  private:
    ArrayStorage& operator = ( const ArrayStorage& );
  };


    /**
      * DeleteAtDestructionArray
      *   array of pointers which are deleted at destruction.
      *   Use this for arrays of pointers if you want mem to be cleaned up automatically.
      *********************************************************************************** */
  template<class tType>
  class DeleteAtDestructionArray : public ArrayStorage<tType*>
  {
  public:
      /**
        * DeleteAtDestructionArray
        * @param ac_nItems the number of pointers to allocate
        */
    DeleteAtDestructionArray( const unsigned ac_nItems ) :
      ArrayStorage<tType*>( ac_nItems, true )
    {
    }

      /**
        * Destructor.
        * Deletes all memory pointed to by the tType* pointers
        */
    ~DeleteAtDestructionArray()
    {
      const unsigned nSize = ArrayStorage<tType*>::mf_nGetBufferSize();
      for( unsigned i = 0 ; i < nSize ; ++i )
        delete ArrayStorage<tType*>::mf_Get( i );
    }
  };

    /**
      * Find the index of a tType in an ArrayAccess.
      * @param ac_pToFind the item to search for
      * @param ac_ToSearch the array to search
      * @param ac_nIndexToStartSearch the index to start searching at, use to improve speed
      * @return the index, or -1 if the item wasn't found
      */
  template<class tType>
  int sf_nFindInArray( const tType* ac_pToFind, const ArrayAccess<tType*>& ac_ToSearch, const int ac_nIndexToStartSearch = 0 )
  {
    const int nChan = (int) ac_ToSearch.mf_nGetBufferSize();
    for( int i = ac_nIndexToStartSearch ; i < nChan ; ++i )
    {
      if( ac_pToFind == ac_ToSearch( i ) )
        return i;
    }
    return -1;
  }

}

#endif //#ifndef __STRARRAY_H__
