cmake_minimum_required(VERSION 3.22)
project(simple_dsu_sample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -fsized-deallocation)

add_executable(simple_dsu_sample samples/simple_dsu_sample.cpp)
