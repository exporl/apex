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
  * [ file ] streamapp_conf.h
  * [ fmod ] default
  * <^>
  */

  /**
    * @file streamapp_conf.h
    * This file contains configuration options set through macros,
    * which will enable or disable specific features.
    */

#ifndef __STREAMAPP_CONF_H__
#define __STREAMAPP_CONF_H__

  /**
    * streamapp_main_namespace
    * The main namespace name.
    */
#define streamapp_main_namespace str3amapp

  /**
    * S_V2_EX
    * TEMPORARY
    * Use when compiling v2+soma against v3.
    * When defined, excludes certain headers that v2 has no business with.
    * Don't define when compiling v3, that won't work out..
    */
#ifndef S_V2_EX
  //#define S_V2_EX
#else
  #ifndef S_NO_GLOBAL_OPERATOR_OVERLOAD
    #define S_NO_GLOBAL_OPERATOR_OVERLOAD
  #endif
  #ifndef S_JUST_CORE
    #define S_JUST_CORE
  #endif
  #ifndef S_USE_QT
    #define S_USE_QT
  #endif
  #ifndef S_USE_QT_SUPPRESS_WARN
    #define S_USE_QT_SUPPRESS_WARN
  #endif
  #ifndef S_USE_STL
    #define S_USE_STL
  #endif
  #undef S_USE_STREAMAPP
  #undef S_USE_APP
  #undef S_USE_CUSTOM_HEAP
  #undef S_TRACK_MEMORY
#endif

  /**
    * S_CUSTOM_HEAP
    * Define this to use the custom heap for *all* memory allocation,
    * and witness allocation get 100 times faster.
    */
#ifndef S_CUSTOM_HEAP
  //#define S_CUSTOM_HEAP
#endif

  /**
    * S_NO_GLOBAL_OPERATOR_OVERLOAD
    * Define this to disable overloading of the global new and delete operators.
    * Use only if you're sure what you're doing: when defining this and at the same time
    * using a custom allocator, if you @c new something, it's memory will not come from
    * your custom allocator. This isn't necessarily a bad thing, but it leads to a weird
    * allocation policy.
    */
#ifndef S_NO_GLOBAL_OPERATOR_OVERLOAD
  //#define S_NO_GLOBAL_OPERATOR_OVERLOAD
#endif

  /**
    * S_TRACK_MEMORY
    * Define this to track memory allocations and deallocations,
    * hence memory leaks.
    * @see memory.h for usage instructions
    */
#ifndef S_TRACK_MEMORY
  #define S_TRACK_MEMORY
#endif

  /**
    * S_ALWAYS_ASSERT
    * Define this to use the s_assert macro in release builds.
    */
#ifndef S_ALWAYS_ASSERT
  #define S_ALWAYS_ASSERT
#endif

  /**
    * S_ALWAYS_DBG
    * Define this to use the s_dbg macros in release builds.
    */
#ifndef S_ALWAYS_DBG
  #define S_ALWAYS_DBG
#endif

  /**
    * S_JUSTCORE
    * Define this to include only the core files,
    * and not the application files.
    * If this is not defined, the next macros
    * will be evaluated to see which parts to include.
    */
#ifndef S_JUSTCORE
  //#define S_JUSTCORE
#endif

  /**
    * S_USE_IPHLPAPI
    * Enables MAC adress functions on win32
    */
#ifndef S_USE_IPHLPAPI
  //#define S_USE_IPHLPAPI
#endif

  /**
    * S_NO_DEBUGGER_DBG
    * Disables use of OutputDebugString on win32
    */
#ifndef S_NO_DEBUGGER_DBG
  //#define S_NO_DEBUGGER_DBG
#endif

  /**
    * S_SPEC_PRINT
    * Define to add info printing to classes supproting it.
    */
#ifndef S_SPEC_PRINT
  //#define S_SPEC_PRINT
#endif

  /**
    * S_USE_STREAMAPP
    * Define to include streamapp/ files.
    */
#ifndef S_USE_STREAMAPP
  #define S_USE_STREAMAPP
#endif

#ifdef S_USE_STREAMAPP

    /**
      * S_USE_STREAMAPP_ASIO
      * Define for using SteinBerg VST ASIO SDK
      */
  #ifndef S_USE_STREAMAPP_ASIO
    //#define S_USE_STREAMAPP_ASIO
  #endif

    /**
      * S_USE_STREAMAPP_PORTAUDIO
      * Define for using Portaudio SDK
      */
  #ifndef S_USE_STREAMAPP_PORTAUDIO
    //#define S_USE_STREAMAPP_PORTAUDIO
  #endif

    /**
      * S_USE_STREAMAPP_FFTREAL
      * Define for using processors/spectrum.h
      */
  #ifndef S_USE_STREAMAPP_FFTREAL
    //#define S_USE_STREAMAPP_FFTREAL
  #endif

    /**
      * S_USE_STREAMAPP_MIXER
      * Define for using files in streamapp/mixer/
      */
  #ifndef S_USE_STREAMAPP_MIXER
    //#define S_STREAMAPP_USE_MIXER
  #endif

    /**
      * S_USE_STREAMAPP_NSP
      * Define for using Intel NSP libraries
      */
  #ifndef S_USE_STREAMAPP_NSP
    //#define S_USE_STREAMAPP_NSP
  #endif

#else

  #undef S_USE_ASIO
  #undef S_USE_PORTAUDIO
  #undef S_USE_MIXER
  #undef S_USE_NSP

#endif

  /**
    * S_USE_APP
    * Define to include files in the app dir.
    */
#ifndef S_USE_APP
  #define S_USE_APP
#endif

  /**
    * S_USE_GRAPHICS
    * Define to include files in the graphics dir.
    */
#ifndef S_USE_GRAPHICS
  //#define S_USE_GRAPHICS
#endif

  /**
    * S_USE_QT
    * Define to include files in the qt/misc/ dir,
    * and to add QString <-> String and QArray <-> Array stuff
    */
#ifndef S_USE_QT
  //#define S_USE_QT
#endif


#ifdef S_USE_QT

    /**
      * S_USE_QT_SUPPRESS_WARN
      * Define to suppress lost of warnings when using VisualStudio.
      * Take care not to mask your own warnings though!
      */
  #ifndef S_USE_QT_SUPPRESS_WARN
    #define S_USE_QT_SUPPRESS_WARN
  #endif

    /**
      * S_USE_QTGUI
      * Define to include files in the qt/gui/ dir.
      */
  #ifndef S_USE_QT_GUI
    //#define S_USE_QT_GUI
  #endif

    /**
      * S_USE_QWT
      * Define to use Qwt graphs
      */
  #ifndef S_USE_QWT
    //#define S_USE_QWT
  #endif

#endif

  /**
    * S_USE_STL
    * Define to enable functions using the STL library,
    * namely strings, containers and io redirection,
    * and for including the files in the stl directory.
    * @see S_USE_STL_FILE
    */
#ifndef S_USE_STL
  //#define S_USE_STL
#endif

  /**
    * S_USE_STL
    * Define to enable FileInputStream and FileOutputStream.
    */
#ifndef S_USE_STL_FILE
  #define S_USE_STL_FILE
#endif

  /**
    * S_MATLABCOMPATIBLE
    * Define to make limits go from 0 to 2^x instead of 2^x - 1
    */
#ifndef S_MATLABCOMPATIBLE
  #define S_MATLABCOMPATIBLE
#endif

  /**
    * S_STLCOMPATIBLE
    * When this is defined, some classes like array and string
    * get extra members with the same name as the STL names
    */
#ifndef S_STLCOMPATIBLE
  #define S_STLCOMPATIBLE
#endif

#endif //#ifndef __STREAMAPP_CONF_H__
