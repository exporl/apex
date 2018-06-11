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

#include "../appcore/threads/thread.h"

#include "manualcallbackrunner.h"

using namespace appcore;
using namespace streamapp;

ManualCallbackRunner::ManualCallbackRunner(IEofCheck &a_Checker)
    : m_Checker(&a_Checker), mv_nMillisSeconds(0)
{
}

ManualCallbackRunner::~ManualCallbackRunner()
{
}

bool ManualCallbackRunner::mp_bStart(Callback &a_Callback)
{
    while (!m_Checker->mf_bEof()) {
        a_Callback.mf_Callback();
        m_Checker->mf_Callback();
        if (mv_nMillisSeconds)
            IThread::sf_Sleep(mv_nMillisSeconds);
    }
    return true;
}

bool ManualCallbackRunner::mp_bStart(Callback &a_Callback,
                                     const unsigned nTimesOrEnd)
{
    unsigned i = nTimesOrEnd;
    while (m_Checker->mf_bEof() && i > 0) {
        a_Callback.mf_Callback();
        m_Checker->mf_Callback();
        if (mv_nMillisSeconds)
            IThread::sf_Sleep(mv_nMillisSeconds);
        --i;
    }
    return true;
}
