cmake_minimum_required(VERSION 3.14)

project(logika LANGUAGES CXX)

#include(${CMAKE_CURRENT_SOURCE_DIR}/common.cmake)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/resources)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/logika)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/migration)
