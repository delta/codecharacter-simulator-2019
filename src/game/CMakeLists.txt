cmake_minimum_required(VERSION 3.11.1)
project(game)

set(SOURCE_FILES
  src/game.cpp
)

set(INCLUDE_PATH include)

set (Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.68.0 REQUIRED COMPONENTS system)

set(EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports)
set(EXPORTS_FILE_PATH ${EXPORTS_DIR}/game/game_export.h)

add_library(game STATIC ${SOURCE_FILES})
target_link_libraries(
	game simulator_constants state logger drivers player_wrapper Boost::system pthread)

generate_export_header(game EXPORT_FILE_NAME ${EXPORTS_FILE_PATH})

target_include_directories(game PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
	$<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
	$<BUILD_INTERFACE:${EXPORTS_DIR}>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS game EXPORT game_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)
install(EXPORT game_config DESTINATION lib)
install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
install(FILES ${EXPORTS_FILE_PATH} DESTINATION include/game)
