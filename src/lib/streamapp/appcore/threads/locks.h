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

#ifndef __LOCKS_H_
#define __LOCKS_H_

#include "../../defines.h"
#include "criticalsection.h"

namespace appcore
{

/**
  * Lock
  *   enters the CriticalSection on construction,
  *   leaves it again in the destructor.
  *   Use for cleaner and faster code:
  *
  *   @code
  *   int method()
  *   {
  *     mc_pLock.mf_Enter();
  *     int nReturnValue = m_nMyMember;
  *     mc_pLock.mf_Leave();
  *     return nReturnValue;
  *   }
  *   @endcode
  *
  *   Can be replaced by:
  *
  *   @code
  *   int method()
  *   {
  *     const Lock L( mc_pLock ); //enter
  *     return m_nMyMember;       //L's destructor is called here: leave
  *   }
  *   @endcode
  *
  *   @see CriticalSection
  *********************************************************************** */
class Lock
{
public:
    /**
      * Constructor.
      * Enters the CriticalSection
      */
    INLINE Lock(const CriticalSection &ac_Section) : mc_Section(ac_Section)
    {
        ac_Section.mf_Enter();
    }

    /**
      * Destructor.
      * Leaves the CriticalSection
      */
    INLINE ~Lock()
    {
        mc_Section.mf_Leave();
    }

private:
    const CriticalSection &mc_Section;

    Lock(const Lock &);
    const Lock &operator=(const Lock &);
};

/**
  * UnLock
  *   reverse of Lock
  *   make sure not to Unlock a CriticalSection that is not locked!
  ***************************************************************** */
class UnLock
{
public:
    /**
      * Constructor.
      * Leaves the CriticalSection
      */
    INLINE UnLock(const CriticalSection &ac_Section) : mc_Section(ac_Section)
    {
        ac_Section.mf_Leave();
    }

    /**
      * Destructor.
      * Leaves the CriticalSection
      */
    INLINE ~UnLock()
    {
        mc_Section.mf_Enter();
    }

private:
    const CriticalSection &mc_Section;

    UnLock(const UnLock &);
    const UnLock &operator=(const UnLock &);
};
}

#endif //#ifndef __LOCKS_H_
