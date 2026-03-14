@echo off
REM Build script for NarrativeForge on Windows
REM Requires: CMake, Visual Studio, and dependencies

echo ========================================
echo NarrativeForge Build Script
echo ========================================
echo.

REM Check if we're in the right directory
if not exist "cpp\CMakeLists.txt" (
    echo ERROR: CMakeLists.txt not found!
    echo Please run this script from the project root directory.
    pause
    exit /b 1
)

echo Creating build directory...
if not exist "cpp\build" mkdir "cpp\build"

cd "cpp\build"

echo.
echo Configuring CMake...
cmake .. -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed!
    cd ..\..
    pause
    exit /b 1
)

echo.
echo Building NarrativeForge...
cmake --build . --config Release
if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    cd ..\..
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executables can be found in:
echo   cpp\build\Release\
echo.
echo To run the CLI version:
echo   cpp\build\Release\narrativeforge_cli.exe
echo.
echo To run the GUI version:
echo   cpp\build\Release\narrativeforge_gui.exe
echo.

cd ..\..
pause
