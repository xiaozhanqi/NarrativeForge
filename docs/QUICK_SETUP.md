# NarrativeForge 快速环境配置指南

## 当前环境状态

✅ **已就绪：**
- Git已安装 (version 2.53.0)
- CMake便携版已就绪 (cmake-3.19.4-win64-x64)
- 所有项目文件完整
- CMakeLists.txt已优化
- 统一控制器模块已开发

⚠️ **需要配置：**
- vcpkg未设置
- 依赖库未安装

## 快速配置步骤（按顺序执行）

### 第0步（可选）：使用便携版CMake

**好消息！** 便携版CMake已经在项目根目录下！

**快速开始：**
- 直接双击运行：`build_with_portable_cmake.bat`
- 或者手动使用便携版CMake：`cmake-3.19.4-win64-x64\bin\cmake.exe

**注意：** 即使有便携版CMake，您仍然需要配置依赖库！

### 第1步：安装CMake（可选，如果已有便携版）

如果您想安装系统版CMake：
**双击运行：** `cmake-3.28.3-windows-x86_64.msi

**重要提示：**
- 在安装向导中，勾选 **"Add CMake to the system PATH"**
- 完成安装后，**重启PowerShell**或命令提示符

**验证安装：**
```powershell
cmake --version
```
应该显示：`cmake version 3.28.3`

---

### 步骤2: 设置vcpkg

在 `f:\git` 目录下执行：

```powershell
cd f:\git
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

---

### 步骤3: 安装依赖库

在 `f:\git\vcpkg` 目录下执行：

```powershell
.\vcpkg install openssl:x64-windows
.\vcpkg install sqlite3:x64-windows
.\vcpkg install nlohmann-json:x64-windows
```

**注意：** 这一步可能需要较长时间（30分钟+），取决于网络速度。

---

### 步骤4: 构建项目

#### 4.1 打开Developer Command Prompt

按Win键，搜索并打开：
**"Developer Command Prompt for VS 2022"**

#### 4.2 配置和构建

```powershell
cd f:\git\1\cpp
mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_TOOLCHAIN_FILE="f:/git/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
    -DBUILD_GUI=OFF ^
    -DBUILD_WEB=OFF

cmake --build . --config Release
```

---

## 验证构建成功

构建完成后，检查以下文件：

```
f:\git\1\cpp\build\Release\
├── narrativeforge_core.lib       ✓ 核心库
├── narrativeforge_cli.exe        ✓ 命令行版本
└── narrativeforge_unified.exe    ✓ 统一控制器版本 (新)
```

运行测试：
```powershell
cd f:\git\1\cpp\build\Release
.\narrativeforge_unified.exe
```

---

## 常见问题

### Q: CMake安装后仍然找不到？
A: 确保重启了PowerShell或命令提示符，检查PATH环境变量。

### Q: vcpkg安装依赖很慢？
A: 这是正常的，vcpkg会从源码编译依赖库。可以考虑使用预编译的二进制缓存。

### Q: 找不到Visual Studio？
A: 确保安装了"使用C++的桌面开发"工作负载，并且使用Developer Command Prompt。

### Q: 构建时找不到依赖库？
A: 检查CMAKE_TOOLCHAIN_FILE路径是否正确，确保使用了:x64-windows三元组。

---

## 备选方案

如果上述步骤太复杂，可以考虑：

1. **使用Linux/WSL**：依赖安装更容易
2. **使用Docker**：隔离的构建环境
3. **等待预编译版本**：如果有提供的话

---

## 项目文件说明

**核心文件：**
- `cpp/include/narrativeforge/unified_controller.hpp` - 统一控制器头文件
- `cpp/src/core/unified_controller.cpp` - 统一控制器实现
- `cpp/src/main_unified.cpp` - 统一控制器主入口
- `cpp/CMakeLists.txt` - 构建配置（已优化）

**构建脚本：**
- `build_with_portable_cmake.bat` - 使用便携版CMake的一键构建脚本（推荐）
- `build_windows.bat` - Windows标准构建脚本
- `build_simple.bat` - 简化版构建脚本
- `build.sh` - Linux/macOS构建脚本

**文档文件：**
- `README.md` - 项目主文档（已更新到0.5.2）
- `ENVIRONMENT_SETUP_GUIDE.md` - 详细环境配置指南
- `BUILD_SUMMARY.md` - 构建流程总结
- `FINAL_BUILD_REPORT.md` - 最终构建报告
- `QUICK_SETUP.md` - 本文件

---

**最后更新**: 2026-03-15  
**版本**: 0.5.2 beta
