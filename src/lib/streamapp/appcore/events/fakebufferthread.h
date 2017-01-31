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
 
#ifndef __FAKEBUFFERTHREAD_H__
#define __FAKEBUFFERTHREAD_H__

#include "stream.h"
#include "appcore/events/events.h"
#include "appcore/threads/thread.h"
#include "appcore/threads/locks.h"
#include "appcore/threads/criticalsection.h"
#include "containers/matrix.h"

namespace appcore
{

    /**
      * FakeBufferThread
      *   Reads an InputStream and posts events for it.
      *   Reads until the input is finished,
      *   posting an ac_tStreamEvent for every Stream read,
      *   and posting the ac_tDoneEvent when the InputStream is done.
      *   Always deletes input.
      *************************************************************** */
  template< class tType = int >
  class FakeBufferThread : public IEventPoster<tType>, public IThread
  {
  public:
      /**
        * Constructor.
        * Won't start the thread.
        * @param ac_tStreamEvent the event code to post with every Stream
        * @param ac_tDoneEvent the event code to post when the input is read
        * @param ac_sName the thread name
        */
    FakeBufferThread( const tType& ac_tStreamEvent, const tType& ac_tDoneEvent, const std::string& ac_sName ) :
        IThread( ac_sName ),
      m_pInput( 0 ),
      mv_nSleep( 5 ),
      mc_tStream( ac_tStreamEvent ),
      mc_tDone( ac_tDoneEvent )
    {}

      /**
        * Destructor.
        */
    ~FakeBufferThread()
    {
      mp_Stop( 1000 );
      const Lock L( mc_Lock );
      delete m_pInput;
    }

      /**
        * Install the input to read from.
        * @param a_Input a PositionableInputStream
        */
    void mp_InstallInput( streamapp::PositionableInputStream& a_Input )
    {
      const Lock L( mc_Lock );
      delete m_pInput;
      m_pInput = &a_Input;
    }

      /**
        * Seek position.
        * @param ac_lPosition the new position
        */
    void mp_SetPosition( const unsigned long ac_lPosition )
    {
      const Lock L( mc_Lock );
      m_pInput->mp_SeekPosition( ac_lPosition );
    }

      /**
        * Set the period to sleep between each read.
        * @param ac_nMilliSeconds the period in milliseconds
        */
    void mp_SetSleep( const unsigned ac_nMilliSeconds )
    { mv_nSleep = ac_nMilliSeconds; }

      /**
        * Thread function.
        * Reads input until eof, posting events.
        */
    void mp_Run()
    {
      if( m_pInput )
      {
        mc_Lock.mf_Enter();
        bool bEof = m_pInput->mf_bIsEndOfStream();
        mc_Lock.mf_Leave();
        while( !bEof )
        {
          if( mf_bThreadShouldStop() )
            return;
          mc_Lock.mf_Enter();
          Stream& rdata = const_cast<Stream&> ( m_pInput->Read() );
          bEof = m_pInput->mf_bIsEndOfStream();
          mc_Lock.mf_Leave();
          IEventPoster<tType>::mp_DispatchEventToListener( mc_tStream, &rdata );
          if( bEof )
             IEventPoster<tType>::mp_DispatchEventToListener( mc_tDone );
          mf_bWait( mv_nSleep );
        }
      }
    }

  private:
    streamapp::PositionableInputStream* m_pInput;
    unsigned                            mv_nSleep;
    const tType                         mc_tStream;
    const tType                         mc_tDone;
    const CriticalSection               mc_Lock;   //protects m_pInput
  };

}

#endif //#ifndef __FAKEBUFFERTHREAD_H__
