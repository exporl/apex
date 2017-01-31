cd ..\..
C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\bin\vcvars32.bat
;nmake clean
qmake CLEBS_DISABLED="nucleus clarion screeneditor democontroller tests spin"
nmake qmake
nmake
nmake install
"c:\program files\nsis\makensis.exe" build\installer\apex.nsi