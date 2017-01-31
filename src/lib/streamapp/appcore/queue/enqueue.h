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
 
#ifndef __ENQUEUE_H__
#define __ENQUEUE_H__

#include "queuedefines.h"

namespace appcore
{
  template<class tQueueElement/*, class tQueueStorage = std::deque< tQueueElement > */ >
  class QueueBase;

  class tQueueBase;

  /**
    * Until the export keyword is known by all compilers we cannot use it,
    * hence we cannot place a template implementation in a source file.
    */
  /*export*/

    /**
      * EnQueue
      *   enqueues elements on a QueueBase.
      *   Can only be created through QueueBase,
      *   but can safely be deleted anywhere.
      *   @see mf_EnQueue()
      ****************************************** */
  template<class tQueueElement>
  class EnQueue
  {
    friend class QueueBase<tQueueElement>;
  private:
      /**
        * Private Constructor.
        * Only accessible for QueueBase.
        * @param a_QueueBase the base for this queue.
        */
    EnQueue( QueueBase<tQueueElement>& a_QueueBase );

  public:
      /**
        * Destructor.
        */
    ~EnQueue();

      /**
        * Add an element to the queue.
        * Only succeeds if queue status is not gc_eRelesed and not gc_eFull.
        * If the return value is gc_eFull, the element was not added since the queue is full.
        * The client should retry later.
        * If the method returns gc_eRelease, the queue was released and no elements can be queued anymore.
        * @param ac_Elem the element
        * @return mc_eOk for success
        */
    const gt_eQueueStatus& mf_EnQueue( const tQueueElement& ac_Elem );

      /**
        * Get the tQueueBase.
        * Used to query queue info.
        * @return const reference
        */
    const tQueueBase& mf_GetBase();

      /**
        * Release the queue.
        * @see QueueBase::mp_Release()
        */
    void mp_ReleaseQueue();

  private:
    QueueBase<tQueueElement>& m_QMan;

    EnQueue( const EnQueue& );
    EnQueue& operator = ( const EnQueue& );
  };

}

#endif //#ifndef __ENQUEUE_H__
