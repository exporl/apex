call build\tools\jenkins-windows-setup.bat

del apex-windows-%RELEASETYPE%.zip
rmdir /q /s bin\%RELEASETYPE%-installed
rem rmdir /q /s bin
rem rmdir /q /s .build
del *test-results.xml


if "%RELEASETYPE%" == "release" (
    qmake RELEASE=1 || goto :EOF
) else (
    qmake RELEASE= || goto :EOF
)

nmake qmake || goto :EOF
nmake
nmake install || goto :EOF

git show --stat > bin/%RELEASETYPE%-installed/doc/commit.txt
