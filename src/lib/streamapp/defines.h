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

/**
  * @file defines.h
  * Contains main platform specific defined macros.
  */

#ifndef _APEX_SRC_LIB_STREAMAPP_DEFINES_H__
#define _APEX_SRC_LIB_STREAMAPP_DEFINES_H__

// v3 setup
#include "_fromv3/core/platformsetup.h"

// remain compatible
#define INLINE s_finline

#ifdef TRACE
#undef TRACE
#endif

/**
  * main namespace
  **************** */
namespace streamapp
{

/**
  * Get min( a, b );
  */
#define s_min(a, b) ((a < b) ? a : b)

/**
  * Get max( a, b );
  */
#define s_max(a, b) ((a > b) ? a : b)

/**
  * Trace messages.
  */
#define TRACE(text) utils::Tracer::sf_Trace(text);

#ifdef S_DEBUG

/**
  * Output debug string.
  * @see Tracer::sf_TraceDebug()
  */
#define DBG(text) utils::Tracer::sf_TraceDebug(text);

/**
  * Output debug string, printf style.
  * @see Tracer::sf_TraceDebugPrintf()
  */
#define DBGPF utils::Tracer::sf_TraceDebugPrintf

// assert
#if defined S_WIN32
#define s_assertf                                                              \
    {                                                                          \
        __asm int 3                                                            \
    }
#elif defined S_POSIX

#ifndef S_ARM

// ok here's something stupid: when using single stepping
// using gdb, it will just step over the s_assert and
// ignore the SIGTRAP raised.
// It won't ignore it however if running, or if stepping
// over a function that contains the assert inside.
// Keep this in mind when debugging!
#define s_assertf                                                              \
    {                                                                          \
        asm("int $3");                                                         \
    }

#else

#define s_assertf                                                              \
    {                                                                          \
        asm("ldr ip, [r9, #-8]");                                              \
    }

#endif

#elif defined S_MAC
#define s_assertf Debugger();
#elif defined S_C6X
#define s_assertf                                                              \
    {                                                                          \
        asm("int 3")                                                           \
    }
#elif defined S_PSP
#define s_assertf asm("break 3");
#endif

/**
  * Platform-independent assertion.
  * Gets optimized away in release builds.
  */
#define s_assert(expr)                                                         \
    {                                                                          \
        if (!(expr))                                                           \
            s_assertf                                                          \
    }

#else

#define DBG(text)
#define DBGPF(format, args)

#define s_assert(expr)                                                         \
    {                                                                          \
    }
//    #define s_assertf         {}

#endif
}

#endif //#ifndef _APEX_SRC_LIB_STREAMAPP_DEFINES_H__
