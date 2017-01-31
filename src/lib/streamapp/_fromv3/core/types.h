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
  * [ file ] core/types.h
  * [ fmod ] default
  * <^>
  */

  /**
    * @file types.h
    * This file figures out some types that are different for each platform.
    * For now, only 64bit is of concern, for the rest we just check that
    * all basic types have the right size
    * This file must never be included directly, only through core/_main.h.
    *
    * BIG TODO: get a 64bit machine and make sure everything still works ;-]
    *
    */
#ifndef __STR_BASIC_TYPES_H__
#define __STR_BASIC_TYPES_H__

S_NAMESPACE_BEGIN

  //!unsigned type used for position in containers,
  //!element counts etc.
typedef unsigned size_type;

  //!invalid position/no position etc
const size_type gc_nNPos = (size_type) ( -1 );

  //!these won't be used alot,
  //!but typedef them anyway to save keystrokes
  //!for the unsigned ones ;-]
  typedef char                s_int8;
#ifndef S_PSP
  typedef unsigned char       s_uint8;
#else
  typedef u8                  s_uint8;
#endif

typedef short                 s_int16;
typedef unsigned short        s_uint16;

typedef int                   s_int32;
typedef unsigned int          s_uint32;

#if defined S_WIN32
#ifdef GCC_VERSION
  typedef __int64             s_int64;
  typedef unsigned __int64    s_uint64;
#else
  typedef long long           s_int64;
  typedef unsigned long long  s_uint64;
#endif
#elif defined S_LINUX
  typedef long long           s_int64;
  typedef unsigned long long  s_uint64;
#elif defined S_MAC
  typedef long long           s_int64;
  typedef unsigned long long  s_uint64;
#elif defined S_DAVINCI

#elif defined S_C6X
  typedef long long           s_int64;
  typedef unsigned long long  s_uint64;
#elif defined S_PSP
  typedef long long           s_int64;
  typedef unsigned long long  s_uint64; //TODOPSP check psp types
#endif

#ifndef S_TYPES_TESTED
  s_static_assert_any( sizeof( char )     == 1, TypeNotSupported, test0 )
  s_static_assert_any( sizeof( short )    == 2, TypeNotSupported, test1 )
  s_static_assert_any( sizeof( int )      == 4, TypeNotSupported, test2 )
  s_static_assert_any( sizeof( float )    == 4, TypeNotSupported, test3 )
  s_static_assert_any( sizeof( double )   == 8, TypeNotSupported, test4 )
  s_static_assert_any( sizeof( s_int64 )  == 8, TypeNotSupported, test5 )
  #define S_TYPES_TESTED
#endif

S_NAMESPACE_END

#endif //#ifndef __STR_BASIC_TYPES_H__
