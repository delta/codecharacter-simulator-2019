cmake_minimum_required(VERSION 3.11.1)
project(simulator)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Add the cmake folder to the module search path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Always enable colors, since Ninja strips them
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options(-fcolor-diagnostics)
endif ()

# Include Header Export module
include(GenerateExportHeader)

# Set C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)

# Include ClangFormat
if(NOT BUILD_PROJECT STREQUAL "no_tests")
    include(cmake/clang-format.cmake)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(ext/googletest)
add_subdirectory(src/constants)
add_subdirectory(src/main)
add_subdirectory(src/physics)
add_subdirectory(src/state)
add_subdirectory(test)
