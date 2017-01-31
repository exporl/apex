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
 
#ifndef __CALLBACKRUNNER_H__
#define __CALLBACKRUNNER_H__

#include "callback.h"

namespace streamapp
{

    /**
      * CallbackRunner
      *   interface for anything that can call a Callback's cb function
      *   in a periodic manner.
      *   Example: a soundcard, or @see ManualCallbackRunner
      ***************************************************************** */
  class CallbackRunner
  {
  protected:
      /**
        * Protected Constructor.
        */
    CallbackRunner()
    {
    }

  public:
      /**
        * Destructor.
        */
    virtual ~CallbackRunner()
    {
    }

      /**
        * Start running.
        * This will call a_Callback's mf_Callback function periodically.
        * Note that this can be blocking or non-blocking.
        * @param a_Callback the Callback to run
        * @return true if the call succeeds, false for error
        */
    virtual bool mp_bStart( Callback& a_Callback ) = 0;

      /**
        * Stop running.
        * @return true if the call succeeds, false for error
        */
    virtual bool mp_bStop() = 0;

      /**
        * Query status.
        * @return true after a succesfull start call returned and that call was blocking
        */
    virtual bool mf_bIsRunning() const = 0;

      /**
        * Query behaviour.
        * For a blocking runner, mp_bStart will only return after some condition form
        * the subclass is met. For the other case, mp_bStart returns immedeately but
        * there is still something running in another process or thread.
        * @return false for non-blocking
        */
    virtual bool mf_bIsBlocking() const = 0;
  };

}

#endif //#ifndef __CALLBACKRUNNER_H__
