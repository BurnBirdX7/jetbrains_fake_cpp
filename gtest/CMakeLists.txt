cmake_minimum_required(VERSION 3.16)
project(jetbrains_fake_cpp_testing)

# GTEST
find_package(GTest QUIET)
if (NOT GTest_FOUND)
    message("GTest not found! Fetching...")
    FetchContent_Declare(
            GTest
            GIT_REPOSITORY "https://github.com/google/googletest"
            GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(GTest)
endif()


# TESTS

set(TEST_YAML_FILES test1.yaml test2.yaml)
foreach(file ${TEST_YAML_FILES})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${file}
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()


enable_testing()
set(TEST_SRC TaskTest.cpp BuilderTest.cpp)
add_executable(test ${TEST_SRC})
target_link_libraries(test PUBLIC
        GTest::gtest_main
        fake_lib)

include(GoogleTest)
gtest_discover_tests(test)