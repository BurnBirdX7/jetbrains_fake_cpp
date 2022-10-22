cmake_minimum_required(VERSION 3.16)
project(jetbrains_fake_cpp_testing)

find_package(GTest QUIET)
if ( False )
    message(WARNING "GTest not found! Fetching...")
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY "https://github.com/jbeder/yaml-cpp.git"
            GIT_TAG 1b50109f7bea60bd382d8ea7befce3d2bd67da5f
    )
    FetchContent_MakeAvailable(googletest)

endif()

enable_testing()

set(TEST_SRC TaskTest.cpp BuilderTest.cpp)
foreach(src ${SRC})
    set(TEST_SRC "${TEST_SRC};../${src}")
endforeach()

add_executable(test ${TEST_SRC})
target_link_libraries(test PUBLIC
        GTest::gtest_main
        yaml-cpp)

include(GoogleTest)
gtest_discover_tests(test)
