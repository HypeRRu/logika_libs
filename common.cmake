include(${CMAKE_CURRENT_SOURCE_DIR}/external_project.cmake)

find_package(Protobuf CONFIG)
find_package(Protobuf REQUIRED)

add_external_project(resources resources/)
add_external_target(
    logika_resources
    resources/include
    resources/lib
    SHARED
    resources
)
target_link_libraries(logika_resources INTERFACE protobuf::libprotobuf)
