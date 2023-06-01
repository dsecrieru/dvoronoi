include(FetchContent)

FetchContent_Declare(
        jcv
        GIT_REPOSITORY https://github.com/JCash/voronoi.git
        GIT_TAG origin/master
)
FetchContent_MakeAvailable(jcv)

FetchContent_Declare(
        MyGAL
        GIT_REPOSITORY https://github.com/pvigier/MyGAL.git
        GIT_TAG origin/master
)
FetchContent_MakeAvailable(MyGAL)

add_executable(benchmark benchmark.cpp)

target_include_directories(benchmark PRIVATE ${jcv_SOURCE_DIR}/src ${MyGAL_SOURCE_DIR}/include)
