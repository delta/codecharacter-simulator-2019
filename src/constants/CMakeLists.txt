cmake_minimum_required(VERSION 3.11.1)
project(constants)

add_library(constants INTERFACE)
target_link_libraries(constants INTERFACE)

target_include_directories(constants INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS constants EXPORT constants_config
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(EXPORT constants_config DESTINATION lib)
install(DIRECTORY include DESTINATION include)
