
#cmake_minimum_required(VERSION 3.20)
#project(snake_asan_lab CXX)
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(raylib CONFIG REQUIRED)

#add_executable(snake main.cpp)
#target_link_libraries(snake PRIVATE raylib)

#if (MSVC)
  #target_compile_options(snake PRIVATE /Zi /Od /fsanitize=address)
  #target_link_options(snake PRIVATE /INCREMENTAL /fsanitize=address)
#else()
  #target_compile_options(snake PRIVATE
    #-g -O1 -fno-omit-frame-pointer
   # -fsanitize=address,undefined
  #)
 # target_link_options(snake PRIVATE
  #  -fsanitize=address,undefined
  #)
#endif()

cmake_minimum_required(VERSION 3.20)

project(snake_asan_lab LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Debug on single-config generators (Makefiles, Ninja)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Options to control sanitizers
option(ENABLE_ASAN "Enable AddressSanitizer" ON)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer (non-MSVC)" ON)

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

# Sanitizer configuration
# Only apply strong sanitizer flags to Debug-like builds.
# Multi-config (Visual Studio, Xcode) users should select Debug manually.
set(_is_debug FALSE)
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config: assume Debug config will be used
    set(_is_debug TRUE)
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(_is_debug TRUE)
endif()

if(_is_debug)
    if(MSVC)
        # MSVC AddressSanitizer support (VS 2019 16.9+ on x64)
        if(ENABLE_ASAN)
            target_compile_options(snake PRIVATE /Zi /Od /fsanitize=address)
            target_link_options(snake PRIVATE /INCREMENTAL /fsanitize=address)
        endif()
        # UBSan style options are not standardized on MSVC, so skipped.
    else()
        # GCC / Clang (Linux, macOS)
        set(_san_flags "")

        if(ENABLE_ASAN)
            list(APPEND _san_flags -fsanitize=address)
        endif()

        if(ENABLE_UBSAN)
            list(APPEND _san_flags -fsanitize=undefined)
        endif()

        if(_san_flags)
            target_compile_options(snake PRIVATE
                -g -O1 -fno-omit-frame-pointer
                ${_san_flags}
            )
            target_link_options(snake PRIVATE
                ${_san_flags}
            )
        endif()
    endif()
endif()

