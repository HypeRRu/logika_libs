cmake_minimum_required(VERSION 3.14)

project(logika_resources LANGUAGES CXX VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)
set(Protobuf_USE_STATIC_LIBS OFF)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g0")
endif()

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(Protobuf CONFIG)
find_package(Protobuf REQUIRED)

add_library(
    logika_resources
    templates/archive_types.proto
    templates/buses.proto
    templates/channels.proto
    templates/data_types.proto
    templates/devices.proto
    templates/enums.proto
    templates/tag_kinds.proto
    templates/vars.proto
    templates/l4_archive_fields.proto
    templates/l4_archives.proto
    templates/l4_tags.proto
    templates/l4_types.proto
    templates/m4_archive_fields.proto
    templates/m4_archives.proto
    templates/m4_tags.proto
    templates/x6_archive_fields.proto
    templates/x6_archives.proto
    templates/x6_tag_types.proto
    templates/x6_tags.proto
)
target_link_libraries(logika_resources ${Protobuf_LIBRARIES})

protobuf_generate(
    TARGET logika_resources
    LANGUAGE cpp
    PROTOC_OUT_DIR src
    APPEND_PATH templates
    OUT_VAR PROTO_SRCS
)
set(PROTO_INCS ${PROTO_SRCS})
list(
    FILTER PROTO_INCS
    INCLUDE REGEX ".\.pb\.h$"
)
install(
    FILES ${PROTO_INCS}
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include
)
