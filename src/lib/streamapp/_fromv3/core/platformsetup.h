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
  *      -- v3tov2 plugin --
  * Do not modify this file by hand,
  * your changes will be overwritten.
  * <^>
  * [ prog ] apex
  * [ file ] core/platformsetup.h
  * [ fmod ] default
  * <^>
  */

/**
  * @file platformsetup.h
  * This file sets up the platform specific stuff.
  * Since platform (in the sense of OS) isn't bound to the
  * compiler, this file also sets up compiler specific stuff.
  */

#ifndef __STR_PLATFORMSETUP_H__
#define __STR_PLATFORMSETUP_H__

#include <QtGlobal>

// first figure out the platform.
// one of these must be defined externally,
// else compilation won't even start. (whether or not the compiler recognizes
// the #error command, it *will* annoy you)
#if defined WIN32
#define S_WIN32 1
#elif defined Q_OS_UNIX
#define S_LINUX 1
#ifdef DAVINCI_ARM
#define S_DAVINCI 1
#define S_ARM 1
#endif
#elif defined MAC
#define S_MAC 1
#elif defined DAVINCI_C6X
#define S_DAVINCI 1
#define S_C6X 1
#elif defined CHIP_6713 // use name from BSL
#define S_C67XX
#define S_C6X 1
#elif defined CHIP_6416
#define S_C64XX
#define S_C6X 1
#elif defined PSP
#define S_PSP 1
#else
#error !no platform defined!
#endif

// 32bit Win9x
#ifdef S_WIN32

// this will exclude rarely used stuff to speed up compilation;
// if you come across undefined functions like @c timeGetTime(),
// you have to @c #undef one of these (locally in your own code, *not* here)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

// this enforces compile-time type checking on handles,
// which is a Good Thing
#ifndef STRICT
#define STRICT
#endif

// the dll export/import macro
#if defined(S_DLL_EXPORT)
#define s_dll __declspec(dllexport)
#define S_DLL_BUILD
#elif defined(S_DLL_IMPORT)
#define s_dll __declspec(dllimport)
#define S_DLL_BUILD
#endif

// target win2k and higher
// the #ifndef guard is only here to
// allow including windows.h before this file,
// which should not be done
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#ifndef WINVER
#define WINVER 0x501
#endif

// exclude a whole buch of stuff we're not using atm
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
//#define NOMENUS
#define NOICONS
#define NOKEYSTATES
//#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
//#define NOCTLMGR
#define NODRAWTEXT
//#define NOGDI
#define NOKERNEL
//#define NOUSER
#define NONLS
#define NOMB
#define NOMEMMGR
#define NOMETAFILE

#undef NOMINMAX
#define NOMINMAX
//#define NOMSG
#define NOOPENFILE
#define NOSCROLL
//#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
//#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX

#define S_PC

#endif

// Sony PSP, bound to psp-gcc
#ifdef S_PSP

#define S_GOT_COMPILER

#define s_inline inline

//#define bool int

#include <psptypes.h>

#ifdef DEBUG
#define S_DEBUG 1
#endif

#define s_assertf asm("break 3");

#endif

// ARM
#ifdef S_ARM

#define s_inline inline

#ifdef __DEBUG
#undef __DEBUG // don't use demo macros
#endif

#define s_assertf                                                              \
    {                                                                          \
        asm("ldr ip, [r9, #-8]");                                              \
    } // FIXME

#endif

// TI C6XXX, bound to ti compiler
#ifdef S_C6X

#define S_GOT_COMPILER

#define __embedded_cplusplus

#define s_inline inline
#ifdef String
#undef String
#endif

#define S_NO_EXCEPTIONS       // can't use exceptions here
#define S_NO_DECENT_CALLSTACK // CCS callstack problem
#define S_C6X_USE_IRAM        // use internal ram for the custom heap
#undef S_USE_QT
#undef S_USE_STL
#undef S_USE_STL_FILE

#ifdef _DEBUG
#define S_DEBUG 1
#endif

#ifndef S_C6X_CINCLUDE
#include <QtGlobal>
#define s_assertf Q_ASSERT(0);
#else
#define s_assertf
#endif

#endif

// Linux
#if defined S_LINUX

#define S_POSIX
#define S_PC

#endif

// OsX FIXME only tested on Intel Mac >10.4
#ifdef S_MAC

#define S_POSIX
#define S_PC
//#define s_assertf  Debugger();

#endif

// MSVC compiler
#if defined(_MSC_VER)

#if _MSC_VER < 1310
#error !compiler must be at least .NET 2003!
#endif

#define S_GOT_COMPILER

#define s_inline __inline
#define s_finline __forceinline
#define s_noinline __declspec(noinline)

#ifdef _DEBUG
#define S_DEBUG 1
#endif

#define s_assertf                                                              \
    {                                                                          \
        __asm int 3                                                            \
    }

#define s_function_name __FUNCTION__

// g++ compiler
#elif defined(__GNUC__)

#ifndef __GNUG__
#error !use g++!
#endif

// there are tons of differences between each version,
// and I can't be bothered checking them all, so just
// set an acceptable floor
#if (__GNUC__ < 3) || (__GNUC__ > 3 && __GNUC__ < 4 && __GNUC_MINOR__ < 2)
#error !use g++ versions 3.2 or above!
#endif

#define S_GOT_COMPILER

#define s_finline inline
#define s_inline inline

#if defined(DEBUG) || defined(__DEBUG__) // thank you, Dev-Stupid-C++
#define S_DEBUG 1
#endif

/*#ifndef s_assertf
  #define s_assertf  { asm( "int $3" ); }
#endif*/

#define s_function_name __PRETTY_FUNCTION__

#endif

#ifndef S_GOT_COMPILER
#error !compiler not supported!
#endif

// complete
#ifndef s_inline
#define s_inline
#endif

#ifndef s_finline
#define s_finline s_inline
#endif

#ifndef s_noinline
#define s_noinline
#endif

// everything that needs exporting gets this macro
#ifndef s_dll
#define s_dll
#endif

#endif //#ifndef __STR_PLATFORMSETUP_H__
