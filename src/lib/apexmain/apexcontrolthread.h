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
 
#ifndef __APEX_APEXCONTROLTHREAD_H__
#define __APEX_APEXCONTROLTHREAD_H__

#include <appcore/events/qt_events.h>
#include <appcore/events/eventqueuethread.h>

namespace apex
{

     /**
      * ApexControlThread
      *   thread responsible for controlling the trial sequence
      *   by waiting on events/timers in a queue
      ********************************************************* */
  class ApexControlThread : public appcore::EventQueueThread< tEventCode >
  {
  public:
      /**
        * Constructor.
        * @param a_pTarget the event target, normally ApexControl
        */
    ApexControlThread( QObject* a_pTarget ) :
        appcore::EventQueueThread<tEventCode>( "ApexControlThread", 0 ),
      m_Listener( a_pTarget )
    {
      appcore::EventQueueThread<tEventCode>::mp_SetTarget( &m_Listener );
    }

      /**
        * Destructor.
        */
    ~ApexControlThread()
    {}

  private:
      /**
        * Used to fire QCustomEvents
        */
    appcore::QtEventListener<tEventCode> m_Listener;
  };

}


#endif //#ifndef __APEX_APEXCONTROLTHREAD_H__
