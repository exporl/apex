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

/**
  *      -- v3tov2 plugin --
  * Do not modify this file by hand,
  * your changes will be overwritten.
  * <^>
  * [ prog ] apex
  * [ file ] _archs/win32/win32_misc.cpp *
  * [ fmod ] strip namespace
  * [ fmod ] String -> std::string
  * <^>
  */

#ifdef S_DLL_EXPORT

//#pragma comment( lib, "msvcprtd.lib" )
//#pragma comment( lib, "MSVCRTD.lib" )
//#pragma comment( lib, "kernel32.lib" )
//#pragma comment( lib, "user32.lib" )
//#pragma comment( lib, "shell32.lib" )
//#pragma comment( lib, "gdi32.lib" )
//#pragma comment( lib, "vfw32.lib" )
//#pragma comment( lib, "winspool.lib" )
//#pragma comment( lib, "comdlg32.lib" )
//#pragma comment( lib, "winmm.lib" )
//#pragma comment( lib, "wininet.lib" )
//#pragma comment( lib, "ole32.lib" )
//#pragma comment( lib, "oleaut32.lib" )
//#pragma comment( lib, "uuid.lib" )
//#pragma comment( lib, "odbc32.lib" )
//#pragma comment( lib, "odbccp32.lib" )
//#pragma comment( lib, "advapi32.lib")
//#pragma comment( lib, "wsock32.lib" )

#endif

#include "../../utils/tracer.h"

#include "win32_headers.h"

#include <cstdio>

void utils::Tracer::sf_TraceDebug(const std::string &text)
{
    sf_TraceDebugPrintf("%s", text.c_str());
}

void utils::Tracer::sf_TraceDebugPrintf(const char *format, ...)
{
    static char buf[sc_nMaxTextSize];
    static const unsigned sc_nSizeWithoutEnd = sc_nMaxTextSize - 3;

    va_list args;
    va_start(args, format);
    const int n = _vsnprintf(buf, sc_nSizeWithoutEnd, format, args);
    va_end(args);

    char *p = buf + ((n < 0) ? sc_nSizeWithoutEnd : n);
    *p++ = '\r';
    *p++ = '\n';
    *p++ = '\0';

    OutputDebugString(buf);
}
