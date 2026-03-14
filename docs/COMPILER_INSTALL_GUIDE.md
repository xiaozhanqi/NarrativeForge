# 编译器安装指南

## 所需工具

1. **Visual Studio 2022 Community** - C++编译器
2. **CMake 3.16+** - 构建系统
3. **vcpkg** - 依赖管理工具

## 步骤1: 安装 Visual Studio 2022 Community

### 下载 Visual Studio 2022

1. 访问 Visual Studio 下载页面：
   https://visualstudio.microsoft.com/zh-hans/downloads/

2. 点击 **"免费下载"** 按钮，选择 **Visual Studio Community 2022**

### 安装 Visual Studio 2022

1. 运行下载的安装程序（如 `vs_community.exe`）

2. 在安装界面中，选择 **"使用C++的桌面开发"** 工作负载

3. 确保勾选以下组件：
   - MSVC v143 - VS 2022 C++ x64/x86 生成工具
   - Windows 10 SDK (10.0.19041.0) 或更高版本
   - C++ 核心功能
   - C++ 性能工具
   - C++ ATL 用于最新 v143 生成工具
   - C++ MFC 用于最新 v143 生成工具

4. 点击 **"安装"** 按钮开始安装

5. 安装完成后，重启电脑

## 步骤2: 安装 CMake

### 下载 CMake

1. 访问 CMake 下载页面：
   https://cmake.org/download/

2. 下载 **Windows win64-x64 Installer** 版本（如 `cmake-3.28.3-windows-x86_64.msi`）

### 安装 CMake

1. 运行下载的安装程序

2. 在安装界面中，勾选 **"Add CMake to the system PATH for all users"**

3. 点击 **"Next"** 完成安装

4. 安装完成后，打开新的命令提示符，验证安装：
   ```powershell
   cmake --version
   ```
   应该显示 CMake 版本信息

## 步骤3: 安装 vcpkg

### 克隆 vcpkg 仓库

1. 打开命令提示符，执行以下命令：
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   ```

### 初始化 vcpkg

1. 执行引导脚本：
   ```powershell
   .\bootstrap-vcpkg.bat
   ```

2. 集成 vcpkg 到系统：
   ```powershell
   .\vcpkg integrate install
   ```

### 安装项目依赖

1. 安装必要的依赖库：
   ```powershell
   .\vcpkg install openssl:x64-windows
   .\vcpkg install sqlite3:x64-windows
   .\vcpkg install curl:x64-windows
   .\vcpkg install nlohmann-json:x64-windows
   .\vcpkg install qt6-base:x64-windows
   ```

## 验证安装

1. 打开新的命令提示符，执行以下命令：
   ```powershell
   # 检查 Visual Studio 编译器
   cl
   
   # 检查 CMake
   cmake --version
   
   # 检查 vcpkg
   .\vcpkg\vcpkg --version
   ```

2. 所有命令都应该正常执行，显示版本信息

## 编译项目

### 使用构建脚本

1. 进入项目根目录

2. 执行构建脚本：
   ```powershell
   .\build_windows.bat
   ```

### 手动编译

1. 进入 cpp 目录：
   ```powershell
   cd cpp
   mkdir build
   cd build
   ```

2. 配置 CMake：
   ```powershell
   cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=[vcpkg路径]/scripts/buildsystems/vcpkg.cmake
   ```
   注意：将 `[vcpkg路径]` 替换为实际的 vcpkg 安装路径

3. 编译项目：
   ```powershell
   cmake --build . --config Release
   ```

## 常见问题

### 问题1: Visual Studio 安装失败
- 确保有足够的磁盘空间（至少 20GB）
- 确保网络连接稳定
- 尝试以管理员身份运行安装程序

### 问题2: CMake 找不到编译器
- 确保 Visual Studio 安装正确
- 确保重启电脑后再尝试
- 检查环境变量是否正确设置

### 问题3: vcpkg 安装依赖失败
- 确保网络连接稳定
- 尝试使用代理
- 检查防火墙设置

## 技术支持

如果遇到安装问题，可以参考以下资源：

- Visual Studio 文档：https://docs.microsoft.com/zh-cn/visualstudio/
- CMake 文档：https://cmake.org/documentation/
- vcpkg 文档：https://github.com/microsoft/vcpkg

---

**安装完成后，您就可以编译和运行 NarrativeForge 项目了！**
