cd %~dp0\..\..

del *-windows-%RELEASETYPE%.zip
rmdir /q /s bin\%RELEASETYPE%-installed
del *test-results.xml

if "%~1"=="clean" (
    rmdir /q /s bin
    rmdir /q /s .build
)

if "%RELEASETYPE%" == "release" (
    qmake RELEASE=1 || goto :EOF
) else (
    qmake RELEASE= || goto :EOF
)

nmake qmake_all || goto :EOF
nmake || goto :EOF
nmake install || goto :EOF

@if not exist bin\%RELEASETYPE%-installed\doc mkdir bin\%RELEASETYPE%-installed\doc
git show --stat > bin\%RELEASETYPE%-installed\doc\commit.txt
