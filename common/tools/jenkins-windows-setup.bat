cd %~dp0\..\..

call "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
set PATH=C:\Qt\Qt5.5.1\5.5\msvc2010\bin;%PATH%
set PATH=%PATH%;c:\progra~1\7-zip;c:\api-vs2010\nucleus\binaries;c:\api-vs2010\nic3\binaries;c:\Python25
set PYTHON_PATH=c:\api-vs2010\nic3\binaries;c:\api-vs2010\nic3\python
set WIXDIR=c:\api-vs2010\wix3
set SYMSTOREDIR=c:\Program Files (x86)\Windows Kits\10\Debuggers\x86
set APEXSYMSTORE=c:\apex-symbols

:: protect MSPDBSRV from the jenkins tree killer
set ORIG_BUILD_ID=%BUILD_ID%
set BUILD_ID=DoNotKillMe
start mspdbsrv -start -spawn
set BUILD_ID=%ORIG_BUILD_ID%
set ORIG_BUILD_ID=

copy tools\jenkins-windows-localconfig.pri localconfig.pri
