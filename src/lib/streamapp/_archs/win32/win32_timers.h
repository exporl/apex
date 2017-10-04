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

#ifndef TIMERSSS_H
#define TIMERSSS_H

#include "utils/timer.h"
#include "win32_cputicker.h"
#include "win32_ctimer1.h"

#include "win32_headers.h"

#include <cstdio>
#include <iostream>

namespace utils
{
class Timer4 : public ITimer
{ // simple timer utilizing the windows functions
public:
    Timer4()
    {
        QueryPerformanceFrequency(&frequency);
        Reset();
    };

    /**
         * Set the time to zero and start counting.
         */
    virtual void Reset()
    {
        QueryPerformanceCounter(&last_count);
    }

    /**
      * Get the elapsed tim.
      * @return the time (preferably nanoseconds)
      */
    virtual double Elapsed()
    {
        LARGE_INTEGER new_count;
        QueryPerformanceCounter(&new_count);

        double result = double(new_count.QuadPart - last_count.QuadPart) /
                        frequency.QuadPart;
        last_count = new_count;
        return result;
    };

private:
    LARGE_INTEGER frequency; // ticks per second
    LARGE_INTEGER last_count;
};

class Timer3 : public ITimer, public CTIMER
{
public:
    Timer3()
    {
    }
    ~Timer3()
    {
    }

    void Reset()
    {
        InitTimers();
    }

    double Elapsed()
    {
        return double(sinceprev() / 1000); // convert to seconds
    }
};

class Timer2 : public ITimer, public CCPUTicker
{
public:
    Timer2() : m_iElapsed(0)
    {
#ifdef DEBUG
        std::cout << "Calibrating timer..." << std::endl;
#endif
        CCPUTicker::GetCPUFrequency(CCPUTicker::m_freq, CCPUTicker::m_deviation,
                                    250, 4);
        // CCPUTicker::GetCPUFrequency( CCPUTicker::m_freq,
        // CCPUTicker::m_deviation, 1000, 4 );
        // CCPUTicker::m_freq = 2300082933.6875000;
        // CCPUTicker::m_deviation = 241.32812500000000;
    }
    ~Timer2()
    {
    }

    void Reset()
    {
        m_iElapsed = 0;
        CCPUTicker::Measure();
    }

    double Elapsed()
    {
        __int64 temp = CCPUTicker::m_TickCount;
        CCPUTicker::Measure();
        m_iElapsed = CCPUTicker::m_TickCount - temp;
        return (double)m_iElapsed / m_freq;
    }

    double ElapsedCummulative()
    {
        __int64 temp = CCPUTicker::m_TickCount;
        CCPUTicker::Measure();
        m_iElapsed += CCPUTicker::m_TickCount - temp;
        return (double)m_iElapsed / m_freq;
    }

private:
    __int64 m_iElapsed;
};

class SimpleProfiling
{
public:
    SimpleProfiling(const std::string &ac_sName)
        : mc_sName(ac_sName), m_dMean(0), m_dMCnt(0)
    {
        sf_PrintInfo("SimpleProfiler Ready", ac_sName);
    }
    ~SimpleProfiling()
    {
        char temp[128];
        sprintf(temp, "%E", m_dMean / m_dMCnt);
        sf_PrintInfo(mc_sName, temp);
    }

    void Start()
    {
        m_Tick.Reset();
    }

    void Stop()
    {
        m_dMean += m_Tick.Elapsed();
        ++m_dMCnt;
    }

    static void sf_PrintInfo(const std::string &ac_s1, const std::string &ac_s2)
    {
        std::cout << "****************************" << ac_s1.data() << " : "
                  << ac_s2.data() << "\n"
                  << std::endl;
    }

private:
    const std::string mc_sName;
    Timer2 m_Tick;
    double m_dMean;
    double m_dMCnt;

    SimpleProfiling(const SimpleProfiling &);
    SimpleProfiling &operator=(const SimpleProfiling &);
};
}

#endif //#ifndef TIMERSSS_H
