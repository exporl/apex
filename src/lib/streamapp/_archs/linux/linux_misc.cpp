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

#include "../../utils/tracer.h"

#include "linux_headers.h"

#include <stdarg.h>
#include <stdio.h>

#include <iostream>

void utils::Tracer::sf_TraceDebug(const std::string &text)
{
    std::cout << text.c_str() << std::endl;
}

void utils::Tracer::sf_TraceDebugPrintf(const char *format, ...)
{
    static char buf[sc_nMaxTextSize];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    sf_TraceDebug(buf);
}
