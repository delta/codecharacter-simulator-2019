cmake_minimum_required(VERSION 3.11.1)
project(simulator_constants)

add_library(simulator_constants INTERFACE)
target_link_libraries(simulator_constants INTERFACE)

target_include_directories(simulator_constants INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS simulator_constants EXPORT simulator_constants_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(EXPORT simulator_constants_config DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
