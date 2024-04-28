set(LIB_NAME ${CMAKE_PROJECT_NAME}_protocols)

set(CMAKE_CXX_STANDARD 11)

set(
    ${LIB_NAME}_SOURCES_LIST
	${SRC_DIR}/protocols/protocol.cpp
	${SRC_DIR}/protocols/comm_exception.cpp
	${SRC_DIR}/protocols/m4/archive_record.cpp
	${SRC_DIR}/protocols/m4/flash_ring_buffer.cpp
	${SRC_DIR}/protocols/m4/flash_archive4l.cpp
	${SRC_DIR}/protocols/m4/archive4m.cpp
	${SRC_DIR}/protocols/m4/packet.cpp
	${SRC_DIR}/protocols/m4/m4protocol_impl.cpp
	${SRC_DIR}/protocols/m4/m4protocol_api.cpp
	${SRC_DIR}/protocols/x6/spbusprotocol.cpp
)

add_library(${LIB_NAME} SHARED ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_common)
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_meters)
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_connections)

if (BUILD_LOGGER)
	target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_logger)
endif()

if (BUILD_SHARED_LIBS)
	target_compile_definitions(${LIB_NAME} PRIVATE ${LIB_NAME}_EXPORT)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILD_SHARED_LIBS)
endif()

unset(${LIB_NAME})
