call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
call "C:\api-vs2008\Qt-4.8.2\bin\qtvars.bat" vsvars
set PATH=%PATH%;C:\texlive\2011\bin\win32;c:\api-vs2008\nucleus\binaries;C:\texlive\2012\bin\win32;C:\texlive\2013\bin\win32;c:\progra~1\7-zip
set NUCLEUS=c:\api-vs2008\nucleus
set WIXDIR=c:\api-vs2008\wix

copy build\tools\jenkins-windows-localconfig.pri localconfig.pri
