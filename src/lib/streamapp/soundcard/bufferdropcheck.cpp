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

#include "../appcore/threads/locks.h"

#include "bufferdropcheck.h"

#include <iostream>

#ifdef S_WIN32
#include "../_archs/win32/win32_timers.h"
#elif defined S_POSIX
// FIXME add v3 timers
#include <QTime>
#else
#ERROR !no platform defined !
#endif

using namespace streamapp;

namespace
{
ITimer *sf_InitTimer()
{
#ifdef S_WIN32
    return new utils::Timer4();
#else
    return 0;
#endif
}
}

BufferDropCheck::BufferDropCheck(const bool ac_bDeleteSlave)
    : mv_pDlock(0),
      mv_pNlock(0),
      mv_bPrint(true),
      mv_dTime(666.0),
      mc_pTimer(sf_InitTimer()),
      mc_bDelete(ac_bDeleteSlave),
      mc_Lock()
{
}

BufferDropCheck::~BufferDropCheck()
{
    const Lock L(mc_Lock);
    if (mc_bDelete) {
        delete mv_pDlock;
        delete mv_pNlock;
    }
}

void BufferDropCheck::mp_SetMaxCallbackTime(const double ac_dTimInSec)
{
    const Lock L(mc_Lock);
    mv_dTime = ac_dTimInSec;
}

void BufferDropCheck::mf_Callback()
{
    if (mc_pTimer) {
        mc_pTimer->Reset();
        if (mv_pDlock)
            mv_pDlock->mf_Callback();
        if (mc_pTimer->Elapsed() >= mv_dTime) {
            if (mv_pNlock)
                mv_pNlock->mf_Callback();
            if (mv_bPrint)
                std::cout << "BufferDropCheck: Buffer Underrun!" << std::endl;
        }
    } else {
        if (mv_pDlock)
            mv_pDlock->mf_Callback();
    }
}

void BufferDropCheck::mp_InstallMainCallback(Callback *a_pCallback)
{
    const Lock L(mc_Lock);
    if (mc_bDelete)
        delete mv_pDlock;
    mv_pDlock = a_pCallback;
}

void BufferDropCheck::mp_InstallNoLockCallback(Callback *a_pCallback)
{
    const Lock L(mc_Lock);
    if (mv_pNlock)
        delete mv_pNlock;
    mv_pNlock = a_pCallback;
}

Callback *BufferDropCheck::mf_pGetMainCallback()
{
    const Lock L(mc_Lock);
    return mv_pDlock;
}

Callback *BufferDropCheck::mf_pGetNoLockCallback()
{
    const Lock L(mc_Lock);
    return mv_pNlock;
}

void BufferDropCheck::mp_SetPrintToStdOut(const bool ac_bSet /*= true*/)
{
    const Lock L(mc_Lock);
    mv_bPrint = ac_bSet;
}

const bool &BufferDropCheck::mf_bGetPrintToStdOut() const
{
    const Lock L(mc_Lock);
    return mv_bPrint;
}
