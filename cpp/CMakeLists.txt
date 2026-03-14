cmake_minimum_required(VERSION 3.16)
project(NarrativeForge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_GUI "Build Qt GUI" ON)
option(BUILD_WEB "Build Web Server" ON)
option(BUILD_TESTS "Build tests" OFF)

# Dependencies
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

if(BUILD_GUI)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebEngineWidgets)
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
)

# Source files
set(CORE_SOURCES
    src/core/analyzer.cpp
    src/core/generator.cpp
    src/core/api_router.cpp
    src/core/api_key_manager.cpp
    src/core/database.cpp
    src/core/domain_adapter.cpp
)

set(UTIL_SOURCES
    src/util/logger.cpp
    src/util/config.cpp
)

# Core library
add_library(narrativeforge_core STATIC
    ${CORE_SOURCES}
    ${UTIL_SOURCES}
)

target_link_libraries(narrativeforge_core
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
    SQLite::SQLite3
)

# CLI executable
add_executable(narrativeforge_cli
    src/main_cli.cpp
)
target_link_libraries(narrativeforge_cli narrativeforge_core)

# GUI executable
if(BUILD_GUI)
    set(GUI_SOURCES
        src/ui/main_window.cpp
        src/ui/analyzer_widget.cpp
        src/ui/generator_widget.cpp
    )
    
    add_executable(narrativeforge_gui
        src/main_gui.cpp
        ${GUI_SOURCES}
    )
    
    target_link_libraries(narrativeforge_gui
        narrativeforge_core
        Qt6::Core
        Qt6::Widgets
        Qt6::WebEngineWidgets
    )
endif()

# Web server executable
if(BUILD_WEB)
    add_executable(narrativeforge_web
        src/main_web.cpp
        src/web/server.cpp
    )
    
    target_link_libraries(narrativeforge_web
        narrativeforge_core
    )
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install
install(TARGETS narrativeforge_cli RUNTIME DESTINATION bin)
if(BUILD_GUI)
    install(TARGETS narrativeforge_gui RUNTIME DESTINATION bin)
endif()
if(BUILD_WEB)
    install(TARGETS narrativeforge_web RUNTIME DESTINATION bin)
endif()
