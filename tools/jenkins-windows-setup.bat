call "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
set PATH=C:\Qt\Qt5.5.1\5.5\msvc2010\bin;%PATH%
set PATH=%PATH%;C:\texlive\2011\bin\win32;c:\api-vs2010\nucleus\binaries;C:\texlive\2012\bin\win32;C:\texlive\2013\bin\win32;c:\progra~1\7-zip;c:\api-vs2010\nic3\binaries;c:\Python25
set PYTHON_PATH=c:\api-vs2010\nic3\binaries;c:\api-vs2010\nic3\python
set NUCLEUS=c:\api-vs2010\nucleus
set WIXDIR=c:\api-vs2010\wix3

copy tools\jenkins-windows-localconfig.pri localconfig.pri
