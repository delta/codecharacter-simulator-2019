cmake_minimum_required(VERSION 3.11.1)
project(tests)


set(SOURCE_FILES
    test_main.cpp
)

add_executable(tests ${SOURCE_FILES})

target_link_libraries(tests gtest gmock)

install(TARGETS tests
    DESTINATION bin
)
