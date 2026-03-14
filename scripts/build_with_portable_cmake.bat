@echo off
REM 使用便携版CMake构建NarrativeForge

echo ========================================
echo NarrativeForge 便携版CMake构建脚本
echo ========================================
echo.

REM 设置CMake路径
set CMAKE_PATH=%~dp0..\tools\cmake-3.19.4-win64-x64\bin\cmake.exe

echo 使用的CMake: %CMAKE_PATH%
echo.

REM 检查CMake是否存在
if not exist "%CMAKE_PATH%" (
    echo 错误: 找不到CMake便携版！
    echo 请确保cmake-3.19.4-win64-x64目录在项目根目录下
    pause
    exit /b 1
)

REM 检查是否在正确的目录
if not exist "cpp\CMakeLists.txt" (
    echo 错误: 找不到cpp\CMakeLists.txt！
    echo 请从项目根目录运行此脚本
    pause
    exit /b 1
)

REM 设置Visual Studio环境
echo 设置Visual Studio环境...
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 (
    echo 错误: 无法设置Visual Studio环境！
    pause
    exit /b 1
)

echo 创建构建目录...
if exist "cpp\build" rmdir /s /q "cpp\build"
mkdir "cpp\build"

cd "cpp\build"

echo.
echo 配置CMake...
"%CMAKE_PATH%" .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo.
    echo 错误: CMake配置失败！
    cd ..\..
    pause
    exit /b 1
)

echo.
echo 构建NarrativeForge...
nmake
if errorlevel 1 (
    echo.
    echo 错误: 构建失败！
    cd ..\..
    pause
    exit /b 1
)

echo.
echo ========================================
echo 构建成功！
echo ========================================
echo.
echo 可执行文件位于:
echo   cpp\build\
echo.
echo 运行统一控制器版本:
echo   cpp\build\narrativeforge_unified.exe
echo.

cd ..\..
pause
