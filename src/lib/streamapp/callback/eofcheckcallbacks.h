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
 
#ifndef __EOFCALLBACK_H__
#define __EOFCALLBACK_H__

#include "eofcheck.h"
#include "multicallback.h"
#include "manualcallbackrunner.h"
#include "appcore/events/callbackevent.h"
#include "appcore/threads/waitableobject.h"

namespace streamapp
{

    /**
      * EofCheckWaitCallback
      *   combines EofCheck and WaitObjectCallback in a Callback.
      *   When all inputs registered with the IEofCheck are finished,
      *   the WaitableObject is signaled.
      *   @see IEofCheck
      *   @see MultiCallback
      *   @see WaitObjectCallback
      *************************************************************** */
  class EofCheckWaitCallback : public Callback
  {
  public:
      /**
        * Constructor.
        * @param ac_eType the type of IEofCheck to use.
        */
    EofCheckWaitCallback( const EofCheckFactory::mt_eCheckType ac_eType = EofCheckFactory::mv_eSimple ) :
      mc_pChecker( EofCheckFactory::sf_CreateCheck( ac_eType ) )
    {
    }

      /**
        * Destructor.
        */
    ~EofCheckWaitCallback()
    {
      delete mc_pChecker;
    }

      /**
        * Get the IEofCheck used.
        * @return pointer
        */
    INLINE IEofCheck* mp_GetEofCheck()
    {
      return mc_pChecker;
    }

      /**
        * Get the object that will be signaled.
        * @return a WaitableObject refernce
        */
    INLINE const appcore::WaitableObject& mf_GetWaitObject() const
    {
      return m_Waiter.mf_GetWaitObject();
    }

      /**
        * Register the main callback where streaming will be done.
        * This callback is called first, then the IEofCheck.
        * Must be used only once.
        * @param a_Callback the master callback
        */
    void mp_RegisterMasterCallback( Callback& a_Callback )
    {
      assert( m_Callback.mf_nGetNumItems() == 0 ); //allow setting only once
      m_Callback.mp_AddItem( &a_Callback );
      m_Callback.mp_AddItem( mc_pChecker );
      mc_pChecker->mp_InstallCallback( m_Waiter );
    }

      /**
        * Implementation of the Callback method.
        */
    INLINE void mf_Callback()
    {
      m_Callback.mf_Callback();
    }

      /**
        * Start running the callback in a while loop.
        */
    void mf_DoManual()
    {
      ManualCallbackRunner runner( *mc_pChecker );
      runner.mp_bStart( m_Callback );
    }

  private:
    IEofCheck* const            mc_pChecker;
    MultiCallback               m_Callback;
    appcore::WaitObjectCallback m_Waiter;
  };

    /**
      * EofCheckEventCallback
      *   combines EofCheck and CallbackEventPoster in a Callback.
      *   When all inputs registered with the IEofCheck are finished,
      *   the IEventPoster posts an event.
      *   @see IEofCheck
      *   @see MultiCallback
      *   @see CallbackEventPoster
      *************************************************************** */
  template< class tType >
  class EofCheckEventCallback : public Callback
  {
  public:
      /**
        * Constructor.
        * @param ac_nEventType the event to post when done
        * @param ac_eType the IEofCheck type
        */
    EofCheckEventCallback( const tType ac_nEventType, const EofCheckFactory::mt_eCheckType ac_eType = EofCheckFactory::mv_eSimple ) :
      mc_pChecker( EofCheckFactory::sf_CreateCheck( ac_eType ) ),
      m_Poster( ac_nEventType )
    {
    }

      /**
        * Destructor.
        */
    ~EofCheckEventCallback()
    {
    }

      /**
        * Get the IEofCheck used.
        * @return pointer
        */
    INLINE IEofCheck* mp_GetEofCheck()
    {
      return mc_pChecker;
    }

      /**
        * Get the IEventPoster object.
        * @return a reference
        */
    INLINE appcore::CallbackEventPoster<tType>& mp_GetEventPoster()
    {
      return m_Poster;
    }

      /**
        * Register the main callback where streaming will be done.
        * This callback is called first, then the IEofCheck.
        * Must be used only once.
        * @param a_Callback the master callback
        */
    void mp_RegisterMasterCallback( Callback& a_Callback )
    {
      assert( m_Callback.mf_nGetNumItems() == 0 ); //allow setting only once
      m_Callback.mp_AddItem( &a_Callback );
      m_Callback.mp_AddItem( mc_pChecker );
      mc_pChecker->mp_InstallCallback( m_Poster );
    }

      /**
        * Implementation of the Callback method.
        */
    INLINE void mf_Callback()
    {
      m_Callback.mf_Callback();
    }

      /**
        * Runs callback in a while loop.
        * Use for testing.
        */
    void mf_DoManual()
    {
      ManualCallbackRunner runner( *mc_pChecker );
      runner.mp_bStart( m_Callback );
    }

  private:
    IEofCheck* const                mc_pChecker;
    MultiCallback                   m_Callback;
    appcore::CallbackEventPoster<tType> m_Poster;
  };

}

#endif //#ifndef __EOFCALLBACK_H__
