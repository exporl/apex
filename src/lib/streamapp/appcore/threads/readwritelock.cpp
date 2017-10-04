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

#include "readwritelock.h"
#include "locks.h"
#include "thread.h"

#include <QtGlobal>

//#include "utils/tracer.h"
//#include "utils/stringutils.h"
// using namespace utils;
using namespace appcore;

ReadWriteLock::ReadWriteLock()
    : m_nWriters(0),
      m_nWritersWaiting(0),
      m_nWriterID(-1),
      m_ReaderIDs(3),
      m_Readers(3),
      mc_Waiter(),
      mc_Lock()
{
}

ReadWriteLock::~ReadWriteLock()
{
    Q_ASSERT(m_ReaderIDs.mf_nGetNumItems() == 0);
    Q_ASSERT(m_nWriters == 0);
}

void ReadWriteLock::mf_EnterRead() const
{
    const int nThreadID = IThread::mf_nGetCurrentThreadID();
    const Lock L(mc_Lock);

    for (;;) {
        // get index if the reader is already reading
        unsigned i;
        for (i = 0; i < m_ReaderIDs.mf_nGetNumItems(); ++i)
            if (m_ReaderIDs.mf_GetRItem(i) == nThreadID)
                break;

        // see if we can enter, this is the case if
        //- there is a read lock already
        //- there are no writers
        //- one writer and the same
        if (i < m_ReaderIDs.mf_nGetNumItems() ||
            m_nWriters + m_nWritersWaiting == 0 ||
            (nThreadID == m_nWriterID && m_nWriters > 0)) {
            if (i < m_ReaderIDs.mf_nGetNumItems()) {
                // m_Readers.mp_SetItem( i, m_Readers.mf_GetRItem( i ) + 1 );
                ++m_Readers(i);
                // Tracer::sf_TraceDebug( "incremented " + toString( nThreadID )
                // );
            } else {
                m_ReaderIDs.mp_AddItem(nThreadID);
                m_Readers.mp_AddItem(1);
                // Tracer::sf_TraceDebug( "added " + toString( nThreadID ) );
            }

            return;
        }

        const UnLock UL(mc_Lock);
        mc_Waiter.mf_eWaitForSignal(50);
    }
}

void ReadWriteLock::mf_LeaveRead() const
{
    const int nThreadID = IThread::mf_nGetCurrentThreadID();
    const Lock L(mc_Lock);

    // find reader index
    for (unsigned i = 0; i < m_ReaderIDs.mf_nGetNumItems();
         ++i) // must be checked everytime here!!
    {
        if (m_ReaderIDs(i) == nThreadID) {
            // decrement count
            const unsigned nCount = m_Readers(i) - 1;

            // remove or update count
            if (nCount == 0) {
                m_ReaderIDs.mp_RemoveItemAt(i);
                m_Readers.mp_RemoveItemAt(i);
                mc_Waiter.mp_SignalObject();
                // Tracer::sf_TraceDebug( "removed " + toString( nThreadID ) );
            } else {
                m_Readers.mp_SetItem(i, nCount);
                // Tracer::sf_TraceDebug( "decremented " + toString( nThreadID )
                // );
            }
        }
    }
}

void ReadWriteLock::mf_EnterWrite() const
{
    const int nThreadID = IThread::mf_nGetCurrentThreadID();
    const Lock L(mc_Lock);

    for (;;) {
        // see if we can enter
        // this is the case if:
        //- no readers/writers
        //- one reader and the same
        //- one writer and the same
        if (m_ReaderIDs.mf_nGetNumItems() + m_nWriters == 0 ||
            nThreadID == m_nWriterID ||
            (m_ReaderIDs.mf_nGetNumItems() == 1 &&
             m_ReaderIDs.mf_GetRItem(0) == nThreadID)) {
            m_nWriterID = nThreadID;
            ++m_nWriters;
            // Tracer::sf_TraceDebug( "added w " + toString( nThreadID ) );
            break;
        }

        // we couldn't enter so wait some time
        ++m_nWritersWaiting;
        mc_Lock.mf_Leave();
        mc_Waiter.mf_eWaitForSignal(50);
        mc_Lock.mf_Enter();
        --m_nWritersWaiting;
    }
}

void ReadWriteLock::mf_LeaveWrite() const
{
    const Lock L(mc_Lock);

    Q_ASSERT(m_nWriters > 0 &&
             m_nWriterID == IThread::mf_nGetCurrentThreadID());

    if (--m_nWriters == 0) {
        m_nWriterID = 0;
        mc_Waiter.mp_SignalObject();
        // Tracer::sf_TraceDebug( "removed w " + toString( m_nWriterID ) );
    }
}
