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

#include "linux_headers.h"
#include "utils/tracer.h"
#include <stdarg.h>
#include <iostream>
#include <stdio.h>

namespace
{
  const unsigned sc_nMaxTextSize = 128;
}

void utils::Tracer::sf_TraceDebug( const std::string& text )
{
  std::cout << text.c_str() << std::endl;
}

void utils::Tracer::sf_TraceDebugPrintf( const char* format, ... )
{
  static char buf[ sc_nMaxTextSize ];
  va_list args;
  va_start( args, format );
  vsprintf( buf, format, args );
  sf_TraceDebug( buf );
}