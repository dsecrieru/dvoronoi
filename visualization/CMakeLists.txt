cmake_minimum_required(VERSION 3.25)
project(dvoronoi_vis)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.5.1
)
set(BUILD_SHARED_LIBS OFF)
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_NETWORK OFF)
set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_GRAPHICS ON)
set(SFML_BUILD_WINDOW ON)
FetchContent_MakeAvailable(SFML)

include_directories(${PROJECT_SOURCE_DIR}/../..)

add_executable(visualization visualization.cpp)
target_link_libraries(visualization PRIVATE sfml-graphics)