call tools\jenkins-windows-setup.bat

rem Need to copy dlls (Qt, Windows) to be able to run tests
copy bin\%RELEASETYPE%-installed\bin\*.dll bin\%RELEASETYPE%\
copy bin\%RELEASETYPE%-installed\bin\*.pyd bin\%RELEASETYPE%\
xcopy /s /i /y bin\%RELEASETYPE%-installed\bin\cochlear bin\%RELEASETYPE%\cochlear
xcopy /s /i /y bin\%RELEASETYPE%-installed\bin\google bin\%RELEASETYPE%\google

nmake testxml

exit 0
