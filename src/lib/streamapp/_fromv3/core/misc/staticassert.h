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
  * [ file ] core/misc/staticassert.h
  * [ fmod ] default
  * <^>
  */

#ifndef __STR_STATICASSERT_H__
#define __STR_STATICASSERT_H__

S_NAMESPACE_BEGIN

namespace misc
{

  template< bool >
  struct tAssertCheck
  {
    tAssertCheck( ... );
  };

  template<>
  struct tAssertCheck< false >
  {
  };

}

S_NAMESPACE_END

/**
  * s_static_assert
  *   Compile Time Assertion like the one presented by Andrei
  *   (shame on you if you don't know who that is).
  *   The difference is that this one actually compiles???
  *   Maybe it was his intention: there's also a missing semicolon in his example.
  *   Anyway, when used like this:
  *   @code
  *   void testAssert()
  *   {
  *     s_static_assert( sizeof( char ) == sizeof( double ), CompilerWillComplainHere )
  *   }
  *   @endcode
  *   the compiler will generate something like
  *   @verbatim
  *   cannot convert from 'testAssert::Error_StaticAssertionFailed' to 'theAssert<false>'
  *   @endverbatim
  *   since it cannot convert from Error_StaticAssertionFailed to theAssert<false>.
  *   Because everything is done by the compiler, no instructions will be generated
  *   when the assertion is true.
  *   Quite nifty!
  *   @param exp a compile-time constant to evaluate
  *   @param msg message to print
  *   @NOTE due to problems with templates and dlls, the macro is inactive
  *   while building a dll
  */
#ifndef S_DLL_EXPORT
  #define s_static_assert( exp, msg ) \
  { \
    class Error_##msg{};\
    Error_##msg dum; \
    (void) sizeof( ::smn::misc::tAssertCheck< ( exp ) != 0 >( dum ) ); \
  }
#else
  #define s_static_assert( exp, msg )
#endif

  /**
    * s_static_assert_any
    *   same as s_static_assert, but for class or file local use too.
    *   FIXME this doesn't work in temapate classes because @c name
    *   doesn't get instantiated!
    */
#ifndef S_DLL_EXPORT
  #define s_static_assert_any( exp, msg, name ) \
  struct name \
  { \
    class Error_##msg{};\
    Error_##msg dum; \
    void test(){ (void) sizeof( ::smn::misc::tAssertCheck< (exp) != 0 >( dum ) ); } \
    explicit name() { test(); } \
  };
#else
  #define s_static_assert_any( exp, msg, name )
#endif

  /**
    * s_static_assert_inclass
    *   same as s_static_assert, but for class or file local use too.
    *   Note that this can only be used once per class or file, else
    *   tTest gets redefined.
    *   @see s_static_assert_any for a named version
    */
#define s_static_assert_inclass( exp, msg ) s_static_assert_any( exp, msg, tTest )

  /**
    * Shortcut to s_static_assert with a default message.
    */
#define s_static_assert_nomsg( exp ) s_static_assert( exp, StaticAssertionFailed )

  /**
    * Shortcut for writing s_static_assert( 0, exp ).
    */
#define s_dostatic_assert( msg ) s_static_assert( 0, msg )

#endif //#ifndef __STR_STATICASSERT_H__
