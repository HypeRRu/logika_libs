set(LIB_NAME ${CMAKE_PROJECT_NAME}_protocols)

set(
    ${LIB_NAME}_SOURCES_LIST
	${SRC_DIR}/protocols/protocol.cpp
	${SRC_DIR}/protocols/m4/archive_record.cpp
	${SRC_DIR}/protocols/m4/packet.cpp
	${SRC_DIR}/protocols/m4/m4protocol.cpp
)

add_library(${LIB_NAME} ${${LIB_NAME}_SOURCES_LIST})
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_logger)
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_common)
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})

unset(${LIB_NAME})
