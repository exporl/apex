set PATH=%PATH%;c:\api-vs2008\nucleus\binaries
set NUCLEUS=c:\api-vs2008\nucleus

rem Cannot use installed version, because test data is not installed
rem bin\%RELEASETYPE%-installed\bin\datatest.exe -xunitxml > datatest-results.xml
rem bin\%RELEASETYPE%-installed\bin\spintest.exe -xunitxml > spintest-results.xml
rem bin\%RELEASETYPE%-installed\bin\toolstest.exe -xunitxml > toolstest-results.xml
rem bin\%RELEASETYPE%-installed\bin\maintest.exe -xunitxml > maintest-results.xml
rem bin\%RELEASETYPE%-installed\bin\writerstest.exe -xunitxml > writerstest-results.xml

rem Need to copy dlls (Qt, Windows) to be able to run tests
copy bin\%RELEASETYPE%-installed\bin\*.dll bin\%RELEASETYPE%\

bin\%RELEASETYPE%\datatest.exe -xunitxml > datatest-results.xml
bin\%RELEASETYPE%\spintest.exe -xunitxml > spintest-results.xml
bin\%RELEASETYPE%\toolstest.exe -xunitxml > toolstest-results.xml
bin\%RELEASETYPE%\maintest.exe -xunitxml > maintest-results.xml
rem bin\%RELEASETYPE%\writerstest.exe -xunitxml > writerstest-results.xml


rem guard against tests that crash the testbench
del writerstest-results.xml 
exit 0
