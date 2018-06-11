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

#ifndef __OWNEDARRAY_H__
#define __OWNEDARRAY_H__

#include "dynarray.h"

namespace streamapp
{
/**
  * OwnedArray
  *   a DynamicArray for pointers.
  *   All pointers added will be deleted when removed,
  *   so make sure not to delete any yourself.
  **************************************************** */
template <class tType>
class OwnedArray : public DynamicArray<tType *>
{
public:
    /**
      * Constrcutor.
      * @param ac_nInitSize the initial size
      * @param ac_nGrowSize the amount of memory to grow/shrink when needed
      */
    OwnedArray(const unsigned ac_nInitSize = 0, const unsigned ac_nGrowSize = 2)
        : DynamicArray<tType *>(ac_nInitSize, ac_nGrowSize)
    {
    }

    /**
      * Destructor.
      * Deletes all items.
      */
    ~OwnedArray()
    {
        mp_RemoveAllItems();
    }

    /**
      * Set an item.
      * Deletes the previous one.
      * @param ac_nItem the new item
      * @param ac_Item the position to set it at
      */
    INLINE void mp_SetItem(const unsigned ac_nItem, tType *ac_Item)
    {
        delete (DynamicArray<tType *>::mp_Array[ac_nItem]);
        DynamicArray<tType *>::mp_SetItem(ac_nItem, ac_Item);
    }

    /**
      * Remove the item at the back.
      */
    void mp_RemoveLastItem()
    {
        DynamicArray<tType *>::mp_RemoveLastItem();
        delete (
            DynamicArray<tType *>::mp_Array[DynamicArray<tType *>::m_nItems]);
    }

    /**
      * Remove the item at the specified position, shifting the other items
     * down.
      * @param ac_nItemPos the position
      */
    void mp_RemoveItemAt(const unsigned ac_nItemPos) // starts from 0!
    {
        delete (DynamicArray<tType *>::mp_Array[ac_nItemPos]);
        DynamicArray<tType *>::mp_RemoveItemAt(ac_nItemPos);
    }

    /**
      * Remove all items.
      */
    void mp_RemoveAllItems()
    {
        for (unsigned i = 0; i < DynamicArray<tType *>::m_nItems; ++i)
            delete (DynamicArray<tType *>::mp_Array[i]);
        DynamicArray<tType *>::mp_RemoveAllItems();
    }

    /**
      * Remove an item without deleting it.
      * This is against the owned policy, but ok
      * if the item is deleted by the callee.
      * @param a_pItem the item to remove
      * @return the item, or 0 if a_pItem was not found (and nothing was
     * probably deleted)
      */
    tType *mp_RemoveItemNoDelete(tType *a_pItem)
    {
        int nIndex = mf_nGetItemIndex(a_pItem);
        if (nIndex == -1)
            return 0;
        tType *pRet = DynamicArray<tType *>::mf_GetItem((unsigned)nIndex);
        DynamicArray<tType *>::mp_RemoveItemAt((unsigned)nIndex);
        return pRet;
    }

private:
    OwnedArray<tType>(const OwnedArray<tType> &);
    OwnedArray<tType> &operator=(const OwnedArray<tType> &);
};
}

#endif //#ifndef __OWNEDARRAY_H_
