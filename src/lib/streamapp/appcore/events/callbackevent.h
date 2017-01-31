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

#ifndef __CALLBACKEVENT_H__
#define __CALLBACKEVENT_H__

#include "appcore/events/events.h"
#include "appcore/threads/waitableobject.h"
#include "callback/callback.h"

namespace appcore
{
    /**
      * CallbackEventPoster
      *  posts event when Callback's mf_Callback is called
      **************************************************** */
  template<class tType = int>
  class CallbackEventPoster : public appcore::IEventPoster<tType>, public streamapp::Callback
  {
  public:
      /**
        * Constructor.
        * @param ac_nEventType the event to post
        * @param ac_pData the data to post
        */
    CallbackEventPoster( const tType& ac_nEventType, const gt_EventData ac_pData = 0 ) :
      mc_nEventType( ac_nEventType ),
      mc_pData( ac_pData )
    {}

      /**
        * Destructor.
        */
    ~CallbackEventPoster()
    {}

      /**
        * The callback.
        * Posts the event.
        */
    virtual void mf_Callback()
    { IEventPoster<tType>::mp_DispatchEventToListener( mc_nEventType, const_cast<gt_EventData>( mc_pData ) ); }

  private:
    const tType         mc_nEventType;
    const gt_EventData  mc_pData;

    CallbackEventPoster( const CallbackEventPoster& );
    CallbackEventPoster& operator = ( const CallbackEventPoster& );
  };

    /**
      * WaitObjectCallback
      *  signals a waitable object when called back
      ********************************************* */
  class WaitObjectCallback : public streamapp::Callback
  {
  public:
      /**
        * Constructor.
        */
    WaitObjectCallback()
    {}

      /**
        * Destructor.
        */
    ~WaitObjectCallback()
    {}

      /**
        * The callback.
        * Signals the object.
        */
    virtual void mf_Callback()
    { m_WaitObject.mp_SignalObject(); }

      /**
        * Get the waitobject.
        * @return const reference to the object
        */
    const WaitableObject& mf_GetWaitObject() const
    { return m_WaitObject; }

  private:
    const WaitableObject m_WaitObject;

    WaitObjectCallback( const WaitObjectCallback& );
    WaitObjectCallback& operator = ( const WaitObjectCallback& );
  };

}

#endif //#ifndef __CALLBACKEVENT_H__
