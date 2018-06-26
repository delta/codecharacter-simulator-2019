cmake_minimum_required(VERSION 3.11.1)
project(tests)

set(SOURCE_FILES
        test_main.cpp
        physics/vector_test.cpp
        )

add_executable(tests ${SOURCE_FILES})

target_link_libraries(tests physics gtest gmock)

install(TARGETS tests
        DESTINATION bin
        )