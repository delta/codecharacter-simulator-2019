cmake_minimum_required(VERSION 3.11.1)
project(physics)

set(SOURCE_FILES
	src/vector.hpp
)

set_source_files_properties(${SOURCE_FILES} PROPERTIES HEADER_FILE_ONLY 1)

add_library(physics SHARED ${SOURCE_FILES})
set_target_properties(physics PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(physics PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_PATH}>
	$<BUILD_INTERFACE:${EXPORTS_DIR}>
	$<INSTALL_INTERFACE:include>
)

install(DIRECTORY ${INCLUDE_PATH}/ DESTINATION include)
