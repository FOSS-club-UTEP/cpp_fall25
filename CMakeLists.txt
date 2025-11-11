cmake_minimum_required(VERSION 3.20)
project(snake_lab  LANGUAGES C CXX)

# ---------- user options ----------------------------------------------------
option(RAYLIB_STATIC "Build with static raylib (single exe)" ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------- build type (single-config generators) --------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---------- find or fetch raylib --------------------------------------------
# 1.  vcpkg / system package managers (Windows, Linux, macOS)
find_package(raylib 5.0 QUIET CONFIG)
if(NOT raylib_FOUND)
    # 2.  Homebrew on macOS
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(raylib QUIET raylib>=5.0)
    endif()
endif()

# 3.  Nothing found → build raylib ourselves (automatic)
if(NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.5
    )
    # Options for the inner raylib build
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED   OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(raylib)
endif()

# ---------- create your game -------------------------------------------------
add_executable(snake fixed.cpp)

# Link the library (target name differs between “find” and “fetch”)
if(TARGET raylib)          # produced by FetchContent
    target_link_libraries(snake PRIVATE raylib)
else()                      # found by find_package
    target_link_libraries(snake PRIVATE raylib::raylib)
endif()

# ---------- install / package (optional) ------------------------------------
install(TARGETS snake DESTINATION bin)
