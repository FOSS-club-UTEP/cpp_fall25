cmake_minimum_required(VERSION 3.20)
project(snake_asan_lab CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib CONFIG REQUIRED)

add_executable(snake main.cpp)
target_link_libraries(snake PRIVATE raylib)

if (MSVC)
  target_compile_options(snake PRIVATE /Zi /Od /fsanitize=address)
  target_link_options(snake PRIVATE /INCREMENTAL /fsanitize=address)
else()
  target_compile_options(snake PRIVATE
    -g -O1 -fno-omit-frame-pointer
    -fsanitize=address,undefined
  )
  target_link_options(snake PRIVATE
    -fsanitize=address,undefined
  )
endif()
