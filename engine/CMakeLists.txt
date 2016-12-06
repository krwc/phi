include(FindPkgConfig)
pkg_search_module(ASSIMP REQUIRED assimp)
pkg_search_module(GL REQUIRED gl)

option(INTEL_WORKAROUNDS "Enables/Disables Intel driver workarounds" OFF)
include(ExternalProject)

set(MCPP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../libs/mcpp")
ExternalProject_Add(mcpp
    PREFIX "${MCPP_SOURCE_DIR}"
    INSTALL_DIR "${MCPP_SOURCE_DIR}/install"
    DOWNLOAD_DIR "${MCPP_SOURCE_DIR}"
    URL "https://sourceforge.net/projects/mcpp/files/mcpp/V.2.7.2/mcpp-2.7.2.tar.gz/download"
    URL_HASH "MD5=512de48c87ab023a69250edc7a0c7b05"
    CONFIGURE_COMMAND ./configure --enable-mcpplib --prefix=<INSTALL_DIR>
    BUILD_IN_SOURCE 1
)
ExternalProject_Get_Property(mcpp INSTALL_DIR)
set(MCPP_LIBRARIES "${INSTALL_DIR}/lib/libmcpp.a")
set(MCPP_INCLUDES "${INSTALL_DIR}/include")

set(STB_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../libs/stb")
ExternalProject_Add(stb
    SOURCE_DIR "${STB_SOURCE_DIR}"
    GIT_REPOSITORY "https://github.com/nothings/stb.git"
    GIT_TAG "master"
    INSTALL_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
set(STB_INCLUDES "${STB_SOURCE_DIR}")

add_subdirectory(renderer)
add_subdirectory(device)
add_subdirectory(scene)
add_subdirectory(utils)
add_subdirectory(math)
add_subdirectory(io)

find_package(Boost REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(../libs)
include_directories(${MCPP_INCLUDES})
include_directories(${STB_INCLUDES})

file(GLOB_RECURSE PHI_SRC_CPP *.cpp)

add_library(PhiEngine STATIC ${PHI_SRC_CPP})
if(INTEL_WORKAROUNDS)
    add_definitions(-DINTEL_WORKAROUNDS)
endif()
target_link_libraries(PhiEngine ${GL_LIBRARIES} ${ASSIMP_LIBRARIES} ${MCPP_LIBRARIES})
add_dependencies(PhiEngine mcpp stb)
