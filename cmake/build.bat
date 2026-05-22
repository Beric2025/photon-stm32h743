@echo off
REM
REM build.bat - CMake build script for STM32 Embedded Framework (Windows)
REM
REM Usage:
REM   build.bat              Release build (default)
REM   build.bat debug        Debug build
REM   build.bat h750-debug   STM32H750 Debug build
REM   build.bat clean        Clean all build directories
REM
REM Requirements: arm-none-eabi-gcc, cmake (>= 3.16), mingw32-make
REM

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR=%SCRIPT_DIR%.."
set "BUILD_TYPE=%1"
if "%BUILD_TYPE%"=="" set "BUILD_TYPE=release"

if /I "%BUILD_TYPE%"=="clean" (
    echo Cleaning build directories...
    if exist "%PROJECT_DIR%\build" rmdir /s /q "%PROJECT_DIR%\build"
    echo Done.
    goto :eof
)

REM Check tools
where arm-none-eabi-gcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] arm-none-eabi-gcc not found in PATH
    echo Install: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain
    exit /b 1
)
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] cmake not found in PATH
    exit /b 1
)

REM Resolve preset name
if /I "%BUILD_TYPE%"=="debug" (
    set "PRESET=debug"
    set "BUILD_DIR=%PROJECT_DIR%\build\debug"
) else if /I "%BUILD_TYPE%"=="h750-debug" (
    set "PRESET=h750-debug"
    set "BUILD_DIR=%PROJECT_DIR%\build\h750-debug"
) else if /I "%BUILD_TYPE%"=="release" (
    set "PRESET=release"
    set "BUILD_DIR=%PROJECT_DIR%\build\release"
) else (
    echo [ERROR] Unknown build type: %BUILD_TYPE%
    echo Usage: build.bat [release^|debug^|h750-debug^|clean]
    exit /b 1
)

echo === STM32 Embedded Framework Build ===
echo Preset: %PRESET%

echo Configuring...
cmake --preset %PRESET% -S "%PROJECT_DIR%"

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configure failed
    exit /b 1
)

echo Building...
cmake --build "%BUILD_DIR%" -j%NUMBER_OF_PROCESSORS%

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Output: %BUILD_DIR%\stm32-embedded-framework.elf
) else (
    echo [ERROR] Build failed
    exit /b 1
)
