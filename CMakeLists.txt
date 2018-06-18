cmake_minimum_required(VERSION 3.9.6)
project(simulator)

include(GenerateExportHeader)

set(CMAKE_CXX_STANDARD 14)

if(NOT BUILD_PROJECT STREQUAL "no_tests")
    include(clang-format.cmake)
endif()

add_subdirectory(src/main)
