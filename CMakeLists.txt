cmake_minimum_required(VERSION 3.16)
project(jetbrains_fake_cpp)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)

set(SRC src/Task.cpp include/Task.hpp
        src/ExecutionQueueBuilder.cpp include/ExecutionQueueBuilder.hpp)

add_library(fake_lib ${SRC})
add_executable(fake src/main.cpp)

# COMPILER SETUP
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
else()
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(CMAKE_CXX_FLAGS "-Wall -Werror -O0 -g -fsanitize=address")
    else()
        set(CMAKE_CXX_FLAGS "-Wall -Werror -O2")
    endif()
endif()

# YAML
find_package(yaml-cpp QUIET)
if(NOT yaml-cpp_FOUND)
    message("yaml-cpp not found! Fetching...")
    FetchContent_Declare(
            yaml-cpp
            GIT_REPOSITORY "https://github.com/jbeder/yaml-cpp.git"
            GIT_TAG 1b50109f7bea60bd382d8ea7befce3d2bd67da5f
    )
    FetchContent_MakeAvailable(yaml-cpp)
endif()

# GTest
add_subdirectory(gtest)
target_include_directories(fake_lib PUBLIC include)
target_link_libraries(fake_lib PUBLIC yaml-cpp)
target_link_libraries(fake PUBLIC fake_lib)

