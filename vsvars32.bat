@SET VSINSTALLDIR=C:\Program Files\Microsoft Visual Studio 8
@SET VCINSTALLDIR=%VSINSTALLDIR%\VC
@SET FrameworkDir=C:\WINDOWS\Microsoft.NET\Framework
@SET FrameworkVersion=v2.0.50727
@SET FrameworkSDKDir=%VSINSTALLDIR%\SDK\v2.0
@SET Framework35Version=v3.5
@if "%VSINSTALLDIR%"=="" goto error_no_VSINSTALLDIR
@if "%VCINSTALLDIR%"=="" goto error_no_VCINSTALLDIR

@echo Setting environment for using Microsoft Visual Studio 2005 x86 tools.

@call :GetWindowsSdkDir

@if not "%WindowsSdkDir%" == "" (
	set "PATH=%WindowsSdkDir%bin;%PATH%"
	set "INCLUDE=%WindowsSdkDir%include;%INCLUDE%"
	set "LIB=%WindowsSdkDir%lib;%LIB%"
)

@rem
@rem Root of Visual Studio IDE installed files.
@rem
@set DevEnvDir=%VSINSTALLDIR%\Common7\IDE

@set PATH=%VSINSTALLDIR%\Common7\IDE;%VCINSTALLDIR%\BIN;%VSINSTALLDIR%\Common7\Tools;C:\WINDOWS\Microsoft.NET\Framework\v3.5;%VSINSTALLDIR%\SDK\v2.0\bin;C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;%VCINSTALLDIR%\VCPackages;%PATH%
@set INCLUDE=%VCINSTALLDIR%\INCLUDE;%INCLUDE%
@set LIB=%VCINSTALLDIR%\LIB;%VSINSTALLDIR%\SDK\v2.0\lib;%LIB%
@set LIBPATH=C:\WINDOWS\Microsoft.NET\Framework\v3.5;C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;%VCINSTALLDIR%\LIB;%LIBPATH%

@goto end

:GetWindowsSdkDir
@call :GetWindowsSdk2008DirHelper HKLM > nul 2>&1
@if errorlevel 1 call :GetWindowsSdk2003DirHelper HKLM > nul 2>&1
@if errorlevel 1 call :GetWindowsSdk2008DirHelper HKCU > nul 2>&1
@if errorlevel 1 call :GetWindowsSdk2003DirHelper HKCU > nul 2>&1
@if errorlevel 1 set WindowsSdkDir=%VCINSTALLDIR%\PlatformSDK\
@exit /B 0

:GetWindowsSdk2008DirHelper
@for /F "tokens=1,2*" %%i in ('reg query "%1\SOFTWARE\Microsoft\Microsoft SDKs\Windows" /v "CurrentInstallFolder"') DO (
	if "%%i"=="CurrentInstallFolder" (
		SET "WindowsSdkDir=%%k"
	)
)
@if "%WindowsSdkDir%"=="" exit /B 1
@exit /B 0

:GetWindowsSdk2003DirHelper
@for /F "tokens=1,2,3*" %%i in ('reg query "%1\SOFTWARE\Microsoft\MicrosoftSDK\InstalledSDKs\D2FF9F89-8AA2-4373-8A31-C838BF4DBBE1" /v "Install Dir"') DO (
	if "%%i %%j"=="Install Dir" (
		SET "WindowsSdkDir=%%l"
	)
)
@if "%WindowsSdkDir%"=="" exit /B 1
@exit /B 0

:error_no_VSINSTALLDIR
@echo ERROR: VSINSTALLDIR variable is not set. 
@goto end

:error_no_VCINSTALLDIR
@echo ERROR: VCINSTALLDIR variable is not set. 
@goto end

:end
