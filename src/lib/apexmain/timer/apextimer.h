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
 
#ifndef APEXAPEXTIMER_H
#define APEXAPEXTIMER_H

#include <apexmodule.h>
#include <qdatetime.h>

namespace apex {

/**
Implements a timer that measures the response time of the user

@author Tom Francart,,,
*/
class ApexTimer : public ApexModule
{
public:
    ApexTimer(ExperimentRunDelegate& p_rd);

    void start();
    virtual QString GetResultXML() const;

    void stop();

private:
    QTime m_time;
    long m_result;
};

}

#endif
