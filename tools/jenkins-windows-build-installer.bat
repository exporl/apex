call tools\jenkins-windows-setup.bat

rem This must be set before the if condition as all variables inside the if are expanded before entering the block
set "ApexInstallPath=bin\%RELEASETYPE%-installed"

if "%RELEASETYPE%" == "release" (
    cd bin
    7z a ..\apex-windows-%RELEASETYPE%.zip %RELEASETYPE%-installed
    cd ..

    "%WIXDIR%\heat.exe" dir "%ApexInstallPath%" -ag -var env.ApexInstallPath -o ".build\%RELEASETYPE%\apex_generated.wxs" -cg AllFilesGroup -dr INSTALLDIR -srd -t tools\jenkins-windows-wix-cutapexexe.xsl || goto :EOF
    "%WIXDIR%\candle.exe" -out ".build\%RELEASETYPE%\apex_generated.wixobj" ".build\%RELEASETYPE%\apex_generated.wxs"  || goto :EOF
    "%WIXDIR%\candle.exe" -out ".build\%RELEASETYPE%\apex_main.wixobj" tools\jenkins-windows-wix-main.wxs || goto :EOF
    "%WIXDIR%\light.exe" -out apex_3.1.3.msi ".build\%RELEASETYPE%\apex_main.wixobj" ".build\%RELEASETYPE%\apex_generated.wixobj" -ext WixUIExtension  || goto :EOF
)
