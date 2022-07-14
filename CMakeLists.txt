cmake_minimum_required(VERSION 3.22)

project(dsu_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -fsized-deallocation)

add_subdirectory(lib)

option(BUILD_TESTING "Build tests" OFF) #OFF by default

if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
    add_subdirectory(test)
endif()