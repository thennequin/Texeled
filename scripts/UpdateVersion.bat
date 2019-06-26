@echo off
rem Full version format is {major}.{minor}.{build}

if defined APPVEYOR_BUILD_VERSION (
    rem Extract {Major}.{Minor} build version from APPVEYOR_BUILD_VERSION
    setlocal EnableDelayedExpansion  
    
    for /f "tokens=1,2 delims=. " %%a in ("%APPVEYOR_BUILD_VERSION%") do (
        set MAJOR_MINOR_VERSION=%%a.%%b
    )

    echo MAJOR_MINOR_VERSION !MAJOR_MINOR_VERSION!
    
    echo #define TEXELED_VERSION !MAJOR_MINOR_VERSION! > %~dp0\\..\\src\\Version.h
    echo #define TEXELED_BUILD %APPVEYOR_BUILD_NUMBER% >> %~dp0\\..\\src\\Version.h
    echo #define TEXELED_BRANCH %APPVEYOR_REPO_BRANCH% >> %~dp0\\..\\src\\Version.h
    echo #define TEXELED_REVISION %APPVEYOR_REPO_COMMIT% >> %~dp0\\..\\src\\Version.h
    
    endlocal
)
