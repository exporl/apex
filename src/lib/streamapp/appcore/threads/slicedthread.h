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

#ifndef __SLICEDTHREAD_H__
#define __SLICEDTHREAD_H__

#include "containers/dynarray.h"
#include "criticalsection.h"
#include "thread.h"
using namespace streamapp;

namespace appcore
{

/**
  * SlicedThreadTask
  *   a task that can be executed in a SlicedThread.
  *   Clients deriving from this implement the mf_bWork()
  *   method in which they do their short job.
  ******************************************************* */
class SlicedThreadTask
{
protected:
    /**
      * Protected Constructor.
      */
    SlicedThreadTask()
    {
    }

public:
    /**
      * Destructor.
      */
    virtual ~SlicedThreadTask()
    {
    }

    /**
      * Implements the task.
      * After registering with a SlicedThread,
      * this method will get called periodically.
      * The method must not block too long.
      * @return true if the task needs more time,
      * false if it's finished
      */
    virtual bool mf_bWork() = 0;
};

/**
  * SlicedThread
  *   a thread that spreads it's cpu time over clients.
  *   Use this if you have a number of small tasks that
  *   have to be executed periodically, and when it's not
  *   worth having a thread for every single task.
  *   The thread calls all registered clients after each other,
  *   giving them each the time to do their job.
  ************************************************************* */
class SlicedThread : public IThread
{
public:
    /**
      * Constructor.
      * Does not start the thread.
      * @param ac_sName the thread name
      */
    SlicedThread(const String &ac_sName);

    /**
      * Destructor.
      */
    ~SlicedThread();

    /**
      * Register a task.
      * The task's mf_bWork() will be called as soon as possible.
      * Registering the same task twice is not possible.
      * @param ac_pTask the client task
      */
    void mp_RegisterClientTask(SlicedThreadTask *const ac_pTask);

    /**
      * Remove a task from the list.
      * @param ac_pTask the client task
      */
    void mp_UnRegisterClientTask(SlicedThreadTask *const ac_pTask);

    /**
      * Check if the given task is registered.
      * @param ac_pTask the task pointer
      * @return true if in list
      */
    bool mf_bHasTask(SlicedThreadTask *const ac_pTask);

    /**
      * Get the current number of registered tasks.
      * @return the number
      */
    unsigned mf_nGetNumTasks() const;

    /**
      * Get the task at the given index.
      * @param ac_nIndex the index, must be < mf_nGetNumTasks()
      * @return the task pointer
      */
    SlicedThreadTask *mf_pGetTask(const unsigned ac_nIndex);

    /**
      * Set the maximum amount of time allowed between two task calls.
      * If all tasks are having their mf_bWork() method returning false,
      * the thread will idle for this amount of time to save cpu usage.
      * The default is half a second.
      * @param ac_nTimeInMilliSeconds
      */
    void mp_SetMaxIdleTime(const unsigned ac_nTimeInMilliSeconds);

private:
    /**
      * Implementation of the IThread method.
      */
    void mp_Run();

    typedef DynamicArray<SlicedThreadTask *> mt_Tasks;

    mt_Tasks m_TaskList;
    bool mv_bNotify;
    unsigned mv_nCurIndex;
    unsigned mv_nIdleTime;
    SlicedThreadTask *mv_pCurTask;
    const CriticalSection mc_ListLock;
    const CriticalSection mc_TaskLock;
};
}

#endif //#ifndef __SLICEDTHREAD_H__
