cmake_minimum_required(VERSION 3.11.1)
project(simulator)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(GenerateExportHeader)

set(CMAKE_CXX_STANDARD 14)

if(NOT BUILD_PROJECT STREQUAL "no_tests")
    include(clang-format.cmake)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(ext/googletest)
add_subdirectory(src/main)
add_subdirectory(src/physics)
add_subdirectory(src/state)
add_subdirectory(test)
