cmake_minimum_required(VERSION 3.14)

project(logika LANGUAGES CXX)
include(CMakeDependentOption)

# suppress MSVC warnings
if(MSVC)
	add_definitions("/wd4251")
	add_definitions("/wd4275")
	add_definitions("/wd4996")
endif()

option(BUILD_RESOURCES "Build resources library" ON)
cmake_dependent_option(
	BUILD_MIGRATION_TOOL
	"Build resources generation tool"
	ON
	"BUILD_RESOURCES"
	OFF
)

if (BUILD_RESOURCES)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/resources)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/logika)

if (BUILD_MIGRATION_TOOL)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/migration)
endif()
