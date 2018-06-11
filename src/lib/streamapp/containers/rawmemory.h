/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef __RAWMEMORY_H__
#define __RAWMEMORY_H__

#include "defines.h"

namespace streamapp
{

/**
  * RawMemoryAccess
  *   class to manage access to a block of raw memory.
  **************************************************** */
class RawMemoryAccess
{
public:
    /**
      * Default Constructor.
      * Initializes size and data to 0.
      */
    RawMemoryAccess();

    /**
      * Constructor.
      * @param ac_pMemory the memory to access
      * @param ac_nSize the size of the block pointed to by ac_pMemory
      * @param ac_bInitToZero if true, sets al content to 0
      */
    RawMemoryAccess(void *const ac_pMemory, const unsigned ac_nSize,
                    const bool ac_bInitToZero = false);

    /**
      * Destructor.
      * Never deletes the memory.
      */
    virtual ~RawMemoryAccess();

    /**
      * operator ==
      * Returns true if memory is of equal size,
      * and has the same content.
      * @param ac_Rh the object to compare
      * @return true if equal
      */
    bool operator==(const RawMemoryAccess &ac_Rh);

    /**
      * operator !=
      * Inverse of operator ==.
      * @param ac_Rh  the object to compare
      * @return true if not equal
      */
    bool operator!=(const RawMemoryAccess &ac_Rh);

    /**
      * Get a pointer to the memory.
      * @return pointer
      */
    INLINE void *mf_pGetMemory() const
    {
        return m_pData;
    }

    /**
      * Get a casted pointer to the memory.
      * @param tDataType the type to cast to
      * @return pointer
      */
    template <class tDataType>
    INLINE tDataType *mf_pGetMemory() const
    {
        return (tDataType *)m_pData;
    }

    /**
      * Get a casted pointer to the memory at the given location.
      * Use to get all kinds of objects out of one RawMemory block.
      * eg if mem contains an integer and a double, use
      * @code
      * int* pInt = mem.mf_pGetMemory<int>( 0 );
      * double* pDouble = mem.mf_pGetMemory<int>( sizeof( int ) );
      * @endcode
      * @note only use if you know what you're playing with
      * @param ac_nByteOffset the offset in bytes to start the cast from
      * @param tDataType the type to cast to
      * @return pointer
      */
    template <class tDataType>
    INLINE tDataType *mf_pGetMemory(const unsigned ac_nByteOffset) const
    {
        return (tDataType *)(m_pData + ac_nByteOffset);
    }

    /**
      * Get an element.
      * @param ac_nOffset the offset in the memory
      * @return a reference, alwasy settable
      */
    INLINE char &operator[](const unsigned ac_nOffset) const
    {
        return m_pData[ac_nOffset];
    }

    /**
      * Get the size of the memory.
      * Size is in bytes.
      * @return the size
      */
    INLINE const unsigned &mf_nGetSize() const
    {
        return mv_nSize;
    }

    /**
      * Copy memory contents from external memory.
      * If ac_nSize is bigger then the current size, not everything is copied.
      * @param ac_pToCopy pointer to the data to copy
      * @param ac_nDestOffset the offset in this memory
      * @param ac_nSize the size to copy
      */
    void mp_CopyFrom(const void *const ac_pToCopy,
                     const unsigned ac_nDestOffset, const unsigned ac_nSize);

    /**
      * Copy memory to external memory.
      * Copy as much as possible, if not enough data is available,
      * the remains are filled with zeros.
      * @param ac_pToCopy destination memory
      * @param ac_nSourceOffset offset in this memory to start from
      * @param ac_nSize the size to copy
      */
    void mf_CopyTo(void *const ac_pToCopy, const unsigned ac_nSourceOffset,
                   const unsigned ac_nSize) const;

    /**
      * Fill the entire memory with the given character.
      * @param ac_Char the character to use.
      */
    void mp_FillWith(const char ac_Char);

protected:
    mutable char *m_pData;     // protected to allow RawMemory to change these
    mutable unsigned mv_nSize; //
};

/**
  * RawMemory
  *   class to manage a resizable block of raw memory.
  *   Note that when getting pointers to the memory,
  *   these will become invalid as the memory resizes.
  **************************************************** */
class RawMemory : public RawMemoryAccess
{
public:
    /**
      * Constructor.
      * Initializes size and data to 0.
      */
    RawMemory();

    /**
      * Destructor.
      * Frees memory.
      */
    ~RawMemory();

    /**
      * Constructor.
      * @param ac_nInitialSize the size to allocate
      * @param ac_bInitToZero if true, sets al content to 0
      */
    RawMemory(const unsigned ac_nInitialSize,
              const bool ac_bInitToZero = false);

    /**
      * Copy Constructor.
      * @param ac_Rh the one to copy
      */
    RawMemory(const RawMemoryAccess &ac_Rh);

    /**
      * Constructor.
      * Copies memory from ac_ToCopy.
      * @param ac_pToCopy the data
      * @param ac_nSize the size of ac_ToCopy
      */
    RawMemory(const void *const ac_pToCopy, const unsigned ac_nSize);

    /**
      * Assignment Operator.
      * @param ac_Rh the object to copy
      * @return reference to this
      */
    RawMemory &operator=(const RawMemoryAccess &ac_Rh);

    /**
      * Create a RawMemoryAccess object for a part of the memory.
      * @param ac_nStartOffSet start offset in bytes
      * @param ac_nLength length in bytes, is clipped if too long
      * @return a RawMemoryAcces object
      */
    RawMemoryAccess mf_GetAccess(const unsigned ac_nStartOffSet,
                                 const unsigned ac_nLength);

    /**
      * Resize the memory.
      * ac_bInitToZero has only effect if the new size is bigger then the
     * current size.
      * @param ac_nNewSize the new size
      * @param ac_bInitToZero if true, initializes newly allocated part to 0
      */
    void mp_Resize(const unsigned ac_nNewSize,
                   const bool ac_bInitToZero = false);

    /**
      * Resize the memory, but only if it's smaller that the given size.
      * @param ac_nNewSize  the new size
      * @param ac_bInitToZero  if true, initializes newly allocated part to 0
      */
    void mp_ResizeIfSmaller(const unsigned ac_nNewSize,
                            const bool ac_bInitToZero = false);

    /**
      * Remove a part of the block.
      * Removes a part from the memory and closes the gap around it,
      * shifting down any subsequent data.
      * If size is out of range, it is clipped.
      * Memory is resized.
      * @param ac_nStartOffset start of the part to remove
      * @param ac_nSize size of the part
      */
    void mp_RemovePart(const unsigned ac_nStartOffset, const unsigned ac_nSize);

    /**
      * Shift a section of the block down.
      * ac_nSize bytes above ac_nStartOffset are placed at the beginning
      * of the memory.
      * If size is out of range, it is clipped.
      * Memory is not resized.
      * @param ac_nStartOffset the start offset
      * @param ac_nSize the number of bytes to shift
      * @param ac_bClear if true, fills everything above ac_nSize with zeros.
      */
    void mp_ShiftDown(const unsigned ac_nStartOffset, const unsigned ac_nSize,
                      const bool ac_bClear = true);

    /**
      * Add memory.to the current block.
      * Resizes the memory.
      * @param ac_pToAppend  pointer to the data to append
      * @param ac_nSize
      */
    void mp_Append(const void *const ac_pToAppend, const unsigned ac_nSize);
};
}

#endif //#ifndef __RAWMEMORY_H__
