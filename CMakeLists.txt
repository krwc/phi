cmake_minimum_required(VERSION 2.8.11)
project(phi)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -Wpedantic -O3")
set(CMAKE_CXX_FLAGS_DEBUG   "-Wall -Wextra -Wpedantic -ggdb -Og")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_TYPE Debug)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

file(COPY assets DESTINATION .)

add_subdirectory(engine)
add_subdirectory(editor)
