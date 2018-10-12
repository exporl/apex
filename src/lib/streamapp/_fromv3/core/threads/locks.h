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
  * [ file ] core/threads/locks.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_LOCKS_H__
#define __STR_LOCKS_H__

#include "criticalsection.h"

S_NAMESPACE_BEGIN

namespace threads
{

/**
  * Lock
  *   enters the tCriticalSection on construction,
  *   leaves it again in the destructor.
  *   Use for cleaner and faster code:
  *
  *   @code
  *   int method()
  *   {
  *     mc_Lock.mf_Enter();
  *     int nReturnValue = m_nMyMember;
  *     mc_Lock.mf_Leave();
  *     return nReturnValue;
  *   }
  *   @endcode
  *
  *   Can be replaced by:
  *
  *   @code
  *   int method()
  *   {
  *     const Lock L( mc_Lock ); //enter
  *     return m_nMyMember;      //L's destructor is called here: leave
  *   }
  *   @endcode
  *
  *   @see CriticalSection
  ********************************************************************** */
template <class tCriticalSection = CriticalSection>
class TLock
{
public:
    /**
      * Constructor.
      * Enters the CriticalSection
      */
    s_finline TLock(const tCriticalSection &ac_Section) s_no_throw
        : mc_Section(ac_Section)
    {
        ac_Section.mf_Enter();
    }

    /**
      * Destructor.
      * Leaves the CriticalSection
      */
    s_finline ~TLock() s_no_throw
    {
        mc_Section.mf_Leave();
    }

    /**
      * Get the CriticalSection used by this Lock.
      * @return a reference
      */
    s_finline const tCriticalSection &mf_GetCriticalSection() const s_no_throw
    {
        return mc_Section;
    }

    s_track_mem_leaks(TLock)

        private : const tCriticalSection &mc_Section;

    TLock(const TLock &);
    const TLock &operator=(const TLock &);
};

typedef TLock<> Lock;

/**
  * UnLock
  *   reverse of Lock
  *   make sure not to Unlock a CriticalSection that is not locked!
  ***************************************************************** */
template <class tCriticalSection = CriticalSection>
class TUnLock
{
public:
    /**
      * Constructor.
      * Leaves the CriticalSection
      */
    s_finline TUnLock(const tCriticalSection &ac_Section) s_no_throw
        : mc_Section(ac_Section)
    {
        ac_Section.mf_Leave();
    }

    /**
      * Destructor.
      * Leaves the CriticalSection
      */
    s_finline ~TUnLock() s_no_throw
    {
        mc_Section.mf_Enter();
    }

    /**
      * Get the CriticalSection used by this Lock.
      * @return a reference
      */
    s_finline const tCriticalSection &mf_GetCriticalSection() const s_no_throw
    {
        return mc_Section;
    }

    s_track_mem_leaks(TUnLock)

        private : const tCriticalSection &mc_Section;

    TUnLock(const TUnLock &);
    const TUnLock &operator=(const TUnLock &);
};

typedef TUnLock<> UnLock;
}

S_NAMESPACE_END

#endif //#ifndef __STR_LOCKS_H__
