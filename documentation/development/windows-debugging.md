### Symbol store

On windows the following command adds the symbols (consisting of executables, shared objects and debugging symbols) to the symbol store:

`
"%SYMSTOREDIR%\symstore.exe" add /r /s %APEXSYMSTORE% /t "Apex" /v "Build %BUILD_NUMBER%" /f "bin\%RELEASETYPE%-installed\bin\*.*"
`

This is executed every APEX master build.

### Crash dumps

When APEX crashes it will make a minidump at the location of the executable. Together with the symbols from the symbol store these can be used to recreate the state APEX was in at the point of the crash.

### WinDbg

#### Symbols

We use WinBbg (part of the Windows SDK) as a debugger. Before we load in the crash dump we load the symbols. Open WinDbg and edit the symbol file path (Ctrl+s) and add the following:

`
.sympath srv*https://msdl.microsoft.com/download/symbols;c:\apex-symbols
`

The part before the semicolon adds the symbols for Windows libraries. The part after adds our own, edit this path if the symbols are stored elsewhere.

Note that the Qt symbols and library symbols should be added as well by executing a command similar to the one above. They should only be added once, so when using an existing store they're probably present.

#### Crashdump

Load the crashdump (Ctrl+D). In the command Window you probably want to start with `!analyze` which will pinpoint the cause of the crash. `.reload /f /v` is also useful to force the reloading of all symbols.

