cmake_minimum_required(VERSION 3.14)
project(mybenchmark CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.4
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(benchmark)

add_executable(mybenchmark test.cpp
        utils.h
        plf_colony.h
        plf_list.h)

target_link_libraries(mybenchmark
        PRIVATE
        benchmark::benchmark
        pthread
)

target_compile_options(mybenchmark PRIVATE -O3 -DNDEBUG)
