cmake_minimum_required(VERSION 2.8.11)
project(phi)
set(CMAKE_CXX_FLAGS "-std=c++14 -march=native -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FindPkgConfig)
pkg_search_module(GLFW3 REQUIRED glfw3)
pkg_search_module(GL REQUIRED gl)

file(GLOB_RECURSE SRC_CPP src/*.cpp)
file(GLOB_RECURSE SRC_HPP src/*.h)

include_directories(src)
add_executable(phi ${SRC_CPP})
target_link_libraries(phi ${GLFW3_LIBRARIES} ${GL_LIBRARIES})
