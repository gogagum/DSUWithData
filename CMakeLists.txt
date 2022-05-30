cmake_minimum_required(VERSION 3.22)

project(dsu_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -fsized-deallocation)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        dsu_test
        test/default_data_tests.cpp
        test/custom_data_tests.cpp
)
target_link_libraries(
        dsu_test
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(dsu_test)
