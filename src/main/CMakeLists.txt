cmake_minimum_required(VERSION 3.11.1)
project(main)

set(SOURCE_FILES
	main.cpp
)

set(INCLUDE_PATH include)

add_executable(main ${SOURCE_FILES})

target_link_libraries(main game physics state logger drivers player_wrapper pthread)

target_include_directories(main PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS main
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)
