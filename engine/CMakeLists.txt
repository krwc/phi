include(FindPkgConfig)
pkg_search_module(ASSIMP REQUIRED assimp)
pkg_search_module(GLFW3 REQUIRED glfw3)
pkg_search_module(GL REQUIRED gl)

add_subdirectory(renderer)
add_subdirectory(device)
add_subdirectory(scene)
add_subdirectory(utils)
add_subdirectory(math)
add_subdirectory(io)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(../libs)

file(GLOB_RECURSE PHI_SRC_CPP *.cpp)

add_library(PhiEngine SHARED ${PHI_SRC_CPP})
target_link_libraries(PhiEngine ${GL_LIBRARIES} ${ASSIMP_LIBRARIES})
