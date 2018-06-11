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

#include "win32_ctimer1.h"

CTIMER::CTIMER()
{
    prev = 0;
    CTIMER::TicksPerMS = 0;
    QueryPerformanceCounter((LARGE_INTEGER *)&(CTIMER::LASTCALL));
}

void CTIMER::WaitTillMS(__int64 Time)
{
    __int64 Temp = 0;
    while ((Temp / CTIMER::TicksPerMS) < Time) {
        QueryPerformanceCounter((LARGE_INTEGER *)&Temp);
        Temp *= (1000 / CTIMER::TicksPerMS);
        Sleep(2);
    }
    CTIMER::LASTCALL = Temp;
}

void CTIMER::InitTimers()
{
    __int64 Temp = 0;
    QueryPerformanceFrequency((LARGE_INTEGER *)&Temp);
    CTIMER::TicksPerMS = Temp / 1000;
}

__int64 CTIMER::TicksPerMS = 0;

CTIMER::~CTIMER()
{
    CTIMER::LASTCALL = 0;
}

bool CTIMER::HasPassedMS(__int64 time)
{
    __int64 Temp = 0;

    QueryPerformanceCounter((LARGE_INTEGER *)&Temp);
    if (((Temp / CTIMER::TicksPerMS) - CTIMER::LASTCALL) > time) {
        CTIMER::LASTCALL = Temp;
        return true;
    }
    return false;
}

bool CTIMER::HasPassedSinceStartMS(__int64 time)
{
    __int64 Temp = 0;

    QueryPerformanceCounter((LARGE_INTEGER *)&Temp);
    if ((Temp / CTIMER::TicksPerMS) > time) {
        CTIMER::LASTCALL = Temp;
        return true;
    }
    return false;
}

DWORD CTIMER::GetPassedMS()
{
    QueryPerformanceCounter((LARGE_INTEGER *)&(CTIMER::LASTCALL));
    CTIMER::LASTCALL /= CTIMER::TicksPerMS;
    return (DWORD)CTIMER::LASTCALL;
}

DWORD CTIMER::sinceprev()
{
    __int64 ret = GetPassedMS() - prev;
    prev += ret;
    return (DWORD)ret;
}

DWORD CTIMER::GetPassed(int Sec)
{
    __int64 Temp;
    __int64 Passed;
    QueryPerformanceCounter((LARGE_INTEGER *)&(Temp));
    Temp /= CTIMER::TicksPerMS;
    Temp *= 1000;
    Temp /= Sec;
    Passed = Temp - CTIMER::LASTCALL;
    CTIMER::LASTCALL = Temp;
    return (DWORD)Passed;
}
