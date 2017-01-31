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

#ifndef __TRACER_H__
#define __TRACER_H__

#include "../defines.h"

#ifndef S_C6X
#include <string>
typedef std::string String;
#else
#include <std.h>
#endif

namespace utils
{

    /**
      * Tracer
      *   application-wide logging class.
      *   A subclass of Tracer can be installed, and then all
      *   sf_Trace() calls get handled there.
      *   The other methods output to the debugger; it is
      *   platform-specific how this is done. On Windows with
      *   VisualStudio, this will actually print to the debugger
      *   window, all others will just print to std::cout.
      ********************************************************** */
  class Tracer
  {
  protected:
      /**
        * Protected Constructor.
        */
    Tracer();

      /**
        * To be implemented in subclasses.
        * @param ac_sMessage the text
        */
    virtual void mp_TraceOne( const String& ac_sMessage ) = 0;

    static bool sv_bTraceAll;

  public:
      /**
        * Destructor.
        */
    virtual ~Tracer();

      /**
        * Set the tracer to use.
        * Note that what you put in here is not deleted automatically!
        * @param ac_pTracer the new tracer or 0 to disable logging (default)
        * @param ac_bDeletePrevious if true, deletes the previous tracer.
        */
    static void sf_SetCurrentTracer( Tracer* const ac_pTracer, const bool ac_bDeletePrevious = false );

      /**
        * Write a string to the current tracer.
        * @param ac_sMessage the string
        */
    static void sf_Trace( const String& ac_sMessage );

      /**
        * Write a message to the debugger.
        * @param ac_sMessage the message
        * @see DBG
        */
    static void sf_TraceDebug( const String& ac_sMessage );

      /**
        * Write a message to the debugger in printf() style.
        * @param ac_pFormat the message
        * @param  arguments
        * @see DBGPF
        */
    static void sf_TraceDebugPrintf( const char* ac_pFormat, ... );

      /**
        * Set to send all messages to TRACE().
        * If this is set to true, all commands passed to DBG or DBGPF
        * are also passed to TRACE.
        * Use this to get a full log of an application.
        * @param ac_bSet true to set, false to unset (=the default)
        */
    INLINE static void mp_SetTraceAll( const bool ac_bSet )
    {
      sv_bTraceAll = ac_bSet;
    }

      /**
        * The maximum text length is 128.
        * Should be sufficient..
        */
    static const unsigned sc_nMaxTextSize = 128;
  };

}

#endif //#ifndef __TRACER_H__
