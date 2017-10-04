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

#ifndef __STRDYNARRAY_H__
#define __STRDYNARRAY_H__

#include "../defines.h"

#include <QtGlobal>

#ifndef S_C6X
#include <cstdlib> //malloc
#else
#include <stdlib.h>
#endif
#include <string.h> //memmove

namespace streamapp
{

/**
  * Allocates initial memory for DynamicArray.
  * @param ac_nSize the number of items
  * @param ac_bInit if true, sets all items to zero
  * @return
  */
template <class tType>
tType *f_InitDynMem(const unsigned ac_nSize, const bool ac_bInit)
{
    if (ac_nSize != 0) {
        tType *c_pArray = (tType *)malloc(sizeof(tType) * ac_nSize);

        if (ac_bInit)
            memset(c_pArray, 0, sizeof(tType) * ac_nSize);

        return c_pArray;
    }
    return 0;
}

/**
  * DynamicArray
  *   provides resizable array for primitive types.
  *   Works like eg std::vector, but is much faster (about 20times) since
  *   it uses realloc, instead of first delete and then new.
  *   TODO check if it's feasible to shrink
  *********************************************************************** */
template <class tType>
class DynamicArray
{
public:
    /**
      * Constrcutor.
      * @param ac_nInitSize the initial size
      * @param ac_nGrowSize the amount of memory to grow/shrink when needed
      */
    DynamicArray(const unsigned ac_nInitSize = 0,
                 const unsigned ac_nGrowSize = 2)
        : m_nSize(ac_nInitSize),
          m_nItems(0),
          m_nGrow(ac_nGrowSize),
          mp_Array(f_InitDynMem<tType>(ac_nInitSize, true))
    {
    }

    /**
      * Destructor.
      * Doesn't delete any items, just cleans up memory.
      */
    virtual ~DynamicArray()
    {
        free(mp_Array);
    }

    /**
      * Get the current number of items.
      * @return the number
      */
    INLINE const unsigned &mf_nGetNumItems() const
    {
        return m_nItems;
    }

    /**
      * Check if array is empty.
      * return true if mf_nGetNumItems() == 0
      */
    INLINE bool mf_bIsEmpty() const
    {
        return m_nItems == 0;
    }

    /**
      * Get the index of the specified item.
      * Only searches for the first occurrence of the item!
      * @param ac_nSearchOffset the offset to start searching at
      * @param a_ItemToSearch the item to search
      * @return the index, or -1 if not found
      */
    int mf_nGetItemIndex(const tType a_ItemToSearch,
                         const unsigned ac_nSearchOffset = 0) const
    {
        for (unsigned i = ac_nSearchOffset; i < m_nItems; ++i) {
            if (mp_Array[i] == a_ItemToSearch)
                return (int)i;
        }
        return -1;
    }

    /**
      * Get an item.
      * @param ac_nItem the index
      * @return the item
      */
    INLINE tType mf_GetItem(const unsigned ac_nItem) const
    {
        Q_ASSERT(ac_nItem < m_nItems);
        return mp_Array[ac_nItem];
    }

    /**
      * Get an item reference.
      * @param ac_nItem the index
      * @return the item
      */
    INLINE tType &operator()(const unsigned ac_nItem)
    {
        Q_ASSERT(ac_nItem < m_nItems);
        return mp_Array[ac_nItem];
    }

    /**
      * Get a const item reference.
      * @param ac_nItem the index
      * @return the reference, very const
      */
    INLINE const tType &operator()(const unsigned ac_nItem) const
    {
        Q_ASSERT(ac_nItem < m_nItems);
        return mp_Array[ac_nItem];
    }

    /**
      * Get a const reference to an item.
      * @param ac_nItem the index
      * @return the reference
      */
    INLINE const tType &mf_GetRItem(const unsigned ac_nItem) const
    {
        Q_ASSERT(ac_nItem < m_nItems);
        return mp_Array[ac_nItem];
    }

    /**
      * Add an item at the back.
      * @param ac_Item the item to add
      */
    virtual void mp_AddItem(tType ac_Item)
    {
        if (m_nSize == 0) {
            m_nSize = m_nGrow;
            mp_Array = f_InitDynMem<tType>(m_nSize, true);
        }

        mp_Array[m_nItems] = ac_Item;
        m_nItems++;

        // allocate some more
        if (m_nItems == m_nSize) {
            m_nSize += m_nGrow;
            mp_Array = (tType *)realloc(mp_Array, sizeof(tType) * m_nSize);
        }
    }

    /**
      * Add an item at the back, if it's not in the list already
      * @param ac_Item the item to add
      */
    virtual void mp_AddItemIfNotThere(tType ac_Item)
    {
        if (mf_nGetItemIndex(ac_Item) != -1)
            return;
        mp_AddItem(ac_Item);
    }

    /**
      * Set an item.
      * @param ac_nItem the new item
      * @param ac_Item the position to set it at
      */
    INLINE virtual void mp_SetItem(const unsigned ac_nItem, tType ac_Item)
    {
        Q_ASSERT(ac_nItem <= m_nSize);
        mp_Array[ac_nItem] = ac_Item;
    }

    /**
      * Remove the item at the back.
      */
    INLINE virtual void mp_RemoveLastItem()
    {
        m_nItems--;
    }

    /**
      * Remove the item at the specified position, shifting the other items
     * down.
      * @param ac_nItemPos the position
      */
    virtual void mp_RemoveItemAt(const unsigned ac_nItemPos)
    {
        Q_ASSERT(ac_nItemPos < m_nItems);

        memset(&mp_Array[ac_nItemPos], 0, sizeof(tType **));
        const unsigned nRunTo = m_nItems - 1;
        for (unsigned i = ac_nItemPos; i < nRunTo; ++i) {
            const unsigned nSrc = i + 1;
            memmove(&mp_Array[i], &mp_Array[nSrc], sizeof(tType **));
            memset(&mp_Array[nSrc], 0, sizeof(tType **));
        }
        m_nItems--;
    }

    /**
      * Remove the specified item, shifting the other items down.
      * Only searches for the first occurrence of the item!
      * Doesn't do anything when the item is not found.
      * @param a_ItemToRemove the item to remove
      */
    void mp_RemoveItem(const tType &a_ItemToRemove)
    {
        const int nIndex = mf_nGetItemIndex(a_ItemToRemove);
        if (nIndex >= 0)
            mp_RemoveItemAt(nIndex);
    }

    /**
      * Remove all occurrences of the given item.
      * Items after it are shifted down.
      * @param a_ItemToRemove  the item to remove
      */
    void mp_RemoveItemOcc(const tType &a_ItemToRemove)
    {
        int nIndex = mf_nGetItemIndex(a_ItemToRemove);
        while (nIndex >= 0) {
            mp_RemoveItemAt(nIndex);
            nIndex = mf_nGetItemIndex(a_ItemToRemove, nIndex);
        }
    }

    /**
      * Remove all items
      */
    INLINE virtual void mp_RemoveAllItems()
    {
        mp_Clear();
    }

protected:
    /**
      * Clear the entire array memory.
      * Sets number of items to zero.
      */
    void mp_Clear()
    {
        for (unsigned i = 0; i < m_nItems; ++i)
            mp_Array[i] = 0;
        m_nItems = 0;
    }

    mutable unsigned m_nSize;
    mutable unsigned m_nItems;
    mutable unsigned m_nGrow;
    tType *mp_Array;

private:
    DynamicArray<tType>(const DynamicArray<tType> &);
    DynamicArray<tType> &operator=(const DynamicArray<tType> &);
};
}

#endif //#ifndef __STRDYNARRAY_H__
