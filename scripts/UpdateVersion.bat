@echo off
if defined APPVEYOR_BUILD_NUMBER (
    echo ^#define TEXELED_VERSION %APPVEYOR_BUILD_VERSION% > %~dp0\\..\\src\\Version.h
    echo ^#define TEXELED_BUILD %APPVEYOR_BUILD_NUMBER% >> %~dp0\\..\\src\\Version.h
    echo ^#define TEXELED_BRANCH %APPVEYOR_REPO_BRANCH% >> %~dp0\\..\\src\\Version.h
    echo ^#define TEXELED_REVISION %APPVEYOR_REPO_COMMIT% >> %~dp0\\..\\src\\Version.h
)