# NarrativeForge 编译指南 - 快速启动版

## 当前环境检查

根据检查结果，您的系统缺少以下编译工具：
- ❌ CMake (构建系统)
- ❌ Visual Studio 或其他 C++ 编译器

## 快速安装指南

### 步骤1: 安装 Visual Studio 2022

1. 下载 Visual Studio 2022 Community 版
   - 下载地址: https://visualstudio.microsoft.com/zh-hans/downloads/

2. 安装时选择 **"使用C++的桌面开发"** 工作负载
   - 确保勾选所有 C++ 相关组件

### 步骤2: 安装 CMake

1. 下载 CMake 3.16 或更高版本
   - 下载地址: https://cmake.org/download/

2. 安装时选择 **"Add CMake to the system PATH"**

### 步骤3: 安装 vcpkg (推荐)

1. 打开命令提示符，执行以下命令：
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install
   ```

2. 安装依赖库：
   ```powershell
   .\vcpkg install openssl:x64-windows
   .\vcpkg install sqlite3:x64-windows
   .\vcpkg install curl:x64-windows
   .\vcpkg install nlohmann-json:x64-windows
   .\vcpkg install qt6-base:x64-windows
   ```

## 编译步骤

### 方法1: 使用提供的构建脚本

1. 打开命令提示符，进入项目根目录
2. 执行：
   ```powershell
   .\build_windows.bat
   ```

### 方法2: 手动编译

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

3. 编译：
   ```powershell
   cmake --build . --config Release
   ```

## 验证编译结果

编译成功后，可执行文件位于：
```
cpp\build\Release\
├── narrativeforge_cli.exe      # 命令行版本
├── narrativeforge_gui.exe      # GUI版本
└── narrativeforge_web.exe      # Web服务器版本
```

## 运行程序

### 命令行版本
```powershell
.\cpp\build\Release\narrativeforge_cli.exe
```

### GUI版本
```powershell
.\cpp\build\Release\narrativeforge_gui.exe
```

## 常见问题

### 问题1: CMake 找不到 Visual Studio
- 确保已安装 "使用C++的桌面开发" 工作负载
- 确保 Visual Studio 版本为 2019 或更高

### 问题2: 找不到依赖库
- 确保使用 vcpkg 安装了所有依赖
- 确保设置了正确的 CMAKE_TOOLCHAIN_FILE

### 问题3: 编译失败
- 检查是否有足够的磁盘空间
- 检查内存是否足够 (建议至少 4GB)
- 查看详细错误信息并解决

## 联系支持

如果遇到编译问题，请参考完整的编译指南：
- `BUILD_GUIDE.md` - 详细编译指南
- GitHub Issues: https://github.com/xiaozhanqi/NarrativeForge/issues

---

**完成时间**: 2026-03-15
