cmake_minimum_required(VERSION 3.11.1)
project(tests)

set(SOURCE_FILES
	test_main.cpp
	# physics/vector_test.cpp
	# state/map_test.cpp
	# state/soldier_test.cpp
	# state/villager_test.cpp
	# state/factory_test.cpp
	# state/state_test.cpp
	state/path_planner_test.cpp
	# logger/logger_test.cpp
	# state/state_syncer_test.cpp
)

add_executable(tests ${SOURCE_FILES})

include_directories(.)

target_link_libraries(tests physics state logger gtest gmock)

install(TARGETS tests
	DESTINATION bin
)
