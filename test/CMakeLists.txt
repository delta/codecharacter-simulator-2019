cmake_minimum_required(VERSION 3.11.1)
project(tests)

set(SOURCE_FILES
	test_main.cpp
	physics/vector_test.cpp
	state/map_test.cpp
	state/soldier_test.cpp
	state/villager_test.cpp
	state/factory_test.cpp
	state/state_test.cpp
	state/path_planner_test.cpp
	state/state_syncer_test.cpp
	state/command_giver_test.cpp
	logger/logger_test.cpp
	llvm_pass/llvm_pass_test.cpp
	drivers/timer_test.cpp
	drivers/shared_memory/shm_test.cpp
	drivers/main_driver_test.cpp
)

if(NOT BUILD_PROJECT STREQUAL "all")
	include(${CMAKE_INSTALL_PREFIX}/lib/physics_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/state_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/drivers_config.cmake)
	include(${CMAKE_INSTALL_PREFIX}/lib/logger_config.cmake)
endif()

include_directories(.)

add_executable(tests ${SOURCE_FILES})

add_executable(shm_client drivers/shared_memory/shm_client.cpp)

add_executable(main_driver_test_player drivers/main_driver_test_player)

target_link_libraries(tests physics constants simulator_constants state drivers logger player_wrapper gtest gmock)
target_link_libraries(tests player_code_test_0 player_code_test_1 player_code_test_2)

target_link_libraries(shm_client state drivers)

target_link_libraries(main_driver_test_player drivers)

install(TARGETS tests shm_client main_driver_test_player DESTINATION bin)
