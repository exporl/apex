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

#ifndef __TRACERMEMORY_H__
#define __TRACERMEMORY_H__

#include "appcore/threads/locks.h"
#include "containers/rawmemory.h"
#include "tracer.h"
using namespace appcore;
using namespace streamapp;

namespace utils
{

/**
  * TracerMemory
  *   a Tracer implementation that logs to memory.
  *   All messages are placed in a RawMemory block
  *   as subsequent null-terminated strings.
  *   When the maximum size is reached, half of the
  *   block is cleared, the rest is shifted down.
  ************************************************ */
class TracerMemory : public Tracer
{
public:
    /**
      * Constructor.
      * @param ac_nInitSize the initial size of the memory
      * @param ac_nMaxSize the maximum size of the memory
      */
    TracerMemory(const unsigned ac_nInitSize = 1024,
                 const unsigned ac_nMaxSize = 4096);

    /**
      * Destructor.
      */
    ~TracerMemory();

    /**
      * Set to call sf_TraceDebug() when a message is traced.
      * @param ac_bSet true for yes
      */
    void mp_SetTraceDebug(const bool ac_bSet = true)
    {
        mv_bDebug = ac_bSet;
    }

    /**
      * Get a reference to the memory block.
      * @return a const reference. Const Is Good.
      */
    const RawMemory &mf_GetTrace() const
    {
        return m_Trace;
    }

    /**
      * Get the actual number of bytes traced in the memory.
      * @return the number
      */
    const unsigned &mf_nGetTraceLength() const
    {
        return mv_nWritePoint;
    }

protected:
    /**
      * Implementation of the Tracer method.
      */
    void mp_TraceOne(const String &ac_sMessage);

private:
    RawMemory m_Trace;
    bool mv_bDebug;
    unsigned mv_nWritePoint;
    const unsigned mc_nMaxSize;
    const CriticalSection mc_Lock;

    TracerMemory(const TracerMemory &);
    TracerMemory &operator=(const TracerMemory &);
};
}

#endif //#ifndef __TRACERMEMORY_H__
