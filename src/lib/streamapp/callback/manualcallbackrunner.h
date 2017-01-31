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
 
#ifndef __MANUALCALLBACKRUNNER_H__
#define __MANUALCALLBACKRUNNER_H__

#include "eofcheck.h"
#include "callbackrunner.h"

namespace streamapp
{

    /**
      * ManualCallbackRunner
      *   class to manually call a Callback in a loop.
      *   Very handy for testing without a soundcard.
      *   Requires EofCheck, since it doesn't make much sense
      *   just putting a Callback in an infinite loop
      ******************************************************* */
  class ManualCallbackRunner : public CallbackRunner
  {
  public:
      /**
        * Constructor.
        * @param a_Checker the IEofCheck to use.
        */
    ManualCallbackRunner( IEofCheck& a_Checker );

      /**
        * Destructor.
        */
    ~ManualCallbackRunner();

      /**
        * Implementation of the CallbackRunner method.
        * Runs until EofCheck's end.
        */
    bool mp_bStart( Callback& a_Callback );

      /**
        * Run a specified number of times.
        * Or, until EofCheck's end
        * @param a_Callback the Callback to run
        * @param nTimes the number of times
        * @return always true
        */
    bool mp_bStart( Callback& a_Callback, const unsigned nTimes );

      /**
        * Set the period.
        * Sets the number of milliseconds to Sleep() between calls of Callback.
        * @param ac_nMillisSeconds the time
        */
    INLINE void mp_SetPeriod( const unsigned ac_nMillisSeconds )
    {
      mv_nMillisSeconds = ac_nMillisSeconds;
    }

      /**
        * Get the current period.
        * @return the period
        */
    INLINE const unsigned& mf_nGetPeriod() const
    {
      return mv_nMillisSeconds;
    }

      /**
        * Implementation of the CallbackRunner method.
        */
    INLINE bool mp_bStop()
    {
      return false;
    }

      /**
        * Implementation of the CallbackRunner method.
        */
    INLINE bool mf_bIsRunning() const
    {
      return false;
    }

      /**
        * Implementation of the CallbackRunner method.
        */
    INLINE bool mf_bIsBlocking() const
    {
      return true;
    }

  private:
    IEofCheck*  m_Checker;
    unsigned    mv_nMillisSeconds;

    ManualCallbackRunner( const ManualCallbackRunner& );
    ManualCallbackRunner& operator = ( const ManualCallbackRunner& );
  };
}

#endif //#ifndef __MANUALCALLBACKRUNNER_H__
