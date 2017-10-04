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

#ifndef __SEMAPHORES_H__
#define __SEMAPHORES_H__

namespace appcore
{

/**
  * WaitableObject
  *   allows threads to wait for events triggered by other threads.
  *   A thread can call mf_eWaitForSignal() on a WaitableObject,
  *   and this will suspend the calling thread until another
  *   thread wakes it up by calling the mp_SignalObject() method.
  ***************************************************************** */
class WaitableObject
{
public:
    /**
      * Constructor.
      */
    WaitableObject();

    /**
      * ~Destructor.
      */
    ~WaitableObject();

    /**
      * Return codes for mf_eWaitForSignal().
      */
    enum mt_eWaitResult {
        wait_ok,      //!< the object was signaled within the time specified
        wait_timeout, //!< the object was not signaled within the time specified
        wait_failed   //!< there was a serious error, like killing the object
    };

    /**
      * This will make the calling thread idle until timeout, or until
      * another thread calls mp_SignalObject().
      * @param ac_nTimeoutMilliSeconds the time to wait, use -1 to wait forever
      * @return one of mt_eWaitResult
      */
    mt_eWaitResult
    mf_eWaitForSignal(const int ac_nTimeoutMilliSeconds = -1) const;

    /**
      * Wakes up any threads that are currently waiting on this object.
      */
    void mp_SignalObject() const;

    /**
      * Resets the event to an unsignalled state.
      * If it's not already signalled, this does nothing.
      */
    void mp_ResetObject() const;

private:
    void *m_hHandle;

    WaitableObject(const WaitableObject &);
    const WaitableObject &operator=(const WaitableObject &);
};
}

#endif //#ifndef __SEMAPHORES_H__
