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
 
#ifndef __DEQUEUE_H__
#define __DEQUEUE_H__

#include "queuedefines.h"
#include "appcore/threads/waitableobject.h"

namespace appcore
{

  template< class tQueueElement/*, class tQueueStorage = std::deque< tQueueElement > */ >
  class QueueBase;

    /**
      * QueueReturn
      *   struct returned when getting an element from DeQueue.
      *   m_tElement is only valid as the last queue element if
      *   m_eStatus == gc_eOk. Else it is a dummy element.
      ********************************************************** */
  template<class tQueueElement>
  struct QueueReturn
  {
    gt_eQueueReturnStatus m_eStatus;
    tQueueElement         m_tElement;
  };

    /**
      * DeQueue
      *   dequeues elements of a QueueBase.
      *   Can only be created through QueueBase,
      *   can be deleted anywhere.
      *   @see mf_DeQueue()
      ****************************************** */
  template<class tQueueElement>
  class DeQueue
  {
    friend class QueueBase<tQueueElement>;
  private:
      /**
        * Private Constructor.
        * Only accessible for QueueBase.
        * @param a_QueueBase the base for this queue.
        */
    DeQueue( QueueBase<tQueueElement>& a_QueueBase );

  public:
      /**
        * Destructor.
        * calls mp_ReleaseQueue( false ).
        */
    ~DeQueue();

      /**
        * Try to dequeue an element.
        * Only succeeds if the queue status is not gc_eReleased or gc_eEmpty.
        * If the return status is gc_eTimeOut, there were no new elements queued
        * within the timeout interval.
        * If the return status is gc_eError, the queue was released.
        * If the return status is gc_eOk, the return contains a valid element
        * @param ac_nTimeOut the timeout interval in milliseconds, or -1 for infinite
        * @param ac_bPreview if true, doesn't remove the item from the queue.
        * @return a QueueReturn with eventually the last queue element
        */
    QueueReturn<tQueueElement>& mf_DeQueue( const int ac_nTimeOut, const bool ac_bPreview = false );

      /**
        * Try to get the latest queue element without removing it from the queue.
        * @see mf_DeQueue
        * @return a QueueReturn with eventually the last queue element
        */
    QueueReturn<tQueueElement>& mf_Preview( const int ac_nTimeOut )
    {
      return mf_DeQueue( ac_nTimeOut, true );
    }

      /**
        * Get the tQueueBase.
        * Used to query queue info.
        * @return const reference
        */
    const tQueueBase& mf_GetBase();

      /**
        * Release the queue.
        * The DeQueue calling this will get notified once.
        * @see QueueBase::mp_Release()
        */
    void mp_ReleaseQueue();

      /**
        * Unregister from the queue.
        * Calling this method tells QueueBase not to signal this object
        * if there are new queue items, or if the queue is released.
        * If ac_bAlsoDestroy is true, mp_ReleaseQueue() is called too,
        * and one notification still happens.
        */
    void mp_UnregisterFromQueue( const bool ac_bAlsoDestroy = false );

  private:
      /**
        * Notifies there is a new element, or that the queue is released.
        */
    void mp_Notify();

    QueueReturn<tQueueElement>  m_Return;
    QueueReturn<tQueueElement>  m_NoReturn;
    const WaitableObject        mc_Waiter;
    QueueBase<tQueueElement>&   m_QMan;

    DeQueue( const DeQueue& );
    DeQueue& operator = ( const DeQueue& );
  };

}

#endif //#ifndef __DEQUEUE_H__
