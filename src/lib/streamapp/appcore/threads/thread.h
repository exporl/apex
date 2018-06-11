/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef __ITHREAD_H_
#define __ITHREAD_H_

#include "../../defines.h"

#include "criticalsection.h"
#include "waitableobject.h"

#ifndef S_C6X
#include <string>
typedef std::string String;
#else
#include <std.h>
#endif

namespace appcore
{

/**
  * IThread
  *   thread encapsulation.
  *   Subclasses do their thing in mp_Run(),
  *   which is the actual thread function.
  ****************************************** */
class IThread
{
public:
    /**
      * Constructor.
      * This does not start the thread, it just sets up it's internals so
      * it's ready to start.
      * @param ac_sName a name for the thread. On platforms that support it,
      * this will set the actual thread name via the kernel API, which is nice
     * for debugging.
      */
    IThread(const String &ac_sName);

    /**
      * Destructor.
      * Deleting an IThread bject that is running is Bad,
      * so make sure the mp_Run() function has exited properly first.
      */
    virtual ~IThread();

    /**
      * Priorities.
      */
    enum mt_ePriority {
        priority_idle = 0, //!< this will completely disable thread execution
        priority_verylow = 1,
        priority_low = 3,
        priority_normal = 5,
        priority_high = 7,
        priority_realtime = 9
    };

    /**
      * Thread function.
      * Should check mf_bThreadShouldStop regularly, eg
      * could look like this:
      *
      * @code
      * void mp_Run()
      * {
      *   while (!mf_bThreadShouldStop() )
      *   {
      *     bool busy = false;
      *
      *     if( mf_bThreadShouldStop() )
      *       return;
      *
      *     busy = true;
      *
      *     //do interesting stuff here
      *
      *     if( !busy )
      *       mf_bWait( 500 );
      *   }
      * }
      * @endcode
      */
    virtual void mp_Run() = 0;

    /**
      * Start the thread with the same priority as the parent thread.
      * Doesn't do anything if the thread was already started.
      */
    void mp_Start();

    /**
      * Start the thread with the given priority.
      * Doesn't do anything if the thread was already started.
      * @param mt_ePriority the priority
      * @see mt_ePriority
      */
    void mp_Start(const mt_ePriority mt_ePriority);

    /**
      * Tries to stop the thread.
      * Causes mf_bThreadShouldStop() to return true. If mp_Run() doesn't exit
      * within the specified timeout, the thread gets forcibly killed, which
      * is quite a bad thing to happen,as it could still be holding locks, etc
      * which might be needed by other parts of the program.
      * @see mf_bThreadShouldStop()
      */
    void mp_Stop(const int ac_nTimeoutInMSec);

    /**
      * Query state.
      * @return true if the thread is currently running
      */
    bool mf_bIsRunning() const;

    /**
      * Set the flag to tell the thread it should stop.
      * Calling this means that mf_bThreadShouldStop() returns true.
      * @see mf_bThreadShouldStop()
      */
    void mp_SetThreadShouldStop();

    /**
      * Check whether the thread has been told to stop running.
      * Threads need to check this regularly, and if it returns true,
      * they should return their run() method asap.
      * @see mp_stop()
      */
    INLINE bool mf_bThreadShouldStop() const
    {
        return mv_bShouldStop;
    }

    /**
      * Waits for the thread to stop.
      * This will wait until mf_bIsRunning() returns false,
      * or until the timeout expires.
      * @param ac_nTimeoutInMSec the time to wait, in milliseconds; -1 means
     * forever
      * @return true if the thread exits, or false if the timeout expires first.
      */
    bool mf_bWaitForThreadToStop(const int ac_nTimeoutInMSec) const;

    /**
      * Set the thread's priority.
      * @see mt_ePriority
      * @param ac_Priority the new priority
      */
    void mp_SetPriority(const IThread::mt_ePriority ac_Priority);

    /**
      * Set the caller's thread priority.
      * @see mt_ePriority
      * @param ac_Priority the new priority
      */
    static void
    sf_SetCurrentThreadPriority(const IThread::mt_ePriority ac_Priority);

    /**
      * Make the thread wait for a notification.
      * This puts the thread into sleep until the timeout period expires, or
      * until another thread calls the mf_NotifyWait() method to wake it up.
      * @param ac_nTimeoutInMSec the time to wait, in milliseconds; -1 means
     * forever
      * @return true if the event has been signalled, or false if the timeout
     * expires.
      */
    bool mf_bWait(const int ac_nTimeoutInMSec) const;

    /**
      * Wake up the thread.
      * If the thread has called the mf_bWait() method, this will wake it up.
      * @see mf_bWait
      */
    void mf_NotifyWait() const;

    /**
      * Get the thread's name.
      * @return string reference
      */
    INLINE const String &mf_sGetName() const
    {
        return mc_sName;
    }

    /**
      * Get the thread's ID.
      * @return the unique identifier
      */
    INLINE int mf_nGetThreadID() const
    {
        return mv_nThreadID;
    }

    /**
      * Get the caller's thread ID.
      * @return the unique identifier
      */
    static int mf_nGetCurrentThreadID();

    /**
      * System independent sleep.
      * This can be called from any thread that needs to pause.
      * @param ac_nMilliseconds the time to sleep, -1 for forever
      */
    static void sf_Sleep(const int ac_nMilliseconds);

    /**
      * Get the number of threads that are currently running.
      * @see sf_StopAllThreads()
      * @return the number of IThreads that are running
      */
    static unsigned sf_nGetNumRunningThreads();

    /**
      * Tries to stop all threads that are currently running.
      * @see mp_Stop()
      * @param ac_nTimeoutInMSec
      */
    static void sf_StopAllThreads(const int ac_nTimeoutInMSec);

#ifdef S_WIN32
    static void *sm_hAccurateSleep;
#endif

private:
    const String mc_sName;
    void *volatile mv_hThreadHandle;
    int mv_nThreadID;
    mt_ePriority mv_ePriority;
    bool mv_bShouldStop;
    const WaitableObject mc_Waiter;
    const WaitableObject mc_StartWaiter;
    const CriticalSection mc_StartStopLock;

    friend void f_ThreadEntryPoint(void *a_pUserData);

    /**
      * Thread entry point.
      * Implemented in platform specific code.
      * @param ac_pThreadToRun the IThread's "this"
      */
    static void sf_ThreadEntryPoint(IThread *const ac_pThreadToRun);
};

void f_ThreadEntryPoint(void *a_pUserData);
}

#endif //#ifndef __ITHREAD_H_
