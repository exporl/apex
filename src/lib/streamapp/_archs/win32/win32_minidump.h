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
 
#ifndef __WIN32_MINIDUMP_H__
#define __WIN32_MINIDUMP_H__

#include <windows.h>
#include <tlhelp32.h>

namespace utils
{

    /**
      * MiniDump
      *   use for creating minidumps that can be read by VisualStudio or WinDBG,
      *   and/or display exception information to the user.
      *   Usage: put in main()
      *   @code
      *    __try
      *   {
      *     //application code, cannot contain objects which require stack unwinding
      *   }
      *   __except( MiniDump::Create_Dump( GetExceptionInformation(), 1, 1 ), EXCEPTION_EXECUTE_HANDLER )
      *   {
      *   }
          @endcode
      *
      *   This will catch all interesting crashes (unhandled exceptions of any kind),
      *   and create a file application_name.dmp in the app's working directory.
      *   @note Requires dbghelp.dll for creating the file.
      *   @note best compiled with /Oy-
      ******************************************************************************************************************* */
  class MiniDump
  {
  public:

      /**
        * Create dump.
        * @param a_pException can be either GetExceptionInformation() or NULL.
        * @param ac_bUseFile if TRUE, write a dump file (.dmp) with the name of the current process.
        * @param ac_bShowDialog if TRUE, show message with Get_Exception_Info() dump.
        */
    static void __stdcall Create_Dump( PEXCEPTION_POINTERS a_pException, BOOL ac_bUseFile, BOOL ac_bShowDialog );

  private:
      /**
        * Private Constructor.
        */
    MiniDump()
    {}

      /**
        * Private Destructor.
        */
    ~MiniDump()
    {}
  };

}

#endif //#ifndef __WIN32_MINIDUMP_H__
