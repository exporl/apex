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
 
#ifndef __QUEUEBASE_H__
#define __QUEUEBASE_H__

#include "enqueue.h"
#include "dequeue.h"

#include <deque>
#include "containers/dynarray.h"
#include "appcore/threads/locks.h"
using namespace streamapp;

namespace appcore
{

  template<class tQueueElement>
  class EnQueue;

  template<class tQueueElement>
  class DeQueue;

    /**
      * tQueueBase
      *   base class for QueueBase.
      ***************************** */
  class tQueueBase
  {
  protected:
      /**
        * Protected Constructor.
        * @param ac_nMaxQueueSize the max queue size
        */
    tQueueBase( const unsigned ac_nMaxQueueSize ) :
      mc_nMaxQueueSize( ac_nMaxQueueSize ),
      mv_eStatus( gc_eEmpty )
    {}

    const unsigned          mc_nMaxQueueSize;
    gt_eQueueStatus         mv_eStatus;

  public:
      /**
        * Destructor.
        */
    virtual ~tQueueBase()
    {}

      /**
        * Get the status.
        * @return the status
        */
    INLINE const gt_eQueueStatus& mf_eGetStatus() const
    {
      return mv_eStatus;
    }

      /**
        * Get the maximum queue size.
        * @return the size
        */
    INLINE const unsigned& mf_nGetMaxNumQueueItems() const
    {
      return mc_nMaxQueueSize;
    }

      /**
        * Get the number of items currently queued.
        * @return the number
        */
    virtual unsigned mf_nGetNumInQueue() const = 0;

      /**
        * Release the entire queue.
        * Any pending items are removed, and all DeQueues that are currently
        * registered will be notified of this.
        */
    virtual void mp_Release() = 0;

  private:
    tQueueBase( const tQueueBase& );
    tQueueBase& operator = ( const tQueueBase& );
  };


    /**
      * QueueBase
      *   implements a typical many to one queue communication pattern.
      *   TODO find out how to make less copies of the data
      ***************************************************************** */
  template<class tQueueElement/* ,class tQueueStorage = std::deque< tQueueElement > */>
  class QueueBase : public tQueueBase
  {
    friend class EnQueue<tQueueElement>;
    friend class DeQueue<tQueueElement>;
  public:
      /**
        * Constructor.
        */
    QueueBase( const unsigned ac_nMaxQueueSize ) :
      tQueueBase( ac_nMaxQueueSize )
    {
    }

      /**
        * Destructor.
        * Releases the queue and deletes all registered EnQueue and DeQueue.
        * Should best be called after Releasing to make sure all waiting
        * threads are stopped first.
        */
    ~QueueBase()
    {
      mp_Release();
      const Lock L( mc_Lock );
      const unsigned& nEnq = m_Enqueuers.mf_nGetNumItems();
      for( unsigned i = 0 ; i < nEnq ; ++i )
        delete m_Enqueuers( i );
      const unsigned& nDeq = m_Dequeuers.mf_nGetNumItems();
      for( unsigned i = 0 ; i < nDeq ; ++i )
        delete m_Dequeuers( i );
    }

      /**
        * Get an EnQueue for this queue.
        * @return a pointer
        */
    EnQueue<tQueueElement>* const mf_pRegisterEnQueue()
    {
      EnQueue<tQueueElement>* pRet = new EnQueue<tQueueElement>( *this );
      m_Enqueuers.mp_AddItem( pRet );
      return pRet;
    }

      /**
        * Get a DeQueue for this queue.
        * @return a pointer
        */
    DeQueue<tQueueElement>* const mf_pRegisterDeQueue()
    {
      DeQueue<tQueueElement>* pRet = new DeQueue<tQueueElement>( *this );
      m_Dequeuers.mp_AddItem( pRet );
      return pRet;
    }

      /**
        * Implementation of the tQueueBase method.
        */
    INLINE unsigned mf_nGetNumInQueue() const
    {
      return (unsigned) m_Queue.size();
    }

      /**
        * Implementation of the tQueueBase method.
        */
    void mp_Release()
    {
      const Lock L( mc_Lock );
      mv_eStatus = gc_eReleased;
      const unsigned& nDeq = m_Dequeuers.mf_nGetNumItems();
      for( unsigned i = 0 ; i < nDeq ; ++i )
        m_Dequeuers( i )->mp_Notify();
    }

  private:
      /**
        * Get the item at the back of the queue.
        * Must only be called if there are items queued.
        * @param ac_bPreview if true, doesn't remove the item from the queue.
        * @return the item
        */
    tQueueElement mf_DeQueue( const bool ac_bPreview = false )
    {
      const Lock L( mc_Lock );

      const size_t nSize = m_Queue.size();
      s_assert( nSize );

      tQueueElement ret = m_Queue.back();

      if( !ac_bPreview )
        m_Queue.pop_back();

      if( nSize == 1 )
        mv_eStatus = gc_eEmpty;
      else if( nSize <= mc_nMaxQueueSize )
        mv_eStatus = gc_eFill;

      return ret;
    }

      /**
        * Add an item to the queue.
        * Only succeeds if queue is not full and not released.
        * @param ac_Element the item, will be copied
        * @return the new queue status
        */
    const gt_eQueueStatus& mp_EnQueue( const tQueueElement& ac_Element )
    {
      const Lock L( mc_Lock );
      if( mv_eStatus == gc_eFill )
      {
        m_Queue.push_front( ac_Element );
        DBG( "queued" )
        const unsigned nSize = (unsigned) m_Queue.size();
        if( nSize == mc_nMaxQueueSize )
          mv_eStatus = gc_eFull;
        const unsigned nToNotify = s_min( m_Dequeuers.mf_nGetNumItems(), nSize );
        for( unsigned i = 0 ; i < nToNotify ; ++i )
          m_Dequeuers( i )->mp_Notify();
      }
      else if( mv_eStatus == gc_eEmpty )
      {
        mv_eStatus = gc_eFill;
        DBG( "queued" )
        m_Queue.push_front( ac_Element );
        if( !m_Dequeuers.mf_bIsEmpty() )
          m_Dequeuers( 0 )->mp_Notify();
      }

      return mv_eStatus;
    }

      /**
        * Unregister a DeQueue.
        * @param ac_pDeQueue pointer to the DeQueue
        * @param ac_bDestroy if true, calls mp_Release after unregistering
        */
    void mp_Unregister( DeQueue<tQueueElement>* const ac_pDeQueue, const bool ac_bDestroy )
    {
      m_Dequeuers.mp_RemoveItem( ac_pDeQueue );
      if( ac_bDestroy )
        mp_Release();
    }

      /**
        * Unregister an EnQueue.
        * @param ac_pEnQueue pointer to the EnQueue
        * @param ac_bDestroy if true, calls mp_Release after unregistering
        */
    void mp_Unregister( EnQueue<tQueueElement>* const ac_pEnQueue, const bool ac_bDestroy )
    {
      m_Enqueuers.mp_RemoveItem( ac_pEnQueue );
      if( ac_bDestroy )
        mp_Release();
    }

      /**
        * Obtain the lock for reading from the queue.
        * Used by DeQueue.
        */
    INLINE void mf_ObtainLock()
    {
      mc_Lock.mf_Enter();
    }

      /**
        * Release the lock or reading from the queue.
        * @see mf_ObtainLock()
        */
    INLINE void mf_ReleaseLock()
    {
      mc_Lock.mf_Leave();
    }

    typedef DynamicArray< EnQueue<tQueueElement>* >   mt_Enqueuers;
    typedef DynamicArray< DeQueue<tQueueElement>* >   mt_Dequeuers;
    typedef std::deque< tQueueElement >               mt_Queue;

    mt_Queue                m_Queue;
    mt_Enqueuers            m_Enqueuers;
    mt_Dequeuers            m_Dequeuers;
    const CriticalSection   mc_Lock;

    QueueBase( const QueueBase& );
    QueueBase& operator = ( const QueueBase& );
  };

/*****************************************************************************************************/

  template<class tQueueElement>
  EnQueue<tQueueElement>::EnQueue( QueueBase<tQueueElement>& a_QueueBase ) :
    m_QMan( a_QueueBase )
  {}

  template<class tQueueElement>
  EnQueue<tQueueElement>::~EnQueue()
  {
    m_QMan.mp_Unregister( this, false );
  }

  template<class tQueueElement>
  const gt_eQueueStatus& EnQueue<tQueueElement>::mf_EnQueue( const tQueueElement& ac_Elem )
  {
    return m_QMan.mp_EnQueue( ac_Elem );
  }

  template<class tQueueElement>
  const tQueueBase& EnQueue<tQueueElement>::mf_GetBase()
  {
    return m_QMan;
  }

  template<class tQueueElement>
  void EnQueue<tQueueElement>::mp_ReleaseQueue()
  {
    m_QMan.mp_Release();
  }

/*****************************************************************************************************/

  template<class tQueueElement>
  DeQueue<tQueueElement>::DeQueue( QueueBase<tQueueElement>& a_QueueBase ) :
    m_QMan( a_QueueBase )
  {}

  template<class tQueueElement>
  DeQueue<tQueueElement>::~DeQueue()
  {
    m_QMan.mp_Unregister( this, false );
  }

  template<class tQueueElement>
  void DeQueue<tQueueElement>::mp_Notify()
  {
    mc_Waiter.mp_SignalObject();
  }

  template<class tQueueElement>
  QueueReturn<tQueueElement>& DeQueue<tQueueElement>::mf_DeQueue( const int ac_nTimeOut, const bool ac_bPreview /*= false*/ )
  {
    m_QMan.mf_ObtainLock();

    const gt_eQueueStatus& eSatus( m_QMan.mf_eGetStatus() );

    if( eSatus == gc_eFill || eSatus == gc_eFull )
    {
      m_Return.m_tElement = m_QMan.mf_DeQueue( ac_bPreview );
      m_Return.m_eStatus = gc_eOk;
      m_QMan.mf_ReleaseLock();
      return m_Return;
    }
    else if( eSatus == gc_eReleased )
    {
      m_NoReturn.m_eStatus = gc_eError;
      m_QMan.mf_ReleaseLock();
      return m_NoReturn;
    }

    m_QMan.mf_ReleaseLock();

    if( mc_Waiter.mf_eWaitForSignal( ac_nTimeOut ) == WaitableObject::wait_ok )
    {
      m_QMan.mf_ObtainLock();

      const gt_eQueueStatus& eSatus2( m_QMan.mf_eGetStatus() );

      if( eSatus2 == gc_eFill || eSatus == gc_eFull )
      {
        m_Return.m_tElement = m_QMan.mf_DeQueue( ac_bPreview );
        m_Return.m_eStatus = gc_eOk;
        m_QMan.mf_ReleaseLock();
        return m_Return;
      }
      else if( eSatus2 == gc_eReleased )
      {
        m_NoReturn.m_eStatus = gc_eError;
        m_QMan.mf_ReleaseLock();
        return m_NoReturn;
      }
      else
      {
        DBG( "queue empty before element dequeue" );
        m_NoReturn.m_eStatus = gc_eError;
        m_QMan.mf_ReleaseLock();
        return m_NoReturn;
      }
    }
    else
    {
      m_NoReturn.m_eStatus = gc_eTimeout;
      m_QMan.mf_ReleaseLock();
      return m_NoReturn;
    }
  }

  template<class tQueueElement>
  const tQueueBase& DeQueue<tQueueElement>::mf_GetBase()
  {
    return m_QMan;
  }

  template<class tQueueElement>
  void DeQueue<tQueueElement>::mp_ReleaseQueue()
  {
    m_QMan.mp_Release();
  }

  template<class tQueueElement>
  void DeQueue<tQueueElement>::mp_UnregisterFromQueue( const bool ac_bAlsoDestroy /*= false*/ )
  {
    m_QMan.mp_Unregister( this, ac_bAlsoDestroy );
  }

}

#endif //#ifndef __QUEUEBASE_H__
