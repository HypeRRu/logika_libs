cmake_minimum_required(VERSION 3.14)

project(logika_resources LANGUAGES CXX VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(INCLUDE_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/logika/resources)

#if (UNIX)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g0")
#endif()

set(Protobuf_USE_STATIC_LIBS OFF)
find_package(Protobuf CONFIG)
find_package(Protobuf REQUIRED)

add_library(
    logika_resources
    logika/resources/archive_types.proto
    logika/resources/buses.proto
    logika/resources/channels.proto
    logika/resources/data_types.proto
    logika/resources/devices.proto
    logika/resources/enums.proto
    logika/resources/tag_kinds.proto
    logika/resources/vars.proto
    logika/resources/l4_archive_fields.proto
    logika/resources/l4_archives.proto
    logika/resources/l4_tags.proto
    logika/resources/l4_types.proto
    logika/resources/m4_archive_fields.proto
    logika/resources/m4_archives.proto
    logika/resources/m4_tags.proto
    logika/resources/x6_archive_fields.proto
    logika/resources/x6_archives.proto
    logika/resources/x6_tag_types.proto
    logika/resources/x6_tags.proto
)
target_include_directories(logika_resources PRIVATE ${SRC_DIR})
target_include_directories(logika_resources PRIVATE ${INCLUDE_DIR})

protobuf_generate(
    TARGET logika_resources
    LANGUAGE cpp
    IMPORT_DIRS logika/resources
    PROTOC_OUT_DIR src
    OUT_VAR PROTO_SRCS
)

set(PROTO_INCS ${PROTO_SRCS})
list(
    FILTER PROTO_INCS
    INCLUDE REGEX ".\.pb\.h$"
)

# copy includes
add_custom_command(
    TARGET logika_resources
    POST_BUILD
    COMMAND_EXPAND_LISTS
    COMMAND ${CMAKE_COMMAND} -E copy "$<JOIN:${PROTO_INCS},;>" ${INCLUDE_INSTALL_DIR}/
    COMMENT "Copy include files to ${INCLUDE_INSTALL_DIR}"
)