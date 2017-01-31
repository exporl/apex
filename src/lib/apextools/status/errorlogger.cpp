/******************************************************************************
* Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#include "errorlogger.h"


namespace apex
{

ErrorLogger::ErrorLogger() : errors(new StatusDispatcher())
{
}

ErrorLogger::~ErrorLogger()
{
}

void ErrorLogger::addErrorListener(StatusReporter* listener)
{
    errors->addReporter(listener, false);
}

const StatusReporter& ErrorLogger::logger() const
{
    return *errors;
}

void ErrorLogger::clearLog()
{
    errors->clear();
}

StatusReporter& ErrorLogger::log() const
{
    return *errors;
}

}

