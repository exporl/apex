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
 
#ifndef __DEQUEUETHREAD_H__
#define __DEQUEUETHREAD_H__

#include "queuemanager.h"
#include "utils/checks.h"
#include "appcore/threads/thread.h"

namespace appcore
{

    /**
      * DeQueueThread
      *   an IThread implementation for DeQueue.
      *   Use this need a thread for your DeQueue.
      *   Each time an element arrives on the queue,
      *   mf_NewElement() is called, in which the derived
      *   class does it's business with the element.
      *   @param tQueueElement the type for DeQueue
      *************************************************** */
  template<class tQueueElement>
  class DeQueueThread : public IThread
  {
  public:
      /**
        * Constructor.
        * @param ac_pDeQ pointer to the DeQueue to use
        * @param ac_tEventCode the event code posted with a new tQueueElement
        * @param ac_sThreadName the thread's name
        */
    DeQueueThread( DeQueue<tQueueElement>* const ac_pDeQ, const std::string& ac_sThreadName ) :
        IThread( ac_sThreadName ),
      mv_nPeriod( 100 ),
      mv_bPreview( false ),
      mc_pDeQ( utils::PtrCheck( ac_pDeQ ) )
    {
    }

      /**
        * Constructor Overload for named queue.
        * @param ac_sQueueName the queue name
        * @param ac_tEventCode the event code posted with a new tQueueElement
        * @param ac_sThreadName the thread's name
        */
    DeQueueThread( const std::string& ac_sQueueName,const std::string& ac_sThreadName ) :
        IThread( ac_sThreadName ),
      mv_nPeriod( 100 ),
      mv_bPreview( false ),
      mc_pDeQ( utils::PtrCheck( QueueManager::sf_pInstance()->mf_pGetDeQueue<tQueueElement>( ac_sQueueName ) ) )
    {
    }

      /**
        * Destructor.
        * Calls DeQueue::mp_UnregisterFromQueue(),
        * so must be called before deleting the corresponding QueueBase.
        */
    virtual ~DeQueueThread()
    {
      mc_pDeQ->mp_UnregisterFromQueue( false );
      mp_Stop( 2000 );
    }

      /**
        * Handle dequeued elements.
        * Implementations override this to get the elements from the queue.
        * Is only called for valid elements, so not when there's
        * a timeout or an error.
        * @param a_Element the new element
        */
    virtual void mf_NewElement( tQueueElement& a_Element ) = 0;

      /**
        * Implementation of the IThread method.
        * Runs until the thread must stop, or until the queue is released.
        */
    void mp_Run()
    {
      while( !mf_bThreadShouldStop() )
      {
        const gt_eQueueStatus& status = mc_pDeQ->mf_GetBase().mf_eGetStatus();

        if( status == gc_eFill || status == gc_eFull )
        {
          QueueReturn<tQueueElement>& ret( mc_pDeQ->mf_DeQueue( 0, mv_bPreview ) );
          if( ret.m_eStatus == gc_eOk )
          {
            mf_NewElement( ret.m_tElement );
          }
          else
          {
            DBG( "DeQueueThread: no element" )
          }
        }
        else if( status == gc_eReleased )
        {
          DBG( "DeQueueThread: released" )
          return;
        }

        mf_bWait( mv_nPeriod );
      }
    }

      /**
        * Get the DeQueue for this class.
        * @return const pointer
        */
    INLINE const DeQueue<tQueueElement>* const mf_pGetDeQueue() const
    {
      return mc_pDeQ;
    }

      /**
        * Set the time to wait before trying to get a queue item.
        * @param ac_nPeriod
        */
    INLINE void mp_SetDequeuePeriod( const unsigned ac_nPeriod )
    {
      mv_nPeriod = ac_nPeriod;
    }

      /**
        * Set to use Preview or DeQueue.
        * @param ac_bSet true for preview
        */
    INLINE void mp_SetPreview( const bool ac_bSet )
    {
      mv_bPreview = ac_bSet;
    }

  private:
    unsigned                      mv_nPeriod;
    bool                          mv_bPreview;
    DeQueue<tQueueElement>* const mc_pDeQ;
  };

}

#endif //#ifndef __DEQUEUETHREAD_H__
