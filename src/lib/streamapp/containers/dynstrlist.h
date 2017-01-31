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
 
#ifndef __DYNSTRLIST_H__
#define __DYNSTRLIST_H__

#include "containers/ownedarray.h"
#include "../_fromv3/core/mpl/if.h"

namespace streamapp
{

  template< class tType, bool tOwned >
  struct tSelectArray
  {
    typedef DynamicArray< tType* > mt_NotOwned;
    typedef OwnedArray< tType > mt_Owned;

    typedef typename smn::mpl::tIf< tOwned, mt_Owned, mt_NotOwned >::type type;
  };

    /**
      * DynStreamList
      *   provides management for the classes in multistream.h.
      *   Is implemented using OwnedArray.
      *   Keeps track of the number of channels and buffersize.
      *   @param tInOrOutStream must be an InputStream or an OutputStream
      *   @see DynamicArray
      *   @see dynstrlist.h
      *   @see dynproclist.h
      ******************************************************************* */
  template< class tInOrOutStream, bool tOwned = true >
  class DynStreamList : public tSelectArray< tInOrOutStream, tOwned >::type
  {
  public:

    typedef typename tSelectArray< tInOrOutStream, tOwned >::type tArray;

      /**
        * Constructor.
        */
    DynStreamList() :
        tArray( 3 ),
      mv_nChan( 0 ),
      mv_nSize( 0 )
    {
    }

      /**
        * Destructor.
        * Calls mp_RemoveAllItems().
        */
    virtual ~DynStreamList()
    {
      mp_RemoveAllItems();
    }

      /**
        * Add an item.
        * This will increase the number of channels.
        * @param a_pItem pointer to the InputStream
        */
    virtual void mp_AddItem( tInOrOutStream* a_pItem )
    {
      if( tArray::mf_nGetNumItems() == 0 )
        mv_nSize = a_pItem->mf_nGetBufferSize();
      else
        assert( mv_nSize == a_pItem->mf_nGetBufferSize() );
      mv_nChan += a_pItem->mf_nGetNumChannels();
      tArray::mp_AddItem( a_pItem );
      mp_UpdateMemory();
    }

      /**
        * Remove and delete an item.
        * @param ac_nItemPos the index in the list, must be < mf_nGetNumItems()
        */
    virtual void mp_RemoveItem( const unsigned ac_nItemPos )
    {
      mv_nChan -= tArray::mf_GetItem( ac_nItemPos )->mf_nGetNumChannels();
      tArray::mp_RemoveItemAt( ac_nItemPos );
      mp_UpdateMemory();
    }

      /**
        * Remove and delete an item.
        * @param ac_pItem pointer to the item
        */
    virtual void mp_RemoveItem( tInOrOutStream* const ac_pItem )
    {
      const int index = tArray::mf_nGetItemIndex( ac_pItem );
      if( index >= 0 )
      {
        mv_nChan -= tArray::mf_GetItem( index )->mf_nGetNumChannels();
        tArray::mp_RemoveItemAt( index );
        mp_UpdateMemory();
      }
    }

      /**
        * Remove all items.
        * Deletes all items.
        * Reduces number of channels to zero and resets buffersize used.
        */
    virtual void mp_RemoveAllItems()
    {
      mv_nChan = 0;
      mv_nSize = 0;
      tArray::mp_RemoveAllItems();
      mp_UpdateMemory();
    }

      /**
        * Get a pointer to an item in the list.
        * @param ac_nItemPos the index in the list, must be < mf_nGetNumItems()
        * @return const pointer to the item
        */
    INLINE virtual const tInOrOutStream* mf_pGetItem( const unsigned ac_nItemPos ) const
    {
      return tArray::mf_GetItem( ac_nItemPos );
    }

      /**
        * Get the current number of items in the list.
        * @return the number
        */
    INLINE virtual const unsigned& mf_nGetNumItems() const
    {
      return tArray::mf_nGetNumItems();
    }

      /**
        * Get the total number of channels.
        * @return the number
        */
    INLINE virtual unsigned  mf_nGetNumChannelsR() const
    {
      return mv_nChan;
    }

      /**
        * Get the buffersize.
        * @return the size in samples
        */
    INLINE virtual unsigned  mf_nGetBufferSizeR() const
    {
      return mv_nSize;
    }

  protected:
      /**
        * Update the memory.
        * Called whenever the list is updated.
        * Implementations should override this method, and update their pointer memory
        * to reflect the new number of channels used, if needed.
        */
    virtual void mp_UpdateMemory()
    {
    }

    unsigned    mv_nChan;
    unsigned    mv_nSize;

    DynStreamList( const DynStreamList& );
    DynStreamList& operator = ( const DynStreamList& );
  };

}

#endif //#ifndef __DYNSTRLIST_H__
