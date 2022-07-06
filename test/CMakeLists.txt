include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(
        dsu_test
        default_data_tests.cpp
        custom_data_tests.cpp
)
target_link_libraries(dsu_test LINK_PUBLIC gtest_main DSUWithData)
include(GoogleTest DSUWithData)
gtest_discover_tests(dsu_test)
