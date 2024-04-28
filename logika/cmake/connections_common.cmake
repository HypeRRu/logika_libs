set(LIB_NAME ${CMAKE_PROJECT_NAME}_connections_common)

set(CMAKE_CXX_STANDARD 11)

set(
    ${LIB_NAME}_SOURCES_LIST
    ${SRC_DIR}/connections/connection.cpp
	${SRC_DIR}/connections/utils/types_converter.cpp
)

if (WIN32)
	list(APPEND ${LIB_NAME}_SOURCES_LIST ${SRC_DIR}/connections/common/windows_socket_io.cpp)
elseif(UNIX)
	list(APPEND ${LIB_NAME}_SOURCES_LIST ${SRC_DIR}/connections/common/linux_io.cpp)
endif()

add_compile_definitions(${PROJECT_NAME_UPPER}_USE_CONNECTIONS=)
add_library(${LIB_NAME} SHARED ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_common)

if (BUILD_LOGGER)
	target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_logger)
endif()

if (WIN32)
    target_link_libraries(${LIB_NAME} PUBLIC ws2_32)
endif()

if (BUILD_SHARED_LIBS)
	target_compile_definitions(${LIB_NAME} PRIVATE ${LIB_NAME}_EXPORT)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILD_SHARED_LIBS)
endif()

unset(${LIB_NAME})
