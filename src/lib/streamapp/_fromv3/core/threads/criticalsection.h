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

/**
  *      -- v3tov2 plugin --
  * Do not modify this file by hand,
  * your changes will be overwritten.
  * <^>
  * [ prog ] apex
  * [ file ] core/threads/criticalsection.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_CRITICALSECTION_H__
#define __STR_CRITICALSECTION_H__

#include "../_main.h"

#if defined S_WIN32
// nothing to include here
#elif defined S_POSIX
#include "../../../_archs/linux/linux_headers.h"
#endif

S_NAMESPACE_BEGIN

namespace threads
{

/**
  * CriticalSection
  *   prevents simultaneous access to an object
  *   that gets shared between different threads.
  *   Implemented in the platform-specific code.
  *   See google for a complete criticalsection manual.
  *   @see Lock
  ***************************************************** */
class s_dll CriticalSection
{
public:
    /**
      * Constructor.
      */
    CriticalSection() s_no_throw;

    /**
      * Destructor.
      */
    ~CriticalSection() s_no_throw;

    /**
      * Enters the CriticalSection.
      * If another thread has entered before, the method
      * blocks until that thread leaves it again.
      * Else the method returns immediately.
      */
    void mf_Enter() const s_no_throw;

    /**
      * Try to enter the CriticalSection.immediately.
      * If another thread has entered before, the method
      * returns false and does not enter, else it returns
      * true after entering.
      * @return false if the CriticalSection isn't free
      */
    bool mf_bTryEnter() const s_no_throw;

    /**
      * Leave the CriticalSection.
      * Make sure not to call this on a CriticalSection that
      * has not been entered.
      */
    void mf_Leave() const s_no_throw;

    s_track_mem_leaks(CriticalSection)

        private :
#if defined S_WIN32
        char m_hMutex[24]; //!< aka win32 CriticalSection
#elif defined S_C6X
        LCK_Handle m_hMutex;
#elif defined S_POSIX
        mutable pthread_mutex_t m_hMutex;
#elif defined S_PSP
        SceUID m_hMutex;
#endif

    CriticalSection(const CriticalSection &);
    const CriticalSection &operator=(const CriticalSection &);
};

/**
  * DummyCriticalSection
  *   empty CriticalSection class.
  *   Gets completely optimized away by the compiler.
  *   Used to allow choosing for a real CriticalSection
  *   or none at all.
  *   @see RawMemory for an example
  ***************************************************** */
class s_dll DummyCriticalSection
{
public:
    /**
      * Empty.
      */
    s_finline DummyCriticalSection() s_no_throw
    {
    }

    /**
      * Empty.
      */
    s_finline ~DummyCriticalSection() s_no_throw
    {
    }

    /**
      * Empty.
      */
    s_finline void mf_Enter() const s_no_throw
    {
    }

    /**
      * Always returns true.
      */
    s_finline bool mf_bTryEnter() const s_no_throw
    {
        return true;
    }

    /**
      * Empty.
      */
    s_finline void mf_Leave() const s_no_throw
    {
    }

    s_track_mem_leaks(DummyCriticalSection)
};
}

S_NAMESPACE_END

#endif //#ifndef __STR_CRITICALSECTION_H__
