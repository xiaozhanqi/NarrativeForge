@echo off
REM 简化版构建脚本 - 使用 Visual Studio 命令行工具

echo ========================================
echo NarrativeForge 简化构建脚本
echo ========================================
echo.

REM 检查是否在正确的目录
if not exist "cpp\CMakeLists.txt" (
    echo ERROR: CMakeLists.txt not found!
    echo Please run this script from the project root directory.
    pause
    exit /b 1
)

echo 正在查找 Visual Studio 命令行工具...

REM 查找 Visual Studio 2022
set VS_PATH="C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat"

if exist %VS_PATH% (
    echo Found Visual Studio 2022 Insiders
) else (
    echo ERROR: Visual Studio 2022 not found!
    echo Please install Visual Studio 2022 with C++ desktop development workload.
    pause
    exit /b 1
)

echo.
echo 初始化 Visual Studio 命令行环境...
call %VS_PATH%
if errorlevel 1 (
    echo ERROR: Failed to initialize Visual Studio environment!
    pause
    exit /b 1
)

echo.
echo 创建构建目录...
if not exist "cpp\build" mkdir "cpp\build"

cd "cpp\build"

echo.
echo 配置 CMake...
cmake .. -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed!
    cd ..\..
    pause
    exit /b 1
)

echo.
echo 构建 NarrativeForge...
msbuild narrativeforge_cpp.sln /p:Configuration=Release /m
if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    cd ..\..
    pause
    exit /b 1
)

echo.
echo ========================================
echo 构建完成！
echo ========================================
echo.
echo 可执行文件位于:
echo   cpp\build\Release\
echo.
echo 运行命令行版本:
echo   cpp\build\Release\narrativeforge_cli.exe
echo.
echo 运行GUI版本:
echo   cpp\build\Release\narrativeforge_gui.exe
echo.

cd ..\..
pause
