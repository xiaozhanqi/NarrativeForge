# NarrativeForge 编译指南

## 概述

本文档详细介绍如何在不同平台上编译 NarrativeForge 项目。

## 系统要求

### 最低要求
- **操作系统**: Windows 10+, macOS 10.15+, Ubuntu 20.04+
- **编译器**: GCC 9+, Clang 10+, MSVC 2019+
- **CMake**: 3.16 或更高版本
- **内存**: 4GB RAM 最低，8GB 推荐

### 依赖库
- **OpenSSL**: 用于加密和HTTPS
- **SQLite3**: 用于数据库存储
- **libcurl**: 用于HTTP请求
- **nlohmann/json**: 用于JSON处理
- **Qt6**: 用于GUI界面（可选）

## Windows 编译指南

### 前置条件

1. **安装 Visual Studio 2022**
   - 下载地址: https://visualstudio.microsoft.com/
   - 安装时选择 "使用C++的桌面开发" 工作负载

2. **安装 CMake**
   - 下载地址: https://cmake.org/download/
   - 安装时选择 "Add CMake to the system PATH"

3. **安装 vcpkg (推荐)**
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install
   ```

### 使用 vcpkg 安装依赖

```powershell
.\vcpkg install openssl:x64-windows
.\vcpkg install sqlite3:x64-windows
.\vcpkg install curl:x64-windows
.\vcpkg install nlohmann-json:x64-windows
```

### 编译步骤

#### 方法1: 使用提供的构建脚本（推荐）

```powershell
# 在项目根目录下执行
.\build_windows.bat
```

#### 方法2: 手动编译

```powershell
# 1. 创建构建目录
cd cpp
mkdir build
cd build

# 2. 配置 CMake
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake

# 3. 编译
cmake --build . --config Release
```

### 编译输出

编译成功后，可执行文件位于：
```
cpp\build\Release\
├── narrativeforge_cli.exe      # 命令行版本
├── narrativeforge_gui.exe      # GUI版本
└── narrativeforge_web.exe      # Web服务器版本
```

## Linux 编译指南

### Ubuntu/Debian

#### 安装依赖

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libsqlite3-dev \
    libcurl4-openssl-dev \
    qt6-base-dev \
    nlohmann-json3-dev
```

#### 编译

```bash
# 方法1: 使用提供的构建脚本
./build.sh

# 方法2: 手动编译
cd cpp
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Fedora/RHEL

#### 安装依赖

```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    openssl-devel \
    sqlite-devel \
    libcurl-devel \
    qt6-qtbase-devel \
    nlohmann-json-devel
```

#### 编译

```bash
cd cpp
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Arch Linux

#### 安装依赖

```bash
sudo pacman -S --needed \
    base-devel \
    cmake \
    git \
    openssl \
    sqlite \
    curl \
    qt6-base \
    nlohmann-json
```

#### 编译

```bash
cd cpp
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## macOS 编译指南

### 前置条件

1. **安装 Xcode Command Line Tools**
   ```bash
   xcode-select --install
   ```

2. **安装 Homebrew**
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

3. **安装依赖**
   ```bash
   brew install cmake openssl sqlite curl nlohmann-json qt
   ```

### 编译

```bash
# 方法1: 使用提供的构建脚本
./build.sh

# 方法2: 手动编译
cd cpp
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$(brew --prefix qt)" \
    -DOPENSSL_ROOT_DIR="$(brew --prefix openssl)"
make -j$(sysctl -n hw.ncpu)
```

## CMake 配置选项

### 基本选项

```bash
# 构建类型
-DCMAKE_BUILD_TYPE=Release|Debug|RelWithDebInfo

# 安装前缀
-DCMAKE_INSTALL_PREFIX=/usr/local

# 启用/禁用组件
-DBUILD_GUI=ON|OFF          # 构建GUI
-DBUILD_WEB=ON|OFF          # 构建Web服务器
-DBUILD_TESTS=ON|OFF         # 构建测试
```

### 完整示例

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_GUI=ON \
    -DBUILD_WEB=ON \
    -DBUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX=/opt/narrativeforge
```

## 常见问题

### Windows 相关问题

**Q: CMake找不到 Visual Studio**
```
确保已安装 "使用C++的桌面开发" 工作负载
```

**Q: 找不到依赖库**
```
使用 vcpkg 并设置 CMAKE_TOOLCHAIN_FILE
```

### Linux 相关问题

**Q: 权限错误**
```
不要使用 sudo make install，使用 --prefix 到用户目录
```

**Q: Qt6 找不到**
```
安装 qt6-base-dev 或设置 CMAKE_PREFIX_PATH
```

### macOS 相关问题

**Q: Homebrew 包冲突**
```
brew unlink conflicting-package && brew link correct-package
```

**Q: 代码签名问题**
```
设置 CMAKE_MACOSX_RPATH=ON
```

## 测试编译结果

编译完成后，运行以下命令测试：

```bash
# Linux/macOS
./cpp/build/narrativeforge_cli --help

# Windows
.\cpp\build\Release\narrativeforge_cli.exe --help
```

## 安装

### 系统级安装

```bash
cd cpp/build
sudo make install
```

### 用户级安装

```bash
cd cpp/build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
make install
```

## 清理

```bash
# 清理构建文件
cd cpp
rm -rf build

# 完全清理（包括下载的依赖）
git clean -dfx
```

## 获取帮助

- **GitHub Issues**: https://github.com/xiaozhanqi/NarrativeForge/issues
- **项目文档**: 查看 `README.md`
- **API文档**: 查看 `functionality_report.md`

---

**最后更新**: 2026-03-15
