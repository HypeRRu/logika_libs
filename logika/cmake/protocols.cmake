set(LIB_NAME ${CMAKE_PROJECT_NAME}_protocols)

set(
    ${LIB_NAME}_SOURCES_LIST
	${SRC_DIR}/protocols/protocol.cpp
	${SRC_DIR}/protocols/comm_exception.cpp
	${SRC_DIR}/protocols/m4/archive_record.cpp
	${SRC_DIR}/protocols/m4/packet.cpp
	${SRC_DIR}/protocols/m4/m4protocol_impl.cpp
	${SRC_DIR}/protocols/m4/m4protocol_api.cpp
)

add_library(${LIB_NAME} SHARED ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_logger)
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_common)

if (BUILD_SHARED_LIBS)
	target_compile_definitions(${LIB_NAME} PRIVATE ${LIB_NAME}_EXPORT)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILD_SHARED_LIBS)
endif()

unset(${LIB_NAME})
