# NarrativeForge 环境配置详细指南

## 前置条件检查

### 1. 检查Visual Studio安装

打开PowerShell，运行以下命令检查Visual Studio：

```powershell
# 方法1: 使用vswhere
& "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath

# 方法2: 检查常见安装路径
Test-Path "C:\Program Files\Microsoft Visual Studio\2022\Community"
Test-Path "C:\Program Files\Microsoft Visual Studio\2022\Professional"
Test-Path "C:\Program Files\Microsoft Visual Studio\2022\Enterprise"
```

### 2. 安装CMake

#### 方法A: 使用MSI安装包（推荐）

1. 双击项目根目录的 `cmake-3.28.3-windows-x86_64.msi`
2. 按照安装向导操作
3. **重要**：在安装选项中勾选 "Add CMake to the system PATH"
4. 完成安装后，重启PowerShell或命令提示符

#### 方法B: 使用包管理器

如果您有Chocolatey或Scoop：

```powershell
# Chocolatey
choco install cmake

# Scoop
scoop install cmake
```

### 3. 验证CMake安装

打开新的PowerShell窗口，运行：

```powershell
cmake --version
```

应该显示类似：
```
cmake version 3.28.3
```

## 依赖库配置

### 方案1: 使用vcpkg（推荐）

#### 步骤1: 克隆vcpkg

```powershell
# 在f:\git目录下执行
cd f:\git
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
```

#### 步骤2: 引导vcpkg

```powershell
.\bootstrap-vcpkg.bat
```

#### 步骤3: 集成vcpkg

```powershell
.\vcpkg integrate install
```

#### 步骤4: 安装依赖

```powershell
.\vcpkg install openssl:x64-windows
.\vcpkg install sqlite3:x64-windows
.\vcpkg install nlohmann-json:x64-windows
```

### 方案2: 使用预编译的依赖库

如果您已经有这些库，可以设置以下环境变量：

```powershell
$env:OPENSSL_ROOT_DIR = "路径\到\openssl"
$env:SQLite3_ROOT = "路径\到\sqlite3"
```

## 构建项目

### 步骤1: 打开Developer Command Prompt

**方法A: 从开始菜单**
1. 按Win键，搜索 "Developer Command Prompt for VS 2022"
2. 以管理员身份运行

**方法B: 使用命令行**
```powershell
# 替换为您的Visual Studio路径
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64
```

### 步骤2: 配置CMake

```powershell
cd f:\git\1\cpp
mkdir build
cd build

# 使用vcpkg工具链（假设vcpkg在f:\git\vcpkg）
cmake .. -G "Visual Studio 17 2022" -A x64 `
    -DCMAKE_TOOLCHAIN_FILE="f:/git/vcpkg/scripts/buildsystems/vcpkg.cmake" `
    -DBUILD_GUI=OFF `
    -DBUILD_WEB=OFF
```

### 步骤3: 编译

```powershell
cmake --build . --config Release
```

## 验证构建

```powershell
cd f:\git\1\cpp\build\Release

# 检查文件
dir *.exe

# 测试运行
.\narrativeforge_unified.exe
```

## 常见问题解决

### 问题1: CMake找不到Visual Studio

**解决方案**:
- 确保安装了"使用C++的桌面开发"工作负载
- 使用Developer Command Prompt而不是普通的PowerShell

### 问题2: 找不到依赖库

**解决方案**:
- 确保vcpkg integrate install已执行
- 检查CMAKE_TOOLCHAIN_FILE路径是否正确
- 确保依赖库已使用:x64-windows三元组安装

### 问题3: 编译错误

**解决方案**:
- 确保使用Release配置
- 检查是否有足够的内存（建议8GB+）
- 查看详细的错误信息

## 快速参考

### 完整的一键构建脚本（PowerShell）

创建一个 `build_all.ps1` 文件：

```powershell
# 检查管理员权限
if (-not ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Warning "建议以管理员身份运行"
}

# 设置变量
$ProjectRoot = "f:\git\1"
$VcpkgRoot = "f:\git\vcpkg"

# 1. 检查vcpkg
if (-not (Test-Path $VcpkgRoot)) {
    Write-Host "克隆vcpkg..."
    cd (Split-Path $VcpkgRoot)
    git clone https://github.com/microsoft/vcpkg.git
    cd $VcpkgRoot
    .\bootstrap-vcpkg.bat
    .\vcpkg integrate install
}

# 2. 安装依赖（如果尚未安装）
cd $VcpkgRoot
if (-not (Test-Path "installed\x64-windows\include\openssl")) {
    Write-Host "安装依赖..."
    .\vcpkg install openssl:x64-windows
    .\vcpkg install sqlite3:x64-windows
    .\vcpkg install nlohmann-json:x64-windows
}

# 3. 构建项目
cd $ProjectRoot\cpp
if (-not (Test-Path build)) {
    mkdir build
}
cd build

Write-Host "配置CMake..."
cmake .. -G "Visual Studio 17 2022" -A x64 `
    -DCMAKE_TOOLCHAIN_FILE="$VcpkgRoot/scripts/buildsystems/vcpkg.cmake" `
    -DBUILD_GUI=OFF `
    -DBUILD_WEB=OFF

if ($LASTEXITCODE -eq 0) {
    Write-Host "编译项目..."
    cmake --build . --config Release
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "`n构建成功！" -ForegroundColor Green
        Write-Host "可执行文件位于: $ProjectRoot\cpp\build\Release\"
        Write-Host "运行: .\narrativeforge_unified.exe"
    }
}
```

---

**最后更新**: 2026-03-15
