@SET VSINSTALLDIR=H:\Program Files\Microsoft Visual Studio 8
@SET VCINSTALLDIR=H:\Program Files\Microsoft Visual Studio 8\VC
@SET FrameworkDir=H:\WINDOWS\Microsoft.NET\Framework
@SET FrameworkVersion=v2.0.50727
@SET FrameworkSDKDir=H:\Program Files\Microsoft Visual Studio 8\SDK\v2.0
@if "%VSINSTALLDIR%"=="" goto error_no_VSINSTALLDIR
@if "%VCINSTALLDIR%"=="" goto error_no_VCINSTALLDIR

@echo Setting environment for using Microsoft Visual Studio 2005 x86 tools.

@rem
@rem Root of Visual Studio IDE installed files.
@rem
@set DevEnvDir=H:\Program Files\Microsoft Visual Studio 8\Common7\IDE

@set PATH=H:\Program Files\Microsoft Visual Studio 8\Common7\IDE;H:\Program Files\Microsoft Visual Studio 8\VC\BIN;H:\Program Files\Microsoft Visual Studio 8\Common7\Tools;H:\Program Files\Microsoft Visual Studio 8\Common7\Tools\bin;H:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\bin;H:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\bin;H:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;H:\Program Files\Microsoft Visual Studio 8\VC\VCPackages;%PATH%
@set INCLUDE=H:\Program Files\Microsoft Visual Studio 8\VC\ATLMFC\INCLUDE;H:\Program Files\Microsoft Visual Studio 8\VC\INCLUDE;H:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\include;%INCLUDE%
@set LIB=H:\Program Files\Microsoft Visual Studio 8\VC\ATLMFC\LIB;H:\Program Files\Microsoft Visual Studio 8\VC\LIB;H:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\lib;H:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\lib;%LIB%
@set LIBPATH=H:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;H:\Program Files\Microsoft Visual Studio 8\VC\ATLMFC\LIB

@goto end

:error_no_VSINSTALLDIR
@echo ERROR: VSINSTALLDIR variable is not set. 
@goto end

:error_no_VCINSTALLDIR
@echo ERROR: VCINSTALLDIR variable is not set. 
@goto end

:end

qmake srcV2-template.pro

nmake %1