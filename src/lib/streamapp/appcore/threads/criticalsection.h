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

#ifndef __CRITICALSECTION_H__
#define __CRITICALSECTION_H__

#include "../../defines.h"

#if defined S_WIN32
// nothing to include here
#elif defined S_C6X
#include "c6x_headers.h"
#elif defined S_POSIX
#include "../../_archs/linux/linux_headers.h"
#elif defined S_PSP
#include "psp_headers.h"
#else
#ERROR !no platform defined !
#endif

namespace appcore
{

/**
  * CriticalSection
  *   prevents simultaneous access to an object shared between different
  *threads.
  *   Implemented in the platfrom-specific code.
  *   See google for a complete criticalsection manual.
  *   @see Lock
  *******************************************************************************
  */
class CriticalSection
{
public:
    /**
      * Constructor.
      */
    CriticalSection();

    /**
      * Destructor.
      */
    ~CriticalSection();

    /**
      * Enters the CriticalSection.
      * If another thread has entered before, the method
      * blocks until that thread leaves it again.
      * Else the method returns immedeately.
      */
    void mf_Enter() const;

    /**
      * Trie to enter the CriticalSection.immedeately.
      * If another thread has entered before, the method
      * returns false and doe not enter, else it returns true
      * after entering.
      * @return false if the CriticalSection isn't free
      */
    bool mf_bTryEnter() const;

    /**
      * Leave the CriticalSection.
      * Make sure not to call this on a CriticalSection that
      * has not been entered.
      */
    void mf_Leave() const;

private:
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
class DummyCriticalSection
{
public:
    /**
      * Empty.
      */
    INLINE DummyCriticalSection()
    {
    }

    /**
      * Empty.
      */
    INLINE ~DummyCriticalSection()
    {
    }

    /**
      * Empty.
      */
    INLINE void mf_Enter() const
    {
    }

    /**
      * Always returns true.
      */
    INLINE bool mf_bTryEnter() const
    {
        return true;
    }

    /**
      * Empty.
      */
    INLINE void mf_Leave() const
    {
    }
};
}

#endif //#ifndef __CRITICALSECTION_H__
