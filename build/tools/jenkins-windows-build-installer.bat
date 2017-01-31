call build\tools\jenkins-windows-setup.bat

cd bin
7z a ..\apex-windows-%RELEASETYPE%.zip %RELEASETYPE%-installed
cd ..


if "%RELEASETYPE%" == "release" (
    "%WIXDIR%\candle.exe" -out bin\apex.wixobj build\installer\wix\apex.wxs  || goto :EOF
    "%WIXDIR%\light.exe" -out bin\apex_3.0.x.msi bin\apex.wixobj "%WIXDIR%\wixui.wixlib" -loc "%WIXDIR%\WixUI_en-us.wxl"  || goto :EOF

    "%WIXDIR%\candle.exe" -out bin\spin.wixobj build\installer\wix\spin.wxs  || goto :EOF
    "%WIXDIR%\light.exe" -out bin\apex_spin_plugin_3.0.x.msi bin\spin.wixobj "%WIXDIR%\wixui.wixlib" -loc "%WIXDIR%\WixUI_en-us.wxl"  || goto :EOF
)

