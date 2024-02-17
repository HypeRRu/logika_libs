cmake_minimum_required(VERSION 3.14)

project(logika_migration LANGUAGES CXX VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Protobuf CONFIG)
find_package(Protobuf REQUIRED)

add_executable(
    migration
    main.cpp
    generated/archive_types.pb.cc
    generated/buses.pb.cc
    generated/channels.pb.cc
    generated/data_types.pb.cc
    generated/devices.pb.cc
    generated/l4_archive_fields.pb.cc
    generated/l4_archives.pb.cc
    generated/l4_types.pb.cc
    generated/l4_tags.pb.cc
    generated/m4_archive_fields.pb.cc
    generated/m4_archives.pb.cc
    generated/m4_tags.pb.cc
    generated/tag_kinds.pb.cc
    generated/vars.pb.cc
    generated/x6_archive_fields.pb.cc
    generated/x6_archives.pb.cc
    generated/x6_tags.pb.cc
    generated/x6_tag_types.pb.cc
    generated/enums.pb.cc
)
target_include_directories(migration PRIVATE generated)
target_link_libraries(
    migration
    PRIVATE
    protobuf::libprotobuf
)
