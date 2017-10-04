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

// TITLE:
// High-Resolution Counter Class.
//
// VERSION:
// 1.2
//
// AUTHOR:
// Created by J.M.McGuiness, http://www.hussar.demon.co.uk
//            PJ Naughter,   Email: pjn@indigo.ie Web: http://indigo.ie/~pjn
//
// DESCRIPTION:
// This file declares a class the wraps the Pentium-specific time stamp counter.
// This counter has a resolution in terms of PCLKS (processor clocks) so it can
// be used for direct instruction timings.
//
// VERSION HISTORY:
// 26/3/96 v1.0 J.M.McGuiness Initially created
// 16/7/97 v1.1 PJ Naughter,
//              1. A number of additions including:
//              2. Support for running on Windows NT
//              3. now uses the build-in 64 bit data type "qint64"
//              4. Improved diagnostic info thanks to the above
//              5. Usage of static variables to improve efficiency
//              6. Addition of a function which will convert from CPU ticks to
//              seconds
//              7. Improved adherence to the MFC coding style and standards
// 14/1/99 v1.2 PJ Naughter
//              1. Fixed a bug discovered by David Green-Seed where he was
//              experiencing
//                 access violations when the code was compiled with
//                 optimizations turned
//                 on a Pentium II. The problem was that (on a PII) the RDTSC
//                 instruction
//                 touches more registers than expected. This has now been fixed
//                 by saved
//                 and restoring the EAX and EBX registers around the call to
//                 RDTSC.
//              2. General code tidy up of const functions, parameters etc.
// 03/12/99 v1.2 PJ Naughter 1. Fixed a problem in when compiling in VC 6 in
// release mode.
//
// LEGALITIES:
// Copyright © 1996-1999 by J.M.McGuiness and PJ Naughter, all rights reserved.
//
// This file must not be distributed without the authors prior
// consent.
//
///////////////////////////////////////////////////////////////////////////////

#include "win32_cputicker.h"
#include "win32_headers.h"

#include <math.h>
#include <time.h>

bool CCPUTicker::m_bHasRDTSC = false;
bool CCPUTicker::m_bRunningOnNT = false;
bool CCPUTicker::m_bStaticsCalculated = false;
double CCPUTicker::m_deviation = 0;
double CCPUTicker::m_freq = 0;
bool CCPUTicker::m_bFrequencyCalculated = false;

#ifdef _MSC_VER
#pragma warning(disable : 4702)
#pragma optimize("", off)
#endif

bool CCPUTicker::CheckHasRDTSC()
{
    /*SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    if (sys_info.dwProcessorType==PROCESSOR_INTEL_PENTIUM)
    { */
    try {
#ifdef _MSC_VER
        __asm
        {
        _emit 0x0f
        _emit 0x31
        }
#else
//??asm("") or so for gcc
#endif
    } catch (...) // Check to see if the opcode is defined.
    {
        return false;
    }
    // Check to see if the instruction ticks accesses something that changes.
    volatile ULARGE_INTEGER ts1, ts2;
#ifdef _MSC_VER
    __asm
    {
      xor eax,eax
      _emit 0x0f
      _emit 0xa2
      _emit 0x0f
      _emit 0x31
      mov ts1.HighPart,edx
      mov ts1.LowPart,eax
      xor eax,eax
      _emit 0x0f
      _emit 0xa2
      _emit 0x0f
      _emit 0x31
      mov ts2.HighPart,edx
      mov ts2.LowPart,eax
    }
#else

#endif

    // If we return true then there's a very good chance it's a real RDTSC
    // instruction!
    if (ts2.HighPart == ts1.HighPart)
    {
        if (ts2.LowPart > ts1.LowPart) {
            // TRACE0("RDTSC instruction probably present.\n");
            return true;
        } else {
            // TRACE0("RDTSC instruction NOT present.\n");
            return false;
        }
    }
    else if (ts2.HighPart > ts1.HighPart)
    {

        return true;
    }
    else
    {
        // TRACE0("RDTSC instruction NOT present.\n");
        return false;
    }
    /*}
    else
    {
      TRACE0("RDTSC instruction NOT present.\n");
      return false;
    } */
}
#pragma optimize("", on)

bool CCPUTicker::IsAvailable() const
{
    return m_bHasRDTSC;
}

CCPUTicker::CCPUTicker()
{
    m_TickCount = 0;

    // precalculate the statics
    if (!m_bStaticsCalculated) {
        m_bStaticsCalculated = true;
        m_bHasRDTSC = CheckHasRDTSC();
        m_bRunningOnNT = RunningOnNT();
    }
}

CCPUTicker::CCPUTicker(const CCPUTicker &ticker)
{
    m_TickCount = ticker.m_TickCount;
}

CCPUTicker &CCPUTicker::operator=(const CCPUTicker &ticker)
{
    m_TickCount = ticker.m_TickCount;
    return *this;
}

bool CCPUTicker::RunningOnNT()
{
    return false;
}

#pragma optimize("", off)
void CCPUTicker::Measure()
{
    if (m_bHasRDTSC) {
        volatile ULARGE_INTEGER ts;

// on NT don't bother disabling interrupts as doing
// so will generate a priviledge instruction exception
/*if (!m_bRunningOnNT)
{
  _asm
  {
    cli
  }
} */
#ifdef _MSC_VER
        _asm
        {
      xor eax,eax
      push eax
      push ebx
      push ecx
      push edx
      _emit 0x0f          ; cpuid - serialise the processor
      _emit 0xa2
      pop edx
      pop ecx
      pop ebx
      pop eax
      _emit 0x0f          ; rdtsc
      _emit 0x31
      mov ts.HighPart,edx
      mov ts.LowPart,eax
        }
#endif

        /*if (!m_bRunningOnNT)
        {
          _asm
          {
            sti
          }
        } */

        m_TickCount = ts.QuadPart;
    } else {
        m_TickCount = 0;
    }
}
#pragma optimize("", on)

// The following function will work out the processor clock frequency to a
// specified accuracy determined by the target average deviation required.
// Note that the worst average deviation of the result is less than 5MHz for
// a mean frequency of 90MHz. So basically the target average deviation is
// supplied only if you want a more accurate result, it won't let you get a
// worse one. (Units are Hz.)
//
// (The average deviation is a better and more robust measure than it's cousin
// the standard deviation of a quantity. The item determined by each is
// essentially similar. See "Numerical Recipies", W.Press et al for more
// details.)
//
// This function will run for a maximum of 20 seconds before giving up on
// trying to improve the average deviation, with the average deviation
// actually achieved replacing the supplied target value. Use "max_loops" to
// change this. To improve the value the function converges to increase
// "interval" (which is in units of ms, default value=1000ms).

#pragma optimize("", off)
bool CCPUTicker::GetCPUFrequency(double &frequency, double &target_ave_dev,
                                 unsigned long interval, unsigned int max_loops)
{
    register LARGE_INTEGER goal, period, current;
    register unsigned int ctr = 0;
    double curr_freq, ave_freq; // In Hz.
    double ave_dev, tmp = 0;
    CCPUTicker s, f;
    if (!QueryPerformanceFrequency(&period))
        return false;
    period.QuadPart *= interval;
    period.QuadPart /= 1000;

    // Start of tight timed loop.
    QueryPerformanceCounter(&goal);
    goal.QuadPart += period.QuadPart;
    s.Measure();
    do {
        QueryPerformanceCounter(&current);
    } while (current.QuadPart < goal.QuadPart);
    f.Measure();
    // End of tight timed loop.

    ave_freq = 1000 * ((double)(f.m_TickCount - s.m_TickCount)) / interval;
    do {
        // Start of tight timed loop.
        QueryPerformanceCounter(&goal);
        goal.QuadPart += period.QuadPart;
        s.Measure();
        do {
            QueryPerformanceCounter(&current);
        } while (current.QuadPart < goal.QuadPart);
        f.Measure();
        // End of tight timed loop.

        // Average of the old frequency plus the new.
        curr_freq = 1000 * ((double)(f.m_TickCount - s.m_TickCount)) / interval;
        ave_freq = (curr_freq + ave_freq) / 2;

        // Work out the current average deviation of the frequency.
        tmp += fabs(curr_freq - ave_freq);
        ave_dev = tmp / ++ctr;
    } while (ave_dev > target_ave_dev && ctr < max_loops);
    target_ave_dev = ave_dev;
    //  TRACE2("Estimated the processor clock frequency =%gHz,
    //  dev.=±%gHz.\n",ave_freq,ave_dev);
    frequency = ave_freq;

    return true;
}
#pragma optimize("", on)

double CCPUTicker::GetTickCountAsSeconds() const
{
    if (!m_bFrequencyCalculated) {
        bool bSuccess = CCPUTicker::GetCPUFrequency(m_freq, m_deviation);
        if (!bSuccess)
            return 0;

        m_bFrequencyCalculated = true;
    }

    return ((double)m_TickCount) / m_freq;
}
