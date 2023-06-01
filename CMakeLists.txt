cmake_minimum_required(VERSION 3.25)
project(dvoronoi)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        ref_std_generator
        GIT_REPOSITORY https://github.com/lewissbaker/generator.git
        GIT_TAG origin/main
)
FetchContent_MakeAvailable(ref_std_generator)

add_library(dvoronoi INTERFACE fortune/algorithm.hpp common/diagram.hpp fortune/beachline.hpp fortune/beachline.hpp common/util.hpp fortune/event_queue.hpp fortune/event_queue.hpp benchmark/benchmark.cpp)

target_include_directories(dvoronoi INTERFACE ${stdgenerator_SOURCE_DIR}/include ..)
target_include_directories(dvoronoi INTERFACE ..)

add_subdirectory(benchmark)
add_subdirectory(examples)
