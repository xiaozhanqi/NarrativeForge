# NarrativeForge C++ Version

## Build Instructions

### Prerequisites
- C++17 or higher
- CMake 3.16+
- Qt6 (for GUI)
- OpenSSL
- SQLite3
- libcurl
- nlohmann/json

### Build on Linux/macOS
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Build on Windows
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
msbuild narrativeforge_cpp.sln
```

### Run CLI
```bash
./build/narrativeforge_cli
```

### Run GUI
```bash
./build/narrativeforge_gui
```

### Run Web Server
```bash
./build/narrativeforge_web
```

## Project Structure

```
cpp/
├── CMakeLists.txt
├── include/
│   └── narrativeforge/
│       ├── core.hpp
│       ├── api_router.hpp
│       └── api_key_manager.hpp
├── src/
│   ├── core/
│   │   ├── analyzer.cpp
│   │   ├── generator.cpp
│   │   ├── api_router.cpp
│   │   └── api_key_manager.cpp
│   ├── ui/
│   │   ├── main_window.cpp
│   │   ├── analyzer_widget.cpp
│   │   └── generator_widget.cpp
│   ├── web/
│   │   └── server.cpp
│   ├── main_cli.cpp
│   ├── main_gui.cpp
│   └── main_web.cpp
├── config/
│   ├── analyzer_schema.json
│   └── generator_schema.json
└── README.md
```

## Features

- **Core Engine**: Analysis and Generation pipelines
- **API Router**: Multi-provider support (Kimi, Volcano, OpenAI)
- **Encrypted Storage**: One machine one code API key storage
- **Qt6 GUI**: Modern desktop interface
- **Web Server**: RESTful API
- **CLI**: Command line interface

## Dependencies

- nlohmann/json - JSON parsing
- libcurl - HTTP client
- OpenSSL - Encryption
- SQLite3 - Database
- Qt6 - GUI framework

## License

MIT License
