cmake_minimum_required(VERSION 3.11.1)
project(state)

set(SOURCE_FILES
	src/state.cpp
	src/state_syncer.cpp
	src/state_helpers.cpp
	src/command_giver.cpp
	src/actor/actor.cpp
	src/actor/unit.cpp
	src/actor/soldier.cpp
	src/actor/villager.cpp
	src/actor/factory.cpp
	src/map/map.cpp
	src/gold_manager/gold_manager.cpp
	src/score_manager/score_manager.cpp
	src/actor/soldier_states/soldier_state.cpp
	src/actor/soldier_states/soldier_idle_state.cpp
	src/actor/soldier_states/soldier_attack_state.cpp
	src/actor/soldier_states/soldier_move_state.cpp
	src/actor/soldier_states/soldier_dead_state.cpp
	src/actor/soldier_states/soldier_pursuit_state.cpp
	src/actor/villager_states/villager_state.cpp
	src/actor/villager_states/villager_idle_state.cpp
	src/actor/villager_states/villager_attack_state.cpp
	src/actor/villager_states/villager_build_state.cpp
	src/actor/villager_states/villager_move_state.cpp
	src/actor/villager_states/villager_move_to_build_state.cpp
	src/actor/villager_states/villager_mine_state.cpp
	src/actor/villager_states/villager_move_to_mine_state.cpp
	src/actor/villager_states/villager_dead_state.cpp
	src/actor/villager_states/villager_pursuit_state.cpp
	src/actor/factory_states/factory_state.cpp
	src/actor/factory_states/factory_idle_state.cpp
	src/actor/factory_states/factory_production_state.cpp
	src/actor/factory_states/factory_unbuilt_state.cpp
	src/actor/factory_states/factory_dead_state.cpp
	src/path_planner/path_planner.cpp
	src/path_planner/path_graph.cpp
	src/path_planner/path_cache.cpp
)

set(INCLUDE_PATH include)

set(EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports)
set(EXPORTS_FILE_PATH ${EXPORTS_DIR}/state/state_export.h)

add_library(state STATIC ${SOURCE_FILES})
target_link_libraries(state physics constants)

generate_export_header(state EXPORT_FILE_NAME ${EXPORTS_FILE_PATH})

target_include_directories(state PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
	$<BUILD_INTERFACE:${EXPORTS_DIR}>
	$<INSTALL_INTERFACE:include>
)

if((NOT BUILD_PROJECT STREQUAL "all") AND (NOT BUILD_PROJECT STREQUAL "no_tests"))
	target_include_directories(state PUBLIC
		$<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
	)
else()
	target_include_directories(state PUBLIC
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/logger/include>
	)
endif()

install(TARGETS state EXPORT state_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)
install(EXPORT state_config DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
install(FILES ${EXPORTS_FILE_PATH} DESTINATION include/state)
