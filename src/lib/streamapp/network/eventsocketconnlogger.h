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
 
#ifndef __SOCKETCONNEVENTLOGGER_H__
#define __SOCKETCONNEVENTLOGGER_H__

#include "eventsocketconn.h"

namespace network
{
    /**
      * EventSocketConnLogger
      *   logger for ISocketConnection events.
      *   Logs via TRACE macro, so it's suitable to create
      *   a memory based logger for your app.
      *   Logs everything, including errors.
       **************************************************** */
  class EventSocketConnLogger : public IEventDispatcher<gt_eSockConnEvent>
  {
  public:
      /**
        * Constructor.
        */
    EventSocketConnLogger();

      /**
        * Destructor.
        */
    ~EventSocketConnLogger();

      /**
        * Implementation of the IEventPoster method.
        */
    void mp_DispatchEvent( const gt_eSockConnEvent& ac_tType, gt_EventData a_pData );
  };

}

#endif //#ifndef __SOCKETCONNEVENTLOGGER_H__
