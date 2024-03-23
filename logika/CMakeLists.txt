cmake_minimum_required(VERSION 3.14)

project(logika LANGUAGES CXX VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# @todo Опции для выбора модулей для сборки
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/logger.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/connections.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/meters.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/resource_converters.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/protocols.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.cmake)

add_executable(
	test_main
	${SRC_DIR}/main.cpp
)
target_include_directories(test_main PRIVATE ${INCLUDE_DIR})

target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_logger)
target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_connections)
target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters)
target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_resource_converters)
target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_protocols)
target_link_libraries(test_main PRIVATE ${CMAKE_PROJECT_NAME}_common)

target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_logger)
target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_connections)
target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_meters)
target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_resource_converters)
target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_protocols)
target_include_directories(test_main PRIVATE ${CMAKE_PROJECT_NAME}_common)
