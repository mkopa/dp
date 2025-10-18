@echo off
REM DirPacker Build Script for Windows

setlocal

set BUILD_TYPE=Release
if not "%1"=="" set BUILD_TYPE=%1

set BUILD_DIR=build

echo ======================================
echo DirPacker CMake Build Script (Windows)
echo ======================================
echo Build type: %BUILD_TYPE%
echo.

REM Clean build directory
if exist "%BUILD_DIR%" (
    echo Cleaning existing build directory...
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

REM Configure
echo Configuring CMake...
cmake -G "Visual Studio 17 2022" -A x64 ..
if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR: CMake configuration failed!
    echo.
    echo If you don't have Visual Studio 2022, try:
    echo   cmake -G "Visual Studio 16 2019" -A x64 ..
    echo or use MinGW:
    echo   cmake -G "MinGW Makefiles" ..
    exit /b 1
)

REM Build
echo.
echo Building...
cmake --build . --config %BUILD_TYPE% -j
if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR: Build failed!
    exit /b 1
)

echo.
echo ======================================
echo Build completed successfully!
echo ======================================
echo Executable location: %BUILD_DIR%\%BUILD_TYPE%\dirpacker.exe
echo.
echo To run:
echo   %BUILD_DIR%\%BUILD_TYPE%\dirpacker.exe --help
echo.
echo To install (optional):
echo   cmake --install %BUILD_DIR%
echo ======================================

endlocal
