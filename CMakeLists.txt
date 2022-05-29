cmake_minimum_required(VERSION 3.22)
project(simple_dsu_sample)
project(float_avg_dsu_sample)
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
        test/test.cpp
)
target_link_libraries(
        dsu_test
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(dsu_test)
