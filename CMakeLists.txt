cmake_minimum_required(VERSION 3.14)

project(logika LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3")

# suppress MSVC warnings
if(MSVC)
	add_definitions("/wd4251")
	add_definitions("/wd4275")
	add_definitions("/wd4996")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/resources)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/logika)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/migration)
