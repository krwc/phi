set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Gui)
find_package(Qt5Widgets)
find_package(Qt5Core)

include_directories(../)
include_directories(../libs)
include_directories(../engine)

include(FindPkgConfig)
pkg_search_module(GLFW3 REQUIRED glfw3)

file(GLOB_RECURSE EDITOR_SRC_CPP *.cpp)
file(GLOB_RECURSE EDITOR_SRC_UI *.ui)

add_executable(PhiEditor ${EDITOR_SRC_CPP})
target_link_libraries(PhiEditor PhiEngine ${GLFW3_LIBRARIES} Qt5::Gui Qt5::Widgets Qt5::Core)
