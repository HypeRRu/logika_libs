cmake_minimum_required(VERSION 3.14)

project(logika LANGUAGES CXX VERSION 1.0)
include(CMakeDependentOption)
string(TOUPPER ${CMAKE_PROJECT_NAME} PROJECT_NAME_UPPER)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 17)
add_executable(
	test_main
	${SRC_DIR}/main.cpp
)
target_include_directories(test_main PRIVATE ${INCLUDE_DIR})

# @todo Опции для выбора модулей для сборки
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.cmake)
target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_common)
target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_common)

option(BUILD_LOGGER "Build logger library" ON)
if (BUILD_LOGGER)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/logger.cmake)
	target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_logger)
	target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_logger)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/connections.cmake)
target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_connections)
target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_connections)

option(BUILD_LOGIKA4L "Build Logika4L meters library" ON)
option(BUILD_LOGIKA4M "Build Logika4M meters library" ON)
option(BUILD_LOGIKA6  "Build Logika6 meters library" OFF)

if ((NOT BUILD_LOGIKA4L) AND (NOT BUILD_LOGIKA4M) AND (NOT BUILD_LOGIKA6))
	message( FATAL_ERROR "You must specify at least one type of meters." )
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/meters_common.cmake)
target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters_common)
target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters_common)

if (BUILD_LOGIKA4L OR BUILD_LOGIKA4M)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/meters4.cmake)
	target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters4)
	target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters4)
endif()
if (BUILD_LOGIKA4L)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/meters4l.cmake)
	target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters4l)
	target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters4l)
endif()
if (BUILD_LOGIKA4M)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/meters4m.cmake)
	target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters4m)
	target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters4m)
endif()
if (BUILD_LOGIKA6)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/meters6.cmake)
	target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters6)
	target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters6)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/protocols.cmake)
target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_protocols)
target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_protocols)

cmake_dependent_option(
	BUILD_RESOURCES_CONVERTER
	"Build resources converter library"
	ON
	"BUILD_RESOURCES"
	OFF
)
if (BUILD_RESOURCES_CONVERTER)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/resource_converters.cmake)
	target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_resource_converters)
	target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_resource_converters)
endif()

