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
 
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "containers/matrix.h"

namespace streamapp
{

    /**
      * Queue
      *   simple fixed-size queue implementation.
      *   FIXME not finished, do not use as it leaks
      ******************************************* */
  template< class tType >
  class Queue : public ArrayStorage< tType* >
  {
  public:

      /**
        * Declare the function used to allocate a queue item.
        */
    typedef tType* const (*mt_pfAllocator)();

      /**
        * Declare the function used to copy a queue item.
        */
    typedef void (*mt_pfDuplicator)( const tType* a_pIn, tType* const a_pOut );

      /**
        * Constructor.
        * @param ac_nQueueSize the maximum queue size
        * @param fp_Allocator the allocator for the items
        * @param fp_Duplicator the copier for the items
        */
    Queue( const unsigned ac_nQueueSize, mt_pfAllocator fp_Allocator, mt_pfDuplicator fp_Duplicator ) :
        ArrayStorage< tType* >( ac_nQueueSize ),
      mv_nQueueItems( 0 ),
      mfp_Duplicator( fp_Duplicator )
    {
      for( unsigned i = 0 ; i < ac_nQueueSize ; ++ i )
        ArrayStorage< tType* >::mp_Set( i, fp_Allocator() );
    }

      /**
        * Destructor.
        */
    ~Queue()
    {
      //ArrayStorage will delete mem
    }

      /**
        * Queue an item.
        * @param ac_pItem pointer to the item to queue, will be copied.
        * @return the queued item, or 0 if queue was full
        */
    tType* mp_QueueItem( tType* const ac_pItem )
    {
      if( mv_nQueueItems < ArrayStorage< tType* >::mf_nGetBufferSize() )
      {
        mfp_Duplicator( ac_pItem, ArrayStorage< tType* >::mf_Get( mv_nQueueItems ) );
        ++mv_nQueueItems;
        return ac_pItem;
      }
      return 0;
    }

      /**
        * Dequeue the last item.
        * @return the item, should be deleted by the acquirer
        */
    tType* mp_DeQueueItem()
    {
      if( mv_nQueueItems )
        return ArrayStorage< tType* >::mf_Get( --mv_nQueueItems );
      return 0;
    }

      /**
        * Clear the queue.
        */
    void mp_Reset()
    {
      mv_nQueueItems = 0;
    }

      /**
        * Get the number of queued items.
        * @return the number
        */
    const unsigned& mf_nGetNumQueueItems() const
    { return mv_nQueueItems; }

      /**
        * Get the queue size.
        * @return
        */
    const unsigned& mf_nGetMaxNumQueueItems() const
    { return ArrayStorage< tType* >::mf_nGetBufferSize(); }

  private:
    unsigned mv_nQueueItems;
    mt_pfDuplicator mfp_Duplicator;
  };

}

#endif //__QUEUE_H__
