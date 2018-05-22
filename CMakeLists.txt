cmake_minimum_required(VERSION 3.10)
project(benchmark)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES benchmark.cpp prefetching.cpp)
add_executable(benchmark benchmark.cpp)
set(CMAKE_CXX_FLAGS  "-static-libgcc -static-libstdc++ -O3")
