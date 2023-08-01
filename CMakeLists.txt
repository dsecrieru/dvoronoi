cmake_minimum_required(VERSION 3.25)
project(dvoronoi)

set(CMAKE_CXX_STANDARD 20)

#include(FetchContent)
#FetchContent_Declare(
#        ref_std_generator
#        GIT_REPOSITORY https://github.com/lewissbaker/generator.git
#        GIT_TAG origin/main
#)
#FetchContent_MakeAvailable(ref_std_generator)

add_library(dvoronoi INTERFACE
        include/dvoronoi/common/scalar.hpp
        include/dvoronoi/common/point.hpp
        include/dvoronoi/common/util.hpp
        include/dvoronoi/common/diagram.hpp
        include/dvoronoi/common/box.hpp
        include/dvoronoi/common/priority_queue.hpp
        include/dvoronoi/fortune/config.hpp
        include/dvoronoi/fortune/algorithm.hpp
        include/dvoronoi/fortune/beach_line.hpp
        include/dvoronoi/fortune/event.hpp
        include/dvoronoi/fortune/arc.hpp
        include/dvoronoi/fortune/arc_tree.hpp
        include/dvoronoi/fortune/bound.hpp)

#target_include_directories(dvoronoi INTERFACE ${stdgenerator_SOURCE_DIR}/include ..)
target_include_directories(dvoronoi INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_subdirectory(benchmark)
add_subdirectory(examples)
