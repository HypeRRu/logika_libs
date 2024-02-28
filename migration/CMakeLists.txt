cmake_minimum_required(VERSION 3.14)

project(logika_migration LANGUAGES CXX VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(
    logika_migration
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/utils/enums_string_to_pb.cpp
    ${SRC_DIR}/converters/archive_types.cpp
    ${SRC_DIR}/converters/buses.cpp
    ${SRC_DIR}/converters/channels.cpp
    ${SRC_DIR}/converters/data_types.cpp
    ${SRC_DIR}/converters/devices.cpp
    ${SRC_DIR}/converters/tag_kinds.cpp
    ${SRC_DIR}/converters/vars.cpp
    ${SRC_DIR}/converters/l4_archive_fields.cpp
    ${SRC_DIR}/converters/l4_archives.cpp
    ${SRC_DIR}/converters/l4_tags.cpp
    ${SRC_DIR}/converters/l4_types.cpp
    ${SRC_DIR}/converters/m4_archive_fields.cpp
    ${SRC_DIR}/converters/m4_archives.cpp
    ${SRC_DIR}/converters/m4_tags.cpp
    ${SRC_DIR}/converters/x6_archive_fields.cpp
    ${SRC_DIR}/converters/x6_archives.cpp
    ${SRC_DIR}/converters/x6_tag_types.cpp
    ${SRC_DIR}/converters/x6_tags.cpp
)
target_include_directories(logika_migration PRIVATE ${INCLUDE_DIR})

target_link_libraries(logika_migration logika_resources)
target_include_directories(logika_migration PRIVATE ${logika_resources_INCLUDE_DIR})
