set(LIB_NAME ${CMAKE_PROJECT_NAME}_resource_converters)

set(CMAKE_CXX_STANDARD 17)

set(
    ${LIB_NAME}_SOURCES_LIST
    ${SRC_DIR}/meters/converters/archive_type_converter.cpp
    ${SRC_DIR}/meters/converters/device_converter.cpp
    ${SRC_DIR}/meters/converters/channel_converter.cpp
    ${SRC_DIR}/meters/converters/m4_tag_converter.cpp
    ${SRC_DIR}/meters/converters/m4_archive_converter.cpp
    ${SRC_DIR}/meters/converters/m4_archive_field_converter.cpp
    ${SRC_DIR}/meters/converters/l4_tag_converter.cpp
    ${SRC_DIR}/meters/converters/l4_archive_converter.cpp
    ${SRC_DIR}/meters/converters/l4_archive_field_converter.cpp
)

add_library(${LIB_NAME} SHARED ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_resources)
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_common)
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_meters)

if (BUILD_LOGGER)
	target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_logger)
endif()

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${LIB_NAME} PRIVATE ${LIB_NAME}_EXPORT)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILD_SHARED_LIBS)
endif()

unset(${LIB_NAME})
