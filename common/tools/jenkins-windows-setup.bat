cd %~dp0\..\..

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
set PATH=C:\Qt\Qt5.12.0\5.12.0\msvc2017\bin;%PATH%
set PATH=%PATH%;c:\progra~1\7-zip;c:\api-vs2017\nucleus\binaries;c:\api-vs2017\nic3\binaries;c:\Python25
set PYTHON_PATH=c:\api-vs2017\nic3\binaries;c:\api-vs2017\nic3\python
set WIXDIR=c:\api-vs2017\wix3
set SYMSTOREDIR=c:\Program Files (x86)\Windows Kits\10\Debuggers\x86
set APEXSYMSTORE=c:\apex-symbols

:: protect MSPDBSRV from the jenkins tree killer
set ORIG_BUILD_ID=%BUILD_ID%
set BUILD_ID=DoNotKillMe
start mspdbsrv -start -spawn
set BUILD_ID=%ORIG_BUILD_ID%
set ORIG_BUILD_ID=

copy tools\jenkins-windows-localconfig.pri localconfig.pri
