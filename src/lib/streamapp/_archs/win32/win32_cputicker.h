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

#ifndef __CPUTICKER_H__
#define __CPUTICKER_H__

#include <QtGlobal>

class CCPUTicker
{
public:
    CCPUTicker();
    CCPUTicker(const CCPUTicker &ticker);

    inline CCPUTicker &operator=(const CCPUTicker &ticker);

    // perform the actual measurement
    void Measure();

    // accessors to the actual measurement value
    double GetTickCountAsSeconds() const;
    inline qint64 GetTickCount() const
    {
        return m_TickCount;
    };

    // static methods
    static bool GetCPUFrequency(double &frequency, double &target_ave_dev,
                                unsigned long interval = 1000,
                                unsigned int max_loops = 20);

    // Is high resolution available on this CPU
    bool IsAvailable() const;

protected:
    qint64 m_TickCount;

    static bool m_bHasRDTSC;
    static bool m_bRunningOnNT;
    static bool m_bStaticsCalculated;

    static double m_deviation;
    static double m_freq;
    static bool m_bFrequencyCalculated;

    static bool CheckHasRDTSC();
    static bool RunningOnNT();
};

#endif //__CPUTICKER_H__
