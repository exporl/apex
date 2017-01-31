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

#ifndef __RAWMEMORYBUNDLE_H__
#define __RAWMEMORYBUNDLE_H__

#include "rawmemory.h"

#include <QtGlobal>

namespace streamapp
{

    /**
      *  RawMemoryBundler
      *   class for subsequent adding of objects to a RawMemory block.
      *   Every object is appended after the previous one.
      *   Example:
      *   @code
      *   bundler.mp_SetBundle( pMyMem );
      *   bundler.mp_Nundle<type1>( pType1 );
      *   bundler.mp_Nundle<type1>( pType2 );
      *   bundler.mp_Nundle<type1>( pType3 );
      *   @endcode
      *   The pMyMem now contains subsequent copies of all data added.
      *   Used for example to send large blocks of data over a network:
      *   it is faster to send one big block at once, and let the hardware
      *   decide how to split it, then sending multiple smaller blocks.
      ******************************************************************** */
  class RawMemoryBundler
  {
  public:
      /**
        * Constructor.
        */
    RawMemoryBundler();

      /**
        * Constructor Overload.
        * @param ac_pTargetBundle pointer to the RawMemory to use.
        * @see mp_SetBundle()
        */
    RawMemoryBundler( RawMemory* const ac_pTargetBundle );

      /**
        * Destructor.
        * Doesn't delete pointer to the memory set by mp_SetBundle().
        */
    ~RawMemoryBundler();

      /**
        * Set the RawMemory to use for bundling.
        * This always resets the write position.
        * The object will be resized using mp_ResizeIfSmaller() only,
        * so you can keep the number of allocations down to one if you put
        * the right size in on beforehand.
        * @param ac_pTargetBundle a RawMemory pointer, non-zero
        */
    void mp_SetBundle( RawMemory* const ac_pTargetBundle );

      /**
        * Add the next object to the memory.
        * This copies the object to the next write position,
        * then increases the offset with sizeof( tDataType ).
        * @param tDataType the data type to put into the bundle
        * @param a ac_pToBundle the data
        */
    template<class tDataType>
    void mp_Bundle( const tDataType* ac_pToBundle )
    {
      Q_ASSERT( m_pMem );
      const unsigned nSize = sizeof( tDataType );
      m_pMem->mp_ResizeIfSmaller( mv_nPos + nSize );
      m_pMem->mp_CopyFrom( ac_pToBundle, mv_nPos, nSize );
      mv_nPos += nSize;
    }

      /**
        * Add the next pointer to the memory with the given size.
        * Use this eg for arrays of primitives, or for classes
        * that have dynamically allocated memory.
        * Increases the offset with ac_nSize.
        * @param ac_pToBundle pointer to the data
        * @param ac_nSize number of bytes to append.
        */
    void mp_Bundle( const void* ac_pToBundle, const unsigned ac_nSize )
    {
      Q_ASSERT( m_pMem );
      m_pMem->mp_ResizeIfSmaller( mv_nPos + ac_nSize );
      m_pMem->mp_CopyFrom( ac_pToBundle, mv_nPos, ac_nSize );
      mv_nPos += ac_nSize;
    }

      /**
        * Get a RawMemoryAccess for the data bundled so far.
        * @return a RawMemoryAccess with size == mf_nGetWritePosition()
        */
    RawMemoryAccess mf_GetBundledData() const;

      /**
        * Set the next write position.
        * The next pointer will be formed by memory starting from ac_nByteOffset.
        * Note that this can create gaps, which can be the intent off course.
        * @param ac_nByteOffset the new position
        */
    void mp_SetWritePosition( const unsigned ac_nByteOffset );

      /**
        * Get the current write position.
        * This equals the total size of the memory used so far.
        * @return the position
        */
    INLINE const unsigned& mf_nGetWritePosition() const
    {
      return mv_nPos;
    }

  private:
    unsigned    mv_nPos;
    RawMemory*  m_pMem;
  };

    /**
      * RawMemoryUnBundler
      *   counterpart of RawMemoryBundler.
      *   Subsequently retrieves the requested types from a RawMemory block.
      *   The offset pointed to is updated after every mf_pUnBundle() call.
      *   Example:
      *   @code
      *   unBundler.mp_SetBundle( myMem );
      *   unBundler.mf_pUnBundle<int>;  //offset is now sizeof( int )
      *   unBundler.mf_pUnBundle<int>;  //offset is now 2 * sizeof( int )
      *   @endcode
      *   @note: if you use this to unbundle A while B was bundled,
      *   expect numerous funny things to happen.
      ********************************************************************** */
  class RawMemoryUnBundler
  {
  public:
      /**
        * Constructor.
        */
    RawMemoryUnBundler();

      /**
        * Constructor Overload.
        * @param ac_pBundle pointer to the RawMemoryAccess to use.
        * @see mp_SetBundle()
        */
    RawMemoryUnBundler( RawMemoryAccess* const ac_pBundle );

      /**
        * Destructor.
        * Doesn't delete pointer to the memory set by mp_SetBundle().
        */
    ~RawMemoryUnBundler();

      /**
        * Set the RawMemory to use for unbundling.
        * This always resets the read position.
        * @param ac_pBundle a RawMemoryAccess pointer (can be zero although this isn't of much interest)
        */
    void mp_SetBundle( const RawMemoryAccess* const ac_pBundle );

      /**
        * Get a casted pointer to the next object in the memory.
        * This returns a pointer to the current read offset,
        * then increases the offset with sizeof( tDataType ).
        * @param tDataType the data type to get from the bundle
        * @return a pointer, or 0 if out of range
        */
    template<class tDataType>
    tDataType* mf_pUnBundle() const
    {
      Q_ASSERT( mc_pMem );
      const unsigned nSize = sizeof( tDataType );
      Q_ASSERT( mv_nPos + nSize <= mc_pMem->mf_nGetSize() );
      tDataType* pRet = mc_pMem->mf_pGetMemory<tDataType>( mv_nPos );
      mv_nPos += nSize;
      return pRet;
    }

      /**
        * Get a casted pointer to the next object in the memory.
        * This returns a pointer to the current read offset,
        * then increases the offset with ac_nSize
        * @param tDataType the data type to get from the bundle
        * @param ac_nSize the size in bytes to get
        * @return a pointer, or 0 if out of range
        */
    template<class tDataType>
    tDataType* mf_pUnBundle( const unsigned ac_nSize ) const
    {
      Q_ASSERT( mc_pMem );
      Q_ASSERT( mv_nPos + ac_nSize <= mc_pMem->mf_nGetSize() );
      tDataType* pRet = mc_pMem->mf_pGetMemory<tDataType>( mv_nPos );
      mv_nPos += ac_nSize;
      return pRet;
    }

      /**
        * Set the next read position.
        * The next pointer will be formed by memory starting from ac_nByteOffset.
        * @param ac_nByteOffset the new position, must be in range.
        */
    void mp_SetReadPosition( const unsigned ac_nByteOffset );

      /**
        * Get the current read position.
        * @return the position
        */
    INLINE const unsigned& mf_nGetReadPosition() const
    {
      return mv_nPos;
    }

  private:
    mutable unsigned        mv_nPos;
    const RawMemoryAccess*  mc_pMem;
  };

}

#endif //#ifndef __RAWMEMORYBUNDLE_H__
