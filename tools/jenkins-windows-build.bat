cd %~dp0\..

git submodule update --force --init --remote common
call common\tools\jenkins-windows-git-setup.bat

call common\tools\jenkins-windows-setup.bat
call common\tools\jenkins-windows-build.bat %*

rem Build separate NIC tree

cd src\data\nicinstall
qmake BINDIR=..\..\..\bin\%RELEASETYPE%-installed-nic\bin || goto :EOF
nmake install || goto :EOF
cd ..\..\..

rem Testing

copy bin\%RELEASETYPE%-installed\bin\*.dll bin\%RELEASETYPE%\
xcopy /s /i /y bin\%RELEASETYPE%-installed\bin\google bin\%RELEASETYPE%\google

copy bin\%RELEASETYPE%-installed-nic\bin\*.dll bin\%RELEASETYPE%\
copy bin\%RELEASETYPE%-installed-nic\bin\*.pyd bin\%RELEASETYPE%\
xcopy /s /i /y bin\%RELEASETYPE%-installed-nic\bin\cochlear bin\%RELEASETYPE%\cochlear

nmake testxml

rem Store symbols
if "%~2" == "storesymbols" (
   echo Adding symbols
   "%SYMSTOREDIR%\symstore.exe" add /r /s %APEXSYMSTORE% /t "Apex" /v "Build %BUILD_NUMBER%" /f "bin\%RELEASETYPE%\*.*"
)

rem Zipping and installer

rem This must be set before the if condition as all variables inside the if are expanded before entering the block
set "ApexInstallPath=bin\%RELEASETYPE%-installed"
if "%RELEASETYPE%" == "release" (
    rem Clean some testbench reference files not needed in the installer
    cd bin\%RELEASETYPE%-installed
    rmdir /s /q tests regression cohregressiontests
    cd ..\..

    cd bin
    7z a ..\apex-windows-%RELEASETYPE%.zip %RELEASETYPE%-installed
    7z a ..\apex-windows-%RELEASETYPE%-nic.zip %RELEASETYPE%-installed-nic
    cd ..

    "%WIXDIR%\heat.exe" dir "%ApexInstallPath%" -ag -var env.ApexInstallPath -o ".build\%RELEASETYPE%\apex_generated.wxs" -cg AllFilesGroup -dr INSTALLDIR -srd -t tools\jenkins-windows-wix-cutapexexe.xsl || goto :EOF
    "%WIXDIR%\candle.exe" -out ".build\%RELEASETYPE%\apex_generated.wixobj" ".build\%RELEASETYPE%\apex_generated.wxs"  || goto :EOF
    "%WIXDIR%\candle.exe" -out ".build\%RELEASETYPE%\apex_main.wixobj" tools\jenkins-windows-wix-main.wxs || goto :EOF
    "%WIXDIR%\light.exe" -out apex_4.1.2.msi ".build\%RELEASETYPE%\apex_main.wixobj" ".build\%RELEASETYPE%\apex_generated.wixobj" -ext WixUIExtension  || goto :EOF
)

exit 0
