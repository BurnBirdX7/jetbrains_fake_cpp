cmake_minimum_required(VERSION 3.16)
project(jetbrains_fake_cpp)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /Od")
else()
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(CMAKE_CXX_FLAGS "-Wall -Werror -O0 -g -fsanitize=address")
    else()
        set(CMAKE_CXX_FLAGS "-Wall -Werror -O2")
    endif()
endif()


find_package(yaml-cpp)

set(SRC main.cpp
        Task.cpp Task.hpp
        TaskBuilder.cpp TaskBuilder.hpp)

add_executable(jetbrains_fake_cpp ${SRC})

target_link_libraries(jetbrains_fake_cpp PRIVATE yaml-cpp)
