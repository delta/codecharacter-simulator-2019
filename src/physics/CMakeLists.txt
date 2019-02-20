cmake_minimum_required(VERSION 3.11.1)
project(physics)

add_library(physics INTERFACE)
target_link_libraries(physics INTERFACE)

target_include_directories(physics INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS physics EXPORT physics_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(EXPORT physics_config DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
