cmake_minimum_required(VERSION 3.16)
project(jetbrains_fake_cpp)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /Od")
endif()

find_package(yaml-cpp)

add_executable(jetbrains_fake_cpp main.cpp Task.cpp Task.hpp TaskBuilder.cpp TaskBuilder.hpp proc_exec.hpp)

target_link_libraries(jetbrains_fake_cpp PRIVATE yaml-cpp)
