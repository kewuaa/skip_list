cmake_minimum_required(VERSION 3.15)

project(skip_list LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -O0 -g -ggdb")

set(BUILD_TESTS TRUE CACHE BOOL "if to enable tests")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(BUILD_TESTS FALSE)
endif()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
