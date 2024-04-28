set(LIB_NAME ${CMAKE_PROJECT_NAME}_meters_common)

set(CMAKE_CXX_STANDARD 11)

set(
    ${LIB_NAME}_SOURCES_LIST
    ${SRC_DIR}/meters/types.cpp
	${SRC_DIR}/meters/archive_def.cpp
	${SRC_DIR}/meters/archive_field_def.cpp
	${SRC_DIR}/meters/archive_field.cpp
	${SRC_DIR}/meters/archive.cpp
	${SRC_DIR}/meters/interval_archive.cpp
	${SRC_DIR}/meters/service_archive.cpp
	${SRC_DIR}/meters/data_table.cpp
	${SRC_DIR}/meters/channel.cpp
	${SRC_DIR}/meters/item_def_base.cpp
	${SRC_DIR}/meters/meter.cpp
	${SRC_DIR}/meters/tag_def.cpp
	${SRC_DIR}/meters/tag.cpp
	${SRC_DIR}/meters/data_tag_def.cpp
	${SRC_DIR}/meters/data_tag.cpp
	${SRC_DIR}/meters/utils/types_converter.cpp
)

add_library(${LIB_NAME} SHARED ${${LIB_NAME}_SOURCES_LIST})

add_compile_definitions(${PROJECT_NAME_UPPER}_USE_METERS=)
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_common)

if (BUILD_LOGGER)
	target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_logger)
endif()

if (BUILD_SHARED_LIBS)
	target_compile_definitions(${LIB_NAME} PRIVATE ${LIB_NAME}_EXPORT)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILD_SHARED_LIBS)
endif()

unset(${LIB_NAME})
