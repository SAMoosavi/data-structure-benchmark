cmake_minimum_required(VERSION 3.14)
project(mybenchmark CXX)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.4
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(benchmark)

FetchContent_Declare(
        Boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.90.0/boost-1.90.0-cmake.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP ON
)

FetchContent_MakeAvailable(Boost)

add_executable(mybenchmark test.cpp
        utils.h
        plf_colony.h
        plf_list.h
        stl.h
        unordered_dense.h)

target_link_libraries(mybenchmark
        PRIVATE
        benchmark::benchmark
        Boost::headers
        Boost::container
        pthread
)

target_compile_options(mybenchmark PRIVATE -O3 -DNDEBUG)
