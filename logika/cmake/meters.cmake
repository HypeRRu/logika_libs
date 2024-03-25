set(LIB_NAME ${CMAKE_PROJECT_NAME}_meters)

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
	${SRC_DIR}/meters/logika4/archive_field_def4.cpp
	${SRC_DIR}/meters/logika4/tag_def4.cpp
	${SRC_DIR}/meters/logika4/4l/tag_def4l.cpp
	${SRC_DIR}/meters/logika4/4l/archive_def4l.cpp
	${SRC_DIR}/meters/logika4/4l/archive_field_def4l.cpp
	${SRC_DIR}/meters/logika4/4m/tag_def4m.cpp
	${SRC_DIR}/meters/logika4/4m/archive_def4m.cpp
	${SRC_DIR}/meters/logika4/4m/archive_field_def4m.cpp
	${SRC_DIR}/meters/logika6/tag_def6.cpp
	${SRC_DIR}/meters/logika6/archive_def6.cpp
	${SRC_DIR}/meters/logika6/archive_field_def6.cpp
	${SRC_DIR}/meters/utils/types_converter.cpp
)

add_library(${LIB_NAME} ${${LIB_NAME}_SOURCES_LIST})
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_logger)
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_common)
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})

unset(${LIB_NAME})
