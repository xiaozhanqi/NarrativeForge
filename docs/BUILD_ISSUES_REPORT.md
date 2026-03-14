# NarrativeForge 构建问题分析与解决方案

## 当前状态分析

### 已识别的构建问题

1. **CMake未安装或未在PATH中**
   - 项目根目录有CMake安装包：`cmake-3.28.3-windows-x86_64.msi` 和 `.zip`
   - 需要安装或配置CMake

2. **依赖库缺失**
   - OpenSSL (必需)
   - SQLite3 (必需)
   - nlohmann/json (必需)
   - Qt6 (可选，已禁用)
   - Web服务器组件 (可选，已禁用)

3. **源代码文件缺失**
   - `src/core/database.cpp` (已从CMakeLists.txt中移除)
   - `src/core/domain_adapter.cpp` (已从CMakeLists.txt中移除)
   - `src/util/logger.cpp` (已从CMakeLists.txt中移除)
   - `src/util/config.cpp` (已从CMakeLists.txt中移除)

## 解决方案

### 步骤1: 安装CMake

**方法A: 使用提供的安装包**
```powershell
# 安装MSI包
cmake-3.28.3-windows-x86_64.msi
# 安装时选择 "Add CMake to the system PATH"
```

**方法B: 使用ZIP包**
```powershell
# 解压ZIP包并添加到PATH
Expand-Archive cmake-3.28.3-windows-x86_64.zip -DestinationPath .
$env:PATH += ";$PWD\cmake-3.28.3-windows-x86_64\bin"
```

### 步骤2: 安装vcpkg和依赖库

```powershell
# 克隆vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# 安装必需的依赖
.\vcpkg install openssl:x64-windows
.\vcpkg install sqlite3:x64-windows
.\vcpkg install nlohmann-json:x64-windows
```

### 步骤3: 配置和构建

```powershell
# 回到项目目录
cd f:\git\1\cpp

# 创建构建目录
mkdir build
cd build

# 配置CMake (使用vcpkg工具链)
cmake .. -G "Visual Studio 17 2022" -A x64 `
    -DCMAKE_TOOLCHAIN_FILE="f:/git/vcpkg/scripts/buildsystems/vcpkg.cmake" `
    -DBUILD_GUI=OFF `
    -DBUILD_WEB=OFF

# 构建
cmake --build . --config Release
```

## 简化的替代方案

如果无法安装完整依赖，可以考虑：

1. **使用预编译的二进制文件** (如果可用)
2. **在Linux环境中构建** (依赖更易安装)
3. **使用Docker容器** (隔离的构建环境)

## 最小化CMake配置 (临时方案)

当前CMakeLists.txt已配置为：
- ✅ 禁用GUI构建 (`BUILD_GUI=OFF`)
- ✅ 禁用Web构建 (`BUILD_WEB=OFF`)
- ✅ 移除了缺失的源文件引用
- ✅ 保留了核心功能模块

## 预期构建输出

成功构建后，应在 `cpp/build/Release/` 目录下生成：
- `narrativeforge_core.lib` - 核心库
- `narrativeforge_cli.exe` - 命令行版本
- `narrativeforge_unified.exe` - 统一控制器版本

## 验证步骤

构建完成后，运行以下验证：

```powershell
# 检查可执行文件是否存在
Test-Path cpp\build\Release\narrativeforge_unified.exe

# 尝试运行统一控制器
.\cpp\build\Release\narrativeforge_unified.exe
```

## 下一步

1. 安装CMake
2. 设置vcpkg和依赖
3. 运行完整构建流程
4. 验证构建产物
