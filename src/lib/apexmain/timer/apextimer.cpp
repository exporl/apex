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

#include "apextimer.h"

#include <QDateTime>

namespace apex
{

ApexTimer::ApexTimer(ExperimentRunDelegate &p_rd)
    : ApexModule(p_rd), m_result(-1)
{
}

QString ApexTimer::GetResultXML() const
{
    // qCDebug(APEX_RS, "ApexTimer::GetResultXML\n");

    QString temp("<responsetime unit='ms'>%1</responsetime>");
    temp = temp.arg(m_result);
    //      m_result=-1;

    return temp;
}

void ApexTimer::start()
{
    // qCDebug(APEX_RS, "Starting timer");
    m_result = -1;
    m_time.restart();
}

void ApexTimer::stop()
{
    // qCDebug(APEX_RS, "Stopping timer");
    m_result = m_time.elapsed();
}
}
