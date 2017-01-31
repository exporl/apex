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
 
#ifndef __EVENTQUEUETHREAD_H__
#define __EVENTQUEUETHREAD_H__

#include <deque>
#include <assert.h>
#include <iostream>
#include "appcore/threads/locks.h"
#include "appcore/events/events.h"
#include "appcore/threads/thread.h"

namespace appcore
{

    /**
      * EventQueueThread
      *   Thread that can be triggered to wait for something,
      *   and post an event when done.
      *
      *   Can be triggered to wait for a specified time,
      *   or to wait on a WaitableObject, and post an event when the wait
      *   is finished. Uses a queue so the number of triggers is unlimited
      *   and sequential. In other words: basis for a threaded state machine.
      *
      *   Usage example: if it's needed to sleep() a gui thread for some time,
      *   one cannot just put sleep() in that thread, since then it will suspend
      *   and no events are processed anymore, hence the gui isn't updated.
      *   So derive your class from IEventDispatcher (or a proxy class),
      *   give it an EventQueueThread member, start the thread, and call
      *   mp_StartWaitOnTimer( 0, 5000 ). 5 seconds later your class gets
      *   called with mp_DispatchEvent( 0 ), while in the meantime your class'
      *   own thread stayed free for doing whatever it does.
      *   Note that it might be a good idea to use a CriticalSection to protect
      *   your class' internals
      ************************************************************************** */
  template< class tType = int >
  class EventQueueThread : public IThread
  {
  public:
      /**
        * Thread status.
        */
    enum mt_eStatus
    {
      mc_eIdle,
      mc_eWorking
    };


      /**
        * mt_WaitableItem
        *   base class for items that can be waited for.
        ************************************************ */
    struct mt_WaitableItem
    {
        /**
          * Constructor.
          * @param ac_tID the event ID to post when waiting finishes
          * @param ac_nMaxWaitTime the maximum time to wait
          */
      mt_WaitableItem( const tType& ac_tID, const int ac_nMaxWaitTime ) :
        mc_nID( ac_tID ),
        mc_nWaitTime( ac_nMaxWaitTime )
      {
      }

        /**
          * Destructor.
          */
      virtual ~mt_WaitableItem()
      {
      }

        /**
          * Start waiting.
          * Implementations block here until their timeout
          * condition is met, or until @c mp_Kill() gets called.
          */
      virtual void mf_Wait() = 0;

        /**
          * Kill the waiting.
          * Must take effect asap (like within 50mSec or so)
          */
      virtual void mp_Kill() = 0;

      const tType mc_nID;

    protected:
      const int   mc_nWaitTime;
    };

      /**
        * mt_WaitObjectItem
        *   mt_WaitableItem implementation using a @c WaitableObject.
        ************************************************************* */
    struct mt_WaitObjectItem : public mt_WaitableItem
    {
        /**
          * Constructor.
          * @param ac_tID the event ID
          * @param ac_nMaxWaitTime the maximum time to wait
          * @param ac_pWaitObject the object to wait on; if 0, one is created
          */
      mt_WaitObjectItem( const tType& ac_tID, const int ac_nMaxWaitTime, const WaitableObject* ac_pWaitObject = 0 ) :
          mt_WaitableItem( ac_tID, ac_nMaxWaitTime),
        mc_pWaitObject( ac_pWaitObject ? ac_pWaitObject : new WaitableObject() ),
        mc_bDeleteWaitObject( ac_pWaitObject ? false : true )
      {
      }

        /**
          * Destructor.
          */
      virtual ~mt_WaitObjectItem()
      {
        if( mc_bDeleteWaitObject )
          delete mc_pWaitObject;
      }

        /**
          * Implementation of the mt_WaitableItem method.
          */
      INLINE virtual void mf_Wait()
      {
        mc_pWaitObject->mf_eWaitForSignal( this->mc_nWaitTime );
      }

        /**
          * Implementation of the mt_WaitableItem method.
          */
      INLINE virtual void mp_Kill()
      {
        mc_pWaitObject->mp_SignalObject();
      }

    protected:
      const WaitableObject* mc_pWaitObject;
    private:
      const bool            mc_bDeleteWaitObject;
    };

      /**
        * mt_PollingItem
        *   mt_WaitableItem implementation that polls
        *   the given function until it returns true,
        *   until the wait is killed.
        *   Derives from mt_WaitObjectItem, else we would have
        *   to keep a CriticalSection and a seperate kill flag.
        *   @param tPollData the data type of which a pointer
        *   is passed to the polling function
        ****************************************************** */
    template< class tPollData >
    struct mt_PollingItem : public mt_WaitObjectItem
    {
        /**
          * The polling function.
          */
      typedef bool (*mt_bPollFunc)( tPollData* );

        /**
          * Constructor.
          * @param ac_tID the event ID
          * @param ac_nMaxWaitTime the maximum time to wait
          * @param a_pfnFuncFunc the poll function
          * @param a_pPollData the data to pass to the polling function
          */
      mt_PollingItem( const tType& ac_tID, const int ac_nMaxWaitTime,
                      const mt_bPollFunc a_pfnPollFunc, tPollData* a_pPollData = 0 ) :
          mt_WaitObjectItem( ac_tID, ac_nMaxWaitTime ),
        m_pPollData( a_pPollData ),
        mc_pfnPollFunc( a_pfnPollFunc )
      {
      }

        /**
          * Destructor.
          */
      ~mt_PollingItem()
      {
      }

        /**
          * Implementation of the mt_WaitableItem method.
          */
      void mf_Wait()
      {
        for( ; ; )
        {

//            if( EventQueueThread<tType>::mt_WaitObjectItem::mc_pWaitObject->mf_eWaitForSignal( sc_nTimeDiv ) == WaitableObject::wait_ok )
          if( this->mc_pWaitObject->mf_eWaitForSignal( sc_nTimeDiv ) == WaitableObject::wait_ok )
            break;
          else if( (*mc_pfnPollFunc)( m_pPollData ) )
            break;
        }
      }

   private:
      static const int    sc_nTimeDiv = 50;
      tPollData*          m_pPollData;
      const mt_bPollFunc  mc_pfnPollFunc;
    };


      /**
        * Queue.
        */
    typedef std::deque< mt_WaitableItem* > mt_Queue;


      /**
        * Constructor.
        * @param ac_sName the thread's name
        * @param a_pListener the object to post events to
        */
    EventQueueThread( const std::string& ac_sName, IEventDispatcher< tType >* a_pListener ) :
        IThread( ac_sName ),
      mv_eStatus( mc_eIdle ),
      m_pListener( a_pListener ),
      mc_pWorkToDo( new WaitableObject() ),
      mc_pLock( new CriticalSection() )
    {
    }

      /**
        * Destructor.
        * Removes any pending events from the queue.
        */
    ~EventQueueThread()
    {
      mp_Stop( 1000 );
      delete mc_pWorkToDo;
      delete mc_pLock;
    }

      /**
        * Trigger the thread function to quit.
        * Used until we fix IThread..
        */
    void mp_DoQuit()
    {
      mc_pWorkToDo->mp_SignalObject();
    }

      /**
        * Set a(nother) target.
        * @param a_pListener the dispatcher, or zero to disable posting events
        */
    void mp_SetTarget( IEventDispatcher<tType>* a_pListener )
    {
      const Lock L( *mc_pLock );
      m_pListener = a_pListener;
    }

      /**
        * Implementation of the IThread method.
        */
    void mp_Run()
    {
      for( ; ; )
      {
          //wait until there is work to do
        mc_pWorkToDo->mf_eWaitForSignal( 500 );

          //check if we should exit
        if( mf_bThreadShouldStop() )
        {
          const Lock L( *mc_pLock );
          typename mt_Queue::size_type nLeft = m_Queue.size();
          if( nLeft )
          {
            std::cout << "EventQueueThread::~EventQueueThread: " << (unsigned) nLeft << " left in queue while destructing!" << std::endl;
            while( m_Queue.size() )
              m_Queue.pop_front();
          }
          return;
        }

        mc_pLock->mf_Enter();
        while( m_Queue.size() )
        {
          mv_eStatus = mc_eWorking;
          mt_WaitableItem* item = m_Queue.front();
          mc_pLock->mf_Leave();

          item->mf_Wait();
          if( m_pListener )
            m_pListener->mp_DispatchEvent( item->mc_nID );
          delete item;

          mc_pLock->mf_Enter();
          m_Queue.pop_front();

        #ifdef PRINTQUEUE
          std::cout << "EventQueueThread:: " << m_Queue.size() << " left in queue" << std::endl;
        #endif
        }

         mv_eStatus = mc_eIdle;
         mc_pLock->mf_Leave();
      }
      mc_pLock->mf_Leave();
    }

      /**
        * Trigger to start waiting.
        * @param ac_nMilliSeconds the time to wait before posting the ac_nEventID event
        * @param ac_nEventID the event code to post
        */
    void mp_StartWaitOnTimer( const int ac_nMilliSeconds, const tType& ac_nEventID )
    {
      const Lock L( *mc_pLock );

      m_Queue.push_back( new mt_WaitObjectItem( ac_nEventID, ac_nMilliSeconds ) );
      if( mv_eStatus == mc_eIdle )
        mc_pWorkToDo->mp_SignalObject();

    #ifdef PRINTQUEUE
      std::cout << "EventQueueThread::mp_StartWaitOnTimer " << ac_nEventID << " queued" << std::endl;
    #endif
    }

      /**
        * Trigger to start waiting.
        * @param ac_nMilliSeconds the maximum time to wait on before ac_pObjectToWaitOn
        * @param ac_pObjectToWaitOn the WaitableObject to wait on (using mf_eWaitForSignal)
        * @param ac_nEventID the event code to post
        */
    void mp_StartWaitOnObject( const int ac_nMilliSeconds, const appcore::WaitableObject* ac_pObjectToWaitOn, const tType& ac_nEventID )
    {
      assert( ac_pObjectToWaitOn );
      const Lock L( *mc_pLock );

      m_Queue.push_back( new mt_WaitObjectItem( ac_nEventID, ac_nMilliSeconds, ac_pObjectToWaitOn ) );
      if( mv_eStatus == mc_eIdle )
        mc_pWorkToDo->mp_SignalObject();

    #ifdef PRINTQUEUE
      std::cout << "EventQueueThread::mp_StartWaitOnObject " << ac_nEventID << " queued" << std::endl;
    #endif
    }

      /**
        * Trigger to start waiting.
        * @param ac_nMilliSeconds the time to wait before posting the ac_nEventID event
        * @param ac_nEventID the event code to post
        */
    template< class tPollData >
    void mp_StartWaitOnPoll( const int ac_nMilliSeconds, const tType& ac_nEventID,
                             const typename mt_PollingItem< tPollData >::mt_bPollFunc ac_pfnPollFunc,
                             tPollData* a_pPollData )
    {
      assert( ac_pfnPollFunc );
      const Lock L( *mc_pLock );

      m_Queue.push_back( new mt_PollingItem< tPollData >( ac_nEventID, ac_nMilliSeconds, ac_pfnPollFunc, a_pPollData ) );
      if( mv_eStatus == mc_eIdle )
        mc_pWorkToDo->mp_SignalObject();

    #ifdef PRINTQUEUE
      std::cout << "EventQueueThread::mp_StartWaitOnPoll " << ac_nEventID << " queued" << std::endl;
    #endif
    }

      /**
        * Kill the timer with the given ID.
        * The result depends on what the thread is doing atm:
        * if it's waiting for the given timer, the wait returns immedeately
        * and the event is posted. If the thread is waiting for another timer(s),
        * it just keeps on waiting. When the killed timer is dequeued, it looks
        * as if it was started with a waiting time equal to zero.
        * @param ac_nEventID the ID of the timer to kill
        * @param ac_bKillAll if true, killas all timers with the given ID,
        * else only the first one found.
        */
    void mp_KillTimer( const tType& ac_nEventID, const bool ac_bKillAll = true )
    {
      const Lock L( *mc_pLock );
      const size_t nTimers = m_Queue.size();
      for( size_t i = 0 ; i < nTimers ; ++i )
      {
        mt_WaitableItem* cur = m_Queue[ i ];
        if( cur->mc_nID == ac_nEventID )
        {
          cur->mp_Kill();
          if( !ac_bKillAll )
            return;
        }
      }
    }

      /**
        * Query idle state.
        * @return true if the thread is currently free
        */
    bool mf_bIsIdle() const
    {
      const Lock L( *mc_pLock );
      return mv_eStatus == mc_eIdle;
    }

      /**
        * Query queue.
        * @return 0 if idle, else number of items left
        */
    typename mt_Queue::size_type mf_nGetNumInQueue() const
    {
      const Lock L( *mc_pLock );
      return m_Queue.size();
    }

  private:
    mt_eStatus                mv_eStatus;
    mt_Queue                  m_Queue;
    IEventDispatcher<tType>*  m_pListener;
    const WaitableObject*     mc_pWorkToDo;
    const CriticalSection*    mc_pLock;
  };

}

#endif //#ifndef __EVENTQUEUETHREAD_H__
