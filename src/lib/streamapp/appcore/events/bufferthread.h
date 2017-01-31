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
 
#include "processevent.h"
#include "utils/checks.h"
#include "appcore/threads/locks.h"
#include "appcore/threads/thread.h"
#include "appcore/threads/waitableobject.h"
#include "processors/buffer.h"
#include "containers/pingpong.h"

using namespace utils;
using namespace appcore;
using namespace streamapp;

#include <string>
#include <iostream>

namespace appcore
{

  /**
    * BufferThread
    *   use when mutual access to a sample buffer in an I/O thread
    *   (producer) is needed from a processing thread (consumer).
    *   Uses a typical pingpong buffer scheme: while one buffer gets filled
    *   with samples, the other buffer can be accessed freely or processing.
    *   This avoids having to the entire buffer each time it is full.
    *
    *   Operation: 2 buffers are created at start. Thread0 starts filling buffer0,
    *   and posts a tBufferEvent when it's full. This is catched here, and if the lock
    *   is free, a bufferswitch occurs, so thread0 is now filling buffer1. At the same
    *   time a tBufferEvent is posted to a_Dispatcher with buffer0 as event data, so
    *   thread1 can start processing this buffer.
    *
    *   Apart from bufferevents, any event can be dispatched between the
    *   2 threads. Since there is no queue, if one event is dispatched, all events
    *   dispatched before the first one completes are simply ignored.
    *   (and a "buffer overrun" message is printed)
    *   Everything is thread-safe.
    ********************************************************************************** */
  template< class tType = int, tType tBufferEvent = -1 >
  class BufferThread : public IThread, public IEventDispatcher<tType>
  {
  public:
      /**
        * Constructor.
        * @param a_Dispatcher the dispatcher to call
        * @param ac_pBuffer the buffer proc to use (filled in producer)
        * @param ac_sName the thread's name
        */
    BufferThread( IEventDispatcher<tType>* a_Dispatcher, streamapp::BasicBuffer* const ac_pBuffer, const std::string& ac_sName ) :
        IThread( ac_sName ),
      m_pDispatcher( PtrCheck( a_Dispatcher ) ),
      mc_pBuffer( PtrCheck( ac_pBuffer ) ),
      mc_pBufferCallback( new ProcessEventPoster<tType>( tBufferEvent ) ),
      m_DataCore( ac_pBuffer->mf_nGetNumInputChannels(), ac_pBuffer->mf_nGetOutputSize() ),
      mv_bBusy( false ),
      mv_eType( 0 ),
      mv_pData( 0 ),
      mc_tBufferEvent( tBufferEvent ),
      mc_pWaiter( new WaitableObject() ),
      mc_pSection( new CriticalSection() )
    {
      ac_pBuffer->mp_SetBufferToFill( m_DataCore.mf_GetTxBuffer() );
      ac_pBuffer->mp_SetCallback( mc_pBufferCallback );
      mc_pBufferCallback->mp_InstallEventDispatcher( this );
    }

      /**
        * Destructor.
        */
    ~BufferThread()
    {
      mp_Stop( 1000 );
    }

      /**
        * Dispatch.
        * Prints error if the lock is not free (aka when thread1 is still busy with one of the buffers.
        * @param ac_eEventType the event type, triggers bufferswitch if it's a tBufferEvent
        * @param a_pData the event data, Stream* for a tBufferEvent
        */
    void mp_DispatchEvent( const tType& ac_eEventType, gt_EventData a_pData )
    {
      const Lock L( *mc_pSection );
      if( !mv_bBusy )
      {
        mv_eType = ac_eEventType;
        if( ac_eEventType == mc_tBufferEvent )
        {
          m_DataCore.mp_BufferSwitch();
          mc_pBuffer->mp_SetBufferToFill( m_DataCore.mf_GetTxBuffer() );
          mv_pData = m_DataCore.mf_GetRxBuffer();
        }
        else
          mv_pData = a_pData;
        mc_pWaiter->mp_SignalObject();
      }
      else
        std::cout << IThread::mf_sGetName().c_str() << " thread - lost event" << std::endl;
    }

      /**
        * Thread function.
        */
    void mp_Run()
    {
      for(;;)
      {
        const WaitableObject::mt_eWaitResult eRes = mc_pWaiter->mf_eWaitForSignal( 500 );
        if( mf_bThreadShouldStop() )
          return;
        switch( eRes )
        {
          case WaitableObject::wait_ok :
          {
              //we can't have a lock on the listener dispatching, since that would
              //lock up our own dispatching which is exactly the contrary of what
              //a thread is meant for
              //so just lock the busy flag and check it when an event arrives
            mp_SetBusy( true );
            const tType nType = mv_eType;
            m_pDispatcher->mp_DispatchEvent( nType, mv_pData );
            mp_SetBusy( false );
            break;
          }
          case WaitableObject::wait_failed :
          {
            IThread::mp_Stop( 1000 );
          }
        }//end switch
      }//end for
    }

      /**
        * Replace dispatcher.
        * @param ac_pDispatcher the new dispatcher, must be non-zero
        */
    void mp_ReplaceDispatcher( IEventDispatcher<tType>* const ac_pDispatcher )
    {
      const Lock L( *mc_pSection );
      m_pDispatcher = ac_pDispatcher;
    }

      /**
        * Get current dispatcher.
        * @return the dispatcher
        */
    const IEventDispatcher<tType>* mf_pGetDispatcher() const
    { return m_pDispatcher; }

  private:
      /**
        * Set busy.
        * Protected by lock.
        */
    void mp_SetBusy( const bool ac_bBusy )
    {
      const Lock L( *mc_pSection );
      mv_bBusy = ac_bBusy;
    }

    IEventDispatcher<tType>*          m_pDispatcher;
    streamapp::BasicBuffer* const     mc_pBuffer;           //!< buffer to control
    ProcessEventPoster<tType>* const  mc_pBufferCallback;   //!< callback for buffer,called when buffer is full
    PingPongStreamBuf<StreamType>     m_DataCore;           //!< contains 2 buffers which are read/written sequentially

    volatile bool                     mv_bBusy;             //!< volatile to assure OS doesn't defer changes
    volatile tType                    mv_eType;
    volatile gt_EventData             mv_pData;

    const tType                       mc_tBufferEvent;

    WaitableObject* const             mc_pWaiter;           //!< waiter
    CriticalSection* const            mc_pSection;          //!< data lock
  };

}
