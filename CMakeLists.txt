cmake_minimum_required(VERSION 3.20)

project(snake_lab LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Debug on single-config generators (Makefiles, Ninja)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Try CONFIG package first (vcpkg / package managers), then module
find_package(raylib CONFIG QUIET)
if(NOT raylib_FOUND)
    find_package(raylib QUIET)
endif()

if(NOT raylib_FOUND)
    message(FATAL_ERROR
        "raylib not found.\n"
        "Hints:\n"
        "  macOS:   brew install raylib\n"
        "  Windows: vcpkg install raylib:x64-windows "
        "and pass -DCMAKE_TOOLCHAIN_FILE to CMake.\n"
    )
endif()

add_executable(snake main.cpp)
target_link_libraries(snake PRIVATE raylib)
