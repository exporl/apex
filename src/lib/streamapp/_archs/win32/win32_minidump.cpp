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
 
#include "win32_minidump.h"
#include <iostream>

#ifdef USESQT
  /**
    * If used correctly, the QApplication is already destructed when calling this,
    * but we need it to display the gui.
    */
#include "guicore/minidumpgui.h"
#if QT_VERSION < 0x040000
#include <qapplication.h>
#else
#include <QApplication>
#endif
#endif

using namespace utils;

//#pragma optimize( "y", off )      //generate stack frame pointers for all functions - same as /Oy- in the project
#pragma warning( disable : 4200 )  //nonstandard extension used : zero-sized array in struct/union
#pragma warning( disable : 4100 )  //unreferenced formal parameter
#pragma warning( disable : 4312 ) //conversion from 'DWORD' to 'PSTACK' of greater size

  /**
    * These are from dbghelp.h
    */
typedef struct _MINIDUMP_EXCEPTION_INFORMATION
{
  DWORD  ThreadId;
  PEXCEPTION_POINTERS  ExceptionPointers;
  BOOL  ClientPointers;
} MINIDUMP_EXCEPTION_INFORMATION, *PMINIDUMP_EXCEPTION_INFORMATION;

typedef enum _MINIDUMP_TYPE
{
  MiniDumpNormal =      0x00000000,
  MiniDumpWithDataSegs =    0x00000001,
} MINIDUMP_TYPE;

typedef  BOOL (WINAPI * MINIDUMP_WRITE_DUMP)
(
  IN HANDLE      hProcess,
  IN DWORD      ProcessId,
  IN HANDLE      hFile,
  IN MINIDUMP_TYPE  DumpType,
  IN CONST PMINIDUMP_EXCEPTION_INFORMATION  ExceptionParam, OPTIONAL
  IN PVOID                  UserStreamParam, OPTIONAL
  IN PVOID                  CallbackParam OPTIONAL
);

  /**
    * Help functions
    */
typedef  HANDLE (WINAPI * CREATE_TOOL_HELP32_SNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);
typedef  BOOL (WINAPI * MODULE32_FIRST)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef  BOOL (WINAPI * MODULE32_NEST)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

#define  DUMP_SIZE_MAX  8000  //max size of our dump
#define  CALL_TRACE_MAX  ((DUMP_SIZE_MAX - 2000) / (MAX_PATH + 40))  //max number of traced calls
#define  NL        "\r\n"  //new line

  /**
    * Variables
    */
namespace
{
  HMODULE  hDbgHelp;
  MINIDUMP_WRITE_DUMP  MiniDumpWriteDump_;

  CREATE_TOOL_HELP32_SNAPSHOT  CreateToolhelp32Snapshot_;
  MODULE32_FIRST  Module32First_;
  MODULE32_NEST  Module32Next_;
}

  /**
    * Find module by Ret_Addr (address in the module).
    * Return Module_Name (full path) and Module_Addr (start address).
    * Return TRUE if found.
    */
BOOL WINAPI Get_Module_By_Ret_Addr(PBYTE Ret_Addr, PCHAR Module_Name, PBYTE & Module_Addr)
{
  MODULEENTRY32  M = {sizeof(M)};
  HANDLE  hSnapshot;

  Module_Name[0] = 0;

  if (CreateToolhelp32Snapshot_)
  {
    hSnapshot = CreateToolhelp32Snapshot_(TH32CS_SNAPMODULE, 0);

    if ((hSnapshot != INVALID_HANDLE_VALUE) &&
      Module32First_(hSnapshot, &M))
    {
      do
      {
        if (DWORD(Ret_Addr - M.modBaseAddr) < M.modBaseSize)
        {
          lstrcpyn(Module_Name, M.szExePath, MAX_PATH);
          Module_Addr = M.modBaseAddr;
          break;
        }
      } while (Module32Next_(hSnapshot, &M));
    }

    CloseHandle( hSnapshot );
  }

  return !!Module_Name[ 0 ];
}

  /**
    * Fill Str with call stack info.
    * pException can be either GetExceptionInformation() or NULL.
    * If pException = NULL - get current call stack.
    */
int WINAPI Get_Call_Stack(PEXCEPTION_POINTERS pException, PCHAR Str)
{
  CHAR  Module_Name[MAX_PATH];
  PBYTE  Module_Addr = 0;
  PBYTE  Module_Addr_1;
  int    Str_Len;

  typedef struct STACK
  {
    STACK *  Ebp;
    PBYTE  Ret_Addr;
    DWORD  Param[0];
  } STACK, * PSTACK;

  STACK  Stack = {0, 0};
  PSTACK  Ebp;

  if (pException)    //fake frame for exception address
  {
    Stack.Ebp = (PSTACK)pException->ContextRecord->Ebp;
    Stack.Ret_Addr = (PBYTE)pException->ExceptionRecord->ExceptionAddress;
    Ebp = &Stack;
  }
  else
  {
    Ebp = (PSTACK)&pException - 1;  //frame addr of Get_Call_Stack()

    // Skip frame of Get_Call_Stack().
    if (!IsBadReadPtr(Ebp, sizeof(PSTACK)))
      Ebp = Ebp->Ebp;    //caller ebp
  }

  Str[0] = 0;
  Str_Len = 0;

  // Trace CALL_TRACE_MAX calls maximum - not to exceed DUMP_SIZE_MAX.
  // Break trace on wrong stack frame.
  for (int Ret_Addr_I = 0;
    (Ret_Addr_I < CALL_TRACE_MAX) && !IsBadReadPtr(Ebp, sizeof(PSTACK)) && !IsBadCodePtr(FARPROC(Ebp->Ret_Addr));
    Ret_Addr_I++, Ebp = Ebp->Ebp)
  {
    // If module with Ebp->Ret_Addr found.
    if (Get_Module_By_Ret_Addr(Ebp->Ret_Addr, Module_Name, Module_Addr_1))
    {
      if (Module_Addr_1 != Module_Addr)  //new module
      {
        // Save module's address and full path.
        Module_Addr = Module_Addr_1;
        Str_Len += wsprintf(Str + Str_Len, NL "%08X  %s", Module_Addr, Module_Name);
      }

      // Save call offset.
      Str_Len += wsprintf(Str + Str_Len,
        NL "  +%08X", Ebp->Ret_Addr - Module_Addr);

      // Save 5 params of the call. We don't know the real number of params.
      if (pException && !Ret_Addr_I)  //fake frame for exception address
        Str_Len += wsprintf(Str + Str_Len, "  Exception Offset");
      else if (!IsBadReadPtr(Ebp, sizeof(PSTACK) + 5 * sizeof(DWORD)))
      {
        Str_Len += wsprintf(Str + Str_Len, "  (%X, %X, %X, %X, %X)",
          Ebp->Param[0], Ebp->Param[1], Ebp->Param[2], Ebp->Param[3], Ebp->Param[4]);
      }
    }
    else
      Str_Len += wsprintf(Str + Str_Len, NL "%08X", Ebp->Ret_Addr);
  }

  return Str_Len;
} //Get_Call_Stack

  /**
    * Fill Str with Windows version.
    */
int WINAPI Get_Version_Str(PCHAR Str)
{
  OSVERSIONINFOEX  V = {sizeof(OSVERSIONINFOEX)};  //EX for NT 5.0 and later

  if (!GetVersionEx((POSVERSIONINFO)&V))
  {
    ZeroMemory(&V, sizeof(V));
    V.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx((POSVERSIONINFO)&V);
  }

  if (V.dwPlatformId != VER_PLATFORM_WIN32_NT)
    V.dwBuildNumber = LOWORD(V.dwBuildNumber);  //for 9x HIWORD(dwBuildNumber) = 0x04xx

  return wsprintf(Str,
    NL "Windows:  %d.%d.%d, SP %d.%d, Product Type %d",  //SP - service pack, Product Type - VER_NT_WORKSTATION,...
    V.dwMajorVersion, V.dwMinorVersion, V.dwBuildNumber, V.wServicePackMajor, V.wServicePackMinor, V.wProductType);
}


  /**
    * Allocate Str[DUMP_SIZE_MAX] and return Str with dump, if !pException - just return call stack in Str.
    */
PCHAR WINAPI Get_Exception_Info(PEXCEPTION_POINTERS pException)
{
  PCHAR    Str;
  int      Str_Len;
  int      i;
  CHAR    Module_Name[MAX_PATH];
  PBYTE    Module_Addr;
  HANDLE    hFile;
  FILETIME  Last_Write_Time;
  FILETIME  Local_File_Time;
  SYSTEMTIME  T;

  Str = new CHAR[DUMP_SIZE_MAX];

  if (!Str)
    return NULL;

  Str_Len = 0;
  Str_Len += Get_Version_Str(Str + Str_Len);

  Str_Len += wsprintf(Str + Str_Len, NL "Process:  ");
  GetModuleFileName(NULL, Str + Str_Len, MAX_PATH);
  Str_Len = lstrlen(Str);

  // If exception occurred.
  if (pException)
  {
    EXCEPTION_RECORD &  E = *pException->ExceptionRecord;
    CONTEXT &      C = *pException->ContextRecord;

    // If module with E.ExceptionAddress found - save its path and date.
    if (Get_Module_By_Ret_Addr((PBYTE)E.ExceptionAddress, Module_Name, Module_Addr))
    {
      Str_Len += wsprintf(Str + Str_Len,
        NL "Module:  %s", Module_Name);

      if ((hFile = CreateFile(Module_Name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
      {
        if (GetFileTime(hFile, NULL, NULL, &Last_Write_Time))
        {
          FileTimeToLocalFileTime(&Last_Write_Time, &Local_File_Time);
          FileTimeToSystemTime(&Local_File_Time, &T);

          Str_Len += wsprintf(Str + Str_Len,
            NL "Date Modified:  %02d/%02d/%d",
            T.wMonth, T.wDay, T.wYear);
        }
        CloseHandle(hFile);
      }
    }
    else
    {
      Str_Len += wsprintf(Str + Str_Len,
        NL "Exception Addr:  %08X", E.ExceptionAddress);
    }

    Str_Len += wsprintf(Str + Str_Len,
      NL "Exception Code:  %08X", E.ExceptionCode);

    if (E.ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
    {
      // Access violation type - Write/Read.
      Str_Len += wsprintf(Str + Str_Len,
        NL "%s Address:  %08X",
        (E.ExceptionInformation[0]) ? "Write" : "Read", E.ExceptionInformation[1]);
    }

    // Save instruction that caused exception.
    Str_Len += wsprintf(Str + Str_Len, NL "Instruction: ");
    for (i = 0; i < 16; i++)
      Str_Len += wsprintf(Str + Str_Len, " %02X", PBYTE(E.ExceptionAddress)[i]);

    // Save registers at exception.
    Str_Len += wsprintf(Str + Str_Len, NL "Registers:");
    Str_Len += wsprintf(Str + Str_Len, NL "EAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X", C.Eax, C.Ebx, C.Ecx, C.Edx);
    Str_Len += wsprintf(Str + Str_Len, NL "ESI: %08X  EDI: %08X  ESP: %08X  EBP: %08X", C.Esi, C.Edi, C.Esp, C.Ebp);
    Str_Len += wsprintf(Str + Str_Len, NL "EIP: %08X  EFlags: %08X", C.Eip, C.EFlags);
  } //if (pException)

  // Save call stack info.
  Str_Len += wsprintf(Str + Str_Len, NL "Call Stack:");
  Get_Call_Stack(pException, Str + Str_Len);

  if (Str[0] == NL[0])
    lstrcpy(Str, Str + sizeof(NL) - 1);

  return Str;
}

void WINAPI Create_Dump(PEXCEPTION_POINTERS pException, BOOL File_Flag, BOOL Show_Flag)
{
  HANDLE  hDump_File;
  PCHAR  Str;
  CHAR  Dump_Path[MAX_PATH];
  //DWORD  Bytes;

  Str = Get_Exception_Info(pException);

  if (Show_Flag && Str)
    MessageBox(NULL, Str, "MiniDump", MB_ICONHAND | MB_OK);

  if (File_Flag)
  {
    GetModuleFileName(NULL, Dump_Path, sizeof(Dump_Path));  //path of current process

    /*if (Str)
    {
      lstrcpy(Dump_Path + lstrlen(Dump_Path) - 3, "dmz");

      hDump_File = CreateFile(Dump_Path,
        GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

      WriteFile(hDump_File, Str, lstrlen(Str) + 1, &Bytes, NULL);

      CloseHandle(hDump_File);
    } */

    // If MiniDumpWriteDump() of DbgHelp.dll available.
    if (MiniDumpWriteDump_)
    {
      MINIDUMP_EXCEPTION_INFORMATION  M;

      M.ThreadId = GetCurrentThreadId();
      M.ExceptionPointers = pException;
      M.ClientPointers = 0;

      lstrcpy(Dump_Path + lstrlen(Dump_Path) - 3, "dmp");

      hDump_File = CreateFile(Dump_Path,
        GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

      MiniDumpWriteDump_(GetCurrentProcess(), GetCurrentProcessId(), hDump_File,
        MiniDumpNormal, (pException) ? &M : NULL, NULL, NULL);

      CloseHandle(hDump_File);
    }
  } //if (File_Flag)

  delete Str;
}


void InitDumping()
{
  HMODULE  hKernel32;

    //Try to get MiniDumpWriteDump() address.
  hDbgHelp = LoadLibrary("DBGHELP.DLL");
  MiniDumpWriteDump_ = (MINIDUMP_WRITE_DUMP) GetProcAddress( hDbgHelp, "MiniDumpWriteDump" );
  if( !MiniDumpWriteDump_ )
    std::cout << "MiniDump: warning dbghelp.dll not found, won't create minidump file" << std::endl;

    //Try to get Tool Help library functions.
  hKernel32 = GetModuleHandle( "KERNEL32" );
  CreateToolhelp32Snapshot_ = (CREATE_TOOL_HELP32_SNAPSHOT) GetProcAddress( hKernel32, "CreateToolhelp32Snapshot" );
  Module32First_ = (MODULE32_FIRST)GetProcAddress( hKernel32, "Module32First" );
  Module32Next_ = (MODULE32_NEST)GetProcAddress( hKernel32, "Module32Next" );
}

void CleanDumping()
{
  FreeLibrary( hDbgHelp );
}

void __stdcall MiniDump::Create_Dump( PEXCEPTION_POINTERS pException, BOOL File_Flag, BOOL Show_Flag )
{
  HANDLE hDump_File;
  PCHAR  Str;
  CHAR  Dump_Path[MAX_PATH];

  InitDumping();

  Str = Get_Exception_Info(pException);

#ifndef USESQT
  if( Show_Flag && Str )
    MessageBox( NULL, Str, "MiniDump", MB_ICONHAND | MB_OK );
#endif

  if( File_Flag )
  {
    GetModuleFileName( NULL, Dump_Path, sizeof( Dump_Path ) );  //path of current process

    if( MiniDumpWriteDump_ )
    {
      MINIDUMP_EXCEPTION_INFORMATION  M;

      M.ThreadId = GetCurrentThreadId();
      M.ExceptionPointers = pException;
      M.ClientPointers = 0;

      lstrcpy( Dump_Path + lstrlen( Dump_Path ) - 3, "dmp" );

      hDump_File = CreateFile( Dump_Path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

      MiniDumpWriteDump_( GetCurrentProcess(), GetCurrentProcessId(), hDump_File,  MiniDumpNormal, pException ? &M : NULL, NULL, NULL );

      CloseHandle( hDump_File );
#ifdef USESQT
      if( Show_Flag )
      {
        if( !qApp )
        {
          int argc = 0;
          QApplication app( argc, 0 );
          guicore::ShowMiniDump( Str, Dump_Path ).exec();
        }
        else
        {
          guicore::ShowMiniDump( Str, Dump_Path ).exec();
        }
      }
#endif
    }
  }

  CleanDumping();

  delete Str;
} 