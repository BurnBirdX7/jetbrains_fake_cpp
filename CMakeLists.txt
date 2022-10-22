cmake_minimum_required(VERSION 3.16)
project(jetbrains_fake_cpp)

set(CMAKE_CXX_STANDARD 17)

set(SRC src/Task.cpp src/Task.hpp
        src/ExecutionQueueBuilder.cpp src/ExecutionQueueBuilder.hpp)

add_executable(fake src/main.cpp ${SRC})

# COMPILER SETUP
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /Od")
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
    message(WARNING "yaml-cpp not found! Fetching...")
    include(FetchContent)
    FetchContent_Declare(
            yaml-cpp
            GIT_REPOSITORY "https://github.com/jbeder/yaml-cpp.git"
            GIT_TAG 1b50109f7bea60bd382d8ea7befce3d2bd67da5f
    )
    FetchContent_MakeAvailable(yaml-cpp)
endif()

# GTest
add_subdirectory(gtest)

target_link_libraries(fake PRIVATE yaml-cpp)

