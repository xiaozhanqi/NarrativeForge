# NarrativeForge 环境检查脚本
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "NarrativeForge 环境检查" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 1. 检查Visual Studio
Write-Host "[1/5] 检查Visual Studio..." -ForegroundColor Yellow
$vsInstallPath = $null
try {
    $vsInstallPath = & "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath 2>$null
    if ($vsInstallPath) {
        Write-Host "✅ Visual Studio 已安装: $vsInstallPath" -ForegroundColor Green
    } else {
        Write-Host "❌ 未找到Visual Studio" -ForegroundColor Red
    }
} catch {
    Write-Host "❌ 无法检查Visual Studio" -ForegroundColor Red
}

# 2. 检查CMake
Write-Host ""
Write-Host "[2/5] 检查CMake..." -ForegroundColor Yellow
try {
    $cmakeVersion = cmake --version 2>$null
    if ($cmakeVersion) {
        Write-Host "✅ CMake 已安装" -ForegroundColor Green
        Write-Host "   版本: $($cmakeVersion[0])" -ForegroundColor Gray
    } else {
        Write-Host "❌ CMake 未安装或不在PATH中" -ForegroundColor Red
        Write-Host "   请安装项目根目录的 cmake-3.28.3-windows-x86_64.msi" -ForegroundColor Yellow
    }
} catch {
    Write-Host "❌ 无法检查CMake" -ForegroundColor Red
}

# 3. 检查Git
Write-Host ""
Write-Host "[3/5] 检查Git..." -ForegroundColor Yellow
try {
    $gitVersion = git --version 2>$null
    if ($gitVersion) {
        Write-Host "✅ Git 已安装: $gitVersion" -ForegroundColor Green
    } else {
        Write-Host "⚠️  Git 未找到（vcpkg需要）" -ForegroundColor Yellow
    }
} catch {
    Write-Host "⚠️  无法检查Git" -ForegroundColor Yellow
}

# 4. 检查vcpkg
Write-Host ""
Write-Host "[4/5] 检查vcpkg..." -ForegroundColor Yellow
$vcpkgPaths = @(
    "f:\git\vcpkg",
    "..\vcpkg",
    ".\vcpkg"
)
$vcpkgFound = $false
foreach ($path in $vcpkgPaths) {
    if (Test-Path $path) {
        Write-Host "✅ vcpkg 已找到: $path" -ForegroundColor Green
        $vcpkgFound = $true
        
        # 检查依赖
        if (Test-Path "$path\installed\x64-windows\include\openssl") {
            Write-Host "   ✅ OpenSSL 已安装" -ForegroundColor Green
        } else {
            Write-Host "   ❌ OpenSSL 未安装" -ForegroundColor Red
        }
        
        if (Test-Path "$path\installed\x64-windows\include\sqlite3.h") {
            Write-Host "   ✅ SQLite3 已安装" -ForegroundColor Green
        } else {
            Write-Host "   ❌ SQLite3 未安装" -ForegroundColor Red
        }
        
        if (Test-Path "$path\installed\x64-windows\include\nlohmann") {
            Write-Host "   ✅ nlohmann-json 已安装" -ForegroundColor Green
        } else {
            Write-Host "   ❌ nlohmann-json 未安装" -ForegroundColor Red
        }
        break
    }
}
if (-not $vcpkgFound) {
    Write-Host "❌ vcpkg 未找到" -ForegroundColor Red
}

# 5. 检查项目文件
Write-Host ""
Write-Host "[5/5] 检查项目文件..." -ForegroundColor Yellow
$projectOk = $true

if (Test-Path "cpp\CMakeLists.txt") {
    Write-Host "✅ CMakeLists.txt 存在" -ForegroundColor Green
} else {
    Write-Host "❌ CMakeLists.txt 不存在" -ForegroundColor Red
    $projectOk = $false
}

if (Test-Path "cpp\include\narrativeforge\unified_controller.hpp") {
    Write-Host "✅ 统一控制器头文件存在" -ForegroundColor Green
} else {
    Write-Host "❌ 统一控制器头文件不存在" -ForegroundColor Red
    $projectOk = $false
}

if (Test-Path "cpp\src\core\unified_controller.cpp") {
    Write-Host "✅ 统一控制器实现存在" -ForegroundColor Green
} else {
    Write-Host "❌ 统一控制器实现不存在" -ForegroundColor Red
    $projectOk = $false
}

# 总结
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "检查总结" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "下一步操作：" -ForegroundColor Yellow
Write-Host ""

if (-not $cmakeVersion) {
    Write-Host "1. 安装CMake:" -ForegroundColor White
    Write-Host "   双击运行: cmake-3.28.3-windows-x86_64.msi" -ForegroundColor Gray
    Write-Host "   勾选: Add CMake to the system PATH" -ForegroundColor Gray
    Write-Host ""
}

if (-not $vcpkgFound) {
    Write-Host "2. 设置vcpkg:" -ForegroundColor White
    Write-Host "   cd f:\git" -ForegroundColor Gray
    Write-Host "   git clone https://github.com/microsoft/vcpkg.git" -ForegroundColor Gray
    Write-Host "   cd vcpkg" -ForegroundColor Gray
    Write-Host "   .\bootstrap-vcpkg.bat" -ForegroundColor Gray
    Write-Host "   .\vcpkg integrate install" -ForegroundColor Gray
    Write-Host ""
}

if ($vcpkgFound -and (-not (Test-Path "$path\installed\x64-windows\include\openssl"))) {
    Write-Host "3. 安装依赖:" -ForegroundColor White
    Write-Host "   cd f:\git\vcpkg" -ForegroundColor Gray
    Write-Host "   .\vcpkg install openssl:x64-windows" -ForegroundColor Gray
    Write-Host "   .\vcpkg install sqlite3:x64-windows" -ForegroundColor Gray
    Write-Host "   .\vcpkg install nlohmann-json:x64-windows" -ForegroundColor Gray
    Write-Host ""
}

Write-Host "4. 构建项目:" -ForegroundColor White
Write-Host "   打开: Developer Command Prompt for VS 2022" -ForegroundColor Gray
Write-Host "   cd f:\git\1\cpp" -ForegroundColor Gray
Write-Host "   mkdir build ; cd build" -ForegroundColor Gray
Write-Host "   cmake .. -G `"Visual Studio 17 2022`" -A x64 -DCMAKE_TOOLCHAIN_FILE=`"f:/git/vcpkg/scripts/buildsystems/vcpkg.cmake`" -DBUILD_GUI=OFF -DBUILD_WEB=OFF" -ForegroundColor Gray
Write-Host "   cmake --build . --config Release" -ForegroundColor Gray
Write-Host ""

Write-Host "详细指南请参阅: ENVIRONMENT_SETUP_GUIDE.md" -ForegroundColor Cyan
Write-Host ""
