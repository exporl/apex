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

#ifndef __READWRITELOCK_H__
#define __READWRITELOCK_H__

#include "containers/dynarray.h"
#include "criticalsection.h"
#include "defines.h"
#include "waitableobject.h"
using namespace streamapp;

namespace appcore
{

/**
  * ReadWriteLock
  *   a CriticalSection that allows multiple readers at once.
  *   -# Multiple readers can have the lock at once, but only one
  *   writer is allowed at any time.
  *   -# A writer can only enter the lock if all other readers
  *   and writers have left it.
  *   -# Writers have higher priority then readers when obtaining
  *   -# If a thread has a read lock it can get a write lock if
  *   there are no other readers
  *   -# If a thread has a write lock it can get a read lock.
  *   -# Recursive locking is allowed
  **************************************************************** */
class ReadWriteLock
{
public:
    /**
      * Constructor.
      */
    ReadWriteLock();

    /**
      * Destructor.
      * Results are undefined if lock is being held.
      */
    ~ReadWriteLock();

    /**
      * Lock for reading.
      * Every call must have a matching mf_LeaveRead() call.
      */
    void mf_EnterRead() const;

    /**
      * Unlock from reading.
      * Result undefined if not locked.
      */
    void mf_LeaveRead() const;

    /**
      * Lock for writing.
      * Every call must have a matching mf_LeaveWrite() call.
      */
    void mf_EnterWrite() const;

    /**
      * Unlock from writing.
      * Result undefined if not locked.
      */
    void mf_LeaveWrite() const;

private:
    mutable unsigned m_nWriters;
    mutable unsigned m_nWritersWaiting;
    mutable int m_nWriterID;
    mutable DynamicArray<int> m_ReaderIDs;
    mutable DynamicArray<unsigned> m_Readers;

    const WaitableObject mc_Waiter;
    const CriticalSection mc_Lock;

    ReadWriteLock(const ReadWriteLock &);
    const ReadWriteLock &operator=(const ReadWriteLock &);
};

/**
  * ReadLock
  *   obtains read from a ReadWriteLock.
  ************************************** */
class ReadLock
{
public:
    /**
      * Constructor.
      * Enters the lock for reading.
      * @param ac_Lock the one to enter
      */
    INLINE ReadLock(const ReadWriteLock &ac_Lock) : mc_Lock(ac_Lock)
    {
        mc_Lock.mf_EnterRead();
    }

    /**
      * Destructor.
      * Leaves the lock from reading.
      */
    INLINE ~ReadLock()
    {
        mc_Lock.mf_LeaveRead();
    }

private:
    const ReadWriteLock &mc_Lock;

    ReadLock(const ReadLock &);
    const ReadLock &operator=(const ReadLock &);
};

class WriteLock
{
public:
    /**
      * Constructor.
      * Enters the lock for write access.
      * @param ac_Lock the one to enter
      */
    INLINE WriteLock(const ReadWriteLock &ac_Lock) : mc_Lock(ac_Lock)
    {
        mc_Lock.mf_EnterWrite();
    }

    /**
      * Destructor.
      * Leaves the lock from writing.
      */
    INLINE ~WriteLock()
    {
        mc_Lock.mf_LeaveWrite();
    }

private:
    const ReadWriteLock &mc_Lock;

    WriteLock(const WriteLock &);
    const WriteLock &operator=(const WriteLock &);
};
}

#endif //#ifndef __READWRITELOCK_H__
