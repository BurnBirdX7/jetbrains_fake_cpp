cmake_minimum_required(VERSION 3.16)
project(jetbrains_fake_cpp)

set(CMAKE_CXX_STANDARD 17)

set(SRC main.cpp
        Task.cpp Task.hpp
        ExecutionQueueBuilder.cpp ExecutionQueueBuilder.hpp)

add_executable(fake ${SRC})

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /Od")
else()
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(CMAKE_CXX_FLAGS "-Wall -Werror -O0 -g -fsanitize=address")
    else()
        set(CMAKE_CXX_FLAGS "-Wall -Werror -O2")
    endif()
endif()

if (YAML_IS_SUBDIR)
    message("YAML_IS_SUBDIR is defined...")
    add_subdirectory(yaml-cpp)
    target_link_libraries(fake PRIVATE yaml-cpp)

elseif(YAML_IS_EXTERNAL)
    include(ExternalProject)
    message("YAML_IS_EXTERNAL is defined...")
    set(EXTERNAL_DIR ${CMAKE_BINARY_DIR}/external)
    ExternalProject_Add(yaml-cpp
            GIT_REPOSITORY "https://github.com/jbeder/yaml-cpp"
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}
            STEP_TARGETS yaml-cpp)

    include_directories(${EXTERNAL_DIR}/include)
    target_link_directories(fake PRIVATE ${EXTERNAL_DIR}/lib)
    add_dependencies(fake yaml-cpp)

else()
    message("Special flags are not specified...")
    find_package(yaml-cpp)
    target_link_libraries(fake PRIVATE yaml-cpp)
endif()


