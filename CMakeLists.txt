cmake_minimum_required(VERSION 3.22)

project(dsu_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -fsized-deallocation)

add_subdirectory(lib)
add_subdirectory(test)
